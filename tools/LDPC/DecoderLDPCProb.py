import numpy as np
from .MatrixHandler import MatrixHandler

import pdb

class DecoderLDPCProb:

    H = None

    info_bits_position = None

    K = 0
    N = 0
    sigma = np.nan

    # constructor
    def __init__(self, H):
        self.H = H
        self.N = H.shape[1]
        self.K = self.N - H.shape[0]

    def set_sigma(self, sigma):
        self.sigma = sigma

    def decode(self, received):
        assert not np.isnan(self.sigma)

        pass