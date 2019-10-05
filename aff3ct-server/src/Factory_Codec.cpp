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

bool Factory_Codec::read_arguments(const int argc, const char** argv, factory::Codec_Generic::parameters *params)
{
    PRINT_POINT();
    tools::Argument_handler ah(argc, argv);

    tools::Argument_map_info args;
    tools::Argument_map_group arg_group;

    std::vector<std::string> cmd_warn, cmd_error;

    params->get_description(args);

    auto arg_vals = ah.parse_arguments(args, cmd_warn, cmd_error);

    bool display_help = true;
    try {
        params->store(arg_vals);
        ah.set_help_display_keys(params->display_keys);
    } catch (std::exception&) {
        display_help = true;
    }

    //if (params->display_version)
    //    print_version();

    if (cmd_error.size() || display_help) {
        arg_group["sim"] = "Simulation parameter(s)";
        ah.print_help(args, arg_group, params->display_adv_help);

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
    std::unique_ptr<factory::Codec_Generic::parameters> m_params = 
            std::unique_ptr<factory::Codec_Generic::parameters>(new factory::Codec_Generic::parameters());                        
    
    //factory::Codec_Generic::parameters m_params;
    
    PRINT_POINT();
            
    std::vector<const char *> argv;

    argv.reserve(arg_vec.size());
    for (std::string &arg : arg_vec) 
    {   
        argv.push_back(arg.c_str());
    }

    if (read_arguments(argv.size(), (const char**)&argv[0], m_params.get()) == EXIT_FAILURE) 
    {
        err_stream << "read_arguments failed" << std::endl;
        return newCodec;    
    }
    
    std::unique_ptr<launcher::Codec> codecLauncher;
        
#ifdef AFF3CT_MULTI_PREC
        switch (m_params->sim_prec) {
            //case 8: newCodecLauncher =  std::unique_ptr<simulation::Codec> (factory::Codec_Generic::build<B_8, R_8, Q_8 >(m_params, argv.size(), (const char**)&argv[0], std::cout));    break;
            //case 16: launcher = factory::Codec_Generic::build<B_16, R_16, Q_16>(m_params); break;
            case 32:                
                codecLauncher =  std::unique_ptr<launcher::Codec> (factory::Codec_Generic::build<B_32, R_32, Q_32 >(*m_params, argv.size(), (const char**)&argv[0], std::cout));    
                break;
            //case 64: launcher = factory::Codec_Generic::build<B_64, R_64, Q_64>(m_params); break;
            //default: launcher = nullptr;
                break;
        }
#else
#error Not implemented
#endif
   
        
    if (!codecLauncher) {
        err_stream << "CodecLauncher is not initialized" << std::endl;
        return newCodec;
    }
    
    newCodec =  std::unique_ptr<simulation::Codec> (codecLauncher->build_simu());
    
    if (!newCodec) {
        err_stream << "Codec is not initialized" << std::endl;
        return newCodec;
    }
    
    newCodec->creator = std::move(codecLauncher);
    
    newCodec->initialize();
    
    return newCodec;
}
