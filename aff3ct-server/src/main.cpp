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

#include <aff3ct.hpp>

#define DEF_SINGLETON_DEFAULT( NAME )    \
 public:                        \
    static NAME& instance()      \
    {                            \
       static NAME _instance;    \
       return _instance;         \
    }                            \
 private:                       \
    NAME() = default;               \
    ~NAME() = default;               

class ErrorStorage
{

    DEF_SINGLETON_DEFAULT(ErrorStorage)
  public:
    //pretty report
    void preport(const char *format, ...)
    {
        m_buffer[0] = '\0';
        va_list args;
        va_start(args, format);
        vsnprintf(m_buffer, sizeof (m_buffer) / sizeof (m_buffer[0]) - 1, format, args);

        m_errorString = m_buffer;
        //do something with the error

        va_end(args);
    }

    void report(const std::string &error)
    {
        m_errorString = error;
        m_isRaised = true;
    }

    bool wasError()
    {
        return m_isRaised;
    }

    void reset()
    {
        m_errorString = "";
        m_isRaised = false;
    }
  private:
    char m_buffer[1024];
    bool m_isRaised = false;
    std::string m_errorString = "";

};

#define REPORT_ERR(msg, ...)       ErrorStorage::instance().preport(msg, ##__VA_ARGS__);

using namespace aff3ct;

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
    printf("Caught signal %d\n", s);
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

bool pbMatrixToVector(const aff3ct::Matrix& from, std::vector<float> &to)
{
    if (from.n() != 1) {
        REPORT_ERR("wrong input dim (%u, %u). Shall be vector (%u, %u)",
                from.n(), from.m(), 1, from.m());
        return false;
    }

    to.resize(from.m(), 0.0);

    if (from.m() != from.values_size()) {
        REPORT_ERR("expected %u elements for vector with length %u, but got %u",
                from.m(), from.m(), from.values_size());
        return false;
    }

    for (uint32_t i = 0; i < from.m(); i++) {
        to[i] = (from.values(i));
    }
    return true;
}

bool vector2pbMatrix(std::vector<float> &from, aff3ct::Matrix& to)
{

    to.set_n(1);
    to.set_m((uint32_t) from.size());

    for (size_t i = 0; i < from.size(); i++) {
        to.add_values((float) from[i]);
    }
}

void processClientMessage(aff3ct::Message &recvMessage)
{
    bool bSuccess = false;
    switch (recvMessage.content_case()) {
        case aff3ct::Message::ContentCase::kPushRequest:
        {
            printf("kPushRequest received\n");
            const std::string &var_name = recvMessage.pushrequest().var();
            if (g_MemoryContainer.find(var_name) != g_MemoryContainer.end()) {
                printf("[PushRequest] overrides existing %s\n", var_name.c_str());
            }

            g_MemoryContainer[var_name] = std::vector<float>();
            bSuccess = pbMatrixToVector(recvMessage.pushrequest().mtx(), g_MemoryContainer[var_name]);

            if (!bSuccess) {
                //TODO return error result with message
            } else {
                //TODO return ok
            }
            break;
        }

        case aff3ct::Message::ContentCase::kPullRequest:
        {
            printf("kPullRequest received\n");
            
            const std::string &var_name = recvMessage.pullrequest().var();
            
            
            if (g_MemoryContainer.find(var_name) == g_MemoryContainer.end()) {
                
                //TODO return error result with not found message
            } 
            else 
            {
                //TODO pack message
            }
            break;
        }
        default:
            printf("Protocol error: message (id=%d) shall not be received by server\n", recvMessage.content_case());
            break;
    }
}

int main(int argc, char** argv)
{

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    enableSIGTermHandler();


    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REP);
    socket.bind("tcp://*:5555");

    while (1) {
        zmq::message_t request;
        //  Wait for next request from client
        socket.recv(&request);
        std::cout << "Received Hello" << std::endl;

        aff3ct::Message recvMessage;
        recvMessage.ParseFromArray(request.data(), request.size());

        processClientMessage(recvMessage);
        //  Do some 'work'
        sleep(1);

        //  Send reply back to client
        zmq::message_t reply(5);
        memcpy(reply.data(), "World", 5);
        socket.send(reply);
    }

    google::protobuf::ShutdownProtobufLibrary();

    return run(argc, argv);
}
