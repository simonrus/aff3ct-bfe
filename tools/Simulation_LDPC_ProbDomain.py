from AList.AListReader import AListReader
from LDPC.EncoderLDPCFromH import EncoderLDPCFromH
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


    N_rows, N_cols = reader.readFromFile("/home/simon/work/phd/missfec/lib/aff3ct/conf/dec/LDPC/CCSDS_64_128.alist")

    N = N_cols
    K = N_cols - N_rows

    print("LDPC (%d, %d) read " % (N, K))

    # h_transposed = np.transpose(reader.matrix)

    pdb.set_trace()
    encoder = EncoderLDPCFromH(reader.matrix)

    # main loop

    ebn0_start = 0.0
    ebn0_end = 5.5
    ebn0_step = 0.5

    # for ebn0 in np.arange(ebn0_start, ebn0_end, ebn0_step):
    for ebn0 in np.arange(ebn0_start, ebn0_end, ebn0_step):

        # calculate sigma from snr =)
        sigma = AWGN.ebn0_to_sigma(ebn0, K * 1.0 / N, 1, 1)
        print("sigma is ", sigma)

        for loop in range(1, 10):
            # vector =  (np.random.rand(K) > 0.5).astype(int)
            vector = np.random.binomial(1, 0.5, K)

            # modulate using BPSK
            signal = vector * 2.0 - 1.0
            sigma = 0.2


            received = AWGN.add_noise(signal, sigma)
            print("at ", loop, " ", received)





    vector = np.asarray([1, 0, 1, 1], dtype=int)

    codeword = encoder.encode(vector)

if __name__ == "__main__":
    simulation()