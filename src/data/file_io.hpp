#ifndef FILE_IO_HPP
#define FILE_IO_HPP
#ifndef TOOLS_HPP
#include <fstream>
#include <vector>
#include <iostream>
#endif

template <typename file_name>
size_t get_file_size(file_name fn)
{
    size_t file_size = -1;
    std::fstream fs(fn, std::ios::binary);
    if (!fs.is_open())
        return file_size;
    fs.seekg(0, std::ios::end);
    file_size = fs.tellg();
    fs.seekg(0, std::ios::beg);
    return file_size;
}

template <typename file_name>
void read_file(file_name fn, std::vector<unsigned char> &buffer, int type = 0)
{
    std::ifstream fs;
    if (type)
        fs.open(fn, std::ios::binary);
    else
        fs.open(fn);

    if (!fs.is_open())
    {
        std::cout << "Failed to open file: " << fn << std::endl;
        return;
    }

    size_t file_size = get_file_size(fn);
    buffer.resize(file_size);
    fs.read(reinterpret_cast<char *>(buffer.data()), file_size);
    fs.close();
}

template <typename file_name>
void writeFile(file_name fn, std::vector<unsigned char> &buffer)
{
    std::ofstream file(fn, std::ios::binary);

    if (!file.is_open())
    {
        std::cout << "Failed to open file: " << fn << std::endl;
        return;
    }

    file.write(reinterpret_cast<const char *>(buffer.data()), buffer.size());
    file.close();
}
#endif