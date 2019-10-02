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

#include "aff3ct-addons/simulation/OnlyCodec.hpp"
#include "aff3ct-addons/launcher/OnlyCodec.hpp"

#include <Tools/Documentation/documentation.h>
#include <Launcher/Code/Repetition/Repetition.hpp>
#include <Launcher/Code/LDPC/LDPC.hpp>


using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::OnlyCodecSimulation_name = "OnlyCodec";
const std::string aff3ct::factory::OnlyCodecSimulation_prefix = "only";

OnlyCodec::parameters::parameters(const std::string &prefix)
: Simulation::parameters(Simulation_name, Simulation_prefix) {
}

OnlyCodec::parameters::parameters(const std::string &name, const std::string &prefix)
: Simulation::parameters(Simulation_name, Simulation_prefix) {
}

OnlyCodec::parameters* OnlyCodec::parameters::clone() const 
{
    return new parameters(*this);
}

void OnlyCodec::parameters
::set_cdc(Codec::parameters *cdc)
{
    /* 
     BT
    aff3ct::factory::OnlyCodec::parameters::set_cdc() at OnlyCodec.cpp:64
    aff3ct::launcher::Repetition<aff3ct::launcher::OnlyCodec<int, float, float>, int, float, float>::Repetition() at Repetition.cpp:15
    aff3ct::launcher::Codec* aff3ct::factory::OnlyCodec::parameters::build<int, float, float>() const at OnlyCodec.cpp:144
    aff3ct::launcher::Codec* aff3ct::factory::OnlyCodec::build<int, float, float>() at OnlyCodec.cpp:161
     */ 
         
    this->cdc.reset(cdc); //here comes cdc from Codec_!!! FINDME
}

void OnlyCodec::parameters
::set_src(Source::parameters *src)
{
	this->src.reset(src);
}


const Codec::parameters* OnlyCodec::parameters::get_cdc() const
{
    return this->cdc.get();
}

void OnlyCodec::parameters::get_description(tools::Argument_map_info &args) const 
{
    auto p = this->get_prefix();
    const std::string class_name = "factory::parameters::";

    tools::add_arg(args, p, class_name + "p+cde-type,C",
            tools::Text(tools::Including_set("POLAR", "TURBO", "TURBO_DB", "TPC", "LDPC", "REP", "RA", "RSC", "RSC_DB",
            "BCH", "UNCODED", "RS")),
            tools::arg_rank::REQ);
}

void OnlyCodec::parameters::store(const tools::Argument_map_value &vals) 
{
    auto p = this->get_prefix();

    if (vals.exist({p + "-cde-type", "C"})) this->cde_type = vals.at({p + "-cde-type", "C"}); // required

}

void OnlyCodec::parameters::get_headers(std::map<std::string, header_list>& headers, const bool full) const 
{
    auto p = this->get_prefix();

    headers[p].push_back(std::make_pair("Code type (C)", this->cde_type));
    
    if (this->cdc != nullptr) { this->cdc->get_headers(headers, full); }
    
}

std::vector<std::string> OnlyCodec::parameters::get_names() const
{
    auto n = Simulation::parameters::get_names();
    
    if (cdc    != nullptr) { auto nn = cdc   ->get_names(); for (auto &x : nn) n.push_back(x); }
    
    return n;
}

std::vector<std::string> OnlyCodec::parameters::get_short_names() const
{
    auto n = Simulation::parameters::get_short_names();
    
    if (cdc    != nullptr) { auto nn = cdc   ->get_short_names(); for (auto &x : nn) n.push_back(x); }
    
    return n;
}

std::vector<std::string> OnlyCodec::parameters::get_prefixes() const
{
    auto n = Simulation::parameters::get_prefixes();
    
    if (cdc    != nullptr) { auto nn = cdc   ->get_prefixes(); for (auto &x : nn) n.push_back(x); }
    
    return n;
}

template <typename B, typename R, typename Q>
launcher::Codec* OnlyCodec::parameters::build(const int argc, const char **argv, std::ostream &stream) const
{
    if (this->cde_type == "REP")
    {
        std::cout << "Constructing REP codec" << std::endl;
        return new launcher::Repetition<launcher::OnlyCodec<B,R,Q>,B,R,Q>(argc, argv, stream); 
    }
    
    if (this->cde_type == "LDPC")
    {
        std::cout << "Constructing LDPC codec" << std::endl;
        return new launcher::LDPC<launcher::OnlyCodec<B,R,Q>,B,R,Q>(argc, argv, stream); 
    }
    
    std::cout << "Constructing NON REP codec" << std::endl;
    
#if defined(AFF3CT_SYSTEMC_SIMU)
        #error "SystemC module for OnlyCodec is not implemented"
#else
//	return new simulation::BFER_ite_threads<B,R,Q>(*this);
//    return new simulation::OnlyCodec<B,R,Q> (*this);
    return nullptr;
#endif
}

template <typename B, typename R, typename Q>
launcher::Codec* OnlyCodec::build(const parameters &params, const int argc, const char **argv, std::ostream &stream)
{
	return params.template build<B,R,Q>(argc, argv, stream);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef AFF3CT_MULTI_PREC
template launcher::Codec* OnlyCodec::parameters::build<B_8,R_8,Q_8>(const int argc, const char **argv, std::ostream &stream) const;
template launcher::Codec* OnlyCodec::parameters::build<B_32,R_32,Q_32>(const int argc, const char **argv, std::ostream &stream) const;
template launcher::Codec* OnlyCodec::build<B_8,R_8,Q_8>(const aff3ct::factory::OnlyCodec::parameters &params,const int argc, const char **argv, std::ostream &stream);
template launcher::Codec* OnlyCodec::build<B_32,R_32,Q_32>(const aff3ct::factory::OnlyCodec::parameters &params,const int argc, const char **argv, std::ostream &stream);

//all codecs

//BECAUSE IT IS TEMPLATE LIBRARY!!!
#include <Launcher/Code/Repetition/Repetition.hpp>
#include <Launcher/Code/Repetition/Repetition.cpp>

template class launcher::Repetition<launcher::OnlyCodec<B_8,R_8,Q_8>,B_8,R_8,Q_8>;
template class launcher::Repetition<launcher::OnlyCodec<B_32,R_32,Q_32>,B_32,R_32,Q_32>;

#include <Launcher/Code/LDPC/LDPC.hpp>
#include <Launcher/Code/LDPC/LDPC.cpp>

template class launcher::LDPC<launcher::OnlyCodec<B_8,R_8,Q_8>,B_8,R_8,Q_8>;
template class launcher::LDPC<launcher::OnlyCodec<B_32,R_32,Q_32>,B_32,R_32,Q_32>;



#else
#error "Not implemented"
#endif
// ==================================================================================== explicit template instantiation