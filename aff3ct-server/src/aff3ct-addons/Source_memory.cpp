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
 * T
 */

#include <vector>

#include "Source_memory.hpp"


using namespace aff3ct;
using namespace aff3ct::module;

template <typename B>
Source_memory<B>
::Source_memory(const int K, std::vector<std::vector<B>> &memory, const int start_idx)
: Source<B>(K, memory.size()), source(memory), src_counter(start_idx)
{
	const std::string name = "Source_memory";
	this->set_name(name);
        
        //ok! thats all

	src_counter %= (int)source.size();
}

template <typename B>
void Source_memory<B>
::_generate(B *U_K, const int frame_id)
{
	std::copy(this->source[this->src_counter].begin(),
	          this->source[this->src_counter].end  (),
	          U_K);

	this->src_counter = (this->src_counter +1) % (int)this->source.size();
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef AFF3CT_MULTI_PREC
template class aff3ct::module::Source_memory<B_8>;
template class aff3ct::module::Source_memory<B_16>;
template class aff3ct::module::Source_memory<B_32>;
template class aff3ct::module::Source_memory<B_64>;
#else
template class aff3ct::module::Source_memory<B>;
#endif
