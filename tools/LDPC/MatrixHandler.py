import numpy as np

np.set_printoptions(edgeitems=30, linewidth=100000, formatter=dict(float=lambda x: "\t%5.3g" % x))

import pdb


class MatrixHandler:
    def transform_H_to_G_indentity(self, H_orig, info_bits_position):
        H = H_orig.copy()
        n_rows = H.shape[0]
        n_cols = H.shape[1]
        
        K = n_cols - n_rows

        assert K > 0, ("invalid dimension (n_cols=%d, n_rows=%d)" % (n_cols, n_rows))

        # step 1: move to triangle way on the right side
        for i in range(0, n_rows):
            anchor = None
            # anchor_row = 0
            for j in range(i, n_rows):
                if H[j, i + K] == 1:
                    # swap row with i
                    if anchor is None:
                        anchor = H[j, :].copy()
                        H[j, :] = H[i, :]
                        H[i, :] = anchor
                        # anchor_row = i
                    else:
                        # subtract current row
                        H[j, :] = anchor ^ H[j, :]

        # step 2: make identity matrix on the right side
        for j in range(n_rows-1, -1, -1):       # start from last col and go left
            for i in range(0, j):              # for every row up
                if H[i, j + K] == 1:
                    # eliminate using j-th row
                    H[i, :] = H[i, :] ^ H[j, :]

        # step3: build G
        G = np.zeros((K, n_cols), dtype=type(H[0, 0]))

        temp = H[:, 0:n_rows]

        G[0:K, 0:K] = np.diag(np.ones(K))
        G[0:K, K:n_cols] = temp.transpose()
        return G

    def transform_H_to_G_decomp_LU(self, H, info_bits_position):
        pass
