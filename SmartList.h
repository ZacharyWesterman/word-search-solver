#ifndef SMARTLIST_H_INCLUDED
#define SMARTLIST_H_INCLUDED

#include "Val2D.h"
#include "CompareStrings.h"

class SmartList
{
private:
    CompareMode Mode;
    int ListLength;
    char** WordList;
    Val2D<int>* PosList;

    bool in_use;
    bool sorted;

public:
    SmartList();
    ~SmartList();

    void add(const char*, Val2D<int>);
    void add(const char*);
    const char* word(int) const;
    Val2D<int> pos(int) const;
    int length() const;

    void sort();
    bool is_in_list(const char*) const;
    bool is_in_list(const char*, CompareMode) const;
};


SmartList::SmartList()
{
    Mode = language;
    ListLength = 0;

    in_use = false;
    sorted = false;
}

SmartList::~SmartList()
{
    if (in_use)
    {
        for (int i=0; i<ListLength; i++)
            delete[] WordList[i];

        delete[] WordList;
        delete[] PosList;
    }
}

void SmartList::add(const char* Word)
{
    Val2D<int> val;
    val.X = 0;
    val.Y = 0;

    add(Word, val);
}

void SmartList::add(const char* Word, Val2D<int> Position)
{
    bool already_in_list = false;

    for (int i=0; i<ListLength; i++)
    {
        if (compare(WordList[i], Word, ignore) == 0)
            already_in_list = true;
    }


    if (!already_in_list)
    {
        if (in_use)
        {
            char** tmpWordList = WordList;

            WordList = new char*[ListLength + 1];

            for (int i=0; i<ListLength; i++)
                WordList[i] = tmpWordList[i];

            delete[] tmpWordList;


            Val2D<int>* tmpPosList = PosList;

            PosList = new Val2D<int>[ListLength + 1];

            for (int i=0; i<ListLength; i++)
                PosList[i] = tmpPosList[i];

            delete[] tmpPosList;
        }
        else
        {
            WordList = new char*[1];
            PosList = new Val2D<int>[1];

            in_use = true;
        }



        int position = 0;
        while (Word[position] != '\0')
        {
            position++;
        }
        position++;

        WordList[ListLength] = new char[position];
        for (int i=0; i<position; i++)
        {
            WordList[ListLength][i] = Word[i];
        }

        PosList[ListLength].X = Position.X;
        PosList[ListLength].Y = Position.Y;

        ListLength++;
    }
}


const char* SmartList::word(int index) const
{
    return const_cast<const char*>(WordList[index]);
}

Val2D<int> SmartList::pos(int index) const
{
    return PosList[index];
}

int SmartList::length() const
{
    return ListLength;
}

void SmartList::sort()
{
    bool done = false;


    while (!done)
    {
        done = true;

        for (int i=0; i<(ListLength - 1); i++)
        {

            //if 1st word is greater than 2nd word, swap 'em
            if (compare(WordList[i], WordList[i+1], Mode) == 1)
            {
                char* tmp = WordList[i];
                WordList[i] = WordList[i+1];
                WordList[i+1] = tmp;

                Val2D<int> tmpp = PosList[i];
                PosList[i] = PosList[i+1];
                PosList[i+1] = tmpp;

                done = false;
            }

        }


        //now sort backwards
        for (int i=(ListLength - 1); i>0; i--)
        {

            //if 1st word is less than 2nd word, swap 'em
            if (compare(WordList[i], WordList[i-1], Mode) == -1)
            {
                char* tmp = WordList[i];
                WordList[i] = WordList[i-1];
                WordList[i-1] = tmp;

                Val2D<int> tmpp = PosList[i];
                PosList[i] = PosList[i-1];
                PosList[i-1] = tmpp;

                done = false;
            }

        }
    }


    sorted = true;
}

bool SmartList::is_in_list(const char* Word) const
{
    return is_in_list(Word, Mode);
}


bool SmartList::is_in_list(const char* Word, CompareMode CMode) const
{
    if (sorted)
    {
        //binary search
        int i = 0;
        int j = ListLength - 1;
        int k = (j + i) / 2;

        while (j - i > 0)
        {

            int word_alpha_rank = compare(WordList[k], Word, CMode);

            if (word_alpha_rank == -1)
            {
                i = k + 1;
            }
            else if (word_alpha_rank == 1)
            {
                j = k - 1;
            }
            else
            {
                return true;
            }

            k = (j + i) / 2;
        }

        if (compare(WordList[k], Word, CMode) == 0)
            return true;
    }
    else
    {
        //linear search
        for (int i=0; i<ListLength; i++)
        {
            if (compare(WordList[i], Word, CMode) == 0)
                return true;
        }
    }


    return false;
}

#endif // SMARTLIST_H_INCLUDED
