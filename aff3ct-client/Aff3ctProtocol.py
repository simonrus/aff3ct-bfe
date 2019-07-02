from pb import aff3ct_pb2


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
        ret_value = True
        request = aff3ct_pb2.PushRequest(mtx = Aff3ctProtocol.serialize_matrix(matrix))
        request.var = var
        #request.m

        message_pb = aff3ct_pb2.Message(pushRequest=request)
        stream = message_pb.SerializeToString()
        print('message send')
        socket.send(stream)

        reply = socket.recv()

        message_pb.ParseFromString(reply)

        if message_pb.WhichOneof('content') != 'result':
            return False, "received object of " + field + " instead of result"

        if message_pb.result.type == aff3ct_pb2.ResultType.Value('Success'):
            return True, ''
        else:
            return False, message_pb.result.error_text

