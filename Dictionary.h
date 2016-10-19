#ifndef DICTIONARY_H_INCLUDED
#define DICTIONARY_H_INCLUDED

#include "charDef.h"
#include "CompareStrings.h"
#include "StringLength.h"

#ifndef SORTING_AUTO_DETECT_DEFAULT
#define SORTING_AUTO_DETECT_DEFAULT false
#endif // SORTING_AUTO_DETECT_DEFAULT

#define IS_WORD_COMPARE_MODE_DEFAULT ignore
#define WORD_NOT_IN_DICT ((size_t)-1)


enum DICTIONARY_SORTING
{
    NONE,
    ALPHABETICAL,
    LENGTHWISE
};


class Dictionary
{
private:
    DICTIONARY_SORTING Sorting;
    bool isDecreasingOrder;

    size_t ListSize;
    Char** WordList;

    bool isLoaded;

    size_t getWordIndex_NoSorting(const Char*, CompareMode);

    size_t getWordIndex_AlphaSortingINC(const Char*, CompareMode);
    size_t getWordIndex_AlphaSortingDEC(const Char*, CompareMode);

    size_t getWordIndex_LenWiseSortingINC(const Char*, CompareMode);
    size_t getWordIndex_LenWiseSortingDEC(const Char*, CompareMode);

    void sortByAlphaINC_NON_ITER();
    void sortByLengthINC_NON_ITER();

    bool sortByAlphaINC_ITER();
    bool sortByLengthINC_ITER();

public:
    Dictionary();
    Dictionary(const Char*);
    ~Dictionary();

    void set(const Char*);
    void set(const Char*, bool);

    void clear();

    void get(Char*&);

    void autoDetectSorting();

    size_t wordCount();

    bool sortStep(DICTIONARY_SORTING, bool);

    void setSortOrder(DICTIONARY_SORTING);

    bool isWord(const Char*);
    bool isWord(const Char*, CompareMode);
};

Dictionary::Dictionary()
{
    Sorting = NONE;
    isLoaded = false;
    ListSize = 0;

    isDecreasingOrder = false;
}

Dictionary::Dictionary(const Char* DictString)
{
    Sorting = NONE;
    isLoaded = false;
    ListSize = 0;

    isDecreasingOrder = false;

    set(DictString);
}

Dictionary::~Dictionary()
{
    clear();
}


void Dictionary::set(const Char* DictString)
{
    set(DictString, SORTING_AUTO_DETECT_DEFAULT);
}

void Dictionary::set(const Char* DictString, bool detectSorting)
{
    clear();


    //get word count of dictionary
    bool encounteredWord;
    bool readyCountWord = true;

    size_t i = 0;
    size_t Words = 0;

    while(DictString[i] != 0)
    {
        if (!isSpace(DictString[i]))
        {
            encounteredWord = true;

            if (readyCountWord)
            {
                Words++;
                readyCountWord = false;
            }
        }
        else
        {
            readyCountWord = true;
        }

        i++;
    }


    //if there is only white space in the input,
    //then there are no words in the dictionary.
    if (encounteredWord)
    {
        WordList = new Char*[Words];

        //load words into dictionary
        i = 0;
        size_t thisWord = 0;

        while (DictString[i] != 0)
        {
            if (!isSpace(DictString[i]))
            {
                size_t length = lenNoWhiteSpace(&DictString[i]);

                WordList[thisWord] = new Char[length+1];

                size_t j = 0;
                for(j=0; j<length; j++)
                {
                    WordList[thisWord][j] = DictString[i+j];
                }

                WordList[thisWord][length] = 0;

                thisWord++;

                i += j;

            }

            i++;
        }


        isLoaded = true;
        ListSize = Words;

        if (detectSorting)
            autoDetectSorting();
    }
    else
    {
        isLoaded = false;
        ListSize = 0;
    }
}


