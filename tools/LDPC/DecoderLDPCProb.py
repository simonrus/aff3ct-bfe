from os import waitid_result

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
        assert not np.isnan(self.sigma), "sigma is not set"

        print("received is ", received)

        likelihoods = np.exp(2 * received / self.sigma / self.sigma)
        prob_ones = likelihoods / (likelihoods + 1)

        #Test code
        #prob_ones = np.array(range(0,received.shape[0]))
        #prob_ones = prob_ones / sum(prob_ones)

        pa_prob_ones = prob_ones.copy()
        pa_mtx = np.zeros(self.H.shape)

        pdb.set_trace()
        print("prob_ones ", prob_ones)

        for iter in range(0,self.max_num_iter):
            #horizontal step

            print("HORIZ step input")
            print(pa_prob_ones)
            for row_index in range(0, self.N - self.K):
                row = self.csrow_H.getrow(row_index)
                weight = row.sum()

                pos_ones = np.where(row.toarray().flatten() == 1)


                ##
                #
                #                   1 + П (1 - 2 * P_t)
                # P(sum=odd) =      -------------------
                #                           2
                ##<

                temp = 1 - 2 * pa_prob_ones
                prod = np.prod(temp[pos_ones])
                pa_mtx[row_index, pos_ones] = np.ones(weight) * prod / temp[pos_ones]
                pa_mtx[row_index, pos_ones] = (pa_mtx[row_index, pos_ones] + 1)/2

                #shall be ok

            print("HORIZ step output")
            print(pa_mtx)

            for col_index in range(0, self.N):
                col = self.cscol_H.getcol(col_index)
                weight = col.sum()

                #pdb.set_trace()
                pos_ones = np.where(col.toarray().flatten() == 1)


                ##
                #
                #                   P(C) * П(Xn | c)
                # P(C|X1 .. Xn) =   -------------------
                #                      P(X1..Xn)
                ##<

                temp = pa_mtx[pos_ones, col_index]
                prod = np.prod(temp)
                value = pa_prob_ones[col_index] * (1 << weight) * prod

                pa_prob_ones[col_index] = value #assign new value

        print("DONE")
        print(pa_prob_ones)



        pass

    def decode(self, likelihoods):
        one_prob = []