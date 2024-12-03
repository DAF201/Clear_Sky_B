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
    std::ifstream file(INTERNET_ANGEL, std::ios::binary);
    file.seekg(0, std::ios::end);
    unsigned int size = file.tellg();
    file.seekg(0, std::ios::beg);
    char *file_data = new char[size];
    file.read(file_data, size);
    print(file_data, size);
    std::string s = clear_sky::encode((byte)file_data, size);
    // print(s);
    std::fstream o("INTERNET ANGEL.txt", std::ios::app);
    o.write(s.c_str(), s.length());

    std::ifstream f("INTERNET ANGEL.txt");
    std::string b;
    while (!f.eof())
    {
        f >> b;
    }
    clear_sky::bytes data = clear_sky::decode(b);
    std::ofstream a("INTERNET ANGEL.MP4", std::ios::binary);
    a.write((char *)data.data, data.size);
}

int main()
{
    simple_math_function_test();
}
