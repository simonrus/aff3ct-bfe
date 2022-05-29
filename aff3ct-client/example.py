import zmq
import pdb
import numpy as np
from Aff3ctProtocol import Aff3ctProtocol

if __name__ == "__main__":
    zmq_context = None
    zmq_socket = None
    server_address = 'tcp://localhost:5555'

    # Set use_ipython to True to enable the "ipy" command which embeds and interactive IPython shell

    zmq_context = zmq.Context()

    print("Connecting to aff3ct-server..." + server_address)
    zmq_socket = zmq_context.socket(zmq.REQ)
    zmq_socket.connect(server_address)

    #pdb.set_trace()
    command1 = "init -C LDPC -K 4 -N 7 --enc-type LDPC_H --enc-g-method IDENTITY --dec-h-path codes/ldpc/BCH_7_4_1_strip.alist --dec-type BP_FLOODING --dec-implem GALA -i 200"
    success, error_string = Aff3ctProtocol.do_exec(zmq_socket, command1)

    ## test case for an value
    orig = np.array([10.0])
    success, error_string = Aff3ctProtocol.do_push(zmq_socket, "X", orig)
    success, error_string, got = Aff3ctProtocol.do_pull(zmq_socket, "X")
    assert np.allclose(orig, got), "Test case1 failed"

    ## test case for vector 
    orig = np.array([1.0, 2.0, 3.0])
    success, error_string = Aff3ctProtocol.do_push(zmq_socket, "VEC", orig)
    success, error_string, got = Aff3ctProtocol.do_pull(zmq_socket, "VEC")

    assert np.allclose(orig, got, equal_nan=True), "Test case2 failed"


