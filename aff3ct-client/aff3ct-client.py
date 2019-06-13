import argparse
import sys
import cmd2
import numpy as np


class ValueParser:
    """ fills numpy matrix """

    value = None
    error = None

    @staticmethod
    def read_scalar(str_value):
        return float(str_value)

    @staticmethod
    def is_scalar(str_value):
        try:
            float(str_value)
            return True
        except ValueError:
            return  False

    def reset(self):
        self.value = None
        self.error = None

    def is_error(self):
        return self.error is not None

    def get_error_text(self):
        return self.error

    def get_value(self):
        return self.value

    def parse(self, params):

        if len(params) == 1:  ##Scalar value is passed
            try:
                temp = ValueParser.read_scalar(params[0])
                self.value = np.array([temp], dtype='float')
            except ValueError:
                self.error = ("2nd argument (%s)) shall be a numeric value" % args.params[0])

            return False

        if ',' not in params[0] and \
                ',' not in params[1] and \
                ',' not in params[2]:

            ## Vector case
            try:
                vec_size = int(params)
            except ValueError:
                self.error = "vector size is incorrect"
                return False

            if '.' in params:
                self.error = "vector size shall be integer"

            if vec_size <= 1:
                self.error = "vector size shall be positive and not a scalar"

            ## build matrix
            #value = np.array(1,

        return True


class Aff3ctClient(cmd2.Cmd):
    """ Main loop"""
    Parser = None
    Registers = None

    def __init__(self):

        # Set use_ipython to True to enable the "ipy" command which embeds and interactive IPython shell
        super().__init__(use_ipython=False, multiline_commands=['set'])
        self.Parser = ValueParser()
        self.Registers = {}

    # override standard set
    do_local_set = cmd2.Cmd.do_set

    ################### do speak #################
    speak_parser = argparse.ArgumentParser()
    speak_parser.add_argument('-p', '--piglatin', action='store_true', help='atinLay')
    speak_parser.add_argument('-s', '--shout', action='store_true', help='N00B EMULATION MODE')
    speak_parser.add_argument('-r', '--repeat', type=int, help='output [n] times')
    speak_parser.add_argument('words', nargs='+', help='words to say')

    @cmd2.with_argparser(speak_parser)
    def do_speak(self, args):
        self.poutput("Speak is entered " + str(args.shout))

    ################### set matrix #################
    set_parser = argparse.ArgumentParser()
    set_parser.add_argument('VAR', help='variable name')
    set_parser.add_argument('params', nargs='+', help='params')
    @cmd2.with_argparser(set_parser)
    def do_set(self, args):
        # set VARIABLE nRows,nCols val00 value01 ... val10 val11 ...
        # set VARIABLE nSize val0 val1 val2'''

        self.poutput('VAR is ' + args.VAR)

        if args.VAR.islower():
            self.poutput("1st arg shall be a Variable Name (with capital letter)")
            return

        self.Parser.reset()
        self.Parser.parse(args.params)

        if self.Parser.is_error():
            self.poutput(self.Parser.get_error_text())
            return

        value = self.Parser.get_value()

        self.poutput(np.array2string(value, precision=2, separator=',',suppress_small=True))

        pass


if __name__ == "__main__":
    app = Aff3ctClient()
    sys.exit(app.cmdloop())