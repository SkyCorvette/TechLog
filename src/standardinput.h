// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef EXAMPLE_STANDARDINPUT_H
#define EXAMPLE_STANDARDINPUT_H

#include <zconf.h>
#include "reader.h"

class StandardInput : public Reader
{
public:
    ssize_t read(char *buffer, size_t count) override
    {
        return ::read(STDIN_FILENO, buffer, count);
    }
};

#endif //EXAMPLE_STANDARDINPUT_H