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

#ifndef STRLIB_H
#define STRLIB_H
#include <stdbool.h>
#include <iostream>

namespace strlib
{

    //Removes the characters from the string
    void filterStr(char * str, char toBeRemoved);
    void filterStr(std::string & str, char toBeRemoved);

    //Removes the char from the string
    void removeChar(char * str, unsigned int pos);

    //Locates a string from the second string and returns the beginning index of the found string, case sensitive
    int strLocate(const char * str, const char * searchStr);
    int strLocate(const std::string & str, const std::string & searchStr);

    //Locates a string from the second string and returns the beginning index of the found string, not case sensitive
    int strCaseLocate(const char * str, const char * searchStr);
    int strCaseLocate(std::string & str, const std::string & searchStr);

    //Splits a string using the given delimiter and stores the result into 2d string array
    int splitStr(const char * str_in, const char delim, char **result_out);
    int splitStr(const std::string & str_in, const char delim, std::string ** result_out);

    //Extracts (copies a letter by letter) a string from the str_in to the str_out, stops when the delimiter is found
    int extractStr(const char * str_in, const char delim, char *str_out);
    int extractStr(const char * str_in, const char delim, std::string & str_out);
}

#endif // STRLIB_H


