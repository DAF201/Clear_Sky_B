#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <math.h>
#include <float.h>

#include <vector>
#include <map>
#include <list>

#include <string>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <regex>
#include <cstdio>

#include <any>
#include <typeinfo>
#include <type_traits>

#include <algorithm>

#include <fstream>

using namespace std;

typedef vector<string> str_vec;

#define to_bits(v) bits_print(&v, sizeof(v))

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

template <typename T>
void print(T value)
{
    cout << value << endl;
}

template <typename first, typename... args>
void print(first value, args... rest)
{
    cout << value << endl;
    print(rest...);
}

// split string by space and dump to vector
str_vec str_split_by_space(string input)
{
    string temp = "";
    str_vec result;
    for (int i = 0; i < input.length(); ++i)
    {

        if (input[i] == ' ')
        {
            result.push_back(temp);
            temp = "";
        }
        else
        {
            temp.push_back(input[i]);
        }
    }
    result.push_back(temp);
    return result;
}

typedef vector<string> str_vec;
str_vec str_split_space(string input, int repeat_times)
{
    string temp = "";
    int counter = 0;
    str_vec result;

    if (NULL == strstr(input.c_str(), " "))
    {
        result.push_back(input);
        return result;
    }

    for (int i = 0; i < input.size(); i++)
    {
        if (counter > repeat_times)
        {
            return result;
        }

        if (input[i] == ' ' && counter < repeat_times)
        {
            counter++;
            result.push_back(temp);
            temp = "";
        }
        else
        {
            temp += input[i];
        }
    }

    if (temp != "")
    {
        result.push_back(temp);
        return result;
    }
    else
    {
        return result;
    }
}

// split by char
str_vec str_split_by_char(string input, char sub)
{
    string temp = "";
    str_vec result;
    for (int i = 0; i < input.length(); ++i)
    {

        if (input[i] == sub)
        {
            result.push_back(temp);
            temp = "";
        }
        else
        {
            temp.push_back(input[i]);
        }
    }
    result.push_back(temp);
    return result;
}

// make a string vector to a string
string strvec_to_str(str_vec input_vec)
{
    string temp = "";
    for (int i = 0; i < input_vec.size(); i++)
    {
        temp = temp + input_vec[i];
    }
    return temp;
}

// remove space for cstring
void rm_space(char *input)
{
    int size = strlen(input);
    int index = 0;
    char *temp;
    temp = new char[size];
    for (int i = 0; i < size; i++)
    {
        if (' ' != input[i])
        {
            temp[index] = input[i];
            index++;
        }
    }
    temp[index] = '\0';
    strcpy(input, temp);
    delete temp;
}

// string remove space
void rm_space(string *input) // to avoid I totally forget how to use pointer (SO WHY DON'T JUST PASS IN REFERENCE...)
{
    input->erase(remove(input->begin(), input->end(), ' '), input->end());
}

// remove substring from string
void rm_sub(string &main_str, string &sub_str) // here you go, reference
{
    string replacement = "";
    regex target(sub_str);
    main_str = regex_replace(main_str, target, replacement);
}

// char array remove sub
void rm_sub(char *main_str, const char *sub_str)
{
    string str_main(main_str);
    string str_sub(sub_str);

    // lazy
    rm_sub(str_main, str_sub);

    strcpy(main_str, str_main.c_str());
}

// extract the text between tags, same input type
template <typename TEXT>
string tag_extractor(TEXT source, TEXT tag)
{
    string str_source = string(source);
    string str_tag = string(tag);
    string no_found = "";

    if (str_source.size() < str_tag.size())
    {
        return no_found;
    }

    int head = -1;
    int tail = -1;

    head = str_source.find(str_tag);
    tail = str_source.find(str_tag, head + str_tag.size());

    if (head == -1 || tail == -1)
    {
        return no_found;
    }

    return str_source.substr(head + str_tag.size(), tail - head - str_tag.size());
}

// extract the first text between tags, different input type
template <typename TEXT_TYPE_1, typename TEXT_TYPE_2>
string tag_extractor(TEXT_TYPE_1 source, TEXT_TYPE_2 tag)
{
    string str_source = string(source);
    string str_tag = string(tag);
    string no_found = "";

    if (str_source.size() < str_tag.size())
    {
        return no_found;
    }

    int head = -1;
    int tail = -1;

    head = str_source.find(str_tag);
    tail = str_source.find(str_tag, head + str_tag.size());

    if (head == -1 || tail == -1)
    {
        return no_found;
    }

    return str_source.substr(head + str_tag.size(), tail - head - str_tag.size());
}

// Does not include the last \0
template <typename TEXT>
unsigned int text_size(TEXT input_char)
{
    int counter = 0;
    while (input_char[counter] != '\0' && counter != 2147483647)
    {
        counter++;
    }
    return counter;
}

// int is sub, for example 1 is a sub of 12 or 11...
bool is_sub(int &main_source, int &sub_source)
{
    string main_source_str = to_string(main_source);
    string sub_source_str = to_string(sub_source);

    cout << main_source_str << " " << sub_source_str << endl;

    if (NULL != strstr(main_source_str.c_str(), sub_source_str.c_str()))
    {
        return true;
    }
    else
    {
        return false;
    }
}

// same type TEXT compare
template <typename TEXT>
bool is_sub(TEXT main, TEXT sub)
{
    if (text_size(main) < text_size(sub))
    {
        return false;
    }

    try
    {
        return !(string(main).find(string(sub)) == -1);
    }
    catch (...)
    {
        return false;
    }
}

// different type text compare
template <typename TEXT_TYPE_1, typename TEXT_TYPE_2>
bool is_sub(TEXT_TYPE_1 main, TEXT_TYPE_2 sub)
{
    if (text_size(main) < text_size(sub))
    {
        return false;
    }

    try
    {
        return !(string(main).find(string(sub)) == -1);
    }
    catch (...)
    {
        return false;
    }
}

#endif