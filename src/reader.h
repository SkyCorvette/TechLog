#ifndef TECHLOG_READER_H
#define TECHLOG_READER_H

class Reader {
public:
    virtual ssize_t readNext(char* buffer, size_t count) = 0;
    virtual const char* fileName() = 0;
    virtual ~Reader(){};
};

#endif