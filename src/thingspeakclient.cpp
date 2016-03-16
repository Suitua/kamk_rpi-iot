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

#include "thingspeakclient.h"
#include <stdio.h>
#include <string>
#include "strlib.h"


ThingSpeakClient::ThingSpeakClient()
{
    m_bNewErrMsg = false;
    memset(m_szLastError,0,sizeof(m_szLastError));
    resetFields();

}

ThingSpeakClient::~ThingSpeakClient()
{

}

bool ThingSpeakClient::setField(const int field, const char *val)
{
    if(NULL == val)
        return false;
    if(field < 1 || field > THINGSPEAK_MAX_FIELDS)
    {
        m_bNewErrMsg = true;
        strcpy(m_szLastError, "Field number out of range.");
        return false;
    }

    m_szFieldData[field-1] = val;
    return true;

}

bool ThingSpeakClient::setField(const int field, const int val)
{
    if(field < 1 || field > THINGSPEAK_MAX_FIELDS)
    {
        m_bNewErrMsg = true;
        strcpy(m_szLastError, "Field number out of range.");
        return false;
    }

    m_szFieldData[field-1] = std::to_string(val);
    return true;
}

bool ThingSpeakClient::setField(const int field, const double val)
{    if(field < 1 || field > THINGSPEAK_MAX_FIELDS)
    {
        m_bNewErrMsg = true;
        strcpy(m_szLastError, "Field number out of range.");
        return false;
    }

    m_szFieldData[field-1] = std::to_string(val);
    return true;

}

bool ThingSpeakClient::writeField( const int field, const char *val,const char *wrApiKey)
{
    setField(field,val);
    return writeFields(wrApiKey);
}

bool ThingSpeakClient::writeField(const int field, const int val, const char *wrApiKey)
{
    setField(field,val);
    return writeFields(wrApiKey);
}

bool ThingSpeakClient::writeField(const int field, const double val, const char *wrApiKey)
{
    setField(field,val);
    return writeFields(wrApiKey);
}

float ThingSpeakClient::readFloatField(const int field, const char *channelNum, const char * rApiKey)
{
    const char * szRes = readStringField(field,channelNum,rApiKey);
    if(strcasecmp(szRes,"\"-1\"")== 0)
    {
        m_bNewErrMsg = true;
        strcpy(m_szLastError, "Failed to read field.");
        return 0.0f;  //Failure
    }

    return atof(szRes);

}

int ThingSpeakClient::readIntField(const int field, const char *channelNum, const char * rApiKey)
{
    const char * szRes = readStringField(field,channelNum,rApiKey);
    if(strcasecmp(szRes,"\"-1\"")== 0)
    {
        m_bNewErrMsg = true;
        strcpy(m_szLastError, "Failed to read field.");
        return 0.0f;  //Failure
    }

    return atoi(szRes);
}

const char *ThingSpeakClient::readStringField(const int field, const char *channelNum, const char * rApiKey)
{

    //GET https://api.thingspeak.com/channels/9/feeds/last
    m_httpClient.httpRequest()->setMethod("GET");

    //Compose the URI
    std::string szURI;
    szURI = "https://api.thingspeak.com/channels/";
    szURI += channelNum;
    szURI += "/field/";
    szURI += std::to_string(field);
    szURI += "/last";

    //Add the API key to the URI if it's given (apparently this is the only way it will work, if you are reading data)
    if(NULL != rApiKey)
    {
        szURI += "?api_key=";
        szURI += rApiKey;
    }
    m_httpClient.httpRequest()->setHost(THINGSPEAK_HOSTNAME);

    m_httpClient.httpRequest()->setURI(szURI.c_str());

    if(false ==  m_httpClient.SendRequest())
    {
        m_bNewErrMsg = true;
        strcpy(m_szLastError, "Failed to download data.");
        strcat(m_szLastError,m_httpClient.GetLastError());
        return NULL;
    }

    return m_httpClient.httpResponse()->content();
}

