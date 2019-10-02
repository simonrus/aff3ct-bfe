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

/* 
 * Class implements a Codec FACADE to aff3ct 
 */

#ifndef FACADE_CODEC_H
#define FACADE_CODEC_H

#include <aff3ct.hpp>
#include "aff3ct-errc.h"
#include "aff3ct-addons/Source_memory.hpp"
#include "aff3ct-addons/Monitor_BFER_detailed.hpp"

#include "aff3ct-addons/simulation/Codec.hpp"
#include "aff3ct-addons/factory/OnlyCodec.hpp"

#include <list>
#include <memory>


using namespace aff3ct;

class Facade_Codec {

protected:
    
    std::unique_ptr<simulation::Codec>       m_codec;
    factory::OnlyCodec::parameters              m_params;
    
    std::error_code constructAll();
    
    
    bool                                        m_bDebugPrint = false;
    bool                                        m_bInitialized = false;
    float                                       m_fNoise = 0.0;
public:
    void setNoise(float ebn0);
    
    Facade_Codec():m_params("sim") { };
    virtual ~Facade_Codec() = default;
    
    bool init(std::list<std::string> &arg_vec, std::error_code &ec, std::ostream& err_stream = std::cerr);
    bool reset();
    
    void setDebugPrint(bool bEnabled=true);
    
    void resetMonitor();
  
    
    static bool read_arguments(const int argc, const char** argv, factory::OnlyCodec::parameters &params);
    
};

#endif /* FACADE_CODEC_H */

