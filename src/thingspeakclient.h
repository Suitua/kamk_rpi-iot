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

#ifndef THINGSPEAKCLIENT_H
#define THINGSPEAKCLIENT_H
#include "httpclient.h"
#include <iostream>
#include <stdlib.h>


#define THINGSPEAK_UPDATE_URI       "/update"
#define THINGSPEAK_HOSTNAME         "api.thingspeak.com"

#define THINGSPEAK_MAX_FIELDS       8       //Current TS supports only 8 fields per channel
#define CHANNEL_FEED_MAX_LEN       128



typedef struct
{
    friend class ThingSpeakClient;
public:

    const char * CreatedAt()
    {
        return m_szCreatedAt;
    }

    int EntryId()
    {
        return m_iEntryId;
    }

    const char * StringFieldData(int iField)
    {
        if(iField < 1 || iField > 8)
            return NULL;

        return &m_szFieldData[iField-1][0];
    }

    int IntFieldData(int iField)
    {
        if(iField < 1 || iField > 8)
            return 0;

        return atoi(&m_szFieldData[iField-1][0]);
    }

    float FloatFieldData(int iField)
    {
        if(iField < 1 || iField > 8)
            return 0.0f;

        return atof(&m_szFieldData[iField-1][0]);
    }

private:

    //resets the data
    void reset()
    {
        m_szCreatedAt[0] = '\0';
        m_iEntryId = -1;
        for(int i = 0; i < THINGSPEAK_MAX_FIELDS; i++)
            m_szFieldData[i][0] = 0;
    }

    //Info section
    char m_szCreatedAt[64];          //When the channel was created (time stamp)
    int m_iEntryId;                  //Id of the entry

    //Data
    char m_szFieldData
    [THINGSPEAK_MAX_FIELDS][128];         //Fields

}TS_CHANNEL_LAST;

/*
typedef struct
{
public:

private:
    //Info section
    int m_iId;                       //Channel ID
    char m_szName[128];               //Name of the channel
    char m_szDescription[1024];      //Description of the channel
    double m_fLatitude;
    double m_fLongitude;
    int m_iLastEntryId;              //Id of the last entry
    char m_szCreatedAt[64];          //When the channel was created (time stam
    char m_szUpdatedAt[64];          //When the channel was last updated (time stamp)
    //Data
    char m_Fields
    [CHANNEL_FEED_MAX_LEN]
    [THINGSPEAK_MAX_FIELDS]
    [128];         //Fields

}TS_CHANNEL_FEED;
*/

class ThingSpeakClient
{
public:
    //Constructor
    ThingSpeakClient();
    //Destructor
    ~ThingSpeakClient();

    //Sets a values for the field
    bool setField(const int field, const char * val);
    bool setField(const int field, const int  val);
    bool setField(const int field, const double  val);

    //Writes a single field
    bool writeField(const int field, const char * val, const char * wrApiKey);
    bool writeField(const int field, const int val, const char * wrApiKey);
    bool writeField(const int field, const double val, const char * wrApiKey);

    //Writes multiple, already set fields
    bool writeFields(const char * wrApiKey);

    //Read a single field
    float readFloatField( const int field, const char * channelNum, const char * rApiKey = NULL);
    int readIntField( const int field, const char * channelNum, const char * rApiKey = NULL);
    const char * readStringField(const int field, const char * channelNum,  const char * rApiKey = NULL);

    //Reads the whole channel and returns a struct containing all the channel's info and data
    bool readWholeChannel(TS_CHANNEL_LAST & channel,const char * channelNum,  const char * rApiKey = NULL);


    //------------------------------TalkBack API -----------------------------------//
    //
    //Not implemented yet
    /*

    //Adds a talk back command
    int AddTalkbackCmd(const char * cmd, int position, const char * tbApiKey);

    //Gets a talk back command
    const char * GetTalkbackCmd(int tbId, int cmdId, const char * tbApiKey);

    //Updates an existing talk back command
    const char * UpdateTalkbackCmd(const char * cmd, int position, const char * tbApiKey);

    //Executes the next talk back command
    const char * ExecuteNextTalkbackCmd(int tbId, const char * tbApiKey);

    //Gets the last executed talk back command
    const char *  GetLastExecutedTalkbackCmd(int tbId, const char * tbApiKey);

    */

    //Gets the last error message
    char * GetLastError();


private:

    //Contains the last error message
    char m_szLastError[1024];

    //Indicates if there is a new, unread error message
    bool m_bNewErrMsg;

    //Resets all the data fields
    void resetFields();

    //Convers all the data (fields) into string
    void fieldDataToStr();

    //Field data string
    std::string m_szFieldDataStr;

    //Http client for sending and reading the data with HTTP protocol
    HTTPClient m_httpClient;

    //Parses the http response content
    bool parseContent(const char * szContent,TS_CHANNEL_LAST * pChannelLast);

    //Holds the data for the fields
    std::string m_szFieldData[THINGSPEAK_MAX_FIELDS];

};

#endif // THINGSPEAKCLIENT_H
