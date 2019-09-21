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
#include <Tools/general_utils.h>

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
    
    detectCodecType();
    printCodecType(codecType, std::cout);
            
    const auto seed_enc = rd_engine_seed();
    const auto seed_dec = rd_engine_seed();

    std::unique_ptr<factory::Codec::parameters> params_cdc(params_OnlyCodec.cdc->clone());
    if (params_cdc) {
        std::cout << "params_cdc is OK " << std::endl;
    }
    else
        std::cout << "params_cdc are failed " << std::endl;
    
    params_cdc->enc->seed = seed_enc; 
    params_cdc->dec->seed = seed_dec;
    
#warning "FIXME!!! We do here some COPY! WTF!"

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
    
    m_bInitialized = true;
}


template <typename B, typename R, typename Q>
void OnlyCodec<B,R,Q>
::sockets_binding()
{	
    std::vector<const module::Module*> m_modules;
    using namespace module;
    
    // get the r_encoder and r_decoder modules from the codec module
    auto& r_encoder = codec->get_encoder();
//    auto& r_decoder = codec->get_decoder_siho();
    
    m_modules = {   /*m_source.get(), */
                    r_encoder.get(), 
                    /*m_modem.get(), 
                    m_channel.get(), */
//                    r_decoder.get(), 
                    /*m_monitor.get() */};
        

    switch (codecType) {
        case Type_SISO_SIHO: {
            module::Codec_SISO_SIHO<B,Q> *codecPtr;
            codecPtr =  dynamic_cast<module::Codec_SISO_SIHO<B,Q> *>(codec.get());
            auto& r_decoder = codecPtr->get_decoder_siho();
            m_modules.push_back(r_decoder.get());
            break;
        }

        //FIXME!!! get_decoder_siho() or get_decoder_siso()
        
        case Type_SISO: {
            module::Codec_SISO<B,Q> *codecPtr;
            codecPtr =  dynamic_cast<module::Codec_SISO<B,Q> *>(codec.get());
            auto& r_decoder = codecPtr->get_decoder_siso();
            m_modules.push_back(r_decoder.get());
            break;
        }
        
        case Type_SIHO: {
            module::Codec_SIHO<B,Q> *codecPtr;
            codecPtr =  dynamic_cast<module::Codec_SIHO<B,Q> *>(codec.get());
            auto& r_decoder = codecPtr->get_decoder_siho();
            m_modules.push_back(r_decoder.get());
            break;
        }
        
        case Type_SIHO_HIHO: {
            module::Codec_SIHO_HIHO<B,Q> *codecPtr;
            codecPtr =  dynamic_cast<module::Codec_SIHO_HIHO<B,Q> *>(codec.get());
            auto& r_decoder = codecPtr->get_decoder_siho();
            m_modules.push_back(r_decoder.get());
            break;
        }
        
         case Type_HIHO: {
            module::Codec_HIHO<B,Q> *codecPtr;
            codecPtr =  dynamic_cast<module::Codec_HIHO<B,Q> *>(codec.get());
             auto& r_decoder = codecPtr->get_decoder_hiho();
            m_modules.push_back(r_decoder.get());
            break;
        }
         
         default:
            //FIXME: call assert?
            std::cout << "FAILED" << std::endl;
            return;
            
    }

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
    

    ///home/simon/work/phd/missfec/examples/tasks/src/main.cpp!!
      
    
    
    //(*r_decoder)
//    TODO: (*r_encoder)[enc::sck::encode      ::U_K ].bind((*m_source )[src::sck::generate   ::U_K ]);
    
    /*(*m_modem  )[mdm::sck::modulate    ::X_N1].bind((*r_encoder)[enc::sck::encode     ::X_N ]);
    (*m_channel)[chn::sck::add_noise   ::X_N ].bind((*m_modem  )[mdm::sck::modulate   ::X_N2]);
    (*m_modem  )[mdm::sck::demodulate  ::Y_N1].bind((*m_channel)[chn::sck::add_noise  ::Y_N ]);
    (*r_decoder)[dec::sck::decode_siho ::Y_N ].bind((*m_modem  )[mdm::sck::demodulate ::Y_N2]);
    (*m_monitor)[mnt::sck::check_errors::U   ].bind((*r_encoder)[enc::sck::encode     ::U_K ]);
    (*m_monitor)[mnt::sck::check_errors::V   ].bind((*r_decoder)[dec::sck::decode_siho::V_K ]);*/
    
}

