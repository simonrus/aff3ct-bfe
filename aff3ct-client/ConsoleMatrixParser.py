import numpy as np
import re


class ConsoleMatrixParser:
    """ fills numpy matrix """

    value = None
    error = ''

    @staticmethod
    def read_scalar(str_value):
        return float(str_value)

    @staticmethod
    def is_scalar(str_value):
        try:
            float(str_value)
            return True
        except ValueError:
            return False

    @staticmethod
    def is_valid(matrix):
        return False if matrix is None else np.isnan(matrix).any()

    @staticmethod
    def invalidate(matrix):
        matrix[0] = np.NaN;

    def reset(self):
        self.value = None
        self.error = ''

    def is_error(self):
        return len(self.error) > 0

    def get_error_text(self):
        return self.error

    def get_value(self):
        return self.value

    def parse_matrix_values(self, words, total_rows, total_cols, start_idx, np_target_mtx):
        i = start_idx
        pos = 0

        while i < len(words):
            row = int(pos / total_cols)
            col = pos % total_cols
            try:
                if total_rows == 1:
                    np_target_mtx[col] = float(words[i])
                else:
                    np_target_mtx[row][col] = float(words[i])
                pos += 1
            except ValueError:
                self.error += ('Value error while parsing %s for pos (%d,%d) - N' % (words[i], col, row))
            except IndexError:
                self.error += ('Index error while parsing %s for pos (%d,%d)' % (words[i], col, row))

                self.invalidate(np_target_mtx)
                return False

            i += 1

        if i == len(words) and pos == total_rows * total_cols:
            return True
        else:
            self.invalidate(np_target_mtx)
            self.error += ("Not enough values: filled=%d, expected %d for %d x %d"
                           % (pos, total_rows * total_cols, total_rows, total_cols))
            return False

    def parse(self, params):

        if len(params) == 1:  ##Scalar value is passed
            try:
                temp = self.read_scalar(params[0])
                self.value = np.array([temp], dtype='float')
                return True
            except ValueError:
                self.error += ("2nd argument (%s)) shall be a numeric value" % args.params[0])

            return False

        if len(params) == 2:  ## push VAR size val0 - is not allowded
            self.error += "vector and matrix cannot contain one elem (use scalar)"

            return False

        if ',' not in params[0] and \
                ',' not in params[1] and \
                ',' not in params[2]:

            ## Vector case
            try:
                vec_size = int(params[0])
            except ValueError:
                self.error += "vector size is incorrect"
                return False

            if '.' in params:
                self.error += "vector size shall be integer"

            if vec_size <= 1:
                self.error += "vector size shall be positive and not a scalar (has more then 2 elems)"

            ## build matrix
            self.value = np.zeros([vec_size])
            result = self.parse_matrix_values(params, 1, vec_size, 1, self.value)
            if not self.is_valid(self.value) or result is False:
                return False

            return True

        #The rest is matix case
        pattern = '^[0-9]+ *, *[0-9]+'

        matrix_size_str = ''
        found = False

        shape = []
        for i in [0, 1, 2]:
            matrix_size_str = matrix_size_str + params[i]
            if re.match(pattern, matrix_size_str):
                found = True
                break

        if found:
            matrix_size_cleaned = matrix_size_str.replace(' ', '')
            sizes = matrix_size_cleaned.split(",")


            for size in sizes:
                try:
                    dim_size = int(size)
                    shape.append(dim_size)
                except ValueError:
                    self.error += ("ValueError while parsing size %s in %s(%s)"
                                   % (size, matrix_size_cleaned, matrix_size_str))
                    return False


            #ok, size of matrix is determined(parsed), the i+1... end are numbers
            if len(shape) is not 2:
                self.error += "Currently only 2 dim matrices are supported (provided %d-dim)"%(len(shape))
                return False

            self.value = np.zeros(tuple(shape))
            self.parse_matrix_values(params, shape[0], shape[1], i+1, self.value)
            if not self.is_valid(self.value):
                return False

            return True

        return False