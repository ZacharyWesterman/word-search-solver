#ifndef COMPARESTRINGS_H_INCLUDED
#define COMPARESTRINGS_H_INCLUDED

#include "CharacterCase.h"
#include "charDef.h"

enum CompareMode
{
    strict,
    language,
    ignore
};


//based on characters' numerical order
//returns: -1 if Word1 comes before Word2 (is lesser),
//         0 if Word1 is identical to Word2,
//         1 if Word1 comes after Word2 (is greater)
int compare(const Char* Word1, const Char* Word2, CompareMode Mode)
{
    int position = 0;


    while ( !((Word1[position] == 0) and (Word2[position] == 0)) )
    {
        char Compare1 = Word1[position];
        char Compare2 = Word2[position];


        if (Mode == strict)
        {
            if (Compare1 < Compare2)
            {
                return -1;
            }
            else if (Compare1 > Compare2)
            {
                return 1;
            }
        }
        else if (Mode == language)
        {
            if (isUpper(Compare1) and isLower(Compare2))
            {
                Compare1 += 32;

                if (Compare1 < Compare2)
                {
                    return -1;
                }
                else if (Compare1 >= Compare2)
                {
                    return 1;
                }
            }
            else if (isLower(Compare1) and isUpper(Compare2))
            {
                Compare2 += 32;

                if (Compare1 <= Compare2)
                {
                    return -1;
                }
                else if (Compare1 > Compare2)
                {
                    return 1;
                }
            }
            else
            {
                if (Compare1 < Compare2)
                {
                    return -1;
                }
                else if (Compare1 > Compare2)
                {
                    return 1;
                }
            }
        }
        else if (Mode == ignore)
        {
            if (isUpper(Compare1))
                Compare1 += 32;

            if (isUpper(Compare2))
                Compare2 += 32;


            if (Compare1 < Compare2)
            {
                return -1;
            }
            else if (Compare1 > Compare2)
            {
                return 1;
            }
        }


        position++;
    }


    return 0;
}

#endif // COMPARESTRINGS_H_INCLUDED