template <typename B, typename R, typename Q>
void OnlyCodec<B,R,Q>
::setNoise(float ebn0) 
{
    this->m_fNoise = ebn0;
    if (m_bInitialized) 
    {
        tools::Sigma<> noise;

        const float Rate = (float) params_OnlyCodec.cdc->enc->K / (float)params_OnlyCodec.cdc->enc->N_cw;


        // compute the current sigma for the channel noise
        const auto esn0  = tools::ebn0_to_esn0 (ebn0, Rate);
        const auto sigma = tools::esn0_to_sigma(esn0   );

        noise.set_noise(sigma, ebn0, esn0);

        // update the sigma of the modem and the channel
        codec  ->set_noise(noise);
        //m_modem  ->set_noise(noise);
        //m_channel->set_noise(noise);
    }
}

template <typename B, typename R, typename Q>
void OnlyCodec<B,R,Q>
::encode(int *in, int *out)
{
    PRINT_POINT();

    if ((!codec) || (m_bInitialized == false)) {
        std::cout << "Codec is null" << std::endl;
    }

    auto& r_encoder = codec->get_encoder();
    
    using namespace module;
    
    {
        Socket &socket = (*r_encoder)[enc::sck::encode      ::U_K ];
        std::cout << "Socket (enc::sck::encode      ::U_K ) getName " << socket.get_name() << std::endl;
        std::cout << "Socket (enc::sck::encode      ::U_K ) get_databytes " << socket.get_databytes() << std::endl;
        std::cout << "Socket (enc::sck::encode      ::U_K ) get_datatype_size " << (int) socket.get_datatype_size() << std::endl;
        std::cout << "Socket (enc::sck::encode      ::U_K ) get_datatype_string " << socket.get_datatype_string() << std::endl;
        std::cout << "Socket (enc::sck::encode      ::U_K ) get_n_elmts " << socket.get_n_elmts() << std::endl;
        
        socket_t type = (*r_encoder)[enc::tsk::encode].get_socket_type(socket);
        switch (type) {
            case socket_t::SIN :{
                std::cout << "Socket (enc::sck::encode      ::U_K ) SIN" << std::endl;
                break;
            }
            case socket_t::SIN_SOUT :{
                std::cout << "Socket (enc::sck::encode      ::U_K ) SIN_SOUT" << std::endl;
                break;
            }
            case socket_t::SOUT :{
                std::cout << "Socket (enc::sck::encode      ::U_K ) SOUT" << std::endl;
                break;
            }
        }
    }   
    
    {
        Socket &socket = (*r_encoder)[enc::sck::encode      ::X_N ];
        std::cout << "Socket (enc::sck::encode      ::X_N ) getName " << socket.get_name() << std::endl;
        std::cout << "Socket (enc::sck::encode      ::X_N ) get_databytes " << socket.get_databytes() << std::endl;
        std::cout << "Socket (enc::sck::encode      ::X_N ) get_datatype_size " << (int) socket.get_datatype_size() << std::endl;
        std::cout << "Socket (enc::sck::encode      ::X_N ) get_datatype_string " << socket.get_datatype_string() << std::endl;
        std::cout << "Socket (enc::sck::encode      ::X_N ) get_n_elmts " << socket.get_n_elmts() << std::endl;
        
        socket_t type = (*r_encoder)[enc::tsk::encode].get_socket_type(socket);
        switch (type) {
            case socket_t::SIN :{
                std::cout << "Socket (enc::sck::encode      ::U_K ) SIN" << std::endl;
                break;
            }
            case socket_t::SIN_SOUT :{
                std::cout << "Socket (enc::sck::encode      ::U_K ) SIN_SOUT" << std::endl;
                break;
            }
            case socket_t::SOUT :{
                std::cout << "Socket (enc::sck::encode      ::U_K ) SOUT" << std::endl;
                break;
            }
        }
    }
    
    //pseudo code looks like
    //(*r_encoder)[enc::sck::encode      ::U_K ].bind(in)
    //(*r_encoder)[enc::sck::encode      ::U_K ].bind(out)
    //(*r_encoder).exec()
    
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
