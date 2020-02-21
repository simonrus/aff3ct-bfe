import logging

# for the float_max and float_min
import sys
import numpy as np

np.set_printoptions(edgeitems=30, linewidth=100000, formatter=dict(float=lambda x: "\t%5.3g" % x))

from scipy.sparse import csr_matrix

from .EncoderLDPCFromH import EncoderLDPCFromH
from .Decoder import Decoder

import pdb

np.seterr(divide='ignore', invalid='ignore')

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

        epsilon = sys.float_info.epsilon
        min_llr_value = np.log((epsilon) / (2 - epsilon))
        max_llr_value = np.log((2 - epsilon) / (epsilon))

        for loop in range(0, self.sim_config.ldpc_max_iter):
            # horizontal step
            logging.debug("HORIZ step input")
            self.logging_debug(current_llrs, "current_llrs")

            z = np.tanh(current_llrs / 2)

            for row_index in range(0, self.N - self.K):
                row = self.csrow_H.getrow(row_index)                # can be precalculated (TBD)
                pos_ones = np.where(row.toarray().flatten() == 1)   # can be precalculated (TBD)

                ##
                #
                #                   1 + П tanh(v/2)
                # LLR(sum=odd) =    -------------------
                #                   1 - П tanh(v/2)
                ##

                # KISS version start
                for i in pos_ones[0]:
                    prod = 1
                    for j in pos_ones[0]:
                        if i != j:
                            prod = prod * z[j]
                    ## 
                    if prod == 1.0:
                        pa_mtx[row_index, i] = max_llr_value
                    elif prod == -1:
                        pa_mtx[row_index, i] = min_llr_value
                    else:
                        pa_mtx[row_index, i] = np.log((1 + prod) / (1 - prod))

                # KISS version end


                ''' 
                ##
                # Vectorisation start (a little bit slower )
                # Vectorisation like to count all product and divide by each variable 
                # doesn't make sense
                # because in kiss version we have (w) * (w - 1) multiplication
                # w + (w - 1) division but extra logarithms =)
                #
                prod = np.prod(z[pos_ones])
                pa_mtx[row_index, pos_ones] = np.ones(weight) * prod / z[pos_ones]

                # pdb.set_trace()
                pa_mtx[row_index, pos_ones] = (1 + pa_mtx[row_index, pos_ones])/(1 - pa_mtx[row_index, pos_ones])

                pa_mtx[row_index, pos_ones] = np.log(pa_mtx[row_index, pos_ones])

                for p in pos_ones[0]:
                    if pa_mtx[row_index, p] == np.inf:
                        pa_mtx[row_index, p] = max_llr_value
                    if pa_mtx[row_index, p] == -np.inf:
                        pa_mtx[row_index, p] = min_llr_value

                #pa_mtx[row_index, pos_ones] = np.where(pa_mtx[row_index, pos_ones] == np.inf, max_llr_value, pa_mtx[row_index, pos_ones])
                #pa_mtx[row_index, pos_ones] = np.where(pa_mtx[row_index, pos_ones] == -np.inf, min_llr_value, pa_mtx[row_index, pos_ones])
                # Vectorisation end
                '''



            logging.debug("HORIZ step output")
            self.logging_debug(pa_mtx, "pa_mtx")

            for col_index in range(0, self.N):
                col = self.cscol_H.getcol(col_index)                            # can be precalculated  (TBD)
                pos_ones = np.where(col.toarray().flatten() == 1)               # can be precalculated  (TBD)


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
