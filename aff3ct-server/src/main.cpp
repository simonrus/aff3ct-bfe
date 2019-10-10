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

#include <aff3ct-proto_generated.h>
#include <aff3ct-errc.h>

#include <aff3ct.hpp>

#include "Factory_Codec.hpp"

Aff3ctErrc      g_Error = Aff3ctErrc::NoError;

std::unique_ptr<simulation::Codec> g_codec;

void sigHandler(int s)
{
    TRACELOG(INFO, "Caught signal %d. Exiting", s);
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
#if 0 
/*
 * Converts protobuf matrix to vector
 */
bool pbMatrixToVector(const aff3ct::Matrix& from, std::vector<float> &to)
{
    if (from.n() != 1) {
        TRACELOG(ERROR, "wrong input dim (%u, %u). Shall be vector (%u, %u)",
                from.n(), from.m(), 1, from.m());
        g_Error = Aff3ctErrc::ParsingError;
        return false;
    }

    to.resize(from.m(), 0.0);

    if (from.m() != from.values_size()) {
        TRACELOG(ERROR, "expected %u elements for vector with length %u, but got %u",
                from.m(), from.m(), from.values_size());
        g_Error = Aff3ctErrc::ParsingError;
        return false;
    }

    for (uint32_t i = 0; i < from.m(); i++) {
        to[i] = (from.values(i));
    }
    return true;
}


void invalidatePbMatrix(aff3ct::Matrix* to)
{
    to->set_m(0);
    to->set_n(0);
}

/*
 * Converts vector to protobuf matrix
 */
void vector2pbMatrix(std::vector<float> &from, aff3ct::Matrix* to)
{

    to->set_n(1);
    to->set_m((uint32_t) from.size());

    to->mutable_values()->Reserve(to->n() * to->m());
    to->clear_values();
    for (size_t i = 0; i < from.size(); i++) {
        to->add_values((float) from[i]);
    }
}

#endif 

void logCommand(std::list<std::string> &args)
{
    /** do command logging */
    std::ostringstream ss;
    ss << "Factory received command (" << args.size() << " args):";
    for (std::string &str : args) 
    {
        ss << str << " ";
    }
    TRACELOG(INFO,"%s", ss.str().c_str());
}

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


void processClientMessage(const aff3ct::proto::Message *recvMessage)
{
    LOG_SCOPE_FUNCTION(INFO);
    
    bool bSuccess = false;
    switch (recvMessage->action()) {
    case aff3ct::proto::Action::Action_Push:
        {
            /*std::string var_name = recvMessage.pushrequest().var();
            
            TRACELOG(INFO,"kPushRequest received %s (%d, %d) ", 
                    var_name.c_str(), 
                    recvMessage.pushrequest().mtx().n(),
                    recvMessage.pushrequest().mtx().m());
            
            if (g_MemoryContainer.find(var_name) != g_MemoryContainer.end()) {
                TRACELOG(WARNING,"[PushRequest] overrides existing %s with (%dx%d)", 
                        var_name.c_str(), 
                        recvMessage.pushrequest().mtx().n(),
                        recvMessage.pushrequest().mtx().m());
            }

            g_MemoryContainer[var_name] = std::vector<float>();
            bSuccess = pbMatrixToVector(recvMessage.pushrequest().mtx(), g_MemoryContainer[var_name]);

            if (!bSuccess) {
                aff3ct::Result* result = recvMessage.mutable_result();
                result->set_type(Failed);
                
                std::string error = "pbMatrixToVector failed for "  + var_name;   
                TRACELOG(INFO,"kPullRequest failed: %s", error.c_str());

                result->set_error_text(getLastLogEntry());
            } else {
                aff3ct::Result* result = recvMessage.mutable_result();
                result->set_type(Success);
            }
            */
            
            TRACELOG(INFO,"Push received!");
            return ;
        }

    case aff3ct::proto::Action::Action_Pull:
        {
#if 0
            std::string var_name = recvMessage.pullrequest().var();
        
            TRACELOG(INFO,"kPullRequest received for %s", var_name.c_str());
            
            if (g_MemoryContainer.find(var_name) == g_MemoryContainer.end()) {      
                //TODO return error result with not found message
                
                aff3ct::PullReply* pull_reply = recvMessage.mutable_pullreply();
                invalidatePbMatrix(pull_reply->mutable_mtx());
                
                aff3ct::Result *result = pull_reply->mutable_result();
                result->set_type(Failed);
                
                std::string error = "Can't find variable "  + var_name;   
                TRACELOG(INFO,"kPullRequest failed: %s", error.c_str());
                
                result->set_error_text(getLastLogEntry());
            } 
            else 
            {
                aff3ct::PullReply* pull_reply = recvMessage.mutable_pullreply();
                invalidatePbMatrix(pull_reply->mutable_mtx());
                vector2pbMatrix(g_MemoryContainer[var_name], pull_reply->mutable_mtx());
                
                aff3ct::Result *result = pull_reply->mutable_result();
                result->set_type(Success);
            } 
#endif
            TRACELOG(INFO,"Pull received!");
            return;
        }
        case aff3ct::proto::Action::Action_Exec:
        {
#if 0
            uint32_t argc       = recvMessage.command().argc();
            std::list<std::string> args;
            
            for (int i = 0; i < argc; i++) 
            {
                args.push_back(recvMessage.command().argv(i));
            }
            
            /** do command processing */
            aff3ct::Result* result = recvMessage.mutable_result();
            
            if (processCommand(args, getErrStream())) 
            {
                result->set_type(Success);    
                
            }else 
            {
                result->set_type(Failed);    
                result->set_error_text(getErrStream().str());    
                clearErrStream();
            }
            
#endif     
            TRACELOG(INFO,"Exec received!");
            return recvMessage;
        }
        default:
            //TRACELOG(ERROR,"Protocol error: message (id=%d) shall not be received by server", recvMessage.content_case());
            TRACELOG(ERROR,"Protocol error: message (id=%d) shall not be received by server", 1);
            break;
    }   
}


int main(int argc, char** argv)
{

    loguru::init(argc, argv);
    
    enableSIGTermHandler();

    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REP);
    socket.bind("tcp://*:5555");

    TRACELOG(INFO, "Message loop started");

    
    zmq::message_t reply(2048);
    zmq::message_t request;
    
    while (1) {
    
        //  Wait for next request from client
        socket.recv(&request);
        
        //  Send reply back to client
        
        
        
        const aff3ct::proto::Message *recvMessage = aff3ct::proto::GetMessage(request.data());
        
        //reply?
        processClientMessage(recvMessage);
        
        
        socket.send(reply);
    }

    return 0;
}
