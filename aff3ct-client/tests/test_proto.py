import unittest


import zmq
import pdb
import numpy as np
from Aff3ctProtocol import Aff3ctProtocol

class test_proto(unittest.TestCase):
    zmq_context = None
    zmq_socket = None
    server_address = 'tcp://localhost:5555'

    def __init__(self, *args, **kwargs):
        super(test_proto, self).__init__(*args, **kwargs)
        self.connect()
        self.init()

    def connect(self):
        self.zmq_context = zmq.Context()

        print("Connecting to aff3ct-server ..." + self.server_address, end=" ")
        self.zmq_socket = self.zmq_context.socket(zmq.REQ)
        self.zmq_socket.connect(self.server_address)
        print("done")

    def init(self):
        print("initialize aff3ct ...", end=" ")
        init_command = "init -C LDPC -K 4 -N 7 --enc-type LDPC_H --enc-g-method IDENTITY --dec-h-path codes/ldpc/BCH_7_4_1_strip.alist --dec-type BP_FLOODING --dec-implem GALA -i 200"
        success, error_string = Aff3ctProtocol.do_exec(self.zmq_socket, init_command)
        self.assertTrue(success)
        print("done")

    ##################################################################################################################################
    ## here starts all tests
    ## every test do connect and init 
    ##################################################################################################################################

    def test_pushpull(self):
        # test push poll for single values
        orig = np.array([10.0])
        success, error_string = Aff3ctProtocol.do_push(self.zmq_socket, "X", orig)
        self.assertTrue(success)
        success, error_string, got = Aff3ctProtocol.do_pull(self.zmq_socket, "X")
        self.assertTrue(success)
        self.assertTrue(np.allclose(orig, got))

        # test push poll for vector with numbers
        orig = np.array([1.1,2.2,3.3])
        success, error_string = Aff3ctProtocol.do_push(self.zmq_socket, "X", orig)
        self.assertTrue(success)
        success, error_string, got = Aff3ctProtocol.do_pull(self.zmq_socket, "X")
        self.assertTrue(success)
        self.assertTrue(np.allclose(orig, got))
    
        # test push poll for vector with numbers and NaNs
        orig = np.array([1.1,np.NaN,3.3])
        success, error_string = Aff3ctProtocol.do_push(self.zmq_socket, "X", orig)
        self.assertTrue(success)
        success, error_string, got = Aff3ctProtocol.do_pull(self.zmq_socket, "X")
        self.assertTrue(success)
        self.assertTrue(np.allclose(orig, got, equal_nan=True))



