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


#ifndef THINGSPEAKUPLOADREQUEST_H
#define THINGSPEAKUPLOADREQUEST_H

#ifndef __AVR__
#include <string.h>
#include <stdio.h>
#else
#include <Arduino.h>
#endif
#define MAX_FIELDS          8

#define FIELD_ID_PREFIX     "field"
#define FIELD_SEPARATOR     "&"


class ThingSpeakUploadRequest
{
public:
    //Constructor
    ThingSpeakUploadRequest();
    //Destructor
    ~ThingSpeakUploadRequest();

    //Sets the Api key for the request
    bool SetApiKey(const char * szApiKey);

    //Gets the Api key of the request
    const char * GetApiKey();

    //Sets the field data for the upload request
    bool SetFieldData(unsigned int iField, const char * szData);
    bool SetFieldData(unsigned int iField, const int iData);
    bool SetFieldData(unsigned int iField, const long int iData);
    bool SetFieldData(unsigned int iField, const float fData);

    //Gets the field data of the upload request
    const char * GetFieldData(unsigned int iField);

    //Converts the request object into a string
    void ToString(char * buf);
    //Constructs the request object from a string
    bool FromString(const char * buf);

    //Resets all the data
    void ResetData();



private:

    char m_szApiKey[17];
    char * m_szFieldData[MAX_FIELDS];

};

#endif // THINGSPEAKUPLOADREQUEST_H
