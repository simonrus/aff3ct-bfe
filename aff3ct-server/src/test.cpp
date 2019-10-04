/*
 * MIT License
 * 
 * Copyright (c) 2017 aff3ct
 * Copyright (c) 2019 Sergei Semenov
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
 * Test application for Aff3ct-server backend
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
#include <chrono>

//Libraries to Capture signal 
#include <signal.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */

//Include protobuf generated file
#include <aff3ct.pb.h>
#include <aff3ct-errc.h>


#include <aff3ct.hpp>

#include "Factory_Codec.hpp"

Aff3ctErrc      g_Error = Aff3ctErrc::NoError;

std::unique_ptr<simulation::Codec> g_codec;

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
    return 1;
}

std::map<std::string, std::vector<float>> g_MemoryContainer;

enum ECommand {
    eInit
};

std::map<std::string, enum ECommand> g_supportedCommands = { {"init", eInit}};

bool processCommand(std::list<std::string> &args, std::ostream& err_stream)
{    
    bool result;
    LOG_SCOPE_FUNCTION(INFO);
    
    if (args.empty()) {
        TRACELOG(ERROR, "no arguments provided for a command");
        return false;
    }
    std::string front = args.front();
    
    if (g_supportedCommands.find(front) == g_supportedCommands.end()) 
    {
        TRACELOG(ERROR, "wrong command %s", front.c_str());  
        return false;
    }
    
    std::error_code ec;
    ECommand eCommand = g_supportedCommands[front];

    switch (eCommand) {
        case eInit:
            g_codec = Factory_Codec::create(args, ec, err_stream);
            if (ec) {
                std::cout << ec << std::endl;
                TRACELOG(ERROR, "Model failed to init: %s", ec.message().c_str());
            } else {
                std::cout << "Model initialized" << std::endl;
            }

            break;

        default:
            break;
    }

    return result;
}

void sendCommand(std::vector<std::string> &command)
{
    std::list<std::string> args;

    for (int i = 0; i < command.size(); i++) 
    {
        args.push_back(command[i]);
    }
    
    processCommand(args, getErrStream());
}

int main(int argc, char** argv)
{

    GOOGLE_PROTOBUF_VERIFY_VERSION;
#ifndef NDEBUG    
    //g_model.setDebugPrint(true);
#endif
    loguru::init(argc, argv);
    
    using namespace std::chrono;
    milliseconds us = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
    unsigned int seed = (unsigned int) us.count() % 65536;
    
    //REP codec
    /*std::vector<std::string> init_command = {
        "init",
        "-C",
        "REP",
        "-p",
        "32",
        "-K",
        "8",
        "-N",
        "24",
        "--chn-seed",
        std::to_string(seed),
        "--src-implem",
        "FAST",
        "--chn-implem",
        "FAST",
        "--dec-implem",
        "FAST",
        "-e",
        "500"};*/
    
    
    //LDPC codec (classic approach with identity matrix)
    std::vector<std::string> init_command = {
        "init",
        "-C",
        "LDPC",
        "-K",
        "4",
        "-N",
        "7",
        "--enc-type",
        "LDPC_H",
        "--enc-g-method",
        "IDENTITY",
        "--dec-h-path",
        "codes/ldpc/BCH_7_4_1_strip.alist",
        "--dec-type",
        "BP_FLOODING",
        "--dec-implem",
        "GALA",
        "-i",
        "200"
        };
    sendCommand(init_command);
         
    TRACELOG(INFO, "ErrStream is %s", getErrStream().str().c_str());
    return 0;
}
