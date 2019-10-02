/*
 * MIT License
 * 
 * Copyright (c) 2017 aff3ct
 * Copyright (c) 2018 Sergei Semenov
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

/* 
 * File:   Codec.cpp
 * Author: simon
 * 
 * Created on September 13, 2019, 1:52 PM
 */

#include "Codec.hpp"

#include <Tools/Display/rang_format/rang_format.h>
using namespace aff3ct;
using namespace aff3ct::launcher;

//FIXME:: CopyAndPaste

Codec::Codec(const int argc, const char **argv,
        factory::OnlyCodec::parameters &params,
        std::ostream &stream) :
ah(argc, argv), params_common(params), stream(stream) 
{
    PRINT_POINT();
    
    std::string cmd_line = "";
    cmd_line += std::string(argv[0]) + std::string(" ");
    for (auto i = 1; i < argc; i++) {
        if (argv[i][0] == '-')
            cmd_line += std::string(argv[i]);
        else
            cmd_line += std::string("\"") + std::string(argv[i]) + std::string("\"");

        cmd_line += std::string(" ");
    }
    std::cout << "launcher::Codec::Codec() cmd_line " << cmd_line << std::endl;
    
    //we need to fill params_common based on argc, argv

}

void Codec::get_description_args() {
    PRINT_POINT();
}

void Codec::store_args() {
    PRINT_POINT();
}

int Codec::read_arguments() {
    PRINT_POINT();

    this->get_description_args();

    std::vector<std::string> cmd_error;

    this->arg_vals = ah.parse_arguments(this->args, this->cmd_warn, cmd_error);

    try {
        this->store_args();
        ah.set_help_display_keys(params_common.display_keys);
    } catch (const std::exception& e) {
        auto save = tools::exception::no_backtrace;
        tools::exception::no_backtrace = true;
        cmd_error.emplace_back(e.what());
        tools::exception::no_backtrace = save;
    }

#ifdef AFF3CT_MPI
    if (this->params_common.mpi_rank == 0) {
#endif
        if (params_common.display_help) {
            auto grps = factory::Factory::create_groups({&params_common});
            ah.print_help(this->args, grps, params_common.display_adv_help);
        }

        // print usage
        if (!cmd_error.empty() && !params_common.display_help)
            ah.print_usage(this->args);

        // print the errors
        if (!cmd_error.empty()) std::cerr << std::endl;
        for (unsigned e = 0; e < cmd_error.size(); e++)
            std::cerr << rang::tag::error << cmd_error[e] << std::endl;

        // print the help tags
        if (!cmd_error.empty() && !params_common.display_help) {
            tools::Argument_tag help_tag = {"help", "h"};

            std::string message = "For more information please display the help (\"";
            message += tools::Argument_handler::print_tag(help_tag) += "\").";

            std::cerr << std::endl << rang::tag::info << message << std::endl;
        }
#ifdef AFF3CT_MPI
    }
#endif

    return (!cmd_error.empty() || params_common.display_help) ? EXIT_FAILURE : EXIT_SUCCESS;
}


