#include <iostream>
#include <fstream>
#include <stdlib.h>

#include <windows.h>
#include <Shlwapi.h>

#include "GetWordGridFromFile.h"
#include "GetWordGridSizeFromFile.h"

#include "SmartList.h"
#include "Dictionary.h"
#include "strappend.h"

using namespace std;

#define DICT_NAME "US.dic"


const char* DIRS[] = {"left",
                      "left-down",
                      "down",
                      "right-down",
                      "right",
                      "right-up",
                      "up",
                      "left-up"};

bool findWordPosInGrid(char**, Val2D<int>, const char*, Val2D<int>&, int&);

void listAllWordsInGrid(Dictionary&, char**, Val2D<int>, bool, int, int);


int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cout << "\nDid you forget to include the word-search file?\n";
        cout << "Usage: analyzewordsearch \"fileName.example\" <all {from X} {to Y} {noignore}>\n\n";
        cout << "X = min. characters, Y = max. characters.\n";
        cout << "noignore tells the program to allow duplicate results.\n";
        cout << "<> indicates optional mode which lists all words.\n";
        cout << "{} indicate optional additional parameters.\n";

        return -1;
    }
    else
    {
        cout << "Loading \"" << argv[1] << "\"";

        Val2D<int> size = GetGridSzFromFile(argv[1]);
        char** grid = GetGridFromFile(size, argv[1]);

        cout << " and displaying grid:\n\n";


        cout << "  ";
        for (int i=0; i<size.X; i++)
            cout << (char)('A' + i);
        cout << "\n  ";
        for (int i=0; i<size.X; i++)
            cout << '-';
        cout << endl;

        for (int y=0; y<size.Y; y++)
        {
            cout << (char)('A' + y) << '|';

            for (int x=0; x<size.X; x++)
                cout << grid[y][x];

            cout << "\n";
        }
        cout << endl;


        if (argc >= 3)
        {
            if (compare(argv[2], "all", ignore) == 0)
            {
                char DICT[MAX_PATH];

                GetModuleFileName(NULL, DICT, MAX_PATH);

                int pos = len(DICT) -1;

                while ((DICT[pos] != '\\') && (DICT[pos] != '/')
                       && (pos > 0))
                {
                    DICT[pos] = '\0';
                    pos--;
                }

                for (size_t i=0; i<len(DICT_NAME); i++)
                {
                    DICT[pos+i+1] = DICT_NAME[i];
                }


                cout << "Loading Dictionary...";

                //load data in dict file
                ifstream fin;
                fin.open(DICT);

                if (!fin)
                {
                    cout << "\nUnable to load dictionary \"" << DICT << "\".\n";
                    cout << "Perhaps it was moved or deleted?\n";
                    return -1;
                }

                fin.seekg(0,ios::end);
                streamsize sz = fin.tellg();
                fin.seekg(0,ios::beg);

                char* contents = new char[sz];
                fin.read(contents, sz);

                fin.close();
                //done reading file


                Dictionary dic;
                dic.set(contents, true);

                delete[] contents;

                cout << "Done.\n";

                bool IgnoreDuplicates = !(compare(argv[argc-1], "noignore", ignore) == 0);
                int hibound = -1;
                int lobound = -1;

                cout << "Finding all words in grid";

                int goalarg = argc;
                if (!IgnoreDuplicates)
                    goalarg--;

                if (goalarg >= 7)
                {
                    if ((compare(argv[3], "from", ignore) == 0) &&
                        (compare(argv[5], "to", ignore) == 0))
                    {
                        lobound = atoi(argv[4]);
                        hibound = atoi(argv[6]);

                        cout << ", with " << lobound << "-" << hibound << " characters";
                    }
                }
                else if (goalarg >= 5)
                {
                    if (compare(argv[3], "from", ignore) == 0)
                    {
                        lobound = atoi(argv[4]);
                        cout << ", with " << lobound << "+ characters";
                    }
                    else if (compare(argv[3], "to", ignore) == 0)
                    {
                        hibound = atoi(argv[4]);
                        cout << ", with " << hibound << "- characters";
                    }
                }

                if (!IgnoreDuplicates)
                {
                    cout << ", duplicates included";
                }

                cout << "...\n\n";

                listAllWordsInGrid(dic, grid, size, IgnoreDuplicates, lobound, hibound);

                cout << "\nDone.";
            }
            else
            {
                cout << "unknown command \"" << argv[2] << "\"\n";
            }
        }
        else
        {
            char word[256];

            cout << "(type \"/\" to exit)\n";

            cout << "Search for word: ";
            cin >> word;

            while (word[0] != '/')
            {
                Val2D<int> pos;
                int dir;

                if (findWordPosInGrid(grid, size, word, pos, dir))
                {
                    cout << "Found \"" << word << "\" at position [";
                    cout << (char)('A' + pos.X) << ", " << (char)('A' + pos.Y) << "], going " << DIRS[dir] << ".\n\n";


                }
                else
                {
                    cout << "\"" << word << "\" was not found.\n\n";
                }

                cout << "Search for word: ";
                cin >> word;
            }
        }


        for (int i=0; i<size.Y; i++)
            delete[] grid[i];
        delete[] grid;
    }

    return 0;
}


