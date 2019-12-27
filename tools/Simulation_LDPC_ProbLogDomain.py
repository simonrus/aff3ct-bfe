from AList.AListReader import AListReader
from LDPC.EncoderLDPCFromH import EncoderLDPCFromH
from LDPC.DecoderLDPCProb import DecoderLDPCProb
from Channel.AWGN import AWGN

import numpy as np
import pdb

def simulation():
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
    print("Read matrix with rows=%d and cols=%d " % (n_rows, n_cols))

    N = n_cols
    K = n_cols - n_rows

    print("LDPC (%d, %d) read " % (N, K))

    encoder = EncoderLDPCFromH(reader.matrix)

    decoder = DecoderLDPCProb(reader.matrix)

    # main loop

    ebn0_start = 0.0
    # ebn0_end = 5.5
    ebn0_end = 0.5
    ebn0_step = 0.5

    # for ebn0 in np.arange(ebn0_start, ebn0_end, ebn0_step):
    for ebn0 in np.arange(ebn0_start, ebn0_end, ebn0_step):

        # calculate sigma from snr =)
        sigma = AWGN.ebn0_to_sigma(ebn0, K * 1.0 / N, 1, 1)
        #print("sigma is ", sigma)

        for loop in range(1, 2):
            # vector =  (np.random.rand(K) > 0.5).astype(int)
            vector = np.random.binomial(1, 0.5, K)

            # encode

            codeword = encoder.encode(vector)

            # modulate using BPSK
            print(type(codeword))
            signal = codeword * 2.0 - 1.0
            sigma = 0.2

            #print("at ", loop, " ", signal)
            received = AWGN.add_noise(signal, sigma)
            #print("at ", loop, " w noise ", received)

            # Soft probability
            decoder.set_sigma(sigma)
            decoded_cw = decoder.decodeInProbDomain(received)

            #print("at ", loop, " cw ", decoded_cw)



if __name__ == "__main__":
    simulation()
