#ifndef GRIDSUBSTRING_H_INCLUDED
#define GRIDSUBSTRING_H_INCLUDED

#include "Val2D.h"

#define substrRD(a, b, c, d, e) substrDR(a, b, c, d, e)

char* substrR(char** WordGrid, Val2D<int> GridSize, int Xmin, int Xmax, int Y)
{

    if (Xmin < 0)
        Xmin = 0;

    if (Xmin >= GridSize.X)
        Xmin = GridSize.X - 1;

    if (Xmax < 0)
        Xmax = 0;

    if (Xmax >= GridSize.X)
        Xmax = GridSize.X - 1;

    if (Y < 0)
        Y = 0;

    if (Y >= GridSize.Y)
        Y = GridSize.Y - 1;



    char* SubString = new char[Xmax - Xmin + 2];

    int i = 0;
    for (int x=Xmin; x<=Xmax; x++)
    {
        SubString[i] = WordGrid[Y][x];
        i++;
    }

    SubString[Xmax - Xmin + 1] = '\0';

    return SubString;
}


char* substrL(char** WordGrid, Val2D<int> GridSize, int Xmin, int Xmax, int Y)
{

    if (Xmin < 0)
        Xmin = 0;

    if (Xmin >= GridSize.X)
        Xmin = GridSize.X - 1;

    if (Xmax < 0)
        Xmax = 0;

    if (Xmax >= GridSize.X)
        Xmax = GridSize.X - 1;

    if (Y < 0)
        Y = 0;

    if (Y >= GridSize.Y)
        Y = GridSize.Y - 1;



    char* SubString = new char[Xmax - Xmin + 2];

    int i = 0;
    for (int x=Xmax; x>=Xmin; x--)
    {
        SubString[i] = WordGrid[Y][x];
        i++;
    }

    SubString[Xmax - Xmin + 1] = '\0';

    return SubString;
}


char* substrU(char** WordGrid, Val2D<int> GridSize, int X, int Ymin, int Ymax)
{

    if (Ymin < 0)
        Ymin = 0;

    if (Ymin >= GridSize.Y)
        Ymin = GridSize.Y - 1;

    if (Ymax < 0)
        Ymax = 0;

    if (Ymax >= GridSize.X)
        Ymax = GridSize.X - 1;

    if (X < 0)
        X = 0;

    if (X >= GridSize.X)
        X = GridSize.X - 1;



    char* SubString = new char[Ymax - Ymin + 2];

    int i = 0;
    for (int y=Ymax; y>=Ymin; y--)
    {
        SubString[i] = WordGrid[y][X];
        i++;
    }

    SubString[Ymax - Ymin + 1] = '\0';

    return SubString;
}


char* substrD(char** WordGrid, Val2D<int> GridSize, int X, int Ymin, int Ymax)
{

    if (Ymin < 0)
        Ymin = 0;

    if (Ymin >= GridSize.Y)
        Ymin = GridSize.Y - 1;

    if (Ymax < 0)
        Ymax = 0;

    if (Ymax >= GridSize.X)
        Ymax = GridSize.X - 1;

    if (X < 0)
        X = 0;

    if (X >= GridSize.X)
        X = GridSize.X - 1;



    char* SubString = new char[Ymax - Ymin + 2];

    int i = 0;
    for (int y=Ymin; y<=Ymax; y++)
    {
        SubString[i] = WordGrid[y][X];
        i++;
    }

    SubString[Ymax - Ymin + 1] = '\0';

    return SubString;
}


