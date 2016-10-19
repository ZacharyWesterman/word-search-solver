#ifndef GETWORDGRIDFROMFILE_H_INCLUDED
#define GETWORDGRIDFROMFILE_H_INCLUDED

#include <fstream>
#include "Val2D.h"

char** GetGridFromFile(Val2D<int> GridSize, const char* FileName)
{
    char** WordGrid = new char*[GridSize.Y];
    for (int i=0; i<GridSize.Y; i++)
        WordGrid[i] = new char[GridSize.X];


    std::ifstream InputFile;
    InputFile.open(FileName);


    int x = 0;
    int y = 0;

    char CurrentChar = InputFile.get();

    while (!InputFile.eof())
    {
        if (((CurrentChar >= 'a') and (CurrentChar <= 'z')) or
            ((CurrentChar >= 'A') and (CurrentChar <= 'Z')))
        {
            WordGrid[y][x] = CurrentChar;

            x++;
        }

        if (x >= GridSize.X)
        {
            y++;
            x = 0;
        }

        if (y >= GridSize.Y)
        {
            break;
        }


        CurrentChar = InputFile.get();
    }


    InputFile.close();
    return WordGrid;
}

#endif // GETWORDGRIDFROMFILE_H_INCLUDED
