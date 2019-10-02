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

#include "Facade_Codec.h"
#include <aff3ct-addons/launcher/Codec.hpp>

#include <Tools/Algo/Draw_generator/Gaussian_noise_generator/Gaussian_noise_generator.hpp>

std::error_code Facade_Codec::constructAll()
{
    std::error_code ec = make_error_code(Aff3ctErrc::NoError);
    
    
    return ec;
}

void Facade_Codec::setDebugPrint(bool bEnabled) 
{
    m_bDebugPrint = bEnabled;
     
    if (m_bInitialized) 
    {
        using namespace module;
    
       
    }
}

bool Facade_Codec::reset()
{
    
   // std::unique_ptr<factory::OnlyCodec::parameters>                params(new factory::OnlyCodec::parameters());
    
    //p_params.swap(params);
    return true;
}

bool Facade_Codec::read_arguments(const int argc, const char** argv, factory::OnlyCodec::parameters &params)
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
bool Facade_Codec::init(std::list<std::string> &arg_vec, std::error_code &ec, std::ostream& err_stream)
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
    

    std::unique_ptr<launcher::Codec> codecLauncher;
        
#ifdef AFF3CT_MULTI_PREC
        std::cout << "sim_prec " << m_params.sim_prec << std::endl;
        switch (m_params.sim_prec) {
            
            //case 8: m_codecLauncher =  std::unique_ptr<simulation::Codec> (factory::OnlyCodec::build<B_8, R_8, Q_8 >(m_params, argv.size(), (const char**)&argv[0], std::cout));    break;
            //case 16: launcher = factory::OnlyCodec::build<B_16, R_16, Q_16>(m_params); break;
            case 32: 
                launcher::Codec *lau_codec = factory::OnlyCodec::build<B_32, R_32, Q_32 >(m_params, argv.size(), (const char**)&argv[0], std::cout);
                codecLauncher =  std::unique_ptr<launcher::Codec> (lau_codec);    
               
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
        
    simulation::Codec* codec = codecLauncher->build_simu();
    
    m_codec =  std::unique_ptr<simulation::Codec> (codec);
    
    if (!m_codec) {
        std::cout << "Codec is not initialized" << std::endl;
        return false;
    }
    
    ec = constructAll();
    if (ec)
        return false; 
    
    m_codec->initialize();
    
    m_codec->printCodecInfo(std::cout);
    
    int N = 7;
    int n_frames = 3;
    
    /* ENCODE TEST STARTS */
    std::vector<int32_t> in = {1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1}; //12
    std::vector<int32_t> codeword(n_frames * N);
    std::vector<int32_t> non_codeword(n_frames * N);
    
    m_codec->encode(&in[0], &codeword[0], n_frames);  
    /* ENCODE TEST ENDS */
    
    /* IS codeword test*/
    std::cout << "is_codeword test: ";
    std::cout << ((m_codec->is_codeword(&codeword[0]) == true)?"PASSED":"FAILED");
    std::cout << std::endl;
    
    std::cout << "is_codeword test(not): ";
    non_codeword = codeword;
    
    non_codeword[1] = codeword[1]?0:1; //revert one bit
    std::cout << ((m_codec->is_codeword(&non_codeword[0]) == false)?"PASSED":"FAILED");
    std::cout << std::endl;
    
    
    /* DECODE TEST*/
    
    //template <typename R>R aff3ct::tools::ebn0_to_esn0(const R ebn0, const R bit_rate, const int bps)
    //template <typename R>R aff3ct::tools::esn0_to_sigma(const R esn0, const int upsample_factor)
    
    
    float ebn0 = 0;
    float esn0 = ebn0 + 10.0f * std::log10(4.0f/7.0f );
    float sigma = std::sqrt(1 / (2.0f * std::pow(10,(esn0 / 10.0f))));
    //sigma = 0.2;
    
    
    std::unique_ptr<tools::Gaussian_gen<float>> noise_generator = std::unique_ptr<tools::Gaussian_gen<float>> (new tools::Gaussian_gen_fast<float>());
    std::vector<float> noise(n_frames * N);
    std::cout << "Sigma is " << sigma << std::endl;
    noise_generator->generate(noise, sigma);
    
    
    std::vector<float> signal(codeword.size());
    
    for (int i = 0; i < codeword.size(); i++) {
        signal[i] = 1.0 - 2.0*codeword[i];      //BPSK modulation
    }
    
    /* SIHO case */
    std::vector<float> recieved_llr(n_frames * N);
    
    for (int i = 0; i < signal.size(); i++) {
        signal[i] = signal[i] + noise[i];
        recieved_llr[i] = 2 * signal[i] / sigma;
    } 
    
    std::vector<int32_t> decoded(in.size());

    std::cout << "** SIHO case ***************************************" <<  std::endl;
    /* SIHO case */
    try {
        m_codec->decodeSIHO(&recieved_llr[0], &decoded[0], n_frames);
    } catch (const std::exception& e) {
        std::cout << "SIHO decoder failed: " << e.what() << std::endl;
    }

    std::cout << "** HIHO case ***************************************" <<  std::endl;
    /* HIHO case */
    try {
        m_codec->decodeHIHO(&non_codeword[0], &decoded[0], n_frames);
    } catch (const std::exception& e) {
        std::cout << "HIHO decoder failed: " << e.what() << std::endl;
    }

    std::cout << "** SISO case ***************************************" <<  std::endl;
    /* SISO case */
    std::vector<float> decoded_llr(recieved_llr.size());
    try {
        m_codec->decodeSISO(&recieved_llr[0], &decoded_llr[0], n_frames);
    } catch (const std::exception& e) {
        std::cout << "SISO decoder failed: " << e.what() << std::endl;
    }
    std::cout << "** DONE ********************************************" <<  std::endl;
    
    
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

void Facade_Codec::setNoise(float ebn0) 
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


void Facade_Codec::resetMonitor() 
{
//    if (m_bInitialized)
//        m_monitor->reset();
}

