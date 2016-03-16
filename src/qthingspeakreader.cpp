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

#include "qthingspeakreader.h"

TSReader::TSReader(QObject *parent) : QObject (parent)
{
    m_pThingSpeakClient = new ThingSpeakClient();

    m_iReadInterval = READ_INTERVAL_DEFAULT;
    m_bIsRunning= false;
    m_bShutDownRequested = false;

    for(int i = 0; i < THINGSPEAK_MAX_FIELDS; i++)
    {
        m_bFieldMarkedForReading[i] = false;
    }
}

TSReader::~TSReader()
{
    delete m_pThingSpeakClient;
}

void TSReader::SetChannelId(const char *szChannelId)
{
    m_sChannelId = szChannelId;
}
void TSReader::SetReadApiKey(const char *szApiKey)
{
    m_sApiKey = szApiKey;
}

void TSReader::MarkFieldForReading(int iField, bool bEnable)
{
    if(iField == 0 || iField > THINGSPEAK_MAX_FIELDS)
        return;

    m_bFieldMarkedForReading[iField-1] = bEnable;
}

void TSReader::SetReadingInterval(unsigned long ulInterval_msec)
{
    m_iReadInterval = ulInterval_msec;
}

void TSReader::on_start()
{
    if(false == m_bIsRunning)
        threadLoop();
}

void TSReader::on_stop()
{
    m_bShutDownRequested = true;
}
void TSReader::threadLoop()
{
    TS_CHANNEL_LAST channel;
    m_bIsRunning = true;
    while (false == m_bShutDownRequested)
    {
        if(true == m_pThingSpeakClient->readWholeChannel(channel,m_sChannelId.toStdString().c_str(),m_sApiKey.toStdString().c_str()))
        {
            emit(channelDataReady(channel));
            for(int i = 0; i < THINGSPEAK_MAX_FIELDS; i++)
            {
                if(true == m_bFieldMarkedForReading[i])
                {
                    emit(fieldDataReady(i+1,channel.StringFieldData(i+1)));
                }
            }
        }
        usleep(READ_INTERVAL_DEFAULT*1000);
    }
}

QThingSpeakReader::QThingSpeakReader(QObject *parent) : QObject(parent)
{
    m_pReader = new TSReader();
    m_pWorkerThread = new QThread();

    m_pReader->moveToThread(m_pWorkerThread);

    connect(m_pWorkerThread, &QThread::finished, m_pReader, &QObject::deleteLater);

    connect(this, &QThingSpeakReader::startReader, m_pReader, &TSReader::on_start);
    connect(this, &QThingSpeakReader::stopReader, m_pReader, &TSReader::on_stop);

    connect(m_pReader, &TSReader::channelDataReady, this, &QThingSpeakReader::on_channelDataReady);
    connect(m_pReader, &TSReader::fieldDataReady, this, &QThingSpeakReader::on_fieldDataReady);


    m_pWorkerThread->start();

}

QThingSpeakReader::~QThingSpeakReader()
{
    emit(stopReader());
    m_pWorkerThread->quit();

    m_pWorkerThread->wait(1000);

    delete m_pWorkerThread;
    delete m_pReader;
}

void QThingSpeakReader::SetChannelId(const char *szChannelId)
{
    return m_pReader->SetChannelId(szChannelId);
}

void QThingSpeakReader::SetReadApiKey(const char *szApiKey)
{
    return m_pReader->SetReadApiKey(szApiKey);
}

void QThingSpeakReader::MarkFieldForReading(int iField, bool bEnable)
{
    return m_pReader->MarkFieldForReading(iField,bEnable);
}

void QThingSpeakReader::StartReading()
{
    emit(startReader());
}

void QThingSpeakReader::StopReading()
{
    emit(stopReader());
}

void QThingSpeakReader::on_channelDataReady(TS_CHANNEL_LAST channel)
{
    emit(channelDataAvailable(channel));
}

void QThingSpeakReader::on_fieldDataReady(int iField, const char *data)
{
    emit(fieldDataAvailable(iField,data));
}

