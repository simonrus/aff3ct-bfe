import numpy as np


class AListReader:
    def __init__(self):
        ''' some constructor'''
        self.matrix = None
    
    def getMatrix(self):
        return self.matrix

    def readFromFile(self, filename):
        with open(filename) as f:
            lineList = [line.rstrip('\n') for line in f]
        f.close()

        return self.readMatrix(lineList)

    def readMatrix(self, lines):
        n_cols, n_rows = [int(x) for x in lines[0].split()]  # read n, m
        max_col_weight, max_row_weight = [int(x) for x in lines[1].split()]  # read maxcolw, maxroww

        col_weight = [int(x) for x in lines[2].split()]
        row_weight = [int(x) for x in lines[3].split()]

        assert len(col_weight) == n_cols, "The number of columns(%d) shall be same as N(%d)" % (col_weight, n_cols)
        assert len(row_weight) == n_rows, "The number of rows(%d) shall be same as K(%d)" % (row_weight, n_rows)

        self.matrix = np.zeros([n_rows, n_cols], dtype=int)
        i = 4  # Start pos

        cnt = 0                                         # current col
        for c in col_weight:                                  # c is number of 1's in current columns

            pos = [int(x) for x in lines[i].split()]    # read position

            assert c <= max_col_weight, "Error at line %d: number of ones at column(%d) bigger as maximum column number(%d)" \
                                 % (i, c, max_col_weight)
            assert len(pos) >= c, "Error at line %d: number of listed ones (%d) is bigger as number of ones(%d)" \
                                  % (i, len(pos), c)

            assert pos is not None, "Pos is None at cols reading"

            for j in pos:
                if j == 0:
                    continue
                assert j-1 < n_rows
                assert cnt < n_cols
                self.matrix[j-1,cnt] = 1

            i = i + 1
            cnt = cnt + 1

        # check
        cnt = 0                                         # current row
        for r in row_weight:                                  # r is number of 1's in current row
            pos = [int(x) for x in lines[i].split()]    # read positions with ones

            assert r <= max_row_weight, "Error at line %d: number of ones at rows(%d) bigger as maximum row number(%d)" \
                                 % (i, r, max_row_weight)
            assert len(pos) >= r, "Error at line %d: number of listed ones (%d) is bigger as number of ones(%d)" \
                                  % (i, len(pos), c)

            assert pos is not None, "Pos is None at rows reading"

            for j in pos:
                if j == 0:
                    continue
                assert self.matrix[cnt, j-1] == 1, "Error at %d: @(%d, %d) shall be zero" % (i, j, cnt)

            i = i + 1
            cnt = cnt + 1

        return n_rows, n_cols
