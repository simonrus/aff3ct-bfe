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
 * File:   Monitor_BFER_detailed.hpp
 * Author: simon
 *
 * Created on August 18, 2019, 11:36 PM
 */

#ifndef MONITOR_BFER_DETAILED_HPP
#define MONITOR_BFER_DETAILED_HPP

#include "Module/Monitor/BFER/Monitor_BFER.hpp"

namespace aff3ct
{
namespace module
{
template <typename B>
class Monitor_BFER_detailed : public Monitor_BFER<B>
{
    public:
	//Use the same contructors!
        Monitor_BFER_detailed(const int K, const unsigned max_fe, const unsigned max_n_frames = 0, const bool count_unknown_values = false, const int n_frames = 1):
            Monitor_BFER<B>(K, max_fe, max_n_frames, count_unknown_values, n_frames) {}
            
        virtual ~Monitor_BFER_detailed() = default;
        
};
} //namespace module
} //namespace aff3ct
#endif /* MONITOR_BFER_DETAILED_HPP */

