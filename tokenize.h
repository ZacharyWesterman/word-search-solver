#ifndef TOKENIZE_H_INCLUDED
#define TOKENIZE_H_INCLUDED


int wordCount(const char token, const char* _string)
{
    int i = 0;  //counter variable
    bool ready = true; //

    unsigned int current_word = 0;  //hold what number word we're currently at

    while (_string[i] != L'\0')
    {

        if (ready)
        {
            if (_string[i] != token)
            {
                current_word++;
                ready = false;
            }
        }
        else
        {
            if (_string[i] == token)
            {
                ready = true;
            }
        }

        i++;
    }

    return current_word;
}


char* word(unsigned int nth_word, const char token, const char* _string) {

    int i = 0;  //counter variable

    unsigned int current_word = 0;  //hold what number word we're currently at

    //skip any leading tokens
    while ((_string[i] == token) and (_string[i] != L'\0'))
        i++;

    //Get to the nth word, or the end of char array
    while ((_string[i] != L'\0') and (current_word < nth_word))
    {
        //if we run into a token, we have moved on to the next word
        if (_string[i] == token)
        {
            //take care of multiple consecutive tokens
            while ((_string[i+1] == token) and (_string[i+1] != L'\0'))
                i++;//increment counter

            current_word += 1;  //increment current word
        }

        i++;//increment counter
    }

    int offset = i; //offset is starting position of that word
    i = 0;  //reset counter


    //pass any consecutive tokens
    while ((_string[i + offset] == token) and (_string[i + offset] != '\0'))
    {
        offset += 1;//increase the offset
    }

    //get the length of desired word
    while ((_string[i + offset] != token) and (_string[i + offset] != '\0'))
    {
        i++;//increment counter
    }

    int char_count = i; //counter holds length of word
    i = 0;  //reset counter


    //create char array 1 longer than word length (to hold terminating '\0')
    char* full_word = new char[char_count+1];

    //fill array with the appropriate characters
    for (i=0; i<=char_count; i++)
    {
        full_word[i] = _string[i + offset];//copy characters one by one
    }

    full_word[char_count] = L'\0';   //append null character to end of array

    return full_word;   //return resultant char array
}


bool equals(const char* arg1, const char* arg2)
{
    unsigned int pos = 0;

    while ((arg1[pos] != '\0') and (arg2[pos] != '\0'))
    {
        if (arg1[pos] != arg2[pos])
            return false;

        pos++;
    }


    if (arg1[pos] != arg2[pos])
        return false;


    return true;
}


unsigned int len(const char* _string)
{
    unsigned int str_length = 0;

    while (_string[str_length] != '\0')
    {
        str_length++;
    }

    return str_length;
}



bool str_is_ull(const char* _string)
{
    unsigned int position = 0;

    while (_string[position] != '\0')
    {
        if (!((_string[position] <= '9') & (_string[position] >= '0')))
            return false;

        position++;
    }

    return true;
}

unsigned long long str_to_ull(const char* _string)
{
    unsigned long long value = 0;

    unsigned int position = 0;


    while (_string[position] != '\0')
    {
        if ((_string[position] <= '9') & (_string[position] >= '0'))
        {
            value *= 10;
            value += _string[position] - '0';
        }
        else
        {
            return 0;
        }

        position++;
    }

    return value;
}


#endif // TOKENIZE_H_INCLUDED
