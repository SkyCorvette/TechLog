#ifndef EXAMPLE_PARSER_H
#define EXAMPLE_PARSER_H

#include <iostream>
#include "reader.h"

class Parser
{
public:
    Parser(Reader* reader) : _reader (reader)
    {
        _bufferBegin = (char*) malloc(_initialBufferSize);
        _bufferPosition = _bufferBegin;
        _bufferEnd = nullptr;
        _recordBegin = _bufferBegin;
        _recordEnd = nullptr;
    };

    const char* recordBegin()
    {
        return _recordBegin;
    };

    unsigned long recordLength()
    {
        return _recordEnd ? _recordEnd - _recordBegin : 0;
    };

    unsigned long recordNumber()
    {
        return _recordNumber;
    }

    bool next()
    {
        while (true)
        {
            if (!_bufferEnd)
            {
                ssize_t bytesRead = _reader->read(_bufferPosition, _initialBufferSize);

                if (!bytesRead)
                {
                    return false;
                }

                _bufferEnd = _bufferPosition + bytesRead;
            }

            if (_recordEnd)
            {
                _recordBegin = _recordEnd;
            }

            while(*_recordBegin == '\n' && _recordBegin < _bufferEnd)
            {
                ++_recordBegin;
            }

            if ((_recordEnd = findFirst('\n')))
            {
                ++_recordNumber;
                return true;
            }
            else if (_recordBegin == _bufferEnd)
            {
                free(_bufferBegin);
                _bufferBegin = (char*) malloc(_initialBufferSize);
                _bufferPosition = _bufferBegin;
            }
            else
            {
                size_t savedSize = _bufferEnd - _recordBegin;
                memmove(_bufferBegin, _recordBegin, savedSize);
                _bufferBegin = (char*) realloc(_bufferBegin, savedSize + _initialBufferSize);
                _bufferPosition = _bufferBegin + savedSize;
            }
            _recordBegin = _bufferBegin;
            _bufferEnd = nullptr;
        }
    };

private:
    Reader* _reader;
    char* _bufferBegin = nullptr;
    char* _bufferPosition = nullptr;
    char* _bufferEnd = nullptr;
    char* _recordBegin = nullptr;
    char* _recordEnd = nullptr;
    unsigned long _recordNumber = 0;
    unsigned long _initialBufferSize = 32768;

    char* findFirst(const char separator)
    {
        auto buf = _recordBegin;
        char* sp = nullptr;
        char* dq = nullptr;
        char* sq = nullptr;
        auto inDQ = false;
        auto inSQ = false;

        while ((sp = (char*) memchr(buf, separator, _bufferEnd - buf)))
        {
            dq = inSQ ? nullptr : (char*) memchr(buf, '"', sp - buf);
            sq = inDQ ? nullptr : (char*) memchr(buf, '\'', sp - buf);

            if ((dq && sq && dq < sq) || (dq && !sq))
            {
                inDQ = !inDQ;
                buf = dq + 1;
            }
            else if ((dq && dq > sq) || (!dq && sq))
            {
                inSQ = !inSQ;
                buf = sq + 1;
            }
            else if (!inDQ && !inSQ)
            {
                return sp;
            }
            else
            {
                buf = sp + 1;
            }
        }
        return nullptr;
    }
};

#endif //EXAMPLE_PARSER_H