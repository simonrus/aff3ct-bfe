import unittest
import numpy as np
from MatrixHandler import *

class MatrixHandlerTest(unittest.TestCase):
    def test_transform_H_to_G_identity(self):
        
        matrix_H = np.array([   [0,0,1,1,1,0,1],
                                [0,1,1,1,0,1,0],
                                [1,1,1,1,1,1,1]])
        handler = MatrixHandler()                         
        G = handler.transform_H_to_G_indentity(matrix_H, None)
        print (G)
        pass
