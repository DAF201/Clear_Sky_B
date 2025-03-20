#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>
using byte = unsigned char *;
struct bytes
{
    int size;
    byte data;
    void copy(void *dest);
    ~bytes();
};
std::string encode(byte data, long unsigned int size);
std::string encode(const std::string &data);
bytes decode(const std::string &data);