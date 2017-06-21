#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string.h>
#include "reader.h"

class Parser
{
public:
    Parser(Reader* reader);
    ~Parser();
    Parser(const Parser &parser);
    Parser & operator=(const Parser &parser);

    const char* recordBegin();
    size_t recordLength();
    unsigned long recordNumber();
    bool next();

private:
    Reader* _reader;
    char* _bufferBegin = nullptr;
    char* _bufferPosition = nullptr;
    char* _bufferEnd = nullptr;
    char* _recordBegin = nullptr;
    char* _recordEnd = nullptr;
    unsigned long _recordNumber = 0;
    unsigned long _initialBufferSize = 32768;

    char* findFirst(const char separator);
};

#endif //PARSER_H