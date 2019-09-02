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
 * Created on September 2, 2019, 10:10 PM
 */

#include "OnlyCodec.hpp"


using namespace aff3ct;
using namespace aff3ct::launcher;

template <typename B, typename R, typename Q>
OnlyCodec<B,R,Q>
::OnlyCodec(const int argc, const char **argv, std::ostream &stream)
: Launcher(argc, argv, params, stream)
{
	/*
        params.set_src(new factory::Source      ::parameters("src"));
	params.set_crc(new factory::CRC         ::parameters("crc"));
	params.set_mdm(new factory::Modem       ::parameters("mdm"));
	params.set_chn(new factory::Channel     ::parameters("chn"));
	params.set_qnt(new factory::Quantizer   ::parameters("qnt"));
	params.set_mnt_mi(new factory::Monitor_MI  ::parameters("mnt"));
	params.set_mnt_er(new factory::Monitor_BFER::parameters("mnt"));
	params.set_ter(new factory::Terminal    ::parameters("ter"));
        */
}


template <typename B, typename R, typename Q>
void OnlyCodec<B,R,Q>
::get_description_args()
{
    Launcher::get_description_args();
    
    //TODO: CHECK /home/simon/work/phd/missfec/lib/aff3ct/src/Launcher/Simulation/BFER_std.cpp
    
    /*
    params.     get_description(this->args);
    params.src->get_description(this->args);
    */
    
    /*
    auto psrc = params.src     ->get_prefix();
    auto pcrc = params.crc     ->get_prefix();
    */
    
    /*
    if (this->args.exist({penc+"-info-bits", "K"}) || this->args.exist({ppct+"-info-bits", "K"}))
		this->args.erase({psrc+"-info-bits", "K"});
     */
}

template <typename B, typename R, typename Q>
void OnlyCodec<B,R,Q>
::store_args()
{
    Launcher::store_args();
    
    //TODO: CHECK /home/simon/work/phd/missfec/lib/aff3ct/src/Launcher/Simulation/BFER_std.cpp
    /*
    params.store(this->arg_vals);

    params.src->seed = params.local_seed;

    params.src->store(this->arg_vals);

    auto psrc = params.src->get_prefix();
     */
}

template <typename B, typename R, typename Q>
simulation::Simulation* OnlyCodec<B,R,Q>
::build_simu()
{
    return nullptr;
    //TODO !
    //return factory::OnlyCodec::build<B,R,Q>(params); 
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef AFF3CT_MULTI_PREC
template class aff3ct::launcher::OnlyCodec<B_8,R_8,Q_8>;
template class aff3ct::launcher::OnlyCodec<B_16,R_16,Q_16>;
template class aff3ct::launcher::OnlyCodec<B_32,R_32,Q_32>;
template class aff3ct::launcher::OnlyCodec<B_64,R_64,Q_64>;
#else
template class aff3ct::launcher::OnlyCodec<B,R,Q>;
#endif