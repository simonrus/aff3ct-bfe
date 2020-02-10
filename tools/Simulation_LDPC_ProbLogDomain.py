import argparse

from AList.AListReader import AListReader
from LDPC.EncoderLDPCFromH import EncoderLDPCFromH
from Simulator import Simulator
from SimulatorConfig import SimulatorConfig
from Channel.AWGN import AWGN
from tqdm import tqdm
import logging
import importlib

import numpy as np
import pdb

def logging_debug(pa_array, msg=None):
    if logging.getLogger().getEffectiveLevel() == logging.DEBUG:
        prefix = (msg + "\n") if (msg is not None) else "\n"
        
        logging.debug(prefix + np.array2string(pa_array, precision=2, separator=',',suppress_small=True))
        

def simulation(args):
    sim_config = SimulatorConfig(args.config)

    alist_text = ["7 3",
                  "3 4",
                  "1 1 2 2 3 2 1",
                  "4 4 4",
                  "1 0 0",
                  "2 0 0",
                  "1 3 0",
                  "1 2 0",
                  "1 2 3",
                  "2 3 0",
                  "3 0 0",
                  "1 3 4 5",
                  "2 4 5 6",
                  "3 5 6 7"]

    reader = AListReader()

    n_rows, n_cols = reader.readMatrix(alist_text)
    #n_rows, n_cols = reader.readFromFile("/home/simon/work/phd/missfec/lib/aff3ct/conf/dec/LDPC/CCSDS_64_128.alist")
    logging.info("Read matrix with rows=%d and cols=%d " % (n_rows, n_cols))

    N = n_cols
    K = n_cols - n_rows
    logging.info("LDPC (%d, %d) read " % (N, K))
    
    #logging.info("Simulator = %s" % simulator.selected_simulator())
    

    encoder = EncoderLDPCFromH(reader.matrix)

    decoder_class = getattr(importlib.import_module(sim_config.family + "." + sim_config.decoder),sim_config.decoder)
    decoder = decoder_class(reader.matrix)

    sumulation_result_PER = []

    # for ebn0 in np.arange(ebn0_start, ebn0_end, ebn0_step):
    pbar =  tqdm(np.arange(sim_config.ebn0_start, 
                            sim_config.ebn0_end, 
                            sim_config.ebn0_step))
    for ebn0 in pbar:

        # calculate sigma from snr =)
        sigma = AWGN.ebn0_to_sigma(ebn0, K * 1.0 / N, 1, 1)
        decoder.set_sigma(sigma)
        num_iteration = int(1 / (sigma * sigma * sim_config.desired_accuracy))
        
        logging.info("sigma is %s, num_iteration=%d" % (str(sigma), num_iteration))
        pbar.set_description("Simulating ebn0=%f" % ebn0)

        nFailed = 0
        nSuccess = 0
        for loop in tqdm(range(num_iteration)):
            # vector =  (np.random.rand(K) > 0.5).astype(int)
            vector = np.random.binomial(1, 0.5, K)

            codeword = encoder.encode(vector)                   # encode
            signal = codeword * 2.0 - 1.0                       # modulate using BPSK

            #print("at ", loop, " ", signal)
            logging_debug(signal, " Signal at " + str(loop) + " loop")
            received = AWGN.add_noise(signal, sigma)                            # transmit
            logging_debug(received, " Received at " + str(loop) + " loop")
          
            decoded_cw = decoder.decode(received)               # Soft probability

            if decoded_cw is not None:
                logging.debug("at loop " + str(loop) + " ok")        
                nSuccess = nSuccess + 1
            else:
                logging.debug("at loop " + str(loop) + " failed")
                nFailed = nFailed + 1   

        PER = nFailed * 1.0/ (nSuccess + nFailed)
        sumulation_result_PER.append((ebn0,PER))

    print(sumulation_result_PER)

if __name__ == "__main__":
    p = argparse.ArgumentParser()
    # Mandatory
    p.add_argument("--config", required=True, nargs="+", type=str,help="Path to the config file")

    # Optional arguments 
    p.add_argument("--log", help="DEBUG | INFO | WARNING | ERROR | CRITICAL")
     

    args = p.parse_args()
    
    if (args.log):
        logging.basicConfig(level=getattr(logging, args.log))
    
    simulation(args)
