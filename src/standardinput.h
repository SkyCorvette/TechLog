// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef STANDARDINPUT_H
#define STANDARDINPUT_H

#include <fcntl.h>
#include "reader.h"

class StandardInput : public Reader
{
public:
    ssize_t readNext(char *buffer, size_t count)
    {
        return read(STDIN_FILENO, buffer, count);
    }
};

#endif //STANDARDINPUT_H