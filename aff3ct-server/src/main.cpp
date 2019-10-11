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
#include <sstream>

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

Aff3ctErrc g_Error = Aff3ctErrc::NoError;

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

/*
 * Converts flatbuffers matrix to vector
 */
bool fbMatrixToVector(const aff3ct::proto::Matrix *from, std::vector<float> &to)
{
    if (from->n() != 1) {
        TRACELOG(ERROR, "wrong input dim (%u, %u). Shall be vector (%u, %u)",
                from->n(), from->m(), 1, from->m());
        g_Error = Aff3ctErrc::ParsingError;
        return false;
    }

    if (from->values() == nullptr) {
        TRACELOG(ERROR, "pbMatrixToVector failed: no values in matrix");
        g_Error = Aff3ctErrc::ParsingError;
        return false;
    }

    const flatbuffers::Vector<float> *values = from->values();

    to.resize(from->m(), 0.0);

    if (from->m() != values->size()) {
        TRACELOG(ERROR, "expected %u elements for vector with length %u, but got %u",
                from->m(), from->m(), values->size());
        g_Error = Aff3ctErrc::ParsingError;
        return false;
    }


    for (uint32_t i = 0; i < from->m(); i++) {
        to[i] = values->Get(i);
    }
    return true;
}

/*
 * Converts vector to flatbuffer matrix
 */
flatbuffers::Offset<aff3ct::proto::Matrix>
vector2fbMatrix(std::vector<float> &from, flatbuffers::FlatBufferBuilder &builder)
{
    auto values = builder.CreateVector<float>(from);

    aff3ct::proto::MatrixBuilder matrix_builder(builder);
    matrix_builder.add_n(1);
    matrix_builder.add_m((int32_t) from.size());
    matrix_builder.add_values(values);

    return matrix_builder.Finish();
}

void logCommand(std::list<std::string> &args)
{
    /** do command logging */
    std::ostringstream ss;
    ss << "Factory received command (" << args.size() << " args):";
    for (std::string &str : args) {
        ss << str << " ";
    }
    TRACELOG(INFO, "%s", ss.str().c_str());
}

enum ECommand
{
    eInit
};

std::map<std::string, enum ECommand> g_supportedCommands = {
    {"init", eInit}
};

