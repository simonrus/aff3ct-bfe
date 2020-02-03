import os

class Decoder:
    algo = None
    def __init__(self): 
        if "algo" in os.environ:
            self.algo =  os.environ['algo']
        else:
            self.algo = "Default"

    def selected_algorithm(self):
        return self.algo
