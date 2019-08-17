/* 
 * File:   aff3ct-errc.cpp
 * Author: sergei semenov (sergey.semionov@gmail.com)
 *
 * Main application
 */

#include <vector>
#include <list>
#include <map>
#include <iostream>

#include <zmq.hpp>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

//Libraries to Capture signal 
#include <signal.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */

//Include protobuf generated file
#include <aff3ct.pb.h>
#include <aff3ct-errc.h>


#include <aff3ct.hpp>

#include "Model.h"

Aff3ctErrc      g_Error = Aff3ctErrc::NoError;

Model g_model;

void sigHandler(int s)
{
    TRACELOG(INFO, "Caught signal %d. Exiting", s);
    google::protobuf::ShutdownProtobufLibrary();
    exit(1);
}

int enableSIGTermHandler()
{
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = sigHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);
}

std::map<std::string, std::vector<float>> g_MemoryContainer;

bool processCommand(std::list<std::string> &args, std::ostream& err_stream)
{    
    bool result;
    LOG_SCOPE_FUNCTION(INFO);
    
    if (args.empty()) {
        TRACELOG(ERROR, "no arguments provided for a command");
        return false;
    }
    std::string front = args.front();
    
    args.pop_front();
        
    if (front == "init")
    {   
        std::error_code ec;
        result = g_model.init(args, ec, err_stream);
        
        if (ec)
        {
            std::cout << ec << std::endl;
            TRACELOG(ERROR, "Model failed to init: %s", ec.message().c_str());    
        }
        
        return result;
    }
    else {
        TRACELOG(ERROR, "wrong command %s", front.c_str());  
        return false;
    }
    
}

int main(int argc, char** argv)
{

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    loguru::init(argc, argv);
    
    std::vector<std::string> all_args = {
        "init",
        //"--cde-type",
        //"REP",
        "-p",
        "32",
        "-C",
        "REP",
        "-K",
        "8",
        "-N",
        "24",
        "--src-implem",
        "FAST",
        "--chn-implem",
        "FAST",
        "--dec-implem",
        "FAST",
        "-e",
        "500"};
    
    std::list<std::string> args;

    for (int i = 0; i < all_args.size(); i++) 
    {
        args.push_back(all_args[i]);
    }
    
    processCommand(args, getErrStream());
    TRACELOG(INFO, "ErrStream is %s", getErrStream().str().c_str());
    
    return 0;
}
