from pb import aff3ct_pb2


class Aff3ctProtocol:
    def __init__(self):
        pass

    @staticmethod
    def serialize_matrix(matrix):
        matrix = aff3ct_pb2.Matrix()
        matrix.n = 2
        matrix.m = 2
        matrix.values.extend([1.0, 2.0, 3.0, 4.0])
        return matrix


    def do_push(self, var):
        request = aff3ct_pb2.PushRequest(mtx=Aff3ctProtocol.serialize_matrix(None))
        request.var = var

        message = aff3ct_pb2.Message(pushRequest=request)
        stream = message.SerializeToString()

        return True, ''
