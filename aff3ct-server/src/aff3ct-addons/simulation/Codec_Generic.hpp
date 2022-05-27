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
 * File:   Codec_Generic.hpp
 * Author: simon
 *
 * Created on September 12, 2019, 3:06 PM
 */

#ifndef SIMULATION_CODEC_GENERIC_HPP
#define SIMULATION_CODEC_GENERIC_HPP

#include <random>

#include <aff3ct-addons/factory/Codec_Generic.hpp>
#include <Simulation/Simulation.hpp>

#include <Module/Codec/Codec_SISO_SIHO.hpp>
//#include <Module/Codec/Codec_SIHO.hxx>

#include <Factory/Module/Codec/Codec.hpp>
#include <Factory/Module/Codec/Codec_SISO_SIHO.hpp>
#include <Factory/Module/Codec/Codec_SIHO_HIHO.hpp>
#include <Factory/Module/Codec/Codec_HIHO.hpp>

#include "Codec.hpp"

namespace aff3ct
{
namespace simulation
{
    


template <typename B = int, typename R = float, typename Q = R>
class Codec_Generic : public Codec 
{

protected:
    const aff3ct::factory::Codec_Generic&           params_codec;
    std::unique_ptr<module::Codec <B,Q>>                    codec;
    
    std::mt19937                                            rd_engine_seed;
    
    CodecType                                               codecType;
    float                                                   m_fNoise;
    bool                                                    m_bInitialized = false;
    
    void detectCodecType();
    void sockets_binding();
public:
    explicit Codec_Generic(const factory::Codec_Generic& params_codec);
    virtual ~Codec_Generic() = default;
    virtual void initialize();
    
    static void printCodecType(CodecType type, std::ostream &stream = std::cout);
    static CodecType getCodecType(factory::Codec *param);
    
    void setNoise(float ebn0) ;
    
    CodecType getCodecType() {return codecType;}
    virtual void printCodecInfo(std::ostream &stream);
    virtual void encode(void *in, void *out, int n_cw = 1) ;
    virtual bool is_codeword(void *in);

    virtual void decodeHIHO(void *in, void *out, int n_cw = 1) ;
    virtual void decodeSISO(void *in, void *out, int n_cw = 1) ; //output is intrinsic information!
    virtual void decodeSIHO(void *in, void *out, int n_cw = 1) ;
    

};
} //namespace simulation
} //namespace aff3ct

#endif /* SIMULATION_CODEC_GENERIC_HPP */

