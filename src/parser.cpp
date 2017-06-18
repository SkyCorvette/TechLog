// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "parser.h"

Parser::Parser(Reader* reader) : _reader (reader)
{
    _bufferBegin = static_cast<char*>(malloc(_initialBufferSize));
    _bufferPosition = _bufferBegin;
    _bufferEnd = nullptr;
    _recordBegin = _bufferBegin;
    _recordEnd = nullptr;
}

const char* Parser::recordBegin()
{
    return _recordBegin;
}

size_t Parser::recordLength()
{
    return _recordEnd ? static_cast<size_t>(_recordEnd - _recordBegin) : 0;
}

unsigned long Parser::recordNumber()
{
    return _recordNumber;
}

bool Parser::next()
{
    while (true)
    {
        if (!_bufferEnd)
        {
            ssize_t bytesRead = _reader->readNext(_bufferPosition, _initialBufferSize);

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
            _bufferBegin = static_cast<char*>(malloc(_initialBufferSize));
            _bufferPosition = _bufferBegin;
        }
        else
        {
            size_t savedSize = static_cast<size_t>(_bufferEnd - _recordBegin);
            memmove(_bufferBegin, _recordBegin, savedSize);
            char* tmp = static_cast<char*>(realloc(_bufferBegin, savedSize + _initialBufferSize));
            if (tmp)
            {
                _bufferBegin = tmp;
                _bufferPosition = _bufferBegin + savedSize;
            }
        }
        _recordBegin = _bufferBegin;
        _bufferEnd = nullptr;
    }
}

char* Parser::findFirst(const char separator)
{
    auto buf = _recordBegin;
    char* sp = nullptr;
    char* dq = nullptr;
    char* sq = nullptr;
    auto inDQ = false;
    auto inSQ = false;

    while ((sp = static_cast<char*>(memchr(buf, separator, static_cast<size_t>(_bufferEnd - buf)))))
    {
        dq = inSQ ? nullptr : static_cast<char*>(memchr(buf, '"', static_cast<size_t>(sp - buf)));
        sq = inDQ ? nullptr : static_cast<char*>(memchr(buf, '\'', static_cast<size_t>(sp - buf)));

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