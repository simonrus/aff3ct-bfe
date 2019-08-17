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
 * Class implements the aff3ct processing chain
 */

#ifndef MODEL_H
#define MODEL_H

#define B_TYPE  int
#define Q_TYPE  float
#define R_TYPE  float  

#include <aff3ct.hpp>
#include <list>
#include <memory>
#include "aff3ct-errc.h"

using namespace aff3ct;

class Model {

protected:
    std::unique_ptr<factory::Source          ::parameters>   p_src;
    std::unique_ptr<factory::Codec_repetition::parameters>   p_cdc;
    std::unique_ptr<factory::Modem           ::parameters>   p_mdm;
    std::unique_ptr<factory::Channel         ::parameters>   p_chn;
    std::unique_ptr<factory::Monitor_BFER    ::parameters>   p_mnt;
    std::unique_ptr<factory::Terminal        ::parameters>   p_ter;
    
    std::vector<factory::Factory::parameters*> m_paramsList;
    
    tools::Sigma<> m_noise;
    // create the AFF3CT modules
    std::unique_ptr<module::Source          <B_TYPE>> m_source; 
    std::unique_ptr<module::Codec_repetition<B_TYPE, Q_TYPE>> m_codec;  
    std::unique_ptr<module::Modem           <B_TYPE, R_TYPE, Q_TYPE>> m_modem;  
    std::unique_ptr<module::Channel         <R_TYPE>> m_channel;
    std::unique_ptr<module::Monitor_BFER    <B_TYPE>> m_monitor;
    
    std::vector<const module::Module*> m_modules;

    void setNoise(float ebn0);

    std::unique_ptr<launcher::Launcher>         m_launcher;

    std::error_code constructAll();
    void printParameters(std::vector<factory::Factory::parameters*> &paramsList);
public:
    Model() = default;
    virtual ~Model() = default;
    
    
    bool init(std::list<std::string> &arg_vec, std::error_code &ec, std::ostream& err_stream = std::cerr);
    bool reset();
    void iterate();
    
    
};

#endif /* MODEL_H */