void Dictionary::clear()
{
    if (isLoaded)
    {
        for (size_t i=0; i<ListSize; i++)
            delete[] WordList[i];

        delete[] WordList;
    }

    Sorting = NONE;
    isLoaded = false;
    ListSize = 0;

    isDecreasingOrder = false;
}

void Dictionary::get(Char*& DictString)
{
    if (isLoaded)
    {
        if (DictString)
            delete[] DictString;

        //count the total number of characters in the dictionary
        size_t fullLength = 0;

        for (size_t i=0; i<ListSize; i++)
        {
            fullLength += len(WordList[i]);
        }

        //extra size needed to accommodate carriage return and newline
        size_t CRNL = (ListSize - 1) * 2;


        DictString = new Char[fullLength + CRNL + 1];

        //put all words into output CString
        size_t currentChar = 0;

        size_t length = len(WordList[0]);

        //append first word
        for(size_t j=0; j<length; j++)
        {
            DictString[currentChar + j] = WordList[0][j];
        }
        currentChar += length;

        //then for each word after that,
        for (size_t i=1; i<ListSize; i++)
        {
            //append "\r\n"
            DictString[currentChar] = CR;
            currentChar++;
            DictString[currentChar] = NL;
            currentChar++;

            length = len(WordList[i]);

            //append the current word
            for(size_t j=0; j<length; j++)
            {
                DictString[currentChar + j] = WordList[i][j];
            }
            currentChar += length;
        }

        //make output string null-terminated
        DictString[fullLength + CRNL] = 0;
    }

}


void Dictionary::autoDetectSorting()
{
    //for automatic detect how dictionary is sorted
    bool isAlphaOrderedINC = true;
    bool isLenWiseOrderedINC = true;

    bool isAlphaOrderedDEC = true;
    bool isLenWiseOrderedDEC = true;


    for (size_t i=0; i<ListSize-1; i++)
    {
        //compare lengths to determine if dictionary
        //may be in lengthwise order
        size_t length1 = len(WordList[i]);
        size_t length2 = len(WordList[i+1]);

        if (length1 > length2)
        {
            isLenWiseOrderedINC = false;
        }
        else if (length1 < length2)
        {
            isLenWiseOrderedDEC = false;
        }

        //compare words to determine if dictionary
        //may be in alphabetical order
        int compareStatus = compare(WordList[i], WordList[i+1], language);

        if (compareStatus == -1)//first word comes before second alphabetically
        {
            isAlphaOrderedDEC = false;
        }
        else if (compareStatus == 1)//first word comes after second alphabetically
        {
            isAlphaOrderedINC = false;
        }


        //if we have determined that the dictionary is not sorted, break.
        if (!isLenWiseOrderedDEC && !isLenWiseOrderedINC &&
            !isAlphaOrderedDEC && !isAlphaOrderedINC)
            break;
    }


    if (isAlphaOrderedINC)
    {
        if (isAlphaOrderedDEC)
        {
            Sorting = NONE;
        }
        else
        {
            Sorting = ALPHABETICAL;
        }

        isDecreasingOrder = false;
    }
    else if (isAlphaOrderedDEC)
    {
        Sorting = ALPHABETICAL;
        isDecreasingOrder = true;
    }
    else if (isLenWiseOrderedINC)
    {
        if (isLenWiseOrderedDEC)
        {
            Sorting = NONE;
        }
        else
        {
            Sorting = LENGTHWISE;
        }

        isDecreasingOrder = false;
    }
    else if (isLenWiseOrderedDEC)
    {
        Sorting = LENGTHWISE;
        isDecreasingOrder = true;
    }
    else
    {
        Sorting = NONE;
        isDecreasingOrder = false;
    }
}


size_t Dictionary::wordCount()
{
    return ListSize;
}


bool Dictionary::isWord(const Char* Word)
{
    return isWord(Word, IS_WORD_COMPARE_MODE_DEFAULT);
}

