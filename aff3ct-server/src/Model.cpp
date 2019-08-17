/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Model.cpp
 * Author: simon
 * 
 * Created on July 24, 2019, 9:11 PM
 */

#include "Model.h"

std::error_code Model::constructAll()
{
    std::error_code ec = make_error_code(Aff3ctErrc::NoError);
    m_source  = std::unique_ptr<module::Source          <B_TYPE>>                   (p_src->build());  
    
    m_codec   =  std::unique_ptr<module::Codec_repetition <B_TYPE, Q_TYPE>>         (p_cdc->build());  

    m_modem   = std::unique_ptr<module::Modem           <B_TYPE, R_TYPE, Q_TYPE>>   (p_mdm->build()); 
    m_channel = std::unique_ptr<module::Channel         <R_TYPE>>                   (p_chn->build()); 
    m_monitor = std::unique_ptr<module::Monitor_BFER    <B_TYPE>>                   (p_mnt->build()); 
    
    return ec;
}

bool Model::reset()
{
    std::unique_ptr<factory::Source          ::parameters>   src(new factory::Source::parameters());
    std::unique_ptr<factory::Codec_repetition::parameters>   cdc(new factory::Codec_repetition::parameters());
    std::unique_ptr<factory::Modem           ::parameters>   mdm(new factory::Modem           ::parameters());
    std::unique_ptr<factory::Channel         ::parameters>   chn(new factory::Channel         ::parameters());
    std::unique_ptr<factory::Monitor_BFER    ::parameters>   mnt(new factory::Monitor_BFER    ::parameters());
    std::unique_ptr<factory::Terminal        ::parameters>   ter(new factory::Terminal        ::parameters());
    
    p_src.swap(src);
    p_cdc.swap(cdc);
    p_mdm.swap(mdm);
    p_chn.swap(chn);
    p_mnt.swap(mnt);
    p_ter.swap(ter);
}

void Model::printParameters(std::vector<factory::Factory::parameters*> &paramsList)
{
    for (auto const &ptr : paramsList)
    {
        aff3ct::tools::Argument_map_info args;
        ptr->get_description(args);
        
        for (auto const& it : args)
        {
            
            for (auto const &param: it.first)
            {
                std::cout << "[" << param << "]";
            }
            std::cout << ":";
            std::cout << " doc=" << it.second->doc;
            std::cout << " key=" << it.second->key;
            std::cout << std::endl;
        }
        
        
    }
}

/*
 * \ref https://github.com/aff3ct/my_project_with_aff3ct/blob/master/examples/factory/src/main.cpp
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
                        p_ter.get()};
    factory::Command_parser cp(argv.size(), (char**)&argv[0], m_paramsList, true, err_stream);
    
    //printParameters(m_paramsList);
        
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
    
    iterate();
}



void Model::setNoise(float ebn0) 
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

void Model::iterate()
{
    using namespace module;
    setNoise(1.0); //
 
    TRACELOG(INFO,"K=%d, N=%d",p_cdc->enc->K, p_cdc->enc->N_cw);
    
    // get the r_encoder and r_decoder modules from the codec module
    auto& r_encoder = m_codec->get_encoder();
    auto& r_decoder = m_codec->get_decoder_siho();
    
    (*m_source )[src::tsk::generate    ].set_debug(true);
    (*r_encoder)[enc::tsk::encode      ].set_debug(true);
    (*m_modem  )[mdm::tsk::modulate    ].set_debug(true);
    (*m_channel)[chn::tsk::add_noise   ].set_debug(true);
    (*m_modem  )[mdm::tsk::demodulate  ].set_debug(true);
    (*r_decoder)[dec::tsk::decode_siho ].set_debug(true);
    (*m_monitor)[mnt::tsk::check_errors].set_debug(true);
    
    
    (*m_source )[src::tsk::generate    ].exec();
    (*r_encoder)[enc::tsk::encode      ].exec();
    (*m_modem  )[mdm::tsk::modulate    ].exec();
    (*m_channel)[chn::tsk::add_noise   ].exec();
    (*m_modem  )[mdm::tsk::demodulate  ].exec();
    (*r_decoder)[dec::tsk::decode_siho ].exec();
    (*m_monitor)[mnt::tsk::check_errors].exec();
    
    //u.terminal->final_report();
    
    // reset the monitor and the terminal for the next SNR
    m_monitor->reset();
    //u.terminal->reset();
    
    //Print sockets
    Task& task = (*m_source)[src::tsk::generate    ];
    for (auto it = task.sockets.begin(); it != task.sockets.end(); ++it)
    {
        std::shared_ptr<Socket> sock = *it;
        TRACELOG(INFO, "Socket: %s found ", sock->get_name().c_str());
    }
}
/*
void Model::process()
{
    // get the r_encoder and r_decoder modules from the codec module
    auto& r_encoder = m_codec->get_encoder();
    auto& r_decoder = m_codec->get_decoder_siho();

    using namespace module;
    (*m_source )[src::tsk::generate    ].exec();
    (*r_encoder)[enc::tsk::encode      ].exec();
    (*m_modem  )[mdm::tsk::modulate    ].exec();
    (*m_channel)[chn::tsk::add_noise   ].exec();
    (*m_modem  )[mdm::tsk::demodulate  ].exec();
    (*r_decoder)[dec::tsk::decode_siho ].exec();
    (*m_monitor)[mnt::tsk::check_errors].exec();
}
 * */





