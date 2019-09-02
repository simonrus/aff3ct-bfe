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
 * Created on September 2, 2019, 10:14 PM
 */

#ifndef FACTORY_ONLYCODEC_HPP
#define FACTORY_ONLYCODEC_HPP

#include <string>
#include <Factory/Factory.hpp>
#include <Factory/Simulation/Simulation.hpp>

namespace aff3ct
{
namespace factory
{
extern const std::string OnlyCodecSimulation_name;
extern const std::string OnlyCodecSimulation_prefix;

class CodecParameters : public aff3ct::factory::Simulation::parameters
{
public:
        // ------------------------------------------------------------------------------------------------- PARAMETERS
        std::string cde_type;
        // ---------------------------------------------------------------------------------------------------- METHODS
        CodecParameters(const std::string &p = OnlyCodecSimulation_prefix);
        virtual ~CodecParameters() = default;
        virtual CodecParameters* clone() const;

        // CodecParameters construction
        virtual void get_description(tools::Argument_map_info &args) const;
        virtual void store          (const tools::Argument_map_value &vals);
        virtual void get_headers    (std::map<std::string,header_list>& headers, const bool full = true) const;

protected:
        CodecParameters(const std::string &n, const std::string &p);
};
} //namespace factory
} //namespace aff3ct


#endif /* ONLYCODEC_HPP */

