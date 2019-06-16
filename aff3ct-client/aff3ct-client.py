import argparse
import sys
import cmd2
import numpy as np

from ConsoleMatrixParser import ConsoleMatrixParser


class Aff3ctClient(cmd2.Cmd):
    """ Main loop"""
    Parser = None
    Registers = None

    def report_done(self):
        self.poutput("+OK")
        pass

    def report_failed(self, error_str=''):
        if len(error_str):
            self.poutput("-Failed with error: \"" + error_str + "\"")
        else:
            self.poutput("-Failed")
        pass

    def __init__(self):

        # Set use_ipython to True to enable the "ipy" command which embeds and interactive IPython shell
        super().__init__(use_ipython=False, multiline_commands=['push'])
        self.Parser = ConsoleMatrixParser()
        self.Registers = {}

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

        self.Parser.reset()
        self.Parser.parse(args.params)

        if self.Parser.is_error():
            self.report_failed(self.Parser.get_error_text())
            return

        self.Registers[args.VAR] = self.Parser.get_value()
        self.report_done()

    def do_list(self, args):
        # just list available args
        names = ''
        for key in self.Registers:
            names += ' ' + key

        self.poutput(names)
        self.report_done()


if __name__ == "__main__":
    app = Aff3ctClient()
    sys.exit(app.cmdloop())