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
 * Created on September 12, 2019, 3:06 PM
 */

#ifndef SIMULATION_ONLYCODEC_HPP
#define SIMULATION_ONLYCODEC_HPP


#include <Factory/OnlyCodec.hpp>
#include <Simulation/Simulation.hpp>


namespace aff3ct
{
namespace simulation
{
template <typename B = int, typename R = float, typename Q = R>
class OnlyCodec : public Simulation {
private:
    const aff3ct::factory::OnlyCodec::parameters& params_OnlyCodec;
public:
    
    explicit OnlyCodec(const factory::OnlyCodec::parameters& params_OnlyCodec);
    virtual ~OnlyCodec() = default;
    void launch();
   
private:

};
} //namespace simulation
} //namespace aff3ct

#endif /* SIMULATION_ONLYCODEC_HPP */

