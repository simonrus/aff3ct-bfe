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
 * File:   Codec_Generic.cpp
 * Author: simon
 * 
 * Created on September 2, 2019, 10:10 PM
 */

#include "Codec_Generic.hpp"
#include "aff3ct-addons/simulation/Codec_Generic.hpp"


using namespace aff3ct;
using namespace aff3ct::launcher;

template <typename B, typename R, typename Q>
Codec_Generic<B,R,Q>
::Codec_Generic(const int argc, const char **argv, std::ostream &stream)
: Codec(argc, argv, params, stream)
{
    PRINT_POINT();
    params.set_src(new factory::Source      ::parameters("src"));
}


template <typename B, typename R, typename Q>
void Codec_Generic<B,R,Q>
::get_description_args()
{
    PRINT_POINT();
    
    Codec::get_description_args();
    
    params.     get_description(this->args);
    params.src->get_description(this->args);
    
    //TODO: CHECK /home/simon/work/phd/missfec/lib/aff3ct/src/Launcher/Simulation/BFER_std.cpp
    
    
    //params.     get_description(this->args);
    
    
    /*
    auto psrc = params.src     ->get_prefix();
    auto pcrc = params.crc     ->get_prefix();
    */
    
    /*
    if (this->args.exist({penc+"-info-bits", "K"}) || this->args.exist({ppct+"-info-bits", "K"}))
		this->args.erase({psrc+"-info-bits", "K"});
     */
    
    std::string penc_prefix = params.cdc->enc->get_prefix();
    std::cout << "penc_prefix " << penc_prefix << std::endl;
    
}

template <typename B, typename R, typename Q>
void Codec_Generic<B,R,Q>
::store_args()
{
    PRINT_POINT();
    
    std::cout << "launcher::Codec_Generic::store_args() " << std::endl;
    Codec::store_args();
    
    //TODO: CHECK /home/simon/work/phd/missfec/lib/aff3ct/src/Launcher/Simulation/BFER_std.cpp
    /*
    params.store(this->arg_vals);

    params.src->seed = params.local_seed;

    params.src->store(this->arg_vals);

    auto psrc = params.src->get_prefix();
     */
}

template <typename B, typename R, typename Q>
simulation::Codec* Codec_Generic<B,R,Q>
::build_simu()
{
    PRINT_POINT();
    
    if (this->read_arguments() == EXIT_FAILURE) {
        std::cout << "build_simu failed " << std::endl;
        return nullptr;
    }
        
    std::vector<aff3ct::factory::Factory::parameters*> paramsPtrs;
    paramsPtrs.push_back(&params);
    aff3ct::factory::Header::print_parameters(paramsPtrs, true, std::cout);
    
    return new simulation::Codec_Generic<B,R,Q>(params);
    
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef AFF3CT_MULTI_PREC
template class aff3ct::launcher::Codec_Generic<B_8,R_8,Q_8>;
template class aff3ct::launcher::Codec_Generic<B_32,R_32,Q_32>;
#else
template class aff3ct::launcher::Codec_Generic<B,R,Q>;
#error "Not implemented"
#endif