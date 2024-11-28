#ifndef MATH_HPP
#define MATH_HPP

#ifndef TOOLS_HPP
#include <math.h>
#include <float.h>
#include <string>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <cstdio>
using namespace std;

// copied from tools.hpp, needed for debug
void bits_print(void *v, int size)
{
    cout << "Lower bits -> Higher bits" << endl;
    for (int i = 0; i < size; i++)
    {
        char temp_byte;
        memcpy(&temp_byte, (char *)v + i, 1);
        for (int j = 0; j < 8; j++)
        {
            cout << ((temp_byte >> j) & 1);
        }
        cout << " ";
    }
    cout << endl;
}
#endif

namespace mess // mess->math
{

    // for filling mem space
    const static int ZERO = 0;

    inline int RAND_INT(int seed = 0, int lower = INT32_MAX, int upper = INT32_MIN)
    {
        if (!seed)
            srand(time(NULL));
        else
            srand(seed);
        if (lower > upper)
            return rand();
        else
            return lower + (rand() % (upper - lower));
    }

    inline float RAND_DOUBLE(int seed = 0, float lower = DBL_MAX, float upper = DBL_MIN)
    {
        if (!seed)
            srand(time(NULL));
        else
            srand(seed);
        if (lower > upper)
            return RAND_INT() + (float)rand() / RAND_MAX;
        else
            return RAND_INT(0, (int)lower, (int)upper - 1) + RAND_DOUBLE();
    }

#define hash(value) bytes_hash(value, sizeof(value))
    // lower byte -> higher byte
    static unsigned char hash_seed[] = {0xB4, 0x66, 0xC5, 0x0F}; // 264595124, from random int generator
    static unsigned char hash_key = 0x00;

    inline void seed()
    {
        cout << hash_key << endl;
        cout << hash_seed << endl;
    }

    inline void set_hash_seed(int *seed)
    {
        memcpy(hash_seed, seed, 4);
        hash_key = hash_seed[0] ^ hash_seed[1] ^ hash_seed[2] ^ hash_seed[3];
    }

    // pad to 4k bytes with hash key, then copy data into it and do xor for each 4 bytes iteratively using hash seed as init value
    string bytes_hash(void *value, int size)
    {
        unsigned char *buffer = (unsigned char *)malloc(size + (4 - size % 4));
        int char_to_int_convert_buffer;
        int res;
        memcpy(&res, hash_seed, 4);
        memcpy(buffer + (size + (4 - size % 4)) - ((4 - size % 4)), &hash_key, (4 - size % 4));
        memcpy(buffer, value, size);
        for (int i = 0; i < size + (4 - size % 4); i += 4)
        {
            memcpy(&char_to_int_convert_buffer, buffer + i, 4);
            res ^= char_to_int_convert_buffer;
        }
        char c_hex[9];
        sprintf(c_hex, "%X", res); // Uppercase hex
        string s_hex(c_hex);
        return s_hex;
    }

    inline int factor(int number)
    {
        for (int i = (int)floor(pow(number, 0.5)); i > 0; i--)
            if (number % i == 0)
                return i;
        return 1;
    }

    // end of basic functions

    // fraction functions
    inline float extract_fraction(float value)
    {
        return abs(value - floor(value));
    }

    inline int number_length(float value)
    {
        return to_string(value).length();
    }

    inline int decimal_length(float value)
    {
        return to_string(floor(value)).length();
    }

    inline int fraction_length(float value)
    {
        if (value == floor(value))
            return 0;
        string fraction_str = to_string(value);
        fraction_str = fraction_str.substr(fraction_str.find('.') + 1);
        while (fraction_str.back() == '0')
            fraction_str = fraction_str.substr(0, fraction_str.length() - 1);
        return fraction_str.length();
    }

    // end of fraction functions

    // matrix class

#define matrix_iter                      \
    for (int i = 0; i < matrix_row; i++) \
        for (int j = 0; j < matrix_col; j++)

    class matrix
    {
    private:
        // the min length of decimal part
        int decimal = 1;
        // the precision of the fractional part
        int fractional = 2;
        // matrix type:0 math matrix, 1 data matrix
        int matrix_type = 0;
        // matrix body
        float **__matrix = nullptr;
        // remember to add (void*) to the front of __bin_matrix whenever using pointer + address or it will jump to some random space
        char **__bin_matrix = nullptr;
        int data_size = 0;
        int matrix_row, matrix_col;

        // empty data matrix
        matrix(int matrix_col, int matrix_row, string zero)
        {
            this->fractional = 1;
            this->decimal = 1;
            this->matrix_type = 1;

            this->matrix_row = matrix_row;
            this->matrix_col = matrix_col;

            this->__bin_matrix = new char *[matrix_row];
            for (int i = 0; i < matrix_row; i++)
                __bin_matrix[i] = new char[matrix_col];

            data_size = matrix_col * matrix_row;
            if (zero == "0")
                memset(__bin_matrix, 0x00, data_size);
            else
                memset(__bin_matrix, RAND_INT(0, 0, 127), data_size);
        }

    public:
        matrix(void *data, int size)
        {
            this->fractional = 1;
            this->decimal = 1;
            this->matrix_type = 1;

            this->matrix_row = factor(size);
            this->matrix_col = size / matrix_row;

            this->__bin_matrix = new char *[matrix_row];
            for (int i = 0; i < matrix_row; i++)
                __bin_matrix[i] = new char[matrix_col];

            data_size = size;

            memcpy(__bin_matrix, data, size);
        }

