/*
 * MIT License
 * 
 * Copyright (c) 2017 aff3ct
 * Copyright (c) 2019 Sergei Semenov
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Model.h"

void check_callback(Model *model)
{
    std::cout << "Callback with model " << (unsigned long) model << std::endl;
}

void check_detailed_callback(const B_TYPE *U, const B_TYPE *Y, const int frame_id, Model *model)
{
    
    std::cout << "Detailed Callback with model " << (unsigned long) model << 
            " and frame_id " << frame_id << 
            " and K " << model->getK() <<
            std::endl;
    
    std::cout << "{U}: ";
    for (int i = 0; i < model->getK();i++)
    {
        std::cout << U[i] << " "; 
    }
    std::cout << std::endl;
    
    std::cout << "{Y}: ";
    for (int i = 0; i < model->getK();i++)
    {
        std::cout << Y[i] << " "; 
    }
    std::cout << std::endl;
    
    
}

std::error_code Model::constructAll()
{
    std::error_code ec = make_error_code(Aff3ctErrc::NoError);
    
    m_source = std::unique_ptr<module::Source<B_TYPE>>                          (new module::Source_memory<B_TYPE>(p_src->K, m_inputData));
    m_codec   =  std::unique_ptr<module::Codec_repetition<B_TYPE, Q_TYPE>>      (p_cdc->build());  
    m_modem   = std::unique_ptr<module::Modem<B_TYPE, R_TYPE, Q_TYPE>>          (p_mdm->build()); 
    m_channel = std::unique_ptr<module::Channel<R_TYPE>>                        (p_chn->build()); 
    //m_monitor = std::unique_ptr<module::Monitor_BFER_detailed<B_TYPE>>          (p_mnt->build()); 
    m_monitor = std::unique_ptr<module::Monitor_BFER_detailed<B_TYPE>>(
           new module::Monitor_BFER_detailed<B_TYPE>(p_mnt->K, p_mnt->n_frame_errors,p_mnt->max_frame = 0, false, p_mnt->n_frames));  
           
    std::function<void(          void)> callback_fn = std::bind(check_callback, this);
    m_monitor->add_handler_check(callback_fn);
    
    std::function<void(const B_TYPE *, const B_TYPE *, const int)> callback_detailed_fn = std::bind(check_detailed_callback, 
                                                                                                    std::placeholders::_1,
                                                                                                    std::placeholders::_2,
                                                                                                    std::placeholders::_3,
                                                                                                    this);
    m_monitor->add_handler_detailed_check(callback_detailed_fn);
    
    return ec;
}

void Model::setDebugPrint(bool bEnabled) 
{
    m_bDebugPrint = bEnabled;
     
    if (m_bInitialized) 
    {
        using namespace module;
    
        auto& r_encoder = m_codec->get_encoder();
        auto& r_decoder = m_codec->get_decoder_siho();
    
        (*m_source )[src::tsk::generate    ].set_debug(true);
        (*r_encoder)[enc::tsk::encode      ].set_debug(true);
        (*m_modem  )[mdm::tsk::modulate    ].set_debug(true);
        (*m_channel)[chn::tsk::add_noise   ].set_debug(true);
        (*m_modem  )[mdm::tsk::demodulate  ].set_debug(true);
        (*r_decoder)[dec::tsk::decode_siho ].set_debug(true);
        (*m_monitor)[mnt::tsk::check_errors].set_debug(true);
    }
}

bool Model::reset()
{
    std::unique_ptr<factory::Source          ::parameters>   src(new factory::Source::parameters());
    std::unique_ptr<factory::Codec_repetition::parameters>   cdc(new factory::Codec_repetition::parameters());
    std::unique_ptr<factory::Modem           ::parameters>   mdm(new factory::Modem           ::parameters());
    std::unique_ptr<factory::Channel         ::parameters>   chn(new factory::Channel         ::parameters());
    std::unique_ptr<factory::Monitor_BFER    ::parameters>   mnt(new factory::Monitor_BFER    ::parameters());
    std::unique_ptr<factory::Terminal        ::parameters>   ter(new factory::Terminal        ::parameters());
    
    std::unique_ptr<factory::Launcher        ::parameters>   launcher(new factory::Launcher        ::parameters());
    
    p_src.swap(src);
    p_cdc.swap(cdc);
    p_mdm.swap(mdm);
    p_chn.swap(chn);
    p_mnt.swap(mnt);
    p_ter.swap(ter);
    
    p_launcher.swap(launcher);
}

/*
 * \ref https://github.com/aff3ct/my_project_with_aff3ct/blob/master/examples/factory/src/main.cpp
 * \param arg_vec - vector arguments where arg_vec[0] shall be interpreted as a program name
 */
