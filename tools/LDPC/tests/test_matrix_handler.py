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
        print (G)
        pass

    def test_transform_H_to_G_identity_ccsds_128_64(self):
        reader = AListReader()

        K, N = reader.readFromFile("/home/simon/work/phd/missfec/lib/aff3ct/conf/dec/LDPC/CCSDS_64_128.alist")

        print("LDPC K=%d, N=%d" % (K, N))

        h_transposed = np.transpose(reader.matrix)

        handler = MatrixHandler()
        G = handler.transform_H_to_G_indentity(h_transposed, None)
        pass
