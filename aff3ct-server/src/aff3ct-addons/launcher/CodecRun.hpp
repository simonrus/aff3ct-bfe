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
 * File:   CodecRun.hpp
 * Author: simon
 *
 * Created on September 13, 2019, 1:52 PM
 */

#ifndef LAUNCHER_CODECRUN_HPP
#define LAUNCHER_CODECRUN_HPP

#include <aff3ct-addons/factory/OnlyCodec.hpp>
namespace aff3ct
{
namespace launcher
{
/*!
 * \class CodecRun
 *
 * \brief Collects the command line parameters and launches the stand alone codec.
 */
class CodecRun {
public:
    CodecRun(const int argc, const char **argv, 
            factory::OnlyCodec::parameters &params, 
            std::ostream &stream = std::cout);
    
    virtual void get_description_args();
    virtual void store_args();
        
    virtual ~CodecRun() = default;
private:
    
protected:
    //see /home/simon/work/phd/missfec/lib/aff3ct/src/Launcher/Launcher.hpp
    tools::Argument_map_info        args;     /*!< List of the arguments to find in the command line */
    tools::Argument_map_value       arg_vals; /*!< List of the arguments with their values */
    

};
} //namespace launcher
} //namespace aff3ct

#endif /* LAUNCHER_CODECRUN_HPP */

