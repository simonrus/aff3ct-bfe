import numpy as np


class AList:
    def __init__(self):
        ''' some constructor'''
        self.matrix = None
    
    def getMatrix(self):
        return self.matrix

    def readFromFile(self, filename):
        with open(filename) as f:
            lineList = [line.rstrip('\n') for line in f]
        f.close()

        self.readMatrix(lineList)

        pass

    def readMatrix(self, lines):
        N, K = [int(x) for x in lines[0].split()]  # read n, m
        maxcolw, maxroww = [int(x) for x in lines[1].split()]  # read maxcolw, maxroww

        colw = [int(x) for x in lines[2].split()]
        roww = [int(x) for x in lines[3].split()]

        assert len(colw) == N, "The number of columns(%d) shall be same as N(%d)" % (colw, N)
        assert len(roww) == K, "The number of rows(%d) shall be same as K(%d)" % (roww, K)

        self.matrix = np.zeros([N, K], dtype=int)
        i = 4  # Start pos

        cnt = 0                                         # current col
        for c in colw:                                  # c is number of 1's in current columns

            pos = [int(x)-1 for x in lines[i].split()]    # read position and convert to [0..K-1] range

            assert c <= maxcolw, "Error at line %d: number of ones at column(%d) bigger as maximum column number(%d)" \
                                 % (i, c, maxcolw)
            assert len(pos) >= c, "Error at line %d: number of listed ones (%d) is bigger as number of ones(%d)" \
                                  % (i, len(pos), c)

            for j in pos:

                assert j < K
                assert cnt < N
                self.matrix[cnt, j] = 1

            i = i + 1
            cnt = cnt + 1

        # check
        cnt = 0                                         # current row
        for r in roww:                                  # r is number of 1's in current row
            pos = [int(x)-1 for x in lines[i].split()]    # read positions with ones

            assert r <= maxroww, "Error at line %d: number of ones at rows(%d) bigger as maximum row number(%d)" \
                                 % (i, r, maxroww)
            assert len(pos) >= r, "Error at line %d: number of listed ones (%d) is bigger as number of ones(%d)" \
                                  % (i, len(pos), c)

            for j in pos:
                assert self.matrix[j, cnt] == 1, "Error at %d: @(%d, %d) shall be zero" % (i, j, cnt)

            i = i + 1
            cnt = cnt + 1