bool ThingSpeakClient::readWholeChannel(TS_CHANNEL_LAST & channel, const char *channelNum, const char * rApiKey)
{
    //GET https://api.thingspeak.com/channels/9/feeds/last
    m_httpClient.httpRequest()->setMethod("GET");

    //Compose the URI
    std::string szURI;
    szURI = "https://api.thingspeak.com/channels/";
    szURI += channelNum;
    szURI +="/feeds/last";

    //Add the API key to the URI if it's given (apparently this is the only way it will work, if you are reading data)
    if(NULL != rApiKey)
    {
        szURI += "?api_key=";
        szURI += rApiKey;
    }

    m_httpClient.httpRequest()->setURI(szURI.c_str());
    m_httpClient.httpRequest()->setHost(THINGSPEAK_HOSTNAME);

    if(false ==  m_httpClient.SendRequest())
    {
        m_bNewErrMsg = true;
        strcpy(m_szLastError, "Failed to download data.");
        strcat(m_szLastError,m_httpClient.GetLastError());
        return false;
    }

    if(false == parseContent(m_httpClient.httpResponse()->content(),&channel))
        return false;

    return true;

}

bool ThingSpeakClient::writeFields(const char *wrApiKey)
{
    m_httpClient.httpRequest()->setMethod("POST");
    m_httpClient.httpRequest()->setHost(THINGSPEAK_HOSTNAME);
    m_httpClient.httpRequest()->setVersion("HTTP/1.1");
    m_httpClient.httpRequest()->setURI(THINGSPEAK_UPDATE_URI);
    m_httpClient.httpRequest()->addHeaderItem("Content-Type","application/x-www-form-urlencoded");
    m_httpClient.httpRequest()->addHeaderItem("X-THINGSPEAKAPIKEY", wrApiKey);

    fieldDataToStr();
    m_httpClient.httpRequest()->addContent(m_szFieldDataStr.c_str());
    m_httpClient.httpRequest()->setHost(THINGSPEAK_HOSTNAME);

    if(false ==  m_httpClient.SendRequest())
    {
        m_bNewErrMsg = true;
        strcpy(m_szLastError, "Failed to upload data.");
        strcat(m_szLastError,m_httpClient.GetLastError());
        return false;
    }


    if(strcmp(m_httpClient.httpResponse()->statusCode(),"200") != 0)
    {
        m_bNewErrMsg = true;
        strcpy(m_szLastError, "Failed to update field(s). HTTP error");
        return false;
    }

    if(strcmp(m_httpClient.httpResponse()->content(),"0") == 0)
    {
        m_bNewErrMsg = true;
        strcpy(m_szLastError, "Failed to update field(s). Possibly too short update interval.");
        return false;
    }

    resetFields();

    return true;

}

char *ThingSpeakClient::GetLastError()
{
    if (m_bNewErrMsg)
    {
        return m_szLastError;

    }

    else
    {
        return NULL;
    }
}



void ThingSpeakClient::resetFields()
{
    m_szFieldDataStr.clear();

    for(int i = 0; i < THINGSPEAK_MAX_FIELDS; i++)
    {
       m_szFieldData[i].clear();
    }
}

void ThingSpeakClient::fieldDataToStr()
{
    bool first = true;
    for(int i = 0; i < THINGSPEAK_MAX_FIELDS;i++)
    {
        if(!m_szFieldData[i].empty())
        {
            if(false == first)
            {
                 m_szFieldDataStr += "&";
            }

            m_szFieldDataStr += "field";
            m_szFieldDataStr += std::to_string(i+1);
            m_szFieldDataStr += "=";
            m_szFieldDataStr += m_szFieldData[i];

            first = false;
        }
    }
}

