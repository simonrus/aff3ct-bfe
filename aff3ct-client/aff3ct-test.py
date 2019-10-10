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


    success, error_string = Aff3ctProtocol.do_push(zmq_socket, "X", np.array([1]))
