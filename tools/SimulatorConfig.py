import os
from configparser import ConfigParser 

class SimulatorConfig   :
    config_path = None
    def __init__(self, config_path): 
        self.config_path = config_path

        self.prepareParser(config_path)
        
        ## start parsing
        self.parseGeneralSettings()
        if self.family == "LDPC":
            self.parseLDPCSettings()

    def prepareParser(self, config_path):               
        self.configur = ConfigParser() 
        print (self.configur.read(config_path)) 
        print ("Sections : ", self.configur.sections()) 


    def parseGeneralSettings(self):
        self.family = self.configur.get('General','family');                            print ("Code Family : ", self.family) 
        self.log_errors = self.configur.getboolean('General','log_errors');             print ("Log Errors debugged ? : ", self.log_errors) 
        self.ebn0_start = self.configur.getfloat('General','ebn0_start');               print ("ebn0_start ? : ", self.ebn0_start) 
        self.ebn0_end = self.configur.getfloat('General','ebn0_end');                   print ("ebn0_end ? : ", self.ebn0_end) 
        self.ebn0_step =  self.configur.getfloat('General','ebn0_step');                print ("ebn0_step ? : ",self.ebn0_step ) 
        self.desired_accuracy = self.configur.getfloat('General','desired_accuracy');   print ("desired_accuracy ? : ", self.desired_accuracy) 
        self.decoder = self.configur.get('General','decoder');         print ("decodern : ", self.decoder)
        #print ("Port Server : ", configur.getint('server','port')) 
    
    def parseLDPCSettings(self):       
        self.ldpc_max_iter = self.configur.getint('LDPC','max_iter');    print ("Max numIteration : ", self.ldpc_max_iter) 
        

    

    
