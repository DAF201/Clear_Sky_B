#ifndef CODING_HPP
#define CODING_HPP

#ifndef TOOLS_HPP
#include <string>
#include <string.h>
#endif

namespace clear_sky
{
#define byte unsigned char *
    // repalcement of memset
    void set_mem(void *p, char data, int size)
    {
        for (int i = 0; i < size; i++)
            ((byte)p)[i] = data;
    }

    // convertion from int num to str num
    const static char int2str_map[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

    // dictionary
    const static char byte2char_map[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+-";

    // convert a byte to a visible char
    static char byte2char(char c)
    {
        return byte2char_map[c];
    }

    // convert a visible chat to byte
    static char char2byte(char c)
    {
        // hardcoded because I am lazy
        if (c == '+')
            return 62;
        if (c == '-')
            return 63;
        if (c < 48 || c > 122)
            return -1;
        if (c < 58)
            return c - 48;
        if (c < 91)
            return c - 55;
        else
            return c - 61;
    }

    // chunk of bytes, IDK what was encoded so you need to typecast or copy it on your own
    struct bytes
    {
        int size;
        byte data;
        void copy(void *dest) { memcpy(dest, data, size); }
        ~bytes()
        {
            delete data;
            data = NULL;
        }
    };

    std::string encode(byte data, int size)
    {

        // pad to a length divided by 3
        int padded_length = size;
        while (padded_length % 3 != 0)
            padded_length++;

        // get a space divided by 3 and filled with 0
        byte padded_data = (byte)malloc(padded_length);
        set_mem(padded_data, 0, padded_length);

        // number of padding bytes at front
        std::string res;
        res = (int2str_map[padded_length - size]);

        // convert 3 bytes of 8 bits to 4 bytes of 6 bits then use the index to allocate something
        // in another word, we are making 24 bits from 3 bytes of 8 bits to 4 bytes of 6 bits(but still stored as regular bytes with 2 0s in the front),
        // so the regular byte 0101 0101 will become xx xx01 and 00 0101 two bytes and so on
        for (int i = 0; i < size; i = i + 3)
            // & will extract the wanted bit at index
            // | will concatenate bits to byte
            // << and >> move things around

            // new byte 0: 0b00 + byte 0[0:5]
            // new byte 1: 0b00 + byte 0[6:7] + byte 1[0:3]
            // new byte 2: 0b00 + byte 1[4:7] + byte 2[0:1]
            // new byte 3: 0b00 + byte 2[2:7]
            res = res + byte2char((data[i] & 0b11111100) >> 2) +
                  byte2char((((data[i] & 0b00000011) << 4) | ((data[i + 1] & 0b11110000) >> 4))) +
                  byte2char(((data[i + 1] & 0b00001111) << 2) | ((data[i + 2] & 0b11000000) >> 6)) +
                  byte2char(data[i + 2] & 0b00111111);

        return res;
    }

    // encoding for std::std::string only cause this thing has dynamic length and sizeof does not work on it
    std::string encode(std::string data)
    {
        return encode((byte)data.c_str(), data.length());
    }

    bytes decode(std::string data)
    {
        // length of padding bytes
        int pad_size = data[0] - 48;
        byte decode_result = (byte)malloc((data.length() - 1) / 4 * 3);

        bytes result;
        // bytes size = (3/4)chatacter size + padding
        result.size = ((data.length() - 1) / 4 * 3) - pad_size;

        data = data.substr(1);
        std::string buffer;
        char char_buffer[3];

        // for each 4 characters
        for (int i = 0, j = 0; i < data.length(); i += 4)
        {
            // get substring
            buffer = data.substr(i, i + 4);
            // decode to 3 bytes
            char_buffer[0] = ((char2byte(buffer[0]) << 2) | (char2byte(buffer[1]) & 0b00110000) >> 4);
            char_buffer[1] = ((char2byte(buffer[1]) << 4) | (char2byte(buffer[2]) & 0b00111100) >> 2);
            char_buffer[2] = ((((char2byte(buffer[2])) & 0b00000011) << 6) | (char2byte(buffer[3]) & 0b00111111));

            // write to final result
            for (int i = 0; i < 3; i++)
                decode_result[j++] = char_buffer[i];
        }
        result.data = decode_result;
        return result;
    }

}
#endif