bool Dictionary::isWord(const Char* Word, CompareMode Mode)
{
    if (isLoaded)
    {
        size_t wordIndex = WORD_NOT_IN_DICT;


        //if dictionary is unsorted
        if (Sorting == NONE)
        {
            wordIndex = getWordIndex_NoSorting(Word, Mode);
        }
        //if dictionary is sorted alphabetically
        else if (Sorting == ALPHABETICAL)
        {
            if (isDecreasingOrder)
                wordIndex = getWordIndex_AlphaSortingDEC(Word, Mode);
            else
                wordIndex = getWordIndex_AlphaSortingINC(Word, Mode);
        }
        //if dictionary is sorted by length
        else if (Sorting == LENGTHWISE)
        {
            if (isDecreasingOrder)
                wordIndex = getWordIndex_LenWiseSortingDEC(Word, Mode);
            else
                wordIndex = getWordIndex_LenWiseSortingINC(Word, Mode);
        }


        if (wordIndex == WORD_NOT_IN_DICT)
            return false;
        else
            return true;
    }
    else
    {
        return false;
    }
}

//uses modified bubble sort to sort dictionary alphabetically
void Dictionary::sortByAlphaINC_NON_ITER()
{
    if (isLoaded)
    {
        bool done = false;


        while (!done)
        {
            done = true;

            for (size_t i=0; i<(ListSize - 1); i++)
            {

                //if 1st word is greater than 2nd word, swap 'em
                if (compare(WordList[i], WordList[i+1], language) == 1)
                {
                    char* tmp = WordList[i];
                    WordList[i] = WordList[i+1];
                    WordList[i+1] = tmp;

                    done = false;
                }

            }


            //now sort backwards
            for (int i=(ListSize - 1); i>0; i--)
            {

                //if 1st word is less than 2nd word, swap 'em
                if (compare(WordList[i], WordList[i-1], language) == -1)
                {
                    char* tmp = WordList[i];
                    WordList[i] = WordList[i-1];
                    WordList[i-1] = tmp;

                    done = false;
                }

            }
        }


        Sorting = ALPHABETICAL;
        isDecreasingOrder = false;
    }
}

//uses modified bubble sort to sort dictionary by length
void Dictionary::sortByLengthINC_NON_ITER()
{
    if (isLoaded)
    {
        bool done = false;


        while (!done)
        {
            done = true;

            for (size_t i=0; i<(ListSize - 1); i++)
            {

                //if 1st word is longer than 2nd word, swap 'em
                if (len(WordList[i]) > len(WordList[i+1]))
                {
                    char* tmp = WordList[i];
                    WordList[i] = WordList[i+1];
                    WordList[i+1] = tmp;

                    done = false;
                }

            }


            //now sort backwards
            for (int i=(ListSize - 1); i>0; i--)
            {

                //if 1st word is shorter than 2nd word, swap 'em
                if (len(WordList[i]) < len(WordList[i-1]))
                {
                    char* tmp = WordList[i];
                    WordList[i] = WordList[i-1];
                    WordList[i-1] = tmp;

                    done = false;
                }

            }
        }


        Sorting = LENGTHWISE;
        isDecreasingOrder = false;
    }
}


//uses modified bubble sort to sort dictionary alphabetically
bool Dictionary::sortByAlphaINC_ITER()
{
    if (isLoaded)
    {
        bool done = true;

            for (size_t i=0; i<(ListSize - 1); i++)
            {

                //if 1st word is greater than 2nd word, swap 'em
                if (compare(WordList[i], WordList[i+1], language) == 1)
                {
                    char* tmp = WordList[i];
                    WordList[i] = WordList[i+1];
                    WordList[i+1] = tmp;

                    done = false;
                }

            }


            //now sort backwards
            /*for (int i=(ListSize - 1); i>0; i--)
            {

                //if 1st word is less than 2nd word, swap 'em
                if (compare(WordList[i], WordList[i-1], language) == -1)
                {
                    char* tmp = WordList[i];
                    WordList[i] = WordList[i-1];
                    WordList[i-1] = tmp;

                    done = false;
                }

            }*/

        if (done)
        {
            Sorting = ALPHABETICAL;
            isDecreasingOrder = false;
        }

        return !done;
    }

    return false;
}

