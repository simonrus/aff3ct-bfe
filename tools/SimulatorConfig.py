import os
from configparser import ConfigParser 

class SimulatorConfig   :
    config_path = None
    def __init__(self, sim_args):

        config_parser = self.get_config_parser(sim_args.config)
        
        ## start parsing
        self.parse_general_section(config_parser)
        if self.family == "LDPC":
            self.parse_ldpc_section(config_parser)

        self.override_from_args(sim_args)

    @staticmethod
    def get_config_parser(config_path):
        config_parser = ConfigParser()
        print(config_parser.read(config_path))
        print("Sections : ", config_parser.sections())
        return config_parser


    def parse_general_section(self, config_parser):
        self.family = config_parser.get('General', 'family');                            print ("Code Family : ", self.family)
        self.log_errors = config_parser.getboolean('General', 'log_errors');             print ("Log Errors debugged ? : ", self.log_errors)
        self.ebn0_start = config_parser.getfloat('General', 'ebn0_start');               print ("ebn0_start ? : ", self.ebn0_start)
        self.ebn0_end = config_parser.getfloat('General', 'ebn0_end');                   print ("ebn0_end ? : ", self.ebn0_end)
        self.ebn0_step =  config_parser.getfloat('General', 'ebn0_step');                print ("ebn0_step ? : ", self.ebn0_step)
        self.desired_accuracy = config_parser.getfloat('General', 'desired_accuracy');   print ("desired_accuracy ? : ", self.desired_accuracy)
        self.decoder = config_parser.get('General', 'decoder');                          print ("decoder : ", self.decoder)
        self.h_matrix = config_parser.get('General', 'h_matrix');                        print ("h_matrix : ", self.h_matrix)
        #print ("Port Server : ", configur.getint('server','port'))



    def parse_ldpc_section(self, config_parser):
        self.ldpc_max_iter = config_parser.getint('LDPC', 'max_iter');    print ("Max numIteration : ", self.ldpc_max_iter)

    @staticmethod
    def describe_override_arguments(args):
        args.add_argument("-m", type=float, help="[config override] ebn0_start")
        args.add_argument("-M", type=float, help="[config override] ebn0_end")
        args.add_argument("-s", type=float, help="[config override] ebn0_step")
        args.add_argument("--accuracy", type=float, help="[config override] ebn0_step")

    def override_from_args(self, args):
        if (args.m):
            self.ebn0_start = args.m

        if (args.M):
            self.ebn0_end = args.M

        if (args.s):
            self.ebn0_stop = args.s

        if (args.accuracy):
            self.desired_accuracy = args.accuracy;

    
