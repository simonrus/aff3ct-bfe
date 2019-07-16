from pb import aff3ct_pb2
import pdb

class Aff3ctProtocol:
    def __init__(self):
        pass

    @staticmethod
    def serialize_matrix(matrix):
        pb_matrix = aff3ct_pb2.Matrix()
        print ("matrix is ")
        print (matrix.shape)
        print(matrix.ndim)
        if matrix.ndim == 1:
            pb_matrix.n = 1
            pb_matrix.m = matrix.shape[0]

        else:
            pb_matrix.n = matrix.shape[0]
            pb_matrix.m = matrix.shape[1]

        pb_matrix.values.extend(matrix.flatten().tolist())

        return pb_matrix

    @staticmethod
    def do_push(socket, var, matrix):

        request = aff3ct_pb2.PushRequest(mtx = Aff3ctProtocol.serialize_matrix(matrix))
        request.var = var

        message_pb = aff3ct_pb2.Message(pushRequest=request)
        stream = message_pb.SerializeToString()

        socket.send(stream)

        reply = socket.recv()

        message_pb.ParseFromString(reply)

        message_type = message_pb.WhichOneof('content')
        if message_type != 'result':
            return False, "received object of " + message_type + " instead of result"

        if message_pb.result.type == aff3ct_pb2.ResultType.Value('Success'):
            return True, ''
        else:
            return False, message_pb.result.error_text

    @staticmethod
    def do_pull(socket, var_name, to):

        request = aff3ct_pb2.PullRequest(var = var_name)

        message_pb = aff3ct_pb2.Message(pullRequest=request)
        stream = message_pb.SerializeToString()

        socket.send(stream)

        reply = socket.recv()

        message_pb.ParseFromString(reply)

        message_type = message_pb.WhichOneof('content')
        if message_type != 'pullReply':
            return False, "received object of " + message_type + " instead of pullReply"

        if message_pb.pullReply.result.type == aff3ct_pb2.ResultType.Value('Success'):
            return True, ''
        else:
            return False, message_pb.pullReply.result.error_text