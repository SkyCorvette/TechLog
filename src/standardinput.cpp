// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "standardinput.h"

ssize_t StandardInput::readNext(char *buffer, size_t count)
{
    return read(STDIN_FILENO, buffer, count);
}