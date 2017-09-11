#ifndef STANDARDINPUT_H
#define STANDARDINPUT_H

#include <fcntl.h>
#include <unistd.h>
#include "reader.h"

class StandardInput : public Reader {
public:
    ssize_t readNext(char *buffer, size_t count) override;
};

#endif //STANDARDINPUT_H