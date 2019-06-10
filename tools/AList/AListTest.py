import unittest
from AList import *

class MyTestCase(unittest.TestCase):
    def test_alist1(self):
        alist_text = [["7 3"],
                ["3 4"],
                ["1 1 2 2 3 2 1"],
                ["4 4 4"],
                ["1 0 0"],
                ["2 0 0"],
                ["1 3 0"],
                ["1 2 0"],
                ["1 2 3"],
                ["2 3 0"],
                ["3 0 0"],
                ["1 3 4 5"],
                ["2 4 5 6"],
                ["3 5 6 7"]]
        AList reader
        reader.readMatrix(alist_text)

        print reader.matrix

        self.assertEqual(True, False)


if __name__ == '__main__':
    unittest.main()
