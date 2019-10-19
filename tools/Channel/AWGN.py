import numpy as np

class AWGN:
    @staticmethod
    def add_noise(vector, sigma):  #mod_info, snr!
        
        noise = np.random.normal(loc=0.0,scale=sigma, size=len(vector))
        return vector + noise
