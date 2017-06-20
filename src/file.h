#ifndef FILE_H
#define FILE_H

#include <fcntl.h>
#include <unistd.h>
#include "reader.h"

class File : public Reader
{
public:
    File(const char* path);
    ~File();
    ssize_t readNext(char* buffer, size_t count);
private:
    int _file = 0;
};

#endif //FILE_H