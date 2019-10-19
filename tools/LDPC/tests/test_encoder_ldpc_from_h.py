import unittest
import numpy as np
from EncoderLDPCFromH import *
from MatrixHandler import *

class EncoderLDPCFromHTest(unittest.TestCase):
    def test_encode(self):
        H = np.asarray([    [1,0,1,1,1,0,0],
                            [0,1,0,1,1,1,0],
                            [1,1,1,1,1,1,1]], dtype=int)
        
        encoder = EncoderLDPCFromH(H)
        
        vector = np.asarray([1,0,1,1], dtype=int)
        
        codeword = encoder.encode(vector)
        print("Encoder finished")
        print(vector)
        print(codeword)

        self.assertTrue(encoder.is_codeword(codeword))
        pass