bool ThingSpeakClient::parseContent(const char *szContent, TS_CHANNEL_LAST *pChannelLast)
{
    pChannelLast->reset();
    //Is the response a valid (json) object)
    if(szContent[0] != '{')
        return false;

    int iHead = 1;
    unsigned int i = 0;
    bool bOk = false;

    //Get the "created at" property
    if(-1 == (iHead = strlib::strLocate(szContent,"created_at\":")))
    {
        //Parse error
        m_bNewErrMsg = true;
        strcpy(m_szLastError, "Channel feed HTTP content parse error.");
        return false;
    }
    iHead++;

    while(szContent[iHead])
    {
        if(i >= sizeof(pChannelLast->m_szCreatedAt)-1)
        {
            //Parse error
            pChannelLast->m_szCreatedAt[i] = '\0';
            break;
        }
        if(szContent[iHead]==',')
        {
            pChannelLast->m_szCreatedAt[i] = '\0';
            bOk = true;
            break;
        }
        pChannelLast->m_szCreatedAt[i] = szContent[iHead];
        iHead++;
        i++;
    }

    if(false == bOk)
    {
        //Parse error
        m_bNewErrMsg = true;
        strcpy(m_szLastError, "Channel feed HTTP content parse error.");
        pChannelLast->m_szCreatedAt[i] = '\0';
        return false;
    }

    i = 0;
    char buf[16];
    bOk = false;
    //Get the "entry id" property
    if(-1 == (iHead += strlib::strLocate(&szContent[iHead],"entry_id\":")))
    {
        //Parse error
        m_bNewErrMsg = true;
        strcpy(m_szLastError, "Channel feed HTTP content parse error.");
        return false;
    }
    iHead++;
    while(szContent[iHead])
    {
        if(i >= sizeof(buf)-1)
        {
            //Parse error
            m_bNewErrMsg = true;
            strcpy(m_szLastError, "Channel feed HTTP content parse error.");
            buf[i] = '\0';
            break;
        }

        if(szContent[iHead]==',')
        {
            buf[i] = '\0';
            pChannelLast->m_iEntryId = atoi(buf);
            bOk = true;
            break;
        }
        buf[i] = szContent[iHead];
        iHead++;
        i++;
    }

    if(false == bOk)
    {
        //Parse error
        m_bNewErrMsg = true;
        strcpy(m_szLastError, "Channel feed HTTP content parse error.");
        return false;
    }

    //Now get the field values


    //Check if the content contains more field data
    if(-1 == (iHead += strlib::strLocate(&szContent[iHead],"\"field")))
    {
        return true;
    }
    iHead++;
    bOk = false;
    int iField = atoi(&szContent[iHead]);
    if(iField > THINGSPEAK_MAX_FIELDS)
        return true;

    bool bFieldDataFound = false;

    i = 0;
    while(szContent[iHead])
    {
        if(szContent[iHead] == ':')
        {
            bFieldDataFound = true;
            iHead++;
            continue;
        }
        if(bFieldDataFound)
        {
            if(szContent[iHead] == ',' || szContent[iHead] == '}')
            {
                pChannelLast->m_szFieldData[iField-1][i] = '\0';
                bFieldDataFound = false;
                i = 0;
                bOk = true;
                //Check if the content contains more field data
                if(-1 == (iHead += strlib::strLocate(&szContent[iHead],"\"field")))
                {
                    return true;
                }
                iHead++;
                iField = atoi(&szContent[iHead]);
                if(iField > THINGSPEAK_MAX_FIELDS || iField == 0)
                    break;
            }
            else
            {
                pChannelLast->m_szFieldData[iField-1][i] = szContent[iHead];
                i++;
            }

            iHead++;

            continue;
        }

        iHead++;
        bOk = false;

    }

    if(false == bOk)
    {
        //Parse error
        m_bNewErrMsg = true;
        strcpy(m_szLastError, "Channel feed HTTP content parse error.");
        return false;
    }


    //Do a clean up for the data (remove double quotes ("") from the strings )

    strlib::filterStr(pChannelLast->m_szCreatedAt,'\"');

    for(int i = 0; i < THINGSPEAK_MAX_FIELDS;i++)
        strlib::filterStr(&pChannelLast->m_szFieldData[i][0],'\"');


    return true;
}

