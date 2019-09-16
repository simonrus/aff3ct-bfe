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
 * Created on September 12, 2019, 3:06 PM
 */

#include "OnlyCodec.hpp"

using namespace aff3ct;
using namespace aff3ct::simulation;

template <typename B, typename R, typename Q>
OnlyCodec<B,R,Q>
::OnlyCodec(const factory::OnlyCodec::parameters& params_OnlyCodec):
// Simulation(params_OnlyCodec), 
        params_OnlyCodec(params_OnlyCodec)
{
    
}


template <typename B, typename R, typename Q>
void OnlyCodec<B,R,Q>
::initialize()
{
    PRINT_POINT();
}

template <typename B, typename R, typename Q>
void OnlyCodec<B,R,Q>
::iterate(void *in, void *out)
{
    PRINT_POINT();
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef AFF3CT_MULTI_PREC
template class aff3ct::simulation::OnlyCodec<B_8,R_8,Q_8>;
template class aff3ct::simulation::OnlyCodec<B_32,R_32,Q_32>;
#else
#error "Not yet implemented"
#endif
// ==================================================================================== explicit template instantiation