bool findWordPosInGrid(char** grid, Val2D<int> gridsz, const char* word, Val2D<int>& pos, int& direction)
{
    for (int y=0; y<gridsz.Y; y++)
    {
        for (int x=0; x<gridsz.X; x++)
        {
            for (int dir=0; dir<8; dir++)
            {
                int atx = x;
                int aty = y;

                bool good = true;

                for (size_t i=0; i<len(word); i++)
                {
                    if ((atx >= 0) && (atx < gridsz.X) &&
                        (aty >= 0) && (aty < gridsz.Y))
                    {
                        if (toupper(grid[aty][atx]) != toupper(word[i]))
                        {
                            good = false;
                        }
                        else
                        {
                            //cout << atx << ',' << aty << ':' << i << endl;
                        }
                    }
                    else
                    {
                        good = false;
                    }


                    if (dir == 0)
                    {
                        atx += 1;
                        aty += 0;
                    }
                    else if (dir == 1)
                    {
                        atx += 1;
                        aty += 1;
                    }
                    else if (dir == 2)
                    {
                        atx += 0;
                        aty += 1;
                    }
                    else if (dir == 3)
                    {
                        atx += -1;
                        aty += 1;
                    }
                    else if (dir == 4)
                    {
                        atx += -1;
                        aty += 0;
                    }
                    else if (dir == 5)
                    {
                        atx += -1;
                        aty += -1;
                    }
                    else if (dir == 6)
                    {
                        atx += 0;
                        aty += -1;
                    }
                    else if (dir == 7)
                    {
                        atx += 1;
                        aty += -1;
                    }

                }

                if (good)
                {
                    pos.X = x;
                    pos.Y = y;

                    direction = dir;

                    return true;
                }
            }
        }
    }

    return false;
}


void listAllWordsInGrid(Dictionary& dic, char** grid, Val2D<int> gridsz, bool IgnoreDuplicates, int lowBound, int highBound)
{
    SmartList list;

    for (int y=0; y<gridsz.Y; y++)
    {
        for (int x=0; x<gridsz.X; x++)
        {
            for (int dir=0; dir<8; dir++)
            {
                char* word = NULL;

                int atx = x;
                int aty = y;

                int length = 0;

                //bool good = false;

                while ((atx >= 0) && (atx < gridsz.X) &&
                       (aty >= 0) && (aty < gridsz.Y))
                {
                    length++;
                    append(word, grid[aty][atx]);

                    if (dic.isWord(word, ignore))
                    {
                        bool display = true;

                        if ((lowBound > -1) && (length < lowBound))
                            display = false;

                        if ((highBound > -1) && (length > highBound))
                            display = false;


                        if (IgnoreDuplicates)
                        {
                            if (!list.is_in_list(word))
                            {
                                if (display)
                                {
                                    cout << "Found \"" << word << "\" at position [";
                                    cout << (char)('A' + x) << ", " << (char)('A' + y) << "], going " << DIRS[dir] << ".\n";
                                }
                            }

                            list.add(word);
                        }
                        else
                        {
                            if (display)
                            {
                                cout << "Found \"" << word << "\" at position [";
                                cout << (char)('A' + x) << ", " << (char)('A' + y) << "], going " << DIRS[dir] << ".\n";
                            }
                        }

                    }


                    if (dir == 0)
                    {
                        atx += 1;
                        aty += 0;
                    }
                    else if (dir == 1)
                    {
                        atx += 1;
                        aty += 1;
                    }
                    else if (dir == 2)
                    {
                        atx += 0;
                        aty += 1;
                    }
                    else if (dir == 3)
                    {
                        atx += -1;
                        aty += 1;
                    }
                    else if (dir == 4)
                    {
                        atx += -1;
                        aty += 0;
                    }
                    else if (dir == 5)
                    {
                        atx += -1;
                        aty += -1;
                    }
                    else if (dir == 6)
                    {
                        atx += 0;
                        aty += -1;
                    }
                    else if (dir == 7)
                    {
                        atx += 1;
                        aty += -1;
                    }
                }

                if (word)
                    delete[] word;
            }
        }
    }
}
