/*
*  Copyright (C) 2016, Timo Leinonen <timojt.leinonen@gmail.com>
*  
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*  Version 1.0
*  Author: Timo Leinonen
*/

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
    return i+1;

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

    return i+1;
}




