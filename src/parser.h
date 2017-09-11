#ifndef TECHLOG_PARSER_H
#define TECHLOG_PARSER_H

#include <iostream>
#include <cstring>
#include "reader.h"

class Parser {
public:
    explicit Parser(Reader* reader);
    ~Parser();

    const char* recordBegin();
    size_t recordLength();
    long unsigned recordNumber();
    bool next();
    const char* fileName();

private:
    Reader* _reader;
    char* _bufferBegin = nullptr;
    char* _bufferPosition = nullptr;
    char* _bufferEnd = nullptr;
    char* _recordBegin = nullptr;
    char* _recordEnd = nullptr;
    long unsigned _recordNumber = 0;
    const long unsigned _initialBufferSize = 32768;

    char* findFirst(char separator);

    Parser(const Parser &parser);
    Parser& operator=(const Parser &parser);
};

#endif