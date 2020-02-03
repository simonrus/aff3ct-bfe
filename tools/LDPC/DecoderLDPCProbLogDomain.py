import numpy as np

np.set_printoptions(edgeitems=30, linewidth=100000, formatter=dict(float=lambda x: "\t%5.3g" % x))

import logging
from scipy.sparse import csr_matrix
from scipy.sparse import csc_matrix
import sys # for the float_max and float_min
from .MatrixHandler import MatrixHandler
from .EncoderLDPCFromH import EncoderLDPCFromH
from .Decoder import Decoder
import pdb

class DecoderLDPCProbLogDomain(Decoder):
    H = None

    info_bits_position = None

    K = 0
    N = 0
    sigma = np.nan

    max_num_iter = 1

    # constructor
    def __init__(self, H):
        Decoder.__init__(self)
        self.H = H
        self.N = H.shape[1]
        self.K = self.N - H.shape[0]
        self.csrow_H = csr_matrix(H)
        self.cscol_H = csr_matrix(H)
        self.encoder = EncoderLDPCFromH(H)
        self.print_output = False

    def set_sigma(self, sigma):
        self.sigma = sigma

    def logging_debug(self, pa_array, msg=None):
        if logging.getLogger().getEffectiveLevel() == logging.DEBUG:
            prefix = (msg + "\n") if (msg is not None) else "\n"
            
            logging.debug(prefix + np.array2string(pa_array, precision=2, separator=',',suppress_small=True))

    def decodeInProbLogDomain(self, received):
        assert not np.isnan(self.sigma), "sigma is not set"

        logging.debug("received is")
        self.logging_debug(received)


        LLRs = 2 * received / self.sigma / self.sigma
        
        #pa_prob_ones = prob_ones.copy()
        current_LLRs = LLRs.copy()
        pa_mtx = np.zeros(self.H.shape)

        minLLR = np.log(sys.float_info.min)
        maxLLR = np.log(sys.float_info.max)
        
        for iter in range(0,self.max_num_iter):
            #horizontal step

            logging.debug("HORIZ step input")
            self.logging_debug(current_LLRs, "current_LLRs")
            
            for row_index in range(0, self.N - self.K):
                row = self.csrow_H.getrow(row_index)
                weight = row.sum()

                pos_ones = np.where(row.toarray().flatten() == 1)

                ##
                #
                #                   1 + П tanh(v/2)
                # LLR(sum=odd) =    -------------------
                #                   1 - П tanh(v/2)
                ##

                
                ## KISS version
                 
                for i in pos_ones[0]:
                    prod = 1
                    for j in pos_ones[0]:
                        if (i != j):
                            prod = prod * np.tanh(current_LLRs[j] / 2)
                    ## 
                    if prod == 1.0:
                        pa_mtx[row_index, i] = maxLLR
                    elif prod == -1:
                        pa_mtx[row_index, i] = minLLR
                    else:
                        pa_mtx[row_index, i] = np.log((1 + prod) / (1 - prod))
                    

                ## Vectorizing
                #Z = np.tanh(current_LLRs[pos_ones])
                #prod = np.prod(np.tanh(current_LLRs[pos_ones]/2))
                #pa_mtx[row_index, pos_ones] = np.ones(weight) * prod / Z[pos_ones]
                #pa_mtx[row_index, pos_ones] = (1 + pa_mtx[row_index, pos_ones])/(1 - pa_mtx[row_index, pos_ones])
                #pa_mtx[row_index, pos_ones] = np.log(pa_mtx[row_index, pos_ones])
                #shall be ok

            
            logging.debug("HORIZ step output")
            self.logging_debug(pa_mtx, "pa_mtx")
            

            for col_index in range(0, self.N):
                col = self.cscol_H.getcol(col_index)
                weight = col.sum()

                #pdb.set_trace()
                pos_ones = np.where(col.toarray().flatten() == 1)


                ##
                #                   
                # LLR(sum=odd) = LLR0 + sum(LLR_i)    
                #                   
                ##<

                temp = pa_mtx[pos_ones, col_index]
                sum_value = np.sum(temp)
                value = current_LLRs[col_index] + sum_value

                current_LLRs[col_index] = value #assign new value

            
            candidate = current_LLRs > 0
            
            if (self.encoder.is_codeword(candidate)):
                return candidate * 1

        logging.error("FAILED to decode")
        return None



        pass

    def decode(self, likelihoods):
        one_prob = []
