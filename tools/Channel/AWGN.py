import numpy as np

class AWGN:
    @staticmethod
    def add_noise(vector, sigma):  #mod_info, snr!
        
        noise = np.random.normal(loc=0.0, scale=sigma, size=vector.shape)
        return vector + noise

    @staticmethod 
    def getBPS(modStr):
        #TODO - convert qam16 to 4, qpsk - to 2
        return 1;
        
    @staticmethod
    def ebn0_to_esn0(ebn0, code_rate, bps):
        '''
        converts Energy per bit to energy per symbol
        code_rate code rate
        bps depends on modulation (1 - bpsk, 2 - qpsk, 4 - qam16)
        '''
        return ebn0 + 10.0 * np.log10(code_rate * bps)

    @staticmethod
    def esn0_to_sigma(esn0, upsample_factor=1):
        #FIXME: seems to be a bug here. ? sigma_square = 10.^(-esn0/20);
        sigma_square = ((upsample_factor * 1.0) / (2 * pow(10, (esn0/10))))
        sigma = np.sqrt(sigma_square)
        return sigma

    @staticmethod
    def ebn0_to_sigma(ebn0, bit_rate, bps, upsamle_factor=1):
        esn0 = AWGN.ebn0_to_esn0(ebn0, bit_rate, bps)
        sigma = AWGN.esn0_to_sigma(esn0, upsamle_factor)
        return sigma


