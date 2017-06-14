// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef EXAMPLE_READER_H
#define EXAMPLE_READER_H

class Reader
{
public:
    virtual ssize_t read(char* buffer, size_t count) = 0;
};


#endif //EXAMPLE_READER_H