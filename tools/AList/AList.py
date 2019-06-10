import numpy as np

class AList:
    def __init__(self):
        ''' some constructor'''
        self.matrix = None

    def readFile(self, filename):
        with open(filename) as f:

    def readMatrix(self, lines):
            N, K = [int(x) for x in lines[0].split()] # read n, m
            maxcolw, maxroww = [int(x) for x in lines[1].split()] # read maxcolw, maxroww

            colw = [int(x) for x in lines[2].split()]
            roww = [int(x) for x in lines[3].split()]

            assert len(colw) == N
            assert len(roww) == K

            self.matrix = np.zeros([N,K], dtype = int)
            i = 4 #Start pos
            cnt = 0 # current col

            #fill
            for c in colw:
                pos = [int(x) for x in lines[i].split()]

                assert c <= maxcolw
                assert len(pos) == c

                for j in pos:
                    self.matrix[cnt, j] = 1

                i = i + 1
                cnt = cnt + 1

            #check
            cnt = 0
            for r in roww:
                pos = [int(x) for x in lines[i].split()]

                assert r <= maxroww
                assert len(pos) == r

                for j in pos:
                    assert self.matrix[j, cnt] == 1

                i = i + 1
                cnt = cnt + 1