        matrix(int matrix_row = 1, int matrix_col = 1, float init = 0, int fractional = 2)
        {
            this->fractional = fractional;
            this->decimal = to_string(init).length();

            this->matrix_row = matrix_row;
            this->matrix_col = matrix_col;

            this->__matrix = new float *[matrix_row];
            for (int i = 0; i < matrix_row; i++)
                __matrix[i] = new float[matrix_col];

            matrix_iter
                __matrix[i][j] = init;
        }

        char get_byte(int x, int y)
        {
            // return *(__bin_matrix + x * matrix_row + y);
            bits_print(__bin_matrix + x * matrix_col + y, 1);
            return 0;
        }

        float sum()
        {
            float res = 0;
            matrix_iter
                res += __matrix[i][j];
            return res;
        }

        // switch the row and col of the matrix
        matrix transpose()
        {
            /* to test "hello\0", and found must add (void*) to convert to void* pointer before mem operation
            otherwise the char** will cause undefined action when doing memcpy

            Lower bits -> Higher bits (Hello)
            00010110 10100110 00110110 00110110 11110110 00000000
            Lower bits -> Higher bits (copy without (void*))
            00010110 00001001 00000000 10000100 00001100 00001100
            Lower bits -> Higher bits (with (void*) and after transpose)
            00010110 00110110 00000000 10100110 00110110 00000000

            */

            // bits_print(__bin_matrix, 6);
            // bits_print(__bin_matrix + 0, 1);
            // bits_print((void *)__bin_matrix + 1, 1);
            // bits_print((void *)__bin_matrix + 2, 1);
            // bits_print((void *)__bin_matrix + 3, 1);
            // bits_print((void *)__bin_matrix + 4, 1);
            // bits_print((void *)__bin_matrix + 5, 1);
            // bits_print((void *)res.__bin_matrix, 6);

            matrix res = matrix(matrix_row, matrix_col, "0");

            matrix_iter
            {
                memcpy((void *)res.__bin_matrix + j * res.matrix_row + i, (void *)__bin_matrix + i * matrix_row + j, 1);
                // bits_print((void *)__bin_matrix + i * matrix_row + j, 1);
                // bits_print((void *)res.__bin_matrix + j * res.matrix_row + i, 1);
            }

            return res;
        }

        float operator()(int x, int y)
        {
            if (matrix_type)
                return 0;
            return __matrix[x][y];
        }

        void operator()(int x, int y, float value)
        {
            if (matrix_type)
                return;
            __matrix[x][y] = value;
            if (decimal_length(value) > decimal)
            {
                decimal = decimal_length(value);
            }
        }

        matrix operator+(matrix m2)
        {
            if (matrix_type)
                return *this;
            if (matrix_row != m2.matrix_row || matrix_col != m2.matrix_col)
                throw logic_error("addition can only be performed between matrices of same dimensions");

            matrix res = matrix(matrix_row, matrix_col, 0);
            matrix_iter
                res(i, j, (*this)(i, j) + m2(i, j));
            return res;
        }

        matrix operator+(float value)
        {
            if (matrix_type)
                return *this;
            matrix res = matrix(matrix_row, matrix_col, 0);
            matrix_iter
                res(i, j, (*this)(i, j) + value);
            return res;
        }

        template <typename T>
        matrix operator-(T v)
        {
            if (matrix_type)
                return *this;
            *this + v * (-1);
            return *this;
        }

        matrix operator*(matrix m2)
        {
            if (matrix_type)
                return *this;
            if (matrix_col != m2.matrix_row)
                throw logic_error("multiplication can only be performed between matrices with dimensions of of x*y and y*z");

            // C[i][j] = sum(A[i][k]*B[k][j] for k in range(0, A_matrix_col))

            matrix res = matrix(matrix_row, m2.matrix_col, 0);
            for (int i = 0; i < matrix_row; i++)
            {
                for (int j = 0; j < m2.matrix_col; j++)
                {
                    int sum = 0;
                    for (int k = 0; k < matrix_col; k++)
                    {
                        sum += ((*this)(i, k) * m2(k, j));
                    }
                    res(i, j, sum);
                }
            }
            return res;
        }

        matrix operator*(float value)
        {
            if (matrix_type)
                return *this;
            matrix_iter
                __matrix[i][j] *= value;
            return *this;
        }

        // extract data from data matrix
        void operator>>(void *&data)
        {
            if (matrix_type)
            {
                data = new char(matrix_col * matrix_row);
                memcpy(data, __bin_matrix, sizeof(data));
            }
        }

        friend ostream &operator<<(ostream &out, matrix *matrix)
        {
            return out << *matrix;
        }

        friend ostream &operator<<(ostream &out, matrix matrix)
        {
            for (int i = 0; i < matrix.matrix_row; i++)
            {
                for (int j = 0; j < matrix.matrix_col; j++)
                {
                    out << std::setfill(' ') << std::setw(5) << matrix(i, j);
                    if (j != matrix.matrix_col - 1)
                        out << " ";
                }
                out << "\n";
            }
            return out;
        }
    };
    // end of matrix class
}
#endif