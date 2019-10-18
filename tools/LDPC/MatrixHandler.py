import numpy as np

class MatrixHandler:

    def transform_H_to_G_indentity(self, H, info_bits_position):
        n_rows = H.shape[0]
        n_cols = H.shape[1]
        ## stage first move to triangle way
        dim = min(n_rows,n_cols)
        for i in range(0,dim):
            print("next column %s" %(i))
            anchor = None
            anchor_row = 0
            for j in range(i, dim):
                if H[j,i] == 1:
                    #swap row with i
                    if anchor is None:
                        anchor = H[j,:].copy()
                        anchor_row = j
                        H[j,:] = H[i,:]
                        H[i,:] = anchor
                    else:
                        #substract
                        H[j,:] = anchor ^ H[anchor_row,:]
                    #substrace curent row from
    
        return H


    def transform_H_to_G_decomp_LU(self, H, info_bits_position):
        pass
