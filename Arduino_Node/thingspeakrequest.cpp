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


#include "thingspeakrequest.h"

ThingSpeakUploadRequest::ThingSpeakUploadRequest()
{
    for(int i = 0; i < MAX_FIELDS;i++)
    {
        m_szFieldData[i] = NULL;
    }
}

ThingSpeakUploadRequest::~ThingSpeakUploadRequest()
{
    for(int i = 0; i < MAX_FIELDS;i++)
    {
        if(NULL != m_szFieldData[i])
            delete[] m_szFieldData[i];
    }
}

bool ThingSpeakUploadRequest::SetApiKey(const char *szApiKey)
{
    if(strlen(szApiKey) != 16)
        return false;
    strcpy(m_szApiKey, szApiKey);
    return true;
}

const char *ThingSpeakUploadRequest::GetApiKey()
{
    return m_szApiKey;
}

bool ThingSpeakUploadRequest::SetFieldData(unsigned int iField, const char *szData)
{
    if(iField > MAX_FIELDS)
        return false;

    if(NULL == m_szFieldData[iField-1])
        m_szFieldData[iField-1] = new char [strlen(szData)+1];

    strcpy(m_szFieldData[iField-1],szData);

    return true;
}

bool ThingSpeakUploadRequest::SetFieldData(unsigned int iField, const int iData)
{
    if(iField > MAX_FIELDS)
        return false;

    char temp[32];

    sprintf(temp,"%d", iData);

    return SetFieldData(iField,temp);

}

bool ThingSpeakUploadRequest::SetFieldData(unsigned int iField, const long int iData)
{
    if(iField > MAX_FIELDS)
        return false;

    char temp[32];

    sprintf(temp,"%ld", iData);

    return SetFieldData(iField,temp);

}

bool ThingSpeakUploadRequest::SetFieldData(unsigned int iField, const float fData)
{
    if(iField > MAX_FIELDS)
        return false;
    char temp[32];

#ifndef __AVR__
        sprintf(temp,"%f", fData);
#else
        dtostrf(fData,1,5, temp);
#endif

    return SetFieldData(iField,temp);
}

const char *ThingSpeakUploadRequest::GetFieldData(unsigned int iField)
{
    if(iField > MAX_FIELDS)
        return NULL;

    return m_szFieldData[iField-1];
}

void ThingSpeakUploadRequest::ToString(char *buf)
{
    strcpy(buf,m_szApiKey);

    char temp[strlen(FIELD_ID_PREFIX)+3];
    strcat(buf,";");
    bool bFirst = true;

    for(int i = 0; i < MAX_FIELDS; i++)
    {
        if(NULL != m_szFieldData[i])
        {
            if(m_szFieldData[i][0] != '\0')
            {
                if(false == bFirst)
                    strcat(buf,FIELD_SEPARATOR);

                sprintf(temp,"%s%d",FIELD_ID_PREFIX,i+1);
                strcat(buf,temp);
                strcat(buf,"=");
                strcat(buf,m_szFieldData[i]);

                bFirst = false;
            }

        }

    }
}

bool ThingSpeakUploadRequest::FromString(const char *buf)
{

    char temp[64];
    bool bNextFieldIdFound = false;
    bool bNextFieldDataFound = false;

    int idLen = 0;
    int dataLen = 0;
    int fieldId = -1;

    if(strlen(buf) < 16)
        return false;

    //Get the api key
    for(int i = 0; i < 16;i++)
    {
        m_szApiKey[i] = buf[i];
    }
    m_szApiKey[16] = '\0';

    if(buf[16] != ';')
        return false;  //Parse error


    //Get the field data
    for(int i = 16; buf[i] ;i++)
    {
        //Allow for & also as a field separator to support the HTTP like request format
        if(buf[i] == ';' || buf[i] == '&')
        {
            bNextFieldIdFound = true;
            if(true == bNextFieldDataFound)
            {
                temp[dataLen] = '\0';
                dataLen = 0;
                bNextFieldDataFound = false;


                if(fieldId == -1 || fieldId > 8)
                    return false;   //Parse error
                if(NULL == m_szFieldData[fieldId-1])
                {
                    m_szFieldData[fieldId-1] = new char[strlen(temp)+1];
                }
                strcpy(m_szFieldData[fieldId-1],temp);


            }
            continue;
        }

        if(buf[i] == '=')
        {
            bNextFieldDataFound = true;
            if(true == bNextFieldIdFound)
            {
                bNextFieldIdFound = false;
                temp[idLen] = '\0';
                idLen = 0;
                fieldId = temp[strlen(temp)-1]-48;

            }

            continue;
        }

        if(bNextFieldIdFound)
        {
            //Field identifier cannot be longer than "fieldx" which is 6 characters
            if(idLen > 6)
                return false;   //Parse error

            temp[idLen] = buf[i];
            idLen++;
        }

        if(bNextFieldDataFound)
        {
            temp[dataLen] = buf[i];
            dataLen++;
        }

    }

    //Get the last field value
    if(true == bNextFieldDataFound)
    {
        temp[dataLen] = '\0';
        dataLen = 0;
        bNextFieldDataFound = false;


        if(fieldId == -1 || fieldId > 8)
            return false;   //Parse error
        if(NULL == m_szFieldData[fieldId-1])
        {
            m_szFieldData[fieldId-1] = new char[strlen(temp)+1];

        }
        strcpy(m_szFieldData[fieldId-1],temp);


    }

    return true;


}

void ThingSpeakUploadRequest::ResetData()
{
    m_szApiKey[0] = '\0';

    for(int i = 0; i < MAX_FIELDS;i++)
    {
        if(NULL != m_szFieldData[i])
            m_szFieldData[i][0] = '\0';
    }
}

