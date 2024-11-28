#include "src/tools.hpp"
#include "src/db.hpp"
#include "src/defination.hpp"
#include "src/coding.hpp"
#include "src/math.hpp"
void matrix_test()
{
}

void simple_math_function_test()
{
    char c[] = "hello";
    void *d;
    mess::matrix m = mess::matrix(c, sizeof(c));
    m >> d;
    print((char *)d);
}

int main()
{
    simple_math_function_test();
}
