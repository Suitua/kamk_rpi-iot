#include "strlib.h"
#include <string.h>
#include <algorithm>




void strlib::filterStr(char *str, char toBeRemoved)
{
    int i = 0;
    while(str[i])
    {
         if(str[i] == toBeRemoved)
             removeChar(str,i);
         i++;
    }
}

void strlib::filterStr(std::string &str, char toBeRemoved)
{
    str.erase(std::remove(str.begin(), str.end(), toBeRemoved), str.end());
}

void strlib::removeChar(char *str, unsigned int pos)
{
    memmove(&str[pos],&str[pos+1],strlen(str)-pos);
}

int strlib::strLocate(const char *str, const char *searchStr)
{
    int i = 0;
    unsigned int hits = 0;
    int location = 0;

    while(str[i])
    {
        if(str[i] == searchStr[0])
        {
            hits ++;

            for (unsigned int a = 1; a <= strlen(searchStr);a++)
            {
                if (str[i+a] == searchStr[a])
                {
                    hits++;

                    if (hits == strlen(searchStr))
                    {
                        location = i+a;
                        return location;
                    }

                }

                else
                {
                    hits = 0;
                }

            }

        }

        i++;
    }

    return -1;
}

int strlib::strLocate(const std::string &str, const std::string &searchStr)
{
    int i = 0;
    unsigned int hits = 0;
    int location = 0;

    while(str[i])
    {
        if(str[i] == searchStr[0])
        {
            hits ++;

            for (unsigned int a = 1; a <= searchStr.length();a++)
            {
                if (str[i+a] == searchStr[a])
                {
                    hits++;

                    if (hits == searchStr.length())
                    {
                        location = i+a;
                        return location;
                    }

                }

                else
                {
                    hits = 0;
                }

            }

        }

        i++;
    }

    return -1;
}

int strlib::splitStr(const char * str_in, const char delim, char **result_out)
{
    int strCount = 0;
    int letterCount = 0;

    for(int i = 0; str_in[i];i++)
    {
        if(str_in[i] == delim)
        {
            result_out[strCount][letterCount] = '\0';
            strCount++;
            letterCount = 0;

        }
        else
        {
            result_out[strCount][letterCount] = str_in[i];
            letterCount++;
        }
    }
    result_out[strCount][letterCount] = '\0';

    return strCount;

}

int strlib::splitStr(const std::string &str_in, const char delim, std::string **result_out)
{
    int strCount = 0;
    int letterCount = 0;

    for(int i = 0; str_in[i];i++)
    {
        if(str_in[i] == delim)
        {
            result_out[strCount][letterCount] = '\0';
            strCount++;
            letterCount = 0;

        }
        else
        {
            result_out[strCount][letterCount] = str_in[i];
            letterCount++;
        }
    }
    result_out[strCount][letterCount] = '\0';

    return strCount;
}

int strlib::strCaseLocate(const char *str, const char *searchStr)
{
    int i = 0;
    unsigned int hits = 0;
    int location = 0;

    while(str[i])
    {
        if(str[i] == searchStr[0])
        {
            hits ++;

            for (unsigned int a = 1; a <= strlen(searchStr);a++)
            {
                if (tolower(str[i+a]) == tolower(searchStr[a]))
                {
                    hits++;

                    if (hits == strlen(searchStr))
                    {
                        location = i+a;
                        return location;
                    }

                }

                else
                {
                    hits = 0;
                }

            }

        }

        i++;
    }

    return -1;
}

int strlib::strCaseLocate(std::string &str, const std::string &searchStr)
{
    int i = 0;
    unsigned int hits = 0;
    int location = 0;

    while(str[i])
    {
        if(str[i] == searchStr[0])
        {
            hits ++;

            for (unsigned int a = 1; a <= searchStr.length();a++)
            {
                if (tolower(str[i+a]) == tolower(searchStr[a]))
                {
                    hits++;

                    if (hits == searchStr.length())
                    {
                        location = i+a;
                        return location;
                    }

                }

                else
                {
                    hits = 0;
                }

            }

        }

        i++;
    }

    return -1;
}

int strlib::extractStr(const char *str_in, const char delim, char *str_out)
{
    int i = 0;

    for(i = 0; str_in[i];i++)
    {
        if(str_in[i] != delim)
        {
            str_out[i] = str_in[i];

        }
        else
        {
             break;
        }
    }

    str_out[i] = '\0';
    return i;

}


int strlib::extractStr(const char * str_in, const char delim, std::string &str_out)
{
    int i = 0;

    for(i = 0; str_in[i];i++)
    {
        if(str_in[i] != delim)
        {
            str_out += str_in[i];

        }
        else
        {
             break;
        }
    }

    return i;
}


int strlib::strToInt(const char *str)
{
    int temp = 0;

    int i;

    for(i = 0;str[i];i++)
    {
        if(str[i] <='9' && str[i] >='0')
        {
            temp = temp * 10 + str[i] -'0';
        }

        else
            break;
    }

    if (i == 0)
        return 0;
    else

        return temp;


}

float strlib::strToFloat(const char *str)
{
    float temp = 0;

    bool decPointFound = false;
    int decimal = 0;

    int i;

    for(i = 0;str[i];i++)
    {
        if(str[i] <='9' && str[i] >='0')
        {
            if (decPointFound)
            {
                decimal++;
                temp = temp  + (double(str[i] -'0')) *(pow(10,-decimal));
            }
            else
            {
               temp = temp * 10 + str[i] -'0';
            }

        }

        else if(str[i] =='.' || str[i] ==',')
        {
           decPointFound = true;
        }


        else
            break;
    }

    if (i == 0)
        return 0;
    else
    {
        return temp;
    }
}


