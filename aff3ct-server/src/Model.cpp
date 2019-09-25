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
#include <aff3ct-addons/launcher/CodecRun.hpp>

std::error_code Model::constructAll()
{
    std::error_code ec = make_error_code(Aff3ctErrc::NoError);
    
    
    return ec;
}

void Model::setDebugPrint(bool bEnabled) 
{
    m_bDebugPrint = bEnabled;
     
    if (m_bInitialized) 
    {
        using namespace module;
    
       
    }
}

bool Model::reset()
{
    
   // std::unique_ptr<factory::OnlyCodec::parameters>                params(new factory::OnlyCodec::parameters());
    
    //p_params.swap(params);
    return true;
}

bool Model::read_arguments(const int argc, const char** argv, factory::OnlyCodec::parameters &params)
{
    PRINT_POINT();
    tools::Argument_handler ah(argc, argv);

    tools::Argument_map_info args;
    tools::Argument_map_group arg_group;

    std::vector<std::string> cmd_warn, cmd_error;

    params.get_description(args);

    auto arg_vals = ah.parse_arguments(args, cmd_warn, cmd_error);

    bool display_help = true;
    try {
        params.store(arg_vals);
        ah.set_help_display_keys(params.display_keys);
    } catch (std::exception&) {
        display_help = true;
    }

    //if (params.display_version)
    //    print_version();

    if (cmd_error.size() || display_help) {
        arg_group["sim"] = "Simulation parameter(s)";
        ah.print_help(args, arg_group, params.display_adv_help);

        if (cmd_error.size()) std::cerr << std::endl;
        for (auto w = 0; w < (int) cmd_error.size(); w++)
            std::cerr << rang::tag::error << cmd_error[w] << std::endl;

        if (cmd_warn.size()) std::cerr << std::endl;
        for (auto w = 0; w < (int) cmd_warn.size(); w++)
            std::cerr << rang::tag::warning << cmd_warn[w] << std::endl;
    }
    return (cmd_error.size()) ? EXIT_FAILURE : EXIT_SUCCESS;
}
/*
 * \ref https://github.com/aff3ct/my_project_with_aff3ct/blob/master/examples/factory/src/main.cpp
 * \param arg_vec - vector arguments where arg_vec[0] shall be interpreted as a program name
 */
bool Model::init(std::list<std::string> &arg_vec, std::error_code &ec, std::ostream& err_stream)
{
    PRINT_POINT();
    
    int exit_code;
    
    std::vector<const char *> argv;

    argv.reserve(arg_vec.size());
    for (std::string &arg : arg_vec) 
    {   
        argv.push_back(arg.c_str());
    }

    reset();

    
    //m_paramsList =  {p_params.get()};

    std::cout << "********** 1st args parsing *********** " << std::endl;
    //factory::Command_parser cp(argv.size(), (char**)&argv[0], m_paramsList, true, err_stream);
    if (read_arguments(argv.size(), (const char**)&argv[0], m_params) == EXIT_FAILURE) 
    {
        std::cout << "read_arguments failed" << std::endl;
        return false;
    
    }
    
    std::cout << "read_arguments finished" << std::endl;
    

    std::unique_ptr<launcher::CodecRun> codecLauncher;
        
#ifdef AFF3CT_MULTI_PREC
        std::cout << "sim_prec " << m_params.sim_prec << std::endl;
        switch (m_params.sim_prec) {
            
            //case 8: m_codecLauncher =  std::unique_ptr<simulation::CodecRun> (factory::OnlyCodec::build<B_8, R_8, Q_8 >(m_params, argv.size(), (const char**)&argv[0], std::cout));    break;
            //case 16: launcher = factory::OnlyCodec::build<B_16, R_16, Q_16>(m_params); break;
            case 32: 
                launcher::CodecRun *codecrun = factory::OnlyCodec::build<B_32, R_32, Q_32 >(m_params, argv.size(), (const char**)&argv[0], std::cout);
                codecLauncher =  std::unique_ptr<launcher::CodecRun> (codecrun);    
               
                break;
            //case 64: launcher = factory::OnlyCodec::build<B_64, R_64, Q_64>(m_params); break;
            //default: launcher = nullptr;
                break;
        }
#else
#error Not implemented
#endif
        
//    if (cp.help_required())
//    {
//        cp.print_help    ();
//        return false;
//    }
//    
    // parse the command for the given parameters and fill them
//    if (cp.parsing_failed())
//    {
//        cp.print_warnings();
//        cp.print_errors  ();
//    
//        //TRACELOG(ERROR,"cp.parsing_failed");
//        //FIXME: Redirect std::cout to log!
//        return false;
//    }
//       
//    // display the headers (= print the AFF3CT parameters on the screen)
//    factory::Header::print_parameters(m_paramsList); 
//    cp.print_warnings();
        
    if (!codecLauncher) {
        std::cout << "CodecLauncher is not initialized" << std::endl;
        return false;
    }
        
    simulation::CodecRun* codec = codecLauncher->build_simu();
    
    m_codec =  std::unique_ptr<simulation::CodecRun> (codec);
    
    if (!m_codec) {
        std::cout << "Codec is not initialized" << std::endl;
        return false;
    }
    
    ec = constructAll();
    if (ec)
        return false; 
    
    m_codec->initialize();
    
    m_codec->printCodecInfo(std::cout);
    
    /* ENCODE TEST STARTS */
    std::vector<int> in = {1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1}; //12
    std::vector<int> out(24);
    
    m_codec->encode(&in[0], &out[0], 3);
    
    std::cout << " input: ";
    for(int &value: in) 
        std::cout << value << " ";
    std::cout << std::endl;

    std::cout << "output: ";
    for(int &value: out) 
        std::cout << value << " ";
    std::cout << std::endl;
    
    
    /* ENCODE TEST ENDS */
    
    
    
    /* DECODE TEST STARTS */
    /* DECODE TEST ENDS */
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


    //use default parameters
    /* DEBUG MODE FOR LAUNCHER ?
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
     * */
    
    
    m_bInitialized = true;
    
    //postponed initialization
    setDebugPrint(m_bDebugPrint);
    setNoise(m_fNoise);

    return true;
}

void Model::setNoise(float ebn0) 
{
    this->m_fNoise = ebn0;
    if (m_bInitialized) 
    {
        tools::Sigma<> noise;

        //const float R = (float) p_cdc->enc->K / (float)p_cdc->enc->N_cw;


        // compute the current sigma for the channel noise
        //        const auto esn0  = tools::ebn0_to_esn0 (ebn0, R);
        //        const auto sigma = tools::esn0_to_sigma(esn0   );
        //
        //        noise.set_noise(sigma, ebn0, esn0);

        // update the sigma of the modem and the channel
        //        m_codecLauncher  ->set_noise(noise);
        //        m_modem  ->set_noise(noise);
        //        m_channel->set_noise(noise);
    }
}


void Model::resetMonitor() 
{
//    if (m_bInitialized)
//        m_monitor->reset();
}


void Model::iterate(void)
{
    using namespace module;
     
    std::cout << "Model::iterate()" << std::endl;
    try {           
        if (m_codec)
        {
            //m_codec->iterate(nullptr, nullptr);
        }
    } catch (std::exception const& e) {
        std::cout << "Exception:" << e.what() << std::endl;
    }
   
}
