/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Model.h
 * Author: simon
 *
 * Created on July 24, 2019, 9:11 PM
 */

#ifndef MODEL_H
#define MODEL_H

#define B_TYPE  int
#define Q_TYPE  float
#define R_TYPE  float  

#include <aff3ct.hpp>
#include <list>
#include <memory>

using namespace aff3ct;

struct Parameters{


};

class Model {

protected:
    std::unique_ptr<factory::Source          ::parameters>   p_src;
    std::unique_ptr<factory::Codec_repetition::parameters>   p_cdc;
    std::unique_ptr<factory::Modem           ::parameters>   p_mdm;
    std::unique_ptr<factory::Channel         ::parameters>   p_chn;
    std::unique_ptr<factory::Monitor_BFER    ::parameters>   p_mnt;
    std::unique_ptr<factory::Terminal        ::parameters>   p_ter;
    
    std::vector<factory::Factory::parameters*> m_paramsList;
    
    tools::Sigma<> m_noise;
    // create the AFF3CT modules
    std::unique_ptr<module::Source          <B_TYPE>> m_source; 
    std::unique_ptr<module::Codec_repetition<B_TYPE, Q_TYPE>> m_codec;  
    std::unique_ptr<module::Modem           <B_TYPE, R_TYPE, Q_TYPE>> m_modem;  
    std::unique_ptr<module::Channel         <R_TYPE>> m_channel;
    std::unique_ptr<module::Monitor_BFER    <B_TYPE>> m_monitor;
    
    std::vector<const module::Module*> m_modules;

    void setNoise(float ebn0);
    
    factory::Launcher::parameters m_params;
    
    std::unique_ptr<launcher::Launcher>         m_launcher;

    void constructCodec();
    void construct();
public:
    Model() = default;
    virtual ~Model() = default;
         
    bool init(std::list<std::string> &arg_vec, std::ostream& err_stream = std::cerr);
    bool reset();
    void iterate();
    
    
};

#endif /* MODEL_H */

