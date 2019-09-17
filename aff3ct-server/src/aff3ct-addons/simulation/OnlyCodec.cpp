/*
 * 
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
 * File:   OnlyCodec.cpp
 * Author: simon
 * 
 * Created on September 12, 2019, 3:06 PM
 */

#include "OnlyCodec.hpp"

using namespace aff3ct;
using namespace aff3ct::simulation;

template <typename B, typename R, typename Q>
OnlyCodec<B,R,Q>
::OnlyCodec(const factory::OnlyCodec::parameters& params_OnlyCodec)
:params_OnlyCodec(params_OnlyCodec)
{
    PRINT_POINT();
    rd_engine_seed.seed(params_OnlyCodec.local_seed);
}


template <typename B, typename R, typename Q>
void OnlyCodec<B,R,Q>
::initialize()
{
    PRINT_POINT();
    
    const auto seed_enc = rd_engine_seed();
    const auto seed_dec = rd_engine_seed();

    std::unique_ptr<factory::Codec::parameters> params_cdc(params_OnlyCodec.cdc->clone());
    if (params_cdc) {
        std::cout << "params_cdc is OK " << std::endl;
                
        std::vector<aff3ct::factory::Factory::parameters*> paramsPtrs;
        paramsPtrs.push_back(params_cdc.get());
        aff3ct::factory::Header::print_parameters(paramsPtrs, true, std::cout);
    }
    else
        std::cout << "params_cdc are failed " << std::endl;
    
    params_cdc->enc->seed = seed_enc;
    params_cdc->dec->seed = seed_dec;

    auto param_siso_siho = dynamic_cast<factory::Codec_SISO_SIHO::parameters*>(params_cdc.get());
    
    if (param_siso_siho == nullptr)
        std::cout << "param_siso_siho is null!!!" << std::endl; 
    
    codec = std::unique_ptr<module::Codec_SISO_SIHO<B,Q>>(param_siso_siho->template build<B,Q>(nullptr));
    
    if (codec)
        std::cout << "codec is OK " << std::endl;
    else
        std::cout << "codec is NULL " << std::endl;
}

template <typename B, typename R, typename Q>
void OnlyCodec<B,R,Q>
::iterate(void *in, void *out)
{
    PRINT_POINT();
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef AFF3CT_MULTI_PREC
template class aff3ct::simulation::OnlyCodec<B_8,R_8,Q_8>;
template class aff3ct::simulation::OnlyCodec<B_32,R_32,Q_32>;
#else
#error "Not yet implemented"
#endif
// ==================================================================================== explicit template instantiation