//uses modified bubble sort to sort dictionary by length
bool Dictionary::sortByLengthINC_ITER()
{
    if (isLoaded)
    {
        bool done = true;

            for (size_t i=0; i<(ListSize - 1); i++)
            {

                //if 1st word is longer than 2nd word, swap 'em
                if (len(WordList[i]) > len(WordList[i+1]))
                {
                    char* tmp = WordList[i];
                    WordList[i] = WordList[i+1];
                    WordList[i+1] = tmp;

                    done = false;
                }

            }


            //now sort backwards
            for (int i=(ListSize - 1); i>0; i--)
            {

                //if 1st word is shorter than 2nd word, swap 'em
                if (len(WordList[i]) < len(WordList[i-1]))
                {
                    char* tmp = WordList[i];
                    WordList[i] = WordList[i-1];
                    WordList[i-1] = tmp;

                    done = false;
                }

            }

        if (done)
        {
            Sorting = LENGTHWISE;
            isDecreasingOrder = false;
        }

        return !done;
    }

    return false;
}


bool Dictionary::sortStep(DICTIONARY_SORTING Sorting, bool decreasingOrder)
{
    if (Sorting == ALPHABETICAL)
    {
        return sortByAlphaINC_ITER();
    }
    else if (Sorting == LENGTHWISE)
    {
        return sortByLengthINC_ITER();
    }
    else
    {
        return false;
    }
}


void Dictionary::setSortOrder(DICTIONARY_SORTING SortingMode)
{
    Sorting = SortingMode;
}


//search for a word in the dictionary,
//given that the dictionary is unsorted.
//uses linear search, and thus is very slow for larger dictionaries.
size_t Dictionary::getWordIndex_NoSorting(const Char* Word, CompareMode Mode)
{
    //linear search
    for (size_t i=0; i<ListSize; i++)
    {
        if (compare(WordList[i], Word, Mode) == 0)
            return i;
    }

    return WORD_NOT_IN_DICT;
}


//search for a word in the dictionary,
//given that the dictionary is sorted alphabetically in INCREASING order (e.g. A-->Z).
//uses pure binary search, and thus is a preferred method.
size_t Dictionary::getWordIndex_AlphaSortingINC(const Char* Word, CompareMode Mode)
{
    //binary search forwards
    size_t i = 0;
    size_t j = ListSize - 1;
    size_t k = (j + i) / 2;

    while (j - i > 0)
    {
        int word_alpha_rank = compare(WordList[k], Word, Mode);

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
            return k;
        }

        k = (j + i) / 2;
    }

    return WORD_NOT_IN_DICT;
}

//search for a word in the dictionary,
//given that the dictionary is sorted alphabetically in DECREASING order (e.g. Z-->A).
//uses pure binary search, and thus is a preferred method.
size_t Dictionary::getWordIndex_AlphaSortingDEC(const Char* Word, CompareMode Mode)
{
    //binary search forwards
    size_t i = 0;
    size_t j = ListSize - 1;
    size_t k = (j + i) / 2;

    while (j - i > 0)
    {
        int word_alpha_rank = compare(WordList[k], Word, Mode);

        if (word_alpha_rank == 1)
        {
            i = k + 1;
        }
        else if (word_alpha_rank == -1)
        {
            j = k - 1;
        }
        else
        {
            return k;
        }

        k = (j + i) / 2;
    }

    return WORD_NOT_IN_DICT;
}


