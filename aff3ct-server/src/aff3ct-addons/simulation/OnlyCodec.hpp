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
 * File:   OnlyCodec.hpp
 * Author: simon
 *
 * Created on September 12, 2019, 3:06 PM
 */

#ifndef SIMULATION_ONLYCODEC_HPP
#define SIMULATION_ONLYCODEC_HPP

#include <random>

#include <aff3ct-addons/factory/OnlyCodec.hpp>
#include <Simulation/Simulation.hpp>

#include <Module/Codec/Codec_SISO_SIHO.hpp>
#include <Factory/Module/Codec/Codec.hpp>
#include <Factory/Module/Codec/Codec_SISO_SIHO.hpp>
#include <Factory/Module/Codec/Codec_SIHO_HIHO.hpp>
#include <Factory/Module/Codec/Codec_HIHO.hpp>
#include "CodecRun.hpp"

namespace aff3ct
{
namespace simulation
{
    
enum CodecType
{
    Type_Unknown        = 0,
    Type_SISO_SIHO      = 1,
    Type_SISO           = 2,
    Type_SIHO           = 3,
    Type_SIHO_HIHO      = 4,
    Type_HIHO           = 5
};

template <typename B = int, typename R = float, typename Q = R>
class OnlyCodec : public CodecRun 
{

protected:
    const aff3ct::factory::OnlyCodec::parameters&           params_OnlyCodec;
    std::unique_ptr<module::Codec <B,Q>>                    codec;
    
    std::mt19937                                            rd_engine_seed;
    
    CodecType                                               codecType;
    float                                                   m_fNoise;
    bool                                                    m_bInitialized = false;
    
    void detectCodecType();
    
    void sockets_binding();
public:
    explicit OnlyCodec(const factory::OnlyCodec::parameters& params_OnlyCodec);
    virtual ~OnlyCodec() = default;
    virtual void initialize();
    
    static void printCodecType(CodecType type, std::ostream &stream = std::cout);
    static CodecType getCodecType(factory::Codec::parameters *param);
    
    void setNoise(float ebn0) ;
    
    virtual void printCodecInfo(std::ostream &stream);
    virtual void encode(void *in, void *out) ;
    

};
} //namespace simulation
} //namespace aff3ct

#endif /* SIMULATION_ONLYCODEC_HPP */

