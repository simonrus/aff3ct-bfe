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
 * File:   Codec.hpp
 * Author: simon
 *
 * Created on September 13, 2019, 11:15 AM
 */

#ifndef SIMULATION_CODEC_HPP
#define SIMULATION_CODEC_HPP

#include <ostream>

#include <Module/Task.hpp>
#include <Module/Socket.hpp>

#include <aff3ct-addons/launcher/Codec.hpp>
#include <aff3ct-addons/factory/Codec_Generic.hpp>

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

class Codec {
public:
    Codec();
    virtual ~Codec() = default;
    
    virtual void initialize() = 0;
       
    virtual void encode(void *in, void *out, int n_cw = 1) = 0;
    virtual bool is_codeword(void *in) = 0;
    
    virtual void decodeHIHO(void *in, void *out, int n_cw = 1) = 0;
    virtual void decodeSISO(void *in, void *out, int n_cw = 1) = 0;
    virtual void decodeSIHO(void *in, void *out, int n_cw = 1) = 0;
    
    virtual void printCodecInfo(std::ostream &stream) = 0;
    
    virtual CodecType getCodecType() = 0;
    
    std::unique_ptr<launcher::Codec>                creator;
    
    int getK();
    int getN();
    
    void enableDebug(bool enable_debug) {this->enable_debug = enable_debug;}
protected:
    static void printSocketInfo(std::ostream &stream, module::Socket &socket);
    static void printSocketTypeInfo(std::ostream &str,module::socket_t type);
    
    bool                                            enable_debug = false;

};
} //namespace simulation
} //namespace aff3ct

#endif /* SIMULATION_CODEC_HPP */

