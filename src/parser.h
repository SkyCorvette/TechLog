#ifndef TECHLOG_PARSER_H
#define TECHLOG_PARSER_H

#include "reader.h"

class Parser {
public:
    explicit Parser(Reader* reader);
    ~Parser();

    [[nodiscard]] const char* recordBegin() const;
    [[nodiscard]] size_t recordLength() const;
    [[nodiscard]] long unsigned recordNumber() const;
    bool next();
    [[nodiscard]] const char* fileName() const;

private:
    Reader* _reader;
    char* _bufferBegin = nullptr;
    char* _bufferPosition = nullptr;
    char* _bufferEnd = nullptr;
    char* _recordBegin = nullptr;
    char* _recordEnd = nullptr;
    long unsigned _recordNumber = 0;
    const long unsigned _initialBufferSize = 32768;

    [[nodiscard]] char* findFirstNewLine() const;
};

#endif