#include "standardinput.h"

ssize_t StandardInput::readNext(char *buffer, size_t count)
{
    return read(STDIN_FILENO, buffer, count);
}