bool Model::init(std::list<std::string> &arg_vec, std::error_code &ec, std::ostream& err_stream)
{
    std::vector<const char *> argv;

    argv.reserve(arg_vec.size());
    for (std::string &arg : arg_vec) 
    {   
        argv.push_back(arg.c_str());
    }

    reset();

    m_paramsList =  {p_src.get(), 
                        p_cdc.get(), 
                        p_mdm.get(), 
                        p_chn.get(), 
                        p_mnt.get(), 
                        p_ter.get(), 
                        p_launcher.get()};

    factory::Command_parser cp(argv.size(), (char**)&argv[0], m_paramsList, true, err_stream);
    
    std::vector<B_TYPE> vectorBuffer(p_src->K);
    m_inputData.push_back(vectorBuffer);
        
    if (cp.help_required())
    {
        cp.print_help    ();
        return false;
    }
    
    // parse the command for the given parameters and fill them
    if (cp.parsing_failed())
    {
        cp.print_warnings();
        cp.print_errors  ();
    
        //TRACELOG(ERROR,"cp.parsing_failed");
        //FIXME: Redirect std::cout to log!
        return false;
    }
       
    // display the headers (= print the AFF3CT parameters on the screen)
    factory::Header::print_parameters(m_paramsList); 
    cp.print_warnings();

    m_launcher = std::unique_ptr<launcher::Launcher> (p_launcher->build<B_TYPE, R_TYPE, Q_TYPE>(argv.size(), (const char**)&argv[0]));
    
    ec = constructAll();
    if (ec)
        return false; 
       

    // get the r_encoder and r_decoder modules from the codec module
    auto& r_encoder = m_codec->get_encoder();
    auto& r_decoder = m_codec->get_decoder_siho();
#ifdef ENABLE_REPORTERS
    // create reporters to display results in the terminal
    std::vector<tools::Reporter*> reporters =
    {
            new tools::Reporter_noise     <>(noise   ), // report the noise values (Es/N0 and Eb/N0)
            new tools::Reporter_BFER      <>(*monitor), // report the bit/frame error rates
            new tools::Reporter_throughput<>(*monitor)  // report the simulation throughputs
    };
    // convert the vector of reporter pointers into a vector of smart pointers
    std::vector<std::unique_ptr<tools::Reporter>> reporters_uptr;
    for (auto rep : reporters) reporters_uptr.push_back(std::unique_ptr<tools::Reporter>(rep));

    // create a terminal that will display the collected data from the reporters
    std::unique_ptr<tools::Terminal> terminal(p_ter.build(reporters_uptr));

    // display the legend in the terminal
    terminal->legend();
#endif // ENABLE_REPORTERS
    
    m_modules = {m_source.get(), 
                    r_encoder.get(), 
                    m_modem.get(), 
                    m_channel.get(),
                    r_decoder.get(), 
                    m_monitor.get()};

    //use default parameters
    for (auto& m : m_modules)
        for (auto& t : m->tasks)
        {
            t->set_autoalloc  (true ); // enable the automatic allocation of the data in the tasks
            t->set_autoexec   (false); // disable the auto execution mode of the tasks
            t->set_debug      (false); // disable the debug mode
            t->set_debug_limit(16   ); // display only the 16 first bits if the debug mode is enabled
            t->set_stats      (true ); // enable the statistics

            // enable the fast mode (= disable the useless verifs in the tasks) if there is no debug and stats modes
            t->set_fast(!t->is_debug() && !t->is_stats());
        }
    
    
    // sockets binding (connect the sockets of the tasks = fill the input sockets with the output sockets)
    using namespace module;
    
    (*r_encoder)[enc::sck::encode      ::U_K ].bind((*m_source )[src::sck::generate   ::U_K ]);
    (*m_modem  )[mdm::sck::modulate    ::X_N1].bind((*r_encoder)[enc::sck::encode     ::X_N ]);
    (*m_channel)[chn::sck::add_noise   ::X_N ].bind((*m_modem  )[mdm::sck::modulate   ::X_N2]);
    (*m_modem  )[mdm::sck::demodulate  ::Y_N1].bind((*m_channel)[chn::sck::add_noise  ::Y_N ]);
    (*r_decoder)[dec::sck::decode_siho ::Y_N ].bind((*m_modem  )[mdm::sck::demodulate ::Y_N2]);
    (*m_monitor)[mnt::sck::check_errors::U   ].bind((*r_encoder)[enc::sck::encode     ::U_K ]);
    (*m_monitor)[mnt::sck::check_errors::V   ].bind((*r_decoder)[dec::sck::decode_siho::V_K ]);
    
    // reset the memory of the r_decoder after the end of each communicatio
    m_monitor->add_handler_check(std::bind(&module::Decoder::reset, r_decoder));

    // initialize the interleaver if this code use an interleaver
    try
    {
            auto& interleaver = m_codec->get_interleaver();
            interleaver->init();
    }
    catch (const std::exception&) { /* do nothing if there is no interleaver */ }

    m_bInitialized = true;
    
    //postponed initialization
    setDebugPrint(m_bDebugPrint);
    setNoise(m_fNoise);
}

void Model::setNoise(float ebn0) 
{
    this->m_fNoise = ebn0;
    if (m_bInitialized) 
    {
        tools::Sigma<> noise;

        const float R = (float) p_cdc->enc->K / (float)p_cdc->enc->N_cw;


        // compute the current sigma for the channel noise
        const auto esn0  = tools::ebn0_to_esn0 (ebn0, R);
        const auto sigma = tools::esn0_to_sigma(esn0   );

        noise.set_noise(sigma, ebn0, esn0);

        // update the sigma of the modem and the channel
        m_codec  ->set_noise(noise);
        m_modem  ->set_noise(noise);
        m_channel->set_noise(noise);
    }
}


void Model::resetMonitor() 
{
    if (m_bInitialized)
        m_monitor->reset();
}


void Model::iterate(void)
{
    using namespace module;
     
    // get the r_encoder and r_decoder modules from the codec module
    auto& r_encoder = m_codec->get_encoder();
    auto& r_decoder = m_codec->get_decoder_siho();    
    
    (*m_source )[src::tsk::generate    ].exec();
    (*r_encoder)[enc::tsk::encode      ].exec();
    (*m_modem  )[mdm::tsk::modulate    ].exec();
    (*m_channel)[chn::tsk::add_noise   ].exec();
    (*m_modem  )[mdm::tsk::demodulate  ].exec();
    (*r_decoder)[dec::tsk::decode_siho ].exec();
    (*m_monitor)[mnt::tsk::check_errors].exec();
   
}
