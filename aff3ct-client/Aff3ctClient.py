import argparse
import cmd2
import zmq
import pdb
from Aff3ctProtocol import Aff3ctProtocol

from ConsoleMatrixParser import ConsoleMatrixParser


class Aff3ctClient(cmd2.Cmd):
    """ Main loop"""
    parser = None
    registers = None
    zmq_context = None
    server_address = 'tcp://localhost:5555'
    zmq_socket = None

    def report_done(self):
        self.poutput("+(REMOTE) OK")
        pass

    def report_failed(self, error_str=''):
        if len(error_str):
            self.poutput("-(REMOTE): Failed with error: \"" + error_str + "\"")
        else:
            self.poutput("-(REMOTE): Failed")
        pass

    def __init__(self):
        # Set use_ipython to True to enable the "ipy" command which embeds and interactive IPython shell
        super().__init__(use_ipython=False, multiline_commands=['push'])
        self.parser = ConsoleMatrixParser()
        self.registers = {}
        self.zmq_context = zmq.Context()

        print("Connecting to aff3ct-server..." + self.server_address)
        self.zmq_socket = self.zmq_context.socket(zmq.REQ)
        self.zmq_socket.connect(self.server_address)

    set_parser = argparse.ArgumentParser()
    set_parser.add_argument('VAR', help='variable name')
    set_parser.add_argument('params', nargs='+', help='params')
    @cmd2.with_argparser(set_parser)
    def do_push(self, args):
        # push VARIABLE nRows,nCols val00 value01 ... val10 val11 ...
        # push VARIABLE nSize val0 val1 val2'''

        if args.VAR.islower():
            self.report_failed("1st arg shall be a Variable Name (with capital letter)")
            return

        self.parser.reset()
        self.parser.parse(args.params)

        if self.parser.is_error():
            self.report_failed(self.parser.get_error_text())
            return

        self.registers[args.VAR] = self.parser.get_value()

        ##Now push
        success, error_string = Aff3ctProtocol.do_push(self.zmq_socket, args.VAR, self.registers[args.VAR])

        if not success:
            self.report_failed(error_string)
        else:
            self.report_done()

    set_parser = argparse.ArgumentParser()
    set_parser.add_argument('VAR', help='variable name')
    @cmd2.with_argparser(set_parser)
    def do_pull(self, args):
        # pull VARIABLE

        if args.VAR.islower():
            self.report_failed("1st arg shall be a Variable Name (with capital letter)")
            return

        success, error_string, matrix = Aff3ctProtocol.do_pull(self.zmq_socket, args.VAR)
        if not success:
            self.report_failed(error_string)
        else:
            self.registers[args.VAR] = matrix
            self.report_done()

    set_parser = argparse.ArgumentParser()
    set_parser.add_argument('VAR', help='variable name')
    @cmd2.with_argparser(set_parser)
    def do_print(self, args):
        if args.VAR.islower():
            self.report_failed("1st arg shall be a Variable Name (with capital letter)")
            return

        if args.VAR not in self.registers:
            self.report_failed("Variable is not found")
            return

        self.poutput(str(self.registers[args.VAR]))

    @cmd2.with_argument_list
    def do_locals(self, args):
        # just list available vars at local side
        names = ''
        for key in self.registers:
            names += ' ' + key

        self.poutput(names)

    @cmd2.with_argument_list
    def do_cmd(self, args):
        """ List item command help """

        success, error_string = Aff3ctProtocol.do_command(self.zmq_socket, args)

        if not success:
            self.report_failed(error_string)
        else:
            self.report_done()

