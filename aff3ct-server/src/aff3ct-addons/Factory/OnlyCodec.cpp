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
 * Created on September 2, 2019, 10:14 PM
 */

#include "OnlyCodec.hpp"
#include "Tools/Documentation/documentation.h"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::OnlyCodecSimulation_name = "OnlyCodec";
const std::string aff3ct::factory::OnlyCodecSimulation_prefix = "only";

CodecParameters::CodecParameters(const std::string &prefix)
: Simulation::parameters(Simulation_name, Simulation_prefix) {
}

CodecParameters::CodecParameters(const std::string &name, const std::string &prefix)
: Simulation::parameters(Simulation_name, Simulation_prefix) {
}

CodecParameters* CodecParameters
::clone() const {
    return new CodecParameters(*this);
}

void CodecParameters
::set_cdc(Codec::parameters *cdc)
{
    this->cdc.reset(cdc);
}

const Codec::parameters* CodecParameters::get_cdc() const
{
    return this->cdc.get();
}

    void CodecParameters
::get_description(tools::Argument_map_info &args) const {
    auto p = this->get_prefix();
    const std::string class_name = "factory::CodecParameters::";

    tools::add_arg(args, p, class_name + "p+cde-type,C",
            tools::Text(tools::Including_set("POLAR", "TURBO", "TURBO_DB", "TPC", "LDPC", "REP", "RA", "RSC", "RSC_DB",
            "BCH", "UNCODED", "RS")),
            tools::arg_rank::REQ);
}

void CodecParameters
::store(const tools::Argument_map_value &vals) {
    auto p = this->get_prefix();

    if (vals.exist({p + "-cde-type", "C"})) this->cde_type = vals.at({p + "-cde-type", "C"}); // required

}

void CodecParameters
::get_headers(std::map<std::string, header_list>& headers, const bool full) const {
    auto p = this->get_prefix();

    headers[p].push_back(std::make_pair("Code type (C)", this->cde_type));
    
    if (this->cdc != nullptr) { this->cdc->get_headers(headers, full); }
    
}

std::vector<std::string> CodecParameters::get_names() const
{
    auto n = Simulation::parameters::get_names();
    
    if (cdc    != nullptr) { auto nn = cdc   ->get_names(); for (auto &x : nn) n.push_back(x); }
    
    return n;
}

std::vector<std::string> CodecParameters::get_short_names() const
{
    auto n = Simulation::parameters::get_short_names();
    
    if (cdc    != nullptr) { auto nn = cdc   ->get_short_names(); for (auto &x : nn) n.push_back(x); }
    
    return n;
}

std::vector<std::string> CodecParameters::get_prefixes() const
{
    auto n = Simulation::parameters::get_prefixes();
    
    if (cdc    != nullptr) { auto nn = cdc   ->get_prefixes(); for (auto &x : nn) n.push_back(x); }
    
    return n;
}

template <typename B, typename R, typename Q>
simulation::OnlyCodec<B,R,Q>* CodecParameters::build() const
{
#if defined(AFF3CT_SYSTEMC_SIMU)
        #error "SystemC module for OnlyCodec is not implemented"
#else
//	return new simulation::BFER_ite_threads<B,R,Q>(*this);
        return nullptr;
#endif
}