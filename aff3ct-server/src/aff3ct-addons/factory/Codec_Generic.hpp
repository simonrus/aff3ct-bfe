/*
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
 * File:   Codec_Generic.hpp
 * Author: simon
 *
 * Created on September 2, 2019, 10:14 PM
 */

#ifndef FACTORY_CODEC_GENERIC_HPP
#define FACTORY_CODEC_GENERIC_HPP

#include <string>
#include <Factory/Factory.hpp>

#include <Factory/Module/Quantizer/Quantizer.hpp>
#include <Factory/Module/Source/Source.hpp>

#include <Factory/Simulation/Simulation.hpp>
#include <Factory/Module/Codec/Codec.hpp>

#define PRINT_POINT(x)      std::cout << __PRETTY_FUNCTION__ << std::endl;

namespace aff3ct
{
namespace launcher
{
class Codec;
}
}

namespace aff3ct
{
namespace factory
{
extern const std::string CodecGenericSimulation_name;
extern const std::string CodecGenericSimulation_prefix;

struct Codec_Generic:public aff3ct::factory::Simulation
{
    class parameters : public Simulation::parameters
    {
    public:
        // ------------------------------------------------------------------------------------------------- PARAMETERS
        std::string cde_type;

        tools::auto_cloned_unique_ptr<Source       ::parameters> src;
        tools::auto_cloned_unique_ptr<Codec        ::parameters> cdc;
        tools::auto_cloned_unique_ptr<Quantizer    ::parameters> qnt;
        // ---------------------------------------------------------------------------------------------------- METHODS
        parameters(const std::string &p = CodecGenericSimulation_prefix);
        virtual ~parameters() = default;
        virtual parameters* clone() const;

        // parameters construction
        virtual void get_description(tools::Argument_map_info &args) const;
        virtual void store          (const tools::Argument_map_value &vals);
        virtual void get_headers    (std::map<std::string,header_list>& headers, const bool full = true) const;

        // Get supported names
        virtual std::vector<std::string> get_names      () const;
        virtual std::vector<std::string> get_short_names() const;
        virtual std::vector<std::string> get_prefixes   () const;

        //Codec settings
        
        void set_src   (Source      ::parameters *src);
        
        void set_cdc   (Codec       ::parameters *cdc);
        const Codec::parameters* get_cdc() const;
        

        // builder
        template <typename B = int, typename R = float, typename Q = R>
        launcher::Codec* build(const int argc, const char **argv, std::ostream &stream) const;

    protected:
        parameters(const std::string &n, const std::string &p);
    }; 
    
    
    template <typename B = int, typename R = float, typename Q = R>
    static launcher::Codec* build(const parameters &params, const int argc, const char **argv, std::ostream &stream);
}; //struct Codec_Generic
} //namespace factory
} //namespace aff3ct


#endif /* FACTORY_CODEC_GENERIC_HPP */

