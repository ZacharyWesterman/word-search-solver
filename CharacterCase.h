#ifndef CHARACTERCASE_H_INCLUDED
#define CHARACTERCASE_H_INCLUDED

bool isUpper(char Character)
{
    if ((Character >= 'A') and (Character <= 'Z'))
        return true;

    return false;
}

bool isLower(char Character)
{
    if ((Character >= 'a') and (Character <= 'z'))
        return true;

    return false;
}


bool isUpper(wchar_t Character)
{
    if ((Character >= L'A') and (Character <= L'Z'))
        return true;

    return false;
}

bool isLower(wchar_t Character)
{
    if ((Character >= L'a') and (Character <= L'z'))
        return true;

    return false;
}

#endif // CHARACTERCASE_H_INCLUDED