bool execCommand(const char* command, std::ostream& err_stream)
{
    bool result;

    std::list<std::string> args;

    LOG_SCOPE_FUNCTION(INFO);

    if (strlen(command) == 0) {
        TRACELOG(ERROR, "no arguments provided for a command");
        return false;
    }

    /* split text into words */
    std::istringstream ss(command);
    do {
        // Read a word 
        std::string word;
        ss >> word;

        args.push_back(word);

        // While there is more to read 
    } while (ss);

    /* determine first workd */
    std::string front = args.front();

    if (g_supportedCommands.find(front) == g_supportedCommands.end()) {
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

void processClientMessage(void *data, size_t size, flatbuffers::FlatBufferBuilder &builder)
{
    LOG_SCOPE_FUNCTION(INFO);

    const aff3ct::proto::Message *recvMessage = aff3ct::proto::GetMessage(data);

    bool bSuccess = false;
    switch (recvMessage->action()) {
        case aff3ct::proto::Action::Action_Push:
        {
            TRACELOG(INFO, "Push received!");

            if ((recvMessage->text() == nullptr) || (recvMessage->matrix() == nullptr)) {
                TRACELOG(ERROR, "There is no var/matrix in push_request!");
            } else {
                std::string var_name = recvMessage->text()->str();

                TRACELOG(INFO, "kPushRequest received %s (%d, %d) ",
                        var_name.c_str(),
                        recvMessage->matrix()->n(),
                        recvMessage->matrix()->m()
                        );

                if (g_MemoryContainer.find(var_name) != g_MemoryContainer.end()) {
                    TRACELOG(WARNING, "[PushRequest] overrides existing %s with (%dx%d)",
                            var_name.c_str(),
                            recvMessage->matrix()->n(),
                            recvMessage->matrix()->m());
                }

                g_MemoryContainer[var_name] = std::vector<float>();
                bSuccess = fbMatrixToVector(recvMessage->matrix(), g_MemoryContainer[var_name]);

            }

            /*build answer*/

            if (!bSuccess) {

                std::string error = getErrStream().str();
                TRACELOG(INFO, "kPullRequest failed: %s", error.c_str());

                auto error_text = builder.CreateString(error.c_str());

                auto result = CreateResult(builder, aff3ct::proto::ResultType_Failed, error_text);

                aff3ct::proto::MessageBuilder message_builder(builder);
                message_builder.add_result(result);
                auto offset = message_builder.Finish();
                builder.Finish(offset);
            } else {
                auto result = CreateResult(builder, aff3ct::proto::ResultType_Success);

                aff3ct::proto::MessageBuilder message_builder(builder);
                message_builder.add_result(result);
                auto offset = message_builder.Finish();
                builder.Finish(offset);
            }


            return;
        }

        case aff3ct::proto::Action::Action_Pull:
        {
            TRACELOG(INFO, "Pull received!");

            if (recvMessage->text() == nullptr) {
                TRACELOG(ERROR, "There is no var/matrix in pull_request!");
            } else {

                std::string var_name = recvMessage->text()->str();

                TRACELOG(INFO, "kPullRequest received for %s", var_name.c_str());

                if (g_MemoryContainer.find(var_name) != g_MemoryContainer.end()) {

                    auto matrix = vector2fbMatrix(g_MemoryContainer[var_name], builder);
                    auto result = CreateResult(builder, aff3ct::proto::ResultType_Success);

                    aff3ct::proto::MessageBuilder message_builder(builder);
                    message_builder.add_result(result);
                    message_builder.add_matrix(matrix);
                    auto offset = message_builder.Finish();
                    builder.Finish(offset);

                    bSuccess = true;
                } else {
                    TRACELOG(ERROR, "pull_request failed: Variable %s not found", var_name.c_str());
                }
            }

            if (!bSuccess) {
                std::string error = getErrStream().str();
                TRACELOG(INFO, "kPullRequest failed: %s", error.c_str());

                auto error_text = builder.CreateString(error.c_str());

                auto result = CreateResult(builder, aff3ct::proto::ResultType_Failed, error_text);

                aff3ct::proto::MessageBuilder message_builder(builder);
                message_builder.add_result(result);
                auto offset = message_builder.Finish();
                builder.Finish(offset);
            }
        }
        case aff3ct::proto::Action::Action_Exec:
        {
            TRACELOG(INFO, "Exec received ");

            if (recvMessage->text() == nullptr) {
                TRACELOG(ERROR, "Action_Exec: no command found");
            }else {
                if (execCommand(recvMessage->text()->c_str(), getErrStream())) {
                    TRACELOG(INFO, "Exec finished (OK)");
                    bSuccess = true;
                } else {
                    TRACELOG(ERROR, "Exec finished (FAILED)");
                }
            }

            if (!bSuccess) {

                std::string error = getErrStream().str();

                auto error_text = builder.CreateString(error.c_str());

                auto result = CreateResult(builder, aff3ct::proto::ResultType_Failed, error_text);

                aff3ct::proto::MessageBuilder message_builder(builder);
                message_builder.add_result(result);
                auto offset = message_builder.Finish();
                builder.Finish(offset);
            } else {
                auto result = CreateResult(builder, aff3ct::proto::ResultType_Success);

                aff3ct::proto::MessageBuilder message_builder(builder);
                message_builder.add_result(result);
                auto offset = message_builder.Finish();
                builder.Finish(offset);
            }
        }
        default:
            //TRACELOG(ERROR,"Protocol error: message (id=%d) shall not be received by server", recvMessage.content_case());
            TRACELOG(ERROR, "Protocol error: message (id=%d) shall not be received by server", 1);
            break;
    }

    clearErrStream();
}

int main(int argc, char** argv)
{

    loguru::init(argc, argv);

    enableSIGTermHandler();

    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REP);
    socket.bind("tcp://*:5555");

    TRACELOG(INFO, "Message loop started");


    
    zmq::message_t request;

    flatbuffers::FlatBufferBuilder builder(1024);


    while (1) {
        //  Wait for next request from client
        socket.recv(&request);

        processClientMessage(request.data(), request.size(), builder);
    
        zmq::message_t reply(builder.GetBufferPointer(), builder.GetSize());
        socket.send(reply);
        
        builder.Reset();
    }

    return 0;
}
