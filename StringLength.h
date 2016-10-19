#ifndef STRINGLENGTH_H_INCLUDED
#define STRINGLENGTH_H_INCLUDED

#include "charDef.h"
#include "isSpace.h"

size_t len(const Char* CString)
{
    size_t amount = 0;


    while (CString[amount] != 0)
    {
        amount++;
    }

    return amount;
}

size_t len(const Char* CString, const Char Delim)
{
    size_t amount = 0;

    while ((CString[amount] != 0) &&
           (CString[amount] != Delim))
    {
        amount++;
    }

    return amount;
}


size_t lenNoWhiteSpace(const Char* CString)
{
    size_t amount = 0;

    while ((CString[amount] != 0) &&
           !isSpace(CString[amount]))
    {
        amount++;
    }

    return amount;
}

#endif // STRINGLENGTH_H_INCLUDED
