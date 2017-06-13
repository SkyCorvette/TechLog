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