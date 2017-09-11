#ifndef TECHLOG_FILE_H
#define TECHLOG_FILE_H

#include <fcntl.h>
#include <unistd.h>
#include "reader.h"

class File : public Reader {
public:
    explicit File(const char* path);
    ~File() override;
    ssize_t readNext(char* buffer, size_t count) override;
    const char* fileName() override;
private:
    int _file = 0;
    const char* _fileName;
    File(const File&);
    File& operator=(const File&);
};

#endif