#include "parser.h"
#include <iostream>
#include <cstring>

Parser::Parser(Reader* reader) : _reader(reader) {
    _bufferBegin = static_cast<char*>(malloc(_initialBufferSize));
    _bufferPosition = _bufferBegin;
    _bufferEnd = nullptr;
    _recordBegin = _bufferBegin;
    _recordEnd = nullptr;
}

Parser::~Parser() {
    free(_bufferBegin);
}

const char* Parser::recordBegin() const {
    return _recordBegin;
}

size_t Parser::recordLength() const {
    return _recordEnd != nullptr ? static_cast<size_t>(_recordEnd - _recordBegin) : 0;
}

unsigned long Parser::recordNumber() const {
    return _recordNumber;
}

bool Parser::next() {
    while (true) {
        if (_bufferEnd == nullptr) {
            ssize_t bytesRead = _reader->readNext(_bufferPosition, _initialBufferSize);
            if (bytesRead == 0) {
                return false;
            }
            _bufferEnd = _bufferPosition + bytesRead;
        }

        if (_recordEnd != nullptr) {
            _recordBegin = _recordEnd;
        }

        while(*_recordBegin == '\n' && _recordBegin < _bufferEnd) {
            ++_recordBegin;
        }

        if ((_recordEnd = findFirstNewLine()) != nullptr) {
            ++_recordNumber;
            return true;
        }
        if (_recordBegin == _bufferEnd) {
            free(_bufferBegin);
            _bufferBegin = static_cast<char*>(malloc(_initialBufferSize));
            _bufferPosition = _bufferBegin;
        }
        else {
            auto savedSize = static_cast<size_t>(_bufferEnd - _recordBegin);
            memmove(_bufferBegin, _recordBegin, savedSize);
            auto tmp = static_cast<char*>(realloc(_bufferBegin, savedSize + _initialBufferSize));
            if (tmp != nullptr) {
                _bufferBegin = tmp;
                _bufferPosition = _bufferBegin + savedSize;
            }
        }
        _recordBegin = _bufferBegin;
        _bufferEnd = nullptr;
    }
}

const char* Parser::fileName() const {
    return _reader->fileName();
}

char* Parser::findFirstNewLine() const {
    auto buf = _recordBegin;
    char* sp = nullptr;
    char* dq = nullptr;
    char* sq = nullptr;
    auto inDQ = false;
    auto inSQ = false;

    while ((sp = static_cast<char*>(memchr(buf, '\n', static_cast<size_t>(_bufferEnd - buf)))) != nullptr) {
        dq = inSQ ? nullptr : static_cast<char*>(memchr(buf, '"', static_cast<size_t>(sp - buf)));
        sq = inDQ ? nullptr : static_cast<char*>(memchr(buf, '\'', static_cast<size_t>(sp - buf)));

        if (((dq != nullptr) && (sq != nullptr) && dq < sq) || ((dq != nullptr) && (sq == nullptr))) {
            inDQ = !inDQ;
            buf = dq + 1;
        }
        else if (((dq != nullptr) && dq > sq) || ((dq == nullptr) && (sq != nullptr))) {
            inSQ = !inSQ;
            buf = sq + 1;
        }
        else if (!inDQ && !inSQ) {
            return sp;
        }
        else {
            buf = sp + 1;
        }
    }
    return nullptr;
}
