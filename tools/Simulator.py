import os

class Simulator:
    sim = None
    def __init__(self): 
        if "sim" in os.environ:
            self.sim =  os.environ['sim']
        else:
            self.sim = "Default"

    def selected_simulator(self):
        return self.sim
