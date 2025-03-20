#ifndef CODING_HPP
#define CODING_HPP
#include <sstream>
#ifndef TOOLS_HPP
#include <string>
#include <cstring>
#include <cstdlib>
#endif

namespace data
{
    using byte = unsigned char *;

    // Mapping for encoding and decoding
    const static char int2str_map[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    const static char byte2char_map[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+-";

    static char byte2char(char c)
    {
        return byte2char_map[c];
    }

    static char char2byte(char c)
    {
        if (c == '+')
            return 62;
        if (c == '-')
            return 63;
        if (c >= '0' && c <= '9')
            return c - '0';
        if (c >= 'A' && c <= 'Z')
            return c - 'A' + 10;
        if (c >= 'a' && c <= 'z')
            return c - 'a' + 36;
        return -1; // Invalid character
    }

    struct bytes
    {
        int size;
        byte data;

        void copy(void *dest) { memcpy(dest, data, size); }

        ~bytes()
        {
            free(data);
            data = NULL;
        }
    };

    std::string encode(byte data, long unsigned int size)
    {
        // Pad data length to a multiple of 3
        long unsigned int padded_length = (size + 2) / 3 * 3;

        // Create a padded copy of the input
        byte padded_data = (byte)malloc(padded_length);
        memset(padded_data, 0, padded_length);
        memcpy(padded_data, data, size);

        std::ostringstream res;
        res << int2str_map[padded_length - size]; // Store padding size

        for (unsigned int i = 0; i < padded_length; i += 3)
        {
            res << byte2char((padded_data[i] & 0b11111100) >> 2);
            res << byte2char(((padded_data[i] & 0b00000011) << 4) | ((padded_data[i + 1] & 0b11110000) >> 4));
            res << byte2char(((padded_data[i + 1] & 0b00001111) << 2) | ((padded_data[i + 2] & 0b11000000) >> 6));
            res << byte2char(padded_data[i + 2] & 0b00111111);
        }

        free(padded_data);
        return res.str();
    }

    std::string encode(const std::string &data)
    {
        return encode((byte)data.c_str(), data.length());
    }

    bytes decode(const std::string &data)
    {
        // Extract padding size from the first character
        long unsigned int pad_size = data[0] - '0';
        long unsigned int decoded_length = (data.length() - 1) / 4 * 3 - pad_size;

        byte decoded_data = (byte)malloc(decoded_length);

        int j = 0;
        for (unsigned int i = 1; i < data.length(); i += 4)
        {
            char b0 = char2byte(data[i]);
            char b1 = char2byte(data[i + 1]);
            char b2 = char2byte(data[i + 2]);
            char b3 = char2byte(data[i + 3]);

            decoded_data[j++] = (b0 << 2) | ((b1 & 0b00110000) >> 4);
            if (j < decoded_length)
                decoded_data[j++] = ((b1 & 0b00001111) << 4) | ((b2 & 0b00111100) >> 2);
            if (j < decoded_length)
                decoded_data[j++] = ((b2 & 0b00000011) << 6) | b3;
        }

        bytes result;
        result.size = decoded_length;
        result.data = decoded_data;
        return result;
    }
}
#endif
