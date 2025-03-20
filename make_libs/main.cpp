#include "shared_coding.hpp"
#include <iostream>
using namespace std;
int main()
{
    char a[] = "hello world";
    char b[12];
    cout << a << endl;
    string data = encode(a);
    cout << data << endl;
    decode(data).copy(b);
    cout << b << endl;
}