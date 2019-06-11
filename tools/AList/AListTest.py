import unittest
from AList import *


class MyTestCase(unittest.TestCase):
    def test_alist_read1(self):
        alist_text = ["7 3",
                      "3 4",
                      "1 1 2 2 3 2 1",
                      "4 4 4",
                      "1 0 0",
                      "2 0 0",
                      "1 3 0",
                      "1 2 0",
                      "1 2 3",
                      "2 3 0",
                      "3 0 0",
                      "1 3 4 5",
                      "2 4 5 6",
                      "3 5 6 7"]

        reader = AList()
        reader.readMatrix(alist_text)

        matrix_ref = np.asarray([[1, 0, 1],
                                 [0, 1, 1],
                                 [1, 0, 1],
                                 [1, 1, 1],
                                 [1, 1, 1],
                                 [0, 1, 1],
                                 [0, 0, 1]], dtype=int);

        self.assertTrue(np.array_equal(matrix_ref, reader.matrix), "Read matrix differs from reference")

    def test_alist_read2(self):
        alist_text = ["12 16",
                      "4 3",
                      "4 4 4 4 4 4 4 4 4 4 4 4",
                      "3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3",
                      "3 8 10 13",
                      "4 7 9 13",
                      "2 5 7 10",
                      "4 6 11 14",
                      "3 9 15 16",
                      "1 6 9 10",
                      "4 8 12 15",
                      "2 6 12 16",
                      "1 7 14 16",
                      "3 5 12 14",
                      "2 11 13 15",
                      "1 5 8 11",
                      "6 9 12",
                      "3 8 11",
                      "1 5 10",
                      "2 4 7",
                      "3 10 12",
                      "4 6 8",
                      "2 3 9",
                      "1 7 12",
                      "2 5 6",
                      "1 3 6",
                      "4 11 12",
                      "7 8 10",
                      "1 2 11",
                      "4 9 10",
                      "5 7 11",
                      "5 8 9"]

        matrix_ref = np.asarray([[0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0],
                                 [0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0],
                                 [0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0],
                                 [0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0],
                                 [0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1],
                                 [1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0],
                                 [0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0],
                                 [0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1],
                                 [1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1],
                                 [0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0],
                                 [0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0],
                                 [1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0]], dtype=int)
        reader = AList()
        reader.readMatrix(alist_text)

        self.assertTrue(np.array_equal(matrix_ref, reader.matrix), "Read matrix differs from reference")


if __name__ == '__main__':
    unittest.main()
