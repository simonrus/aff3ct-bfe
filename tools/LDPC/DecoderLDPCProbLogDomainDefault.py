import logging

# for the float_max and float_min
import sys
import numpy as np

np.set_printoptions(edgeitems=30, linewidth=100000, formatter=dict(float=lambda x: "\t%5.3g" % x))

from scipy.sparse import csr_matrix
from scipy.sparse import csc_matrix

from .MatrixHandler import MatrixHandler
from .EncoderLDPCFromH import EncoderLDPCFromH
from .Decoder import Decoder

import pdb

class DecoderLDPCProbLogDomainDefault(Decoder):
    H = None

    info_bits_position = None

    K = 0
    N = 0
    sigma = np.nan

    # constructor
    def __init__(self, H, sim_config):
        Decoder.__init__(self)
        self.H = H
        self.N = H.shape[1]
        self.K = self.N - H.shape[0]
        self.csrow_H = csr_matrix(H)
        self.cscol_H = csr_matrix(H)
        self.encoder = EncoderLDPCFromH(H)
        self.print_output = False
        self.sim_config = sim_config

    def set_sigma(self, sigma):
        self.sigma = sigma

    def logging_debug(self, pa_array, msg=None):
        if logging.getLogger().getEffectiveLevel() == logging.DEBUG:
            prefix = (msg + "\n") if (msg is not None) else "\n"

            logging.debug(prefix + np.array2string(pa_array, precision=2, separator=',', suppress_small=True))
        pass

    def decode(self, received):
        assert not np.isnan(self.sigma), "sigma is not set"

        logging.debug("received is")
        self.logging_debug(received)

        input_llrs = 2 * received / self.sigma / self.sigma
        current_llrs = input_llrs.copy()
        
        pa_mtx = np.zeros(self.H.shape)

        min_llr_value = np.log(sys.float_info.min)
        max_llr_value = np.log(sys.float_info.max)
        
        for loop in range(0, self.sim_config.ldpc_max_iter):

            # horizontal step
            logging.debug("HORIZ step input")
            self.logging_debug(current_llrs, "current_llrs")
            
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

                ''' KISS version'''
                for i in pos_ones[0]:
                    prod = 1
                    for j in pos_ones[0]:
                        if (i != j):
                            prod = prod * np.tanh(current_llrs[j] / 2)
                    ## 
                    if prod == 1.0:
                        pa_mtx[row_index, i] = max_llr_value
                    elif prod == -1:
                        pa_mtx[row_index, i] = min_llr_value
                    else:
                        pa_mtx[row_index, i] = np.log((1 + prod) / (1 - prod))
                    
                ##
                # Vectorisation
                # Z = np.tanh(current_llrs[pos_ones])
                # prod = np.prod(np.tanh(current_llrs[pos_ones]/2))
                # pa_mtx[row_index, pos_ones] = np.ones(weight) * prod / Z[pos_ones]
                # pa_mtx[row_index, pos_ones] = (1 + pa_mtx[row_index, pos_ones])/(1 - pa_mtx[row_index, pos_ones])
                # pa_mtx[row_index, pos_ones] = np.log(pa_mtx[row_index, pos_ones])
                # shall be ok

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
                value = current_llrs[col_index] + sum_value

                current_llrs[col_index] = value                     # assign new value

            
            candidate = current_llrs > 0
            
            if self.encoder.is_codeword(candidate):
                return candidate * 1

        return None
