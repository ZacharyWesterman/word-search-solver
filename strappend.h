#ifndef STRAPPEND_H_INCLUDED
#define STRAPPEND_H_INCLUDED

#include "StringLength.h"

#ifndef str
#define str char*
#endif // str

#ifndef chr
#define chr char
#endif // chr

#ifndef null
#define null '\0'
#endif // null

//function that appends char B to string A
void append(str& A, const chr B)
{
    int length = 0;

    if (A)
        length = len(A);

    str newA = new chr[length + 2];


    for (int i=0; i<length; i++)
        newA[i] = A[i];

    newA[length] = B;
    newA[length + 1] = null;


    if (A)
        delete[] A;

    A = newA;
}

//function appends string B to string A
void append(str& A, const str B)
{
    int lenA = 0;
    int lenB = 0;

    if (A)
        lenA = len(A);

    if (B)
        lenB = len(B);


    str newA = new chr[lenA + lenB + 1];


    for (int i=0; i<lenA; i++)
        newA[i] = A[i];

    for (int i=0; i<lenB; i++)
        newA[lenA + i] = B[i];

    newA[lenA + lenB] = null;


    if (A)
        delete[] A;

    A = newA;
}

#endif // STRAPPEND_H_INCLUDED
