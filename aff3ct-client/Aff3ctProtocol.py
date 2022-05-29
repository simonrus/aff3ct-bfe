import numpy as np

import flatbuffers
import aff3ct.proto.Action
import aff3ct.proto.Matrix
import aff3ct.proto.Message
import aff3ct.proto.Result
import aff3ct.proto.ResultType

import pdb


class Aff3ctProtocol:
    def __init__(self):
        pass

    @staticmethod
    def serialize_matrix(builder, matrix):
        values = builder.CreateNumpyVector(matrix.flatten().astype(np.float32))
        aff3ct.proto.Matrix.MatrixStart(builder)

        if matrix.ndim == 1:
            aff3ct.proto.Matrix.MatrixAddN(builder, 1)
            aff3ct.proto.Matrix.MatrixAddM(builder, matrix.shape[0])
            num_elements = matrix.shape[0]
        else:
            aff3ct.proto.Matrix.MatrixAddN(builder, matrix.shape[0])
            aff3ct.proto.Matrix.MatrixAddM(builder, matrix.shape[1])
            num_elements = matrix.shape[0] * matrix.shape[1]

        aff3ct.proto.Matrix.MatrixAddValues(builder, values)

        matrix_item = aff3ct.proto.Matrix.MatrixEnd(builder)

        return matrix_item


    @staticmethod
    def deserialize_matrix(fb_matrix):
        
        matrix = fb_matrix.ValuesAsNumpy();
        if fb_matrix.N() == 1:
            return matrix
        else:
            return matrix.reshape(fb_matrix.N(), fb_matrix.M())

    @staticmethod
    def do_push(socket, var, matrix):
        # send request
        builder = flatbuffers.Builder(1024)

        matrix_item = Aff3ctProtocol.serialize_matrix(builder, matrix)
        var_item = builder.CreateString(var)

        aff3ct.proto.Message.MessageStart(builder)
        aff3ct.proto.Message.MessageAddAction(builder, aff3ct.proto.Action.Action().Push)
        aff3ct.proto.Message.MessageAddMatrix(builder, matrix_item)
        aff3ct.proto.Message.MessageAddText(builder, var_item)
        message = aff3ct.proto.Message.MessageEnd(builder)

        builder.Finish(message)
        socket.send(builder.Output())

        # wait for response
        reply = socket.recv()

        #proccess response
        reply = bytearray(reply)
        message = aff3ct.proto.Message.Message.GetRootAsMessage(reply, 0)

        result = message.Result()

        if result is None:
            return False, "do_push() didn't receive Result"

        if result.Type() == aff3ct.proto.ResultType.ResultType.Success :
            return True, ''
        else:
            return False, result.ErrorText()

    @staticmethod
    def do_pull(socket, var_name):
        
        # send request
        builder = flatbuffers.Builder(1024)
        var_item = builder.CreateString(var_name)
        
        aff3ct.proto.Message.MessageStart(builder)
        aff3ct.proto.Message.MessageAddAction(builder, aff3ct.proto.Action.Action().Pull)
        aff3ct.proto.Message.MessageAddText(builder, var_item)
        message = aff3ct.proto.Message.MessageEnd(builder)

        builder.Finish(message)
        socket.send(builder.Output())

        # wait for response
        reply = socket.recv()

        #proccess response
        reply = bytearray(reply)
        message = aff3ct.proto.Message.Message.GetRootAsMessage(reply, 0)

        result = message.Result()
        
        if result is None:
            return False, "do_pull() didn't receive Result"

        if result.Type() == aff3ct.proto.ResultType.ResultType.Success:

            return True, '', Aff3ctProtocol.deserialize_matrix(message.Matrix())
        else:
            return False, message.pullReply.result.error_text, None

    @staticmethod
    def do_exec(socket, command):
        if isinstance(command, list):
            command = ' '.join(command)

        if not isinstance(command, str):
            return False, "do_command accepts now strings"

        # send request
        builder = flatbuffers.Builder(1024)
        command_item = builder.CreateString(command)
        
        aff3ct.proto.Message.MessageStart(builder)
        aff3ct.proto.Message.MessageAddAction(builder, aff3ct.proto.Action.Action().Exec)
        aff3ct.proto.Message.MessageAddText(builder, command_item)
        message = aff3ct.proto.Message.MessageEnd(builder)

        builder.Finish(message)
        socket.send(builder.Output())

        # wait for response
        reply = socket.recv()

        #proccess response
        reply = bytearray(reply)
        message = aff3ct.proto.Message.Message.GetRootAsMessage(reply, 0)

        result = message.Result()

        if result is None:
            return False, "do_command() didn't receive Result"

        if result.Type() == aff3ct.proto.ResultType.ResultType.Success :
            return True, 'Command Executed'
        else:
            return False, result.ErrorText().decode("utf-8")

