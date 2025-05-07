#include "file.h"
#include <fcntl.h>

File::File(const char* path) : _fileName(path) {
    _file = open(path, O_RDONLY | O_NOCTTY | O_NOFOLLOW | O_NONBLOCK | O_CLOEXEC);
    #if defined(_POSIX_ADVISORY_INFO) && (_POSIX_ADVISORY_INFO >= 200112L)
    posix_fadvise(_file, 0 , 0, POSIX_FADV_SEQUENTIAL);
    #endif
    unsigned tmp = 0;

    if ((::pread(_file, &tmp, 3, 0) == 3) && (tmp == 0xBFBBEF)) {
        lseek(_file, 3, SEEK_SET);
    }
}

File::~File() {
    close(_file);
}

ssize_t File::readNext(char* buffer, size_t count) {
    return read(_file, buffer, count);
}

const char* File::fileName() {
    return _fileName;
}