//search for a word in the dictionary,
//given that the dictionary is sorted lengthwise in INCREASING order,
//uses an amalgam of binary and linear search, thus while not as efficient as
//with alphabetical sorting, is much more efficient than no sorting.
size_t Dictionary::getWordIndex_LenWiseSortingINC(const Char* Word, CompareMode Mode)
{
    //modified binary search.
    //binary to find correct length, then
    //linear in both directions to find
    //the correct word.
    size_t i = 0;
    size_t j = ListSize - 1;
    size_t k = (j + i) / 2;

    bool right_size = false;
    size_t word_length = len(Word);

    while (j - i > 0)
    {
        size_t list_word_length = len(WordList[k]);

        if (list_word_length < word_length)
        {
            i = k + 1;
        }
        else if (list_word_length > word_length)
        {
            j = k - 1;
        }
        else if (list_word_length == word_length)
        {
            right_size = true;
            break;
        }

        k = (j + i) / 2;
    }

    //now linear search bidirectionally outward.
    if (right_size)
    {
        if (compare(WordList[k], Word, Mode) == 0)
            return k;

        size_t left_position = k-1;
        size_t right_position = k+1;

        bool look_left = true;
        bool look_right = true;


        while (look_left or look_right)
        {
            if (left_position < 0)
                look_left = false;

            if (right_position >= ListSize)
                look_right = false;


            if (look_left)
            {
                if (len(WordList[left_position]) == word_length)
                {
                    if (compare(WordList[left_position], Word, Mode) == 0)
                    {
                        return left_position;
                    }
                    else
                    {
                        left_position--;
                    }
                }
                else
                {
                    look_left = false;
                }
            }

            if (look_right)
            {
                if (len(WordList[right_position]) == word_length)
                {
                    if (compare(WordList[right_position], Word, Mode) == 0)
                    {
                        return right_position;
                    }
                    else
                    {
                        right_position++;
                    }
                }
                else
                {
                    look_right = false;
                }
            }
        }//done with linear searching
    }

    return WORD_NOT_IN_DICT;
}

//search for a word in the dictionary,
//given that the dictionary is sorted lengthwise in DECREASING order,
//uses an amalgam of binary and linear search, thus while not as efficient as
//with alphabetical sorting, is much more efficient than no sorting.
size_t Dictionary::getWordIndex_LenWiseSortingDEC(const Char* Word, CompareMode Mode)
{
    //modified binary search.
    //binary to find correct length, then
    //linear in both directions to find
    //the correct word.
    size_t i = 0;
    size_t j = ListSize - 1;
    size_t k = (j + i) / 2;

    bool right_size = false;
    size_t word_length = len(Word);

    while (j - i > 0)
    {
        size_t list_word_length = len(WordList[k]);

        if (list_word_length > word_length)
        {
            i = k + 1;
        }
        else if (list_word_length < word_length)
        {
            j = k - 1;
        }
        else if (list_word_length == word_length)
        {
            right_size = true;
            break;
        }

        k = (j + i) / 2;
    }

    //now linear search bidirectionally outward.
    if (right_size)
    {
        if (compare(WordList[k], Word, Mode) == 0)
            return k;

        size_t left_position = k-1;
        size_t right_position = k+1;

        bool look_left = true;
        bool look_right = true;


        while (look_left or look_right)
        {
            if (left_position < 0)
                look_left = false;

            if (right_position >= ListSize)
                look_right = false;


            if (look_left)
            {
                if (len(WordList[left_position]) == word_length)
                {
                    if (compare(WordList[left_position], Word, Mode) == 0)
                    {
                        return left_position;
                    }
                    else
                    {
                        left_position--;
                    }
                }
                else
                {
                    look_left = false;
                }
            }

            if (look_right)
            {
                if (len(WordList[right_position]) == word_length)
                {
                    if (compare(WordList[right_position], Word, Mode) == 0)
                    {
                        return right_position;
                    }
                    else
                    {
                        right_position++;
                    }
                }
                else
                {
                    look_right = false;
                }
            }
        }//done with linear searching
    }

    return WORD_NOT_IN_DICT;
}

#endif // DICTIONARY_H_INCLUDED
