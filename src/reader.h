// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef READER_H
#define READER_H

class Reader
{
public:
    virtual ssize_t readNext(char* buffer, size_t count) = 0;
    virtual ~Reader(){};
};

#endif //READER_H