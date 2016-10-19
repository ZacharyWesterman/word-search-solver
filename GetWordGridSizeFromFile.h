#ifndef GETWORDGRIDSIZEFROMFILE_H_INCLUDED
#define GETWORDGRIDSIZEFROMFILE_H_INCLUDED

#include <fstream>
#include "Val2D.h"

Val2D<int> GetGridSzFromFile(const char* FileName)
{
    Val2D<int> GridSize;

    GridSize.X = 0;
    GridSize.Y = 0;


    std::ifstream InputFile;
    InputFile.open(FileName);

    char CurrentChar = InputFile.get();
    int Alpha_chars = 0;

    bool firstLine = true;

    while (!InputFile.eof())
    {
        if (((CurrentChar >= 'a') and (CurrentChar <= 'z')) or
            ((CurrentChar >= 'A') and (CurrentChar <= 'Z')))
        {
            if (firstLine)
                GridSize.X++;

            Alpha_chars++;
        }
        else if (Alpha_chars > 0)
        {
            firstLine = false;
        }


        CurrentChar = InputFile.get();
    }

    GridSize.Y = Alpha_chars / GridSize.X;


    InputFile.close();
    return GridSize;
}

#endif // GETWORDGRIDSIZEFROMFILE_H_INCLUDED
