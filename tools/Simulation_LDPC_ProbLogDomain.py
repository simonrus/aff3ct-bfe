import argparse
import logging
import importlib
import numpy as np
from tqdm import tqdm



from AList.AListReader import AListReader
from LDPC.EncoderLDPCFromH import EncoderLDPCFromH
from Simulator import Simulator
from SimulatorConfig import SimulatorConfig
from Channel.AWGN import AWGN

import pdb

import scipy.io


def logging_debug(pa_array, msg=None):
    if logging.getLogger().getEffectiveLevel() == logging.DEBUG:
        prefix = (msg + "\n") if (msg is not None) else "\n"
        
        logging.debug(prefix + np.array2string(pa_array, precision=2, separator=',',suppress_small=True))


def simulation(sim_args):
    sim_config = SimulatorConfig(sim_args.config)

    reader = AListReader()

    n_rows, n_cols = reader.readFromFile(sim_config.h_matrix)
    logging.info("Read matrix with rows=%d and cols=%d " % (n_rows, n_cols))

    N = n_cols
    K = n_cols - n_rows
    logging.info("LDPC (%d, %d) read " % (N, K))

    encoder = EncoderLDPCFromH(reader.matrix)

    decoder_class = getattr(importlib.import_module(sim_config.family + "." + sim_config.decoder),sim_config.decoder)
    decoder = decoder_class(reader.matrix, sim_config)

    simulation_result_ebn0 = []
    simulation_result_per = []
    simulation_result_ber = []
    simulation_result_received_packets = []
    simulation_result_dec_fails = []

    pbar = tqdm(np.arange(sim_config.ebn0_start,
                          sim_config.ebn0_end,
                          sim_config.ebn0_step))
    for ebn0 in pbar:

        # calculate sigma from snr =)
        sigma = AWGN.ebn0_to_sigma(ebn0, K * 1.0 / N, 1, 1)
        decoder.set_sigma(sigma)
        num_iteration = int(1 / (sigma * sigma * sim_config.desired_accuracy))
        
        logging.info("sigma is %s, num_iteration=%d" % (str(sigma), num_iteration))
        pbar.set_description("Simulating ebn0=%f" % ebn0)

        n_failed_packets = 0
        n_success_packets = 0
        n_failed_bits = 0
        n_success_bits = 0
        n_failed_decodes = 0

        for loop in tqdm(range(num_iteration)):
            vector = np.random.binomial(1, 0.5, K)

            codeword = encoder.encode(vector)                   # encode
            signal = codeword * 2.0 - 1.0                       # modulate using BPSK

            # print("at ", loop, " ", signal)
            logging_debug(signal, " Signal at " + str(loop) + " loop")
            received = AWGN.add_noise(signal, sigma)                            # transmit
            logging_debug(received, " Received at " + str(loop) + " loop")

            decoded_cw = decoder.decode(received)               # Soft probability

            if decoded_cw is None:
                # Code is systematic. Let's take hard decision
                decoded_cw = (received > 0) * 1
                n_failed_decodes = n_failed_decodes + 1

            vector_r = decoded_cw[0:K]

            mismatches = sum(vector ^ vector_r)
            n_failed_bits += mismatches
            n_success_bits += K - mismatches

            if mismatches:
                n_failed_packets = n_failed_packets + 1
            else:
                n_success_packets = n_success_packets + 1

        per = n_failed_packets * 1.0 / (n_success_packets + n_failed_packets)
        ber = n_failed_bits * 1.0 / (n_success_bits + n_failed_bits)
        simulation_result_ebn0.append(ebn0)
        simulation_result_per.append(per)
        simulation_result_ber.append(ber)
        simulation_result_dec_fails.append(n_failed_decodes)
        simulation_result_received_packets.append(n_success_packets + n_failed_packets)

    if sim_args.outmat:
        scipy.io.savemat(sim_args.outmat, mdict={'ebn0': simulation_result_ebn0,
                                                 'PER': simulation_result_per,
                                                 'BER': simulation_result_ber})

    print(simulation_result_ebn0)
    print(simulation_result_ber)
    print(simulation_result_per)
    print(simulation_result_dec_fails)
    print(simulation_result_received_packets)


if __name__ == "__main__":
    p = argparse.ArgumentParser()
    # Mandatory arguments
    p.add_argument("--config", required=True, type=str, help="Path to the config file")

    # Optional arguments 
    p.add_argument("--log", help="DEBUG | INFO | WARNING | ERROR | CRITICAL")
    p.add_argument("--outmat", type=str, help="output matlab file")

    args = p.parse_args()
    
    if args.log:
        logging.basicConfig(level=getattr(logging, args.log))
        
    simulation(args)
