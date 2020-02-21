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
    sim_config = SimulatorConfig(sim_args)

    reader = AListReader()

    n_rows, n_cols = reader.readFromFile(sim_config.h_matrix)
    logging.info("Read matrix with rows=%d and cols=%d " % (n_rows, n_cols))

    N = n_cols
    K = n_cols - n_rows
    logging.info("LDPC (%d, %d) read " % (N, K))

    encoder = EncoderLDPCFromH(reader.matrix)

    decoder_class = getattr(importlib.import_module(sim_config.family + "." + sim_config.decoder),sim_config.decoder)
    decoder = decoder_class(reader.matrix, sim_config)

    if sim_args.seed:
        np.random.seed(sim_args.seed)

    simulation_result_ebn0 = []
    simulation_result_per = []
    simulation_result_ber = []

    loop = 0
    decoder.set_sigma(1.0)
    n_failed = 0
    n_success = 0
    n_failed_bits = 0
    sim_config.ldpc_max_iter = 2
    # start
    # vector = np.random.binomial(1, 0.5, K)
    vector = np.array([0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1])
    codeword = encoder.encode(vector)                   # encode
    # check codework

    b_failed = False
    for i in range(n_rows):
        sum_in_row = sum(codeword & reader.matrix[i, :])
        check = sum_in_row % 2
        if check:
            b_failed = True
            pdb.set_trace()
            logging.debug("at row " + str(i) + " check failed")

    if not b_failed:
        logging.debug("check are ok ")


    signal = codeword * 2.0 - 1.0                       # modulate using BPSK

    logging_debug(signal, " Signal at " + str(loop) + " loop")
    received = signal / 0.8
    logging_debug(received, " Received at " + str(loop) + " loop")

    pdb.set_trace()

    decoded_cw = decoder.decode(received)               # Soft probability

    if decoded_cw is not None:
        logging.debug("at loop " + str(loop) + " ok")
        n_success = n_success + 1
        n_failed_bits = sum(decoded_cw ^ codeword)
    else:
        logging.debug("at loop " + str(loop) + " failed")
        n_failed = n_failed + 1
        n_failed_bits += N



    if sim_args.outmat:
        scipy.io.savemat(sim_args.outmat, mdict={'ebn0': simulation_result_ebn0,
                                                 'PER': simulation_result_per,
                                                 'BER': simulation_result_ber})

    print(simulation_result_ebn0)
    print(simulation_result_ber)
    print(simulation_result_per)


if __name__ == "__main__":
    p = argparse.ArgumentParser()
    # Mandatory arguments
    p.add_argument("--config", required=True, type=str,help="Path to the config file")

    # Optional arguments 
    p.add_argument("--log", help="DEBUG | INFO | WARNING | ERROR | CRITICAL")
    p.add_argument("--outmat", type=str, help="output matlab file")
    p.add_argument("--seed", type=int, help="random seed")

    SimulatorConfig.describe_override_arguments(p)
    args = p.parse_args()
    
    if args.log:
        logging.basicConfig(level=getattr(logging, args.log))
        
    simulation(args)
