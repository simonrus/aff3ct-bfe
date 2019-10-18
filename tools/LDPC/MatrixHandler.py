import numpy as np

import pdb
class MatrixHandler:

    def transform_H_to_G_indentity(self, H_orig, info_bits_position):
        H = H_orig.copy()
        n_rows = H.shape[0]
        n_cols = H.shape[1]
        
        K = n_cols - n_rows
        assert K > 0, "invalid dimension"

        ## step 1: move to triangle way

        for i in range(0,n_rows): 
            anchor = None
            anchor_row = 0
            for j in range(i, n_rows):
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
    
        ## step 2: make identity matrix 
        for j in range(n_rows-1, -1, -1): ## start from last col and go left
            for i in range (0,j): ## for every row up
                if H[i,j] == 1:
                    # eliminate using j-th row
                    H[i,:] = H[i,:] ^ H[j,:]
        
        ## step3: build G
        G = np.zeros((K, n_cols), dtype=type(H[0,0]))
        temp  = H[:,K-1:n_cols]

        G[0:K, 0:K] = np.diag(np.ones(K))
        G[0:K, K:n_cols] = temp.transpose()
        return G


    def transform_H_to_G_decomp_LU(self, H, info_bits_position):
        pass