char* substrDR(char** WordGrid, Val2D<int> GridSize, int Xmin, int Xmax, int Ymin, int Ymax)
{

    if (Ymin < 0)
        Ymin = 0;

    if (Ymin >= GridSize.Y)
        Ymin = GridSize.Y - 1;

    if (Ymax < 0)
        Ymax = 0;

    if (Ymax >= GridSize.X)
        Ymax = GridSize.X - 1;


    if (Xmin < 0)
        Xmin = 0;

    if (Xmin >= GridSize.X)
        Xmin = GridSize.X - 1;

    if (Xmax < 0)
        Xmax = 0;

    if (Xmax >= GridSize.X)
        Xmax = GridSize.X - 1;



    int Start = std::min(Xmin, Ymin);
    int Stop = std::min(Xmax, Ymax);

    char* SubString = new char[Stop - Start + 2];

    int x = Xmin;
    int y = Ymin;

    for (int i=0; i<=(Stop - Start); i++)
    {
        SubString[i] = WordGrid[y][x];

        x++;
        y++;
    }

    SubString[Stop - Start + 1] = '\0';

    return SubString;
}


char* substrUL(char** WordGrid, Val2D<int> GridSize, int Xmin, int Xmax, int Ymin, int Ymax)
{

    if (Ymin < 0)
        Ymin = 0;

    if (Ymin >= GridSize.Y)
        Ymin = GridSize.Y - 1;

    if (Ymax < 0)
        Ymax = 0;

    if (Ymax >= GridSize.X)
        Ymax = GridSize.X - 1;


    if (Xmin < 0)
        Xmin = 0;

    if (Xmin >= GridSize.X)
        Xmin = GridSize.X - 1;

    if (Xmax < 0)
        Xmax = 0;

    if (Xmax >= GridSize.X)
        Xmax = GridSize.X - 1;



    int Start = std::min(Xmin, Ymin);
    int Stop = std::min(Xmax, Ymax);

    char* SubString = new char[Stop - Start + 2];

    int x = Xmax;
    int y = Ymax;

    for (int i=0; i<=(Stop - Start); i++)
    {
        SubString[i] = WordGrid[y][x];

        x--;
        y--;
    }

    SubString[Stop - Start + 1] = '\0';

    return SubString;
}


char* substrDL(char** WordGrid, Val2D<int> GridSize, int Xmin, int Xmax, int Ymin, int Ymax)
{

    if (Ymin < 0)
        Ymin = 0;

    if (Ymin >= GridSize.Y)
        Ymin = GridSize.Y - 1;

    if (Ymax < 0)
        Ymax = 0;

    if (Ymax >= GridSize.X)
        Ymax = GridSize.X - 1;


    if (Xmin < 0)
        Xmin = 0;

    if (Xmin >= GridSize.X)
        Xmin = GridSize.X - 1;

    if (Xmax < 0)
        Xmax = 0;

    if (Xmax >= GridSize.X)
        Xmax = GridSize.X - 1;



    int Start = std::min(Xmin, Ymin);
    int Stop = std::min(Xmax, Ymax);

    char* SubString = new char[Stop - Start + 2];

    int x = Xmax;
    int y = Ymin;

    for (int i=0; i<=(Stop - Start); i++)
    {
        SubString[i] = WordGrid[y][x];

        x--;
        y++;
    }

    SubString[Stop - Start + 1] = '\0';

    return SubString;
}


char* substrUR(char** WordGrid, Val2D<int> GridSize, int Xmin, int Xmax, int Ymin, int Ymax)
{

    if (Ymin < 0)
        Ymin = 0;

    if (Ymin >= GridSize.Y)
        Ymin = GridSize.Y - 1;

    if (Ymax < 0)
        Ymax = 0;

    if (Ymax >= GridSize.X)
        Ymax = GridSize.X - 1;


    if (Xmin < 0)
        Xmin = 0;

    if (Xmin >= GridSize.X)
        Xmin = GridSize.X - 1;

    if (Xmax < 0)
        Xmax = 0;

    if (Xmax >= GridSize.X)
        Xmax = GridSize.X - 1;



    int Start = std::min(Xmin, Ymin);
    int Stop = std::min(Xmax, Ymax);

    char* SubString = new char[Stop - Start + 2];

    int x = Xmin;
    int y = Ymax;

    for (int i=0; i<=(Stop - Start); i++)
    {
        SubString[i] = WordGrid[y][x];

        x++;
        y--;
    }

    SubString[Stop - Start + 1] = '\0';

    return SubString;
}

#endif // GRIDSUBSTRING_H_INCLUDED
