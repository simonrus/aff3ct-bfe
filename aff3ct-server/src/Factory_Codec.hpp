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
 * Class implements a Codec Factory
 */

#ifndef FACTORY_CODEC_HPP
#define FACTORY_CODEC_HPP

#include <aff3ct.hpp>
#include "aff3ct-errc.h"

#include "aff3ct-addons/simulation/Codec.hpp"
#include "aff3ct-addons/factory/Codec_Generic.hpp"

#include <list>
#include <memory>


using namespace aff3ct;

class Factory_Codec 
{
public:
    static std::unique_ptr<simulation::Codec> create(std::list<std::string> &arg_vec, std::error_code &ec, std::ostream& err_stream = std::cerr);
    
    static bool read_arguments(const int argc, const char** argv, factory::Codec_Generic::parameters &params);

private:    
    Factory_Codec() = default;
    virtual ~Factory_Codec() = default;
    
};

#endif /* FACTORY_CODEC_HPP */

