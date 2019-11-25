import numpy as np
from scipy.sparse import csr_matrix
from scipy.sparse import csc_matrix


from math import exp

from .MatrixHandler import MatrixHandler

import pdb

class DecoderLDPCProb:

    H = None

    info_bits_position = None

    K = 0
    N = 0
    sigma = np.nan

    max_num_iter = 1

    # constructor
    def __init__(self, H):
        self.H = H
        self.N = H.shape[1]
        self.K = self.N - H.shape[0]
        self.csrow_H = csr_matrix(H)
        self.cscol_H = csr_matrix(H)

    def set_sigma(self, sigma):
        self.sigma = sigma

    def decodeInProbDomain(self, received):
        assert not np.isnan(self.sigma)

        print("received is ", received)

        likelihoods = np.exp(2 * received / self.sigma / self.sigma)
        prob_ones = likelihoods / (likelihoods + 1)


        print("prob_ones ", prob_ones)

        for iter in range(0,self.max_num_iter):
            #horizontal step
            pdb.set_trace()
            for row_index in range(1, self.N - self.K):
                row = self.csrow_H.getrow(row_index)
                print(row)




        pass

    def decode(self, likelihoods):
        one_prob = []