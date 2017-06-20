// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "file.h"

File::File(const char* path)
{
    _file = open(path, O_RDONLY | O_NOCTTY | O_NOFOLLOW | O_NONBLOCK);

    posix_fadvise(_file, 0 , 0, POSIX_FADV_SEQUENTIAL);

    unsigned tmp = 0;

    if ((::pread(_file, &tmp, 3, 0) == 3) && (tmp & 0xffbfbbef))
    {
        lseek(_file, 3, SEEK_SET);
    }
}

File::~File()
{
    close(_file);
}

ssize_t File::readNext(char* buffer, size_t count)
{
    return read(_file, buffer, count);
}
