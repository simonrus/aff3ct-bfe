/* 
 * File:   aff3ct-errc.cpp
 * Author: sergei semenov (sergey.semionov@gmail.com)
 *
 * Main application
 */

#include <vector>
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

int run(int argc, char** argv)
{
    // get the AFF3CT version
    std::cout << "#----------------------------------------------------------" << std::endl;
    std::cout << "# This is a basic program using the AFF3CT library (simon.simon.simon)" << std::endl;
    std::cout << "# Feel free to improve it as you want to fit your needs." << std::endl;
    std::cout << "#----------------------------------------------------------" << std::endl;
    std::cout << "#" << std::endl;
    const int fe = 100;
    const int seed = argc >= 2 ? std::atoi(argv[1]) : 0;
    const int K = 32;
    const int N = 128;
    const float R = (float) K / (float) N;
    const float ebn0_min = 0.00f;
    const float ebn0_max = 10.1f;

    std::cout << "# * Simulation parameters: " << std::endl;
    std::cout << "#    ** Frame errors   = " << fe << std::endl;
    std::cout << "#    ** Noise seed     = " << seed << std::endl;
    std::cout << "#    ** Info. bits (K) = " << K << std::endl;
    std::cout << "#    ** Frame size (N) = " << N << std::endl;
    std::cout << "#    ** Code rate  (R) = " << R << std::endl;
    std::cout << "#    ** SNR min   (dB) = " << ebn0_min << std::endl;
    std::cout << "#    ** SNR max   (dB) = " << ebn0_max << std::endl;
    std::cout << "#" << std::endl;

    // buffers to store the data
    std::vector<int > ref_bits(K);
    std::vector<int > enc_bits(N);
    std::vector<float> symbols(N);
    std::vector<float> noisy_symbols(N);
    std::vector<float> LLRs(N);
    std::vector<int > dec_bits(K);

    // create the AFF3CT objects
    module::Source_random<> source(K);
    module::Encoder_repetition_sys<> encoder(K, N);
    module::Modem_BPSK<> modem(N);
    module::Channel_AWGN_LLR<> channel(N, seed);
    module::Decoder_repetition_std<> decoder(K, N);
    module::Monitor_BFER<> monitor(K, fe);

    // create a sigma noise type
    tools::Sigma<> noise;

    // create reporters to display results in the terminal
    std::vector<tools::Reporter*> reporters ={
        new tools::Reporter_noise <>(noise), // report the noise values (Es/N0 and Eb/N0)
        new tools::Reporter_BFER <>(monitor), // report the bit/frame error rates
        new tools::Reporter_throughput<>(monitor) // report the simulation throughputs
    };
    // convert the vector of reporter pointers into a vector of smart pointers
    std::vector<std::unique_ptr < tools::Reporter>> reporters_uptr;
    for (auto rep : reporters) reporters_uptr.push_back(std::unique_ptr<tools::Reporter>(rep));

    // create a terminal that will display the collected data from the reporters
    tools::Terminal_std terminal(reporters_uptr);

    // display the legend in the terminal
    terminal.legend();

    // a loop over the various SNRs
    for (auto ebn0 = ebn0_min; ebn0 < ebn0_max; ebn0 += 1.f) {
        // compute the current sigma for the channel noise
        const auto esn0 = tools::ebn0_to_esn0(ebn0, R);
        const auto sigma = tools::esn0_to_sigma(esn0);

        noise.set_noise(sigma, ebn0, esn0);

        // update the sigma of the modem and the channel
        modem .set_noise(noise);
        channel.set_noise(noise);
        // display the performance (BER and FER) in real time (in a separate thread)
        terminal.start_temp_report();

        // run a small simulation chain
        while (!monitor.fe_limit_achieved() && !terminal.is_interrupt()) {
            source .generate(ref_bits);
            encoder.encode(ref_bits, enc_bits);
            modem .modulate(enc_bits, symbols);
            channel.add_noise(symbols, noisy_symbols);
            modem .demodulate(noisy_symbols, LLRs);
            decoder.decode_siho(LLRs, dec_bits);
            monitor.check_errors(dec_bits, ref_bits);
        }

        // display the performance (BER and FER) in the terminal
        terminal.final_report();

        // if user pressed Ctrl+c twice, exit the SNRs loop
        if (terminal.is_over()) break;

        // reset the monitor and the terminal for the next SNR
        monitor.reset();
        terminal.reset();
    }

    std::cout << "# End of the simulation" << std::endl;
    return 0;
}

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




aff3ct::Message & processClientMessage(aff3ct::Message &recvMessage)
{
    LOG_SCOPE_FUNCTION(INFO);
    
    bool bSuccess = false;
    switch (recvMessage.content_case()) {
        case aff3ct::Message::ContentCase::kPushRequest:
        {
            std::string var_name = recvMessage.pushrequest().var();
            
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
            
            return recvMessage;
        }

        case aff3ct::Message::ContentCase::kPullRequest:
        {
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
           
            return recvMessage;
        }
        default:
            TRACELOG(ERROR,"Protocol error: message (id=%d) shall not be received by server", recvMessage.content_case());
            break;
    }
    
}

int main(int argc, char** argv)
{

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    loguru::init(argc, argv);
    
    enableSIGTermHandler();


    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REP);
    socket.bind("tcp://*:5555");

    TRACELOG(INFO, "Message loop started");

    while (1) {
        zmq::message_t request;
        //  Wait for next request from client
        socket.recv(&request);
        TRACELOG(INFO,  "Received Hello in while loop");

        aff3ct::Message recvMessage;
        recvMessage.ParseFromArray(request.data(), request.size());

        recvMessage = processClientMessage(recvMessage);
        //  Send reply back to client
        zmq::message_t reply(recvMessage.ByteSize());
        recvMessage.SerializeToArray(reply.data(), reply.size());

        socket.send(reply);
    }

    google::protobuf::ShutdownProtobufLibrary();

    return run(argc, argv);
}
