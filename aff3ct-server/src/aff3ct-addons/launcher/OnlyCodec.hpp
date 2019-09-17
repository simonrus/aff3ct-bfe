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
 * Launcher to run stand-alone codec with encode/decode operations
 * Author: simon
 *
 * Created on September 2, 2019, 10:10 PM
 */

#ifndef LAUNCHER_ONLYCODEC_HPP
#define LAUNCHER_ONLYCODEC_HPP

#include <aff3ct-addons/launcher/CodecRun.hpp>
#include <aff3ct-addons/factory/OnlyCodec.hpp>

/*
namespace aff3ct {
namespace simulation{
    class CodecRun;
}
}*/

namespace aff3ct {
namespace launcher{
    
template <typename B = int, typename R = float, typename Q = R>
class OnlyCodec: public CodecRun {
    protected:
	factory::OnlyCodec::parameters params;
    public:
	OnlyCodec(const int argc, const char **argv, std::ostream &stream = std::cout);
	virtual ~OnlyCodec() = default;

        simulation::CodecRun* build_simu();
    protected:
	virtual void get_description_args();
	virtual void store_args();

	//virtual simulation::CodecRun* build_simu();
};


} //namespace launcher
} //namespace aff3ct

#endif /* LAUNCHER_ONLYCODEC_HPP */

