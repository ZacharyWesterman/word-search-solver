#ifndef ISSPACE_H_INCLUDED
#define ISSPACE_H_INCLUDED

bool isSpace(const wchar_t _char)
{
    if ((_char == L' ') ||
        (_char == L'\r') ||
        (_char == L'\n') ||
        (_char == L'\t') ||
        (_char == L'\f') ||
        (_char == L'\v'))
        return true;
    else
        return false;
}

bool isSpace(const char _char)
{
    if ((_char == ' ') ||
        (_char == '\r') ||
        (_char == '\n') ||
        (_char == '\t') ||
        (_char == '\f') ||
        (_char == '\v'))
        return true;
    else
        return false;
}

#endif // ISSPACE_H_INCLUDED
