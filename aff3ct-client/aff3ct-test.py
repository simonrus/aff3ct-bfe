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
    success, error_string = Aff3ctProtocol.do_push(zmq_socket, "X", np.array([1]))
