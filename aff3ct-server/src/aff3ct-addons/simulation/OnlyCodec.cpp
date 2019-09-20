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
CodecType OnlyCodec<B,R,Q>
::getCodecType(factory::Codec::parameters *param)
{
    auto param_siso_siho = dynamic_cast<factory::Codec_SISO_SIHO::parameters*>(param);    
    if (param_siso_siho != nullptr)
        return Type_SISO_SIHO;
    
    auto param_siso = dynamic_cast<factory::Codec_SISO::parameters*>(param);    
    if (param_siso != nullptr)
        return Type_SISO;
    
    auto param_siho = dynamic_cast<factory::Codec_SIHO::parameters*>(param);    
    if (param_siho != nullptr)
        return Type_SIHO;
    
    auto param_hiho = dynamic_cast<factory::Codec_HIHO::parameters*>(param);    
    if (param_siso != nullptr)
       return Type_HIHO;
    
    return Type_Unknown;
}

template <typename B, typename R, typename Q>
void OnlyCodec<B,R,Q>
::detectCodecType() {
    codecType  = getCodecType(params_OnlyCodec.cdc.get());   
}

template <typename B, typename R, typename Q>
void OnlyCodec<B,R,Q>
::printCodecType(CodecType type, std::ostream &stream) 
{
   stream << "Codec_SISO_SIHO ";
    if (type == Type_SISO_SIHO) 
       stream << "[*]" << std::endl; 
    else
       stream << "[ ]" << std::endl; 
    
   
   stream << "Codec_SISO      ";
     if (type == Type_SISO) 
       stream << "[*]" << std::endl; 
    else
       stream << "[ ]" << std::endl; 
    
   stream << "Codec_SIHO      ";
    if (type == Type_SIHO) 
       stream << "[*]" << std::endl; 
    else
       stream << "[ ]" << std::endl; 
    
   stream << "Codec_HIHO      ";
    if (type == Type_HIHO) 
       stream << "[*]" << std::endl; 
    else
       stream << "[ ]" << std::endl; 
}

template <typename B, typename R, typename Q>
void OnlyCodec<B,R,Q>
::initialize()
{
    PRINT_POINT();
    
    getCodecType(params_OnlyCodec.cdc.get());
    printCodecType(codecType, std::cout);
            
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
    
    switch(codecType) {
        case Type_SISO_SIHO: {
            auto param_siso_siho = dynamic_cast<factory::Codec_SISO_SIHO::parameters*>(params_OnlyCodec.cdc.get());    
            codec = std::unique_ptr<module::Codec<B,Q>>(param_siso_siho->template build<B,Q>(nullptr));
            break;
        }
            
        case Type_SISO: {
            auto param_siso = dynamic_cast<factory::Codec_SISO::parameters*>(params_OnlyCodec.cdc.get());    
            codec = std::unique_ptr<module::Codec<B,Q>>(param_siso->template build<B,Q>(nullptr));
            break;
        }
            
        case Type_SIHO: {
            auto param_siho = dynamic_cast<factory::Codec_SIHO::parameters*>(params_OnlyCodec.cdc.get());    
            codec = std::unique_ptr<module::Codec<B,Q>>(param_siho->template build<B,Q>(nullptr));
            break;
        }
        
        case Type_HIHO: {
            auto param_hiho = dynamic_cast<factory::Codec_HIHO::parameters*>(params_OnlyCodec.cdc.get());    
            codec = std::unique_ptr<module::Codec<B,Q>>(param_hiho->template build<B,Q>(nullptr));
            break;
        }
        
        default: {
            std::cout << "codec is not initialized! " << std::endl;
        }
    }
    
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
