import numpy as np

from .MatrixHandler import MatrixHandler

import pdb

class EncoderLDPCFromH:
    handler = None
    H = None
    G = None
    info_bits_position = None

    K = 0
    N = 0

    # constructor
    def __init__(self, H):

        self.H = H
        self.N = H.shape[1]
        self.K = self.N - H.shape[0]
        handler = MatrixHandler()
        
        self.G = handler.transform_H_to_G_indentity(self.H, self.info_bits_position)
        pass

    def is_codeword(self, vector):
        n_failed_checks = 0
        assert(len(vector) == self.N), "invalid vector"

        for i in range(0, self.N - self.K):
            if (np.sum(vector & self.H[i, :]) % 2) != 0:
                print ("Failed check at row %d" % i)
                print(vector)
                print(self.H[i, :])
                n_failed_checks = n_failed_checks + 1

        return n_failed_checks == 0
        
    def encode(self, in_data):
        temp = in_data @ self.G

        return temp % 2
