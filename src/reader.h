#ifndef READER_H
#define READER_H

class Reader
{
public:
    virtual ssize_t readNext(char* buffer, size_t count) = 0;
    virtual ~Reader(){};
};

#endif //READER_H