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

#include "Factory_Codec.hpp"
#include <aff3ct-addons/launcher/Codec.hpp>

#include <Tools/Algo/Draw_generator/Gaussian_noise_generator/Gaussian_noise_generator.hpp>

bool Factory_Codec::read_arguments(const int argc, const char** argv, factory::Codec_Generic::parameters &params)
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
std::unique_ptr<simulation::Codec> 
Factory_Codec::create(std::list<std::string> &arg_vec, std::error_code &ec, std::ostream& err_stream)
{
    std::unique_ptr<simulation::Codec> newCodec;
    
    PRINT_POINT();
    
    factory::Codec_Generic::parameters              m_params;
            
    std::vector<const char *> argv;

    argv.reserve(arg_vec.size());
    for (std::string &arg : arg_vec) 
    {   
        argv.push_back(arg.c_str());
    }

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
            
            //case 8: newCodecLauncher =  std::unique_ptr<simulation::Codec> (factory::Codec_Generic::build<B_8, R_8, Q_8 >(m_params, argv.size(), (const char**)&argv[0], std::cout));    break;
            //case 16: launcher = factory::Codec_Generic::build<B_16, R_16, Q_16>(m_params); break;
            case 32: 
                launcher::Codec *lau_codec = factory::Codec_Generic::build<B_32, R_32, Q_32 >(m_params, argv.size(), (const char**)&argv[0], std::cout);
                codecLauncher =  std::unique_ptr<launcher::Codec> (lau_codec);    
               
                break;
            //case 64: launcher = factory::Codec_Generic::build<B_64, R_64, Q_64>(m_params); break;
            //default: launcher = nullptr;
                break;
        }
#else
#error Not implemented
#endif
   
        
    if (!codecLauncher) {
        std::cout << "CodecLauncher is not initialized" << std::endl;
        return false;
    }
        
    simulation::Codec* codec = codecLauncher->build_simu();
    
    newCodec =  std::unique_ptr<simulation::Codec> (codec);
    
    if (!newCodec) {
        std::cout << "Codec is not initialized" << std::endl;
        return false;
    }
    
    newCodec->initialize();
    
    newCodec->printCodecInfo(std::cout);
    
    int N = 7;
    int n_frames = 3;
    
    /* ENCODE TEST STARTS */
    std::vector<int32_t> in = {1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1}; //12
    std::vector<int32_t> codeword(n_frames * N);
    std::vector<int32_t> non_codeword(n_frames * N);
    
    newCodec->encode(&in[0], &codeword[0], n_frames);  
    /* ENCODE TEST ENDS */
    
    /* IS codeword test*/
    std::cout << "is_codeword test: ";
    std::cout << ((newCodec->is_codeword(&codeword[0]) == true)?"PASSED":"FAILED");
    std::cout << std::endl;
    
    std::cout << "is_codeword test(not): ";
    non_codeword = codeword;
    
    non_codeword[1] = codeword[1]?0:1; //revert one bit
    std::cout << ((newCodec->is_codeword(&non_codeword[0]) == false)?"PASSED":"FAILED");
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
        newCodec->decodeSIHO(&recieved_llr[0], &decoded[0], n_frames);
    } catch (const std::exception& e) {
        std::cout << "SIHO decoder failed: " << e.what() << std::endl;
    }

    std::cout << "** HIHO case ***************************************" <<  std::endl;
    /* HIHO case */
    try {
        newCodec->decodeHIHO(&non_codeword[0], &decoded[0], n_frames);
    } catch (const std::exception& e) {
        std::cout << "HIHO decoder failed: " << e.what() << std::endl;
    }

    std::cout << "** SISO case ***************************************" <<  std::endl;
    /* SISO case */
    std::vector<float> decoded_llr(recieved_llr.size());
    try {
        newCodec->decodeSISO(&recieved_llr[0], &decoded_llr[0], n_frames);
    } catch (const std::exception& e) {
        std::cout << "SISO decoder failed: " << e.what() << std::endl;
    }
    std::cout << "** DONE ********************************************" <<  std::endl;
    
    
    /* DECODE TEST ENDS */
  


    return std::move(newCodec);
}
