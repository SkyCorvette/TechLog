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

    const char* recordBegin();
    size_t recordLength();
    unsigned long recordNumber();
    bool next();
    const char* fileName();

private:
    Reader* _reader;
    char* _bufferBegin = nullptr;
    char* _bufferPosition = nullptr;
    char* _bufferEnd = nullptr;
    char* _recordBegin = nullptr;
    char* _recordEnd = nullptr;
    unsigned long _recordNumber = 0;
    const unsigned long _initialBufferSize = 32768;

    char* findFirst(const char separator);

    Parser(const Parser &parser);
    Parser& operator=(const Parser &parser);
};

#endif //PARSER_H