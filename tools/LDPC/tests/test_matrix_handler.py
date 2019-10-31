import unittest
import numpy as np
import sys
sys.path.append('..')

from AList.AListReader import AListReader


from MatrixHandler import *

class MatrixHandlerTest(unittest.TestCase):
    def test_transform_H_to_G_identity(self):
        
        matrix_H = np.array([[0, 0, 1, 1, 1, 0, 1],
                             [0, 1, 1, 1, 0, 1, 0],
                             [1, 1, 1, 1, 1, 1, 1]])
        handler = MatrixHandler()                         
        G = handler.transform_H_to_G_indentity(matrix_H, None)


    def test_transform_H_to_G_identity_ccsds_128_64(self):
        reader = AListReader()

        K, N = reader.readFromFile("/home/simon/work/phd/missfec/lib/aff3ct/conf/dec/LDPC/CCSDS_64_128.alist")

        h_transposed = np.transpose(reader.matrix)

        handler = MatrixHandler()
        G = handler.transform_H_to_G_indentity(h_transposed, None)


    def test_transform_H_to_G_identity_ccsds_64_32(self):
        reader = AListReader()

        '''
        I2 + I3         I1          I0          I2          I0          I3          0           I0              8
        I             I0 + I1       I0          I0          I0          I0          I0          0               8
        I3              I0        I0 + I2       I0          0           I0          I1          I0              8
        I2              I0          I0        I0 + I3       I0'         0'          I0'         I0'             17
        
        5555            5555        5555        5555        4443        3333        4443        4443
        where IX' - idenitiy matrix shifted to X positions right + last row is set to 1s
        '''
        K, N = reader.readFromFile("../../codes/ldpc/CCSDS_ldpc_n32_k16.alist")

        h_transposed = np.transpose(reader.matrix)

        handler = MatrixHandler()

        G = handler.transform_H_to_G_indentity(h_transposed, None)
