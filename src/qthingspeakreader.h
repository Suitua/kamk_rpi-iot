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

#ifndef QTHINGSPEAKREADER_H
#define QTHINGSPEAKREADER_H
#include <QObject>
#include <QThread>
#include "thingspeakclient.h"
#include <QString>

#define READ_INTERVAL_DEFAULT           5000  //msec


class TSReader : public QObject
{
    friend class QThingSpeakReader;
    Q_OBJECT
public:

    explicit TSReader(QObject * parent = 0);
    virtual ~TSReader();
    void SetChannelId(const char * szChannelId);
    void SetReadApiKey(const char * szApiKey);
    void MarkFieldForReading(int iField, bool bEnable);
    void SetReadingInterval(unsigned long ulInterval_msec);

public slots:
    void on_start();
    void on_stop();

private:
    bool m_bFieldMarkedForReading[THINGSPEAK_MAX_FIELDS];
    int m_iReadInterval;
    bool m_bIsRunning;
    bool m_bShutDownRequested;
    QString m_sApiKey;
    QString m_sChannelId;
    ThingSpeakClient * m_pThingSpeakClient;

    void threadLoop();

signals:
    void stopped();
    void fieldDataReady(int iField, const char * data);
    void channelDataReady(TS_CHANNEL_LAST channel);
};

class QThingSpeakReader : public QObject
{
    Q_OBJECT
public:
    explicit QThingSpeakReader(QObject * parent = 0);
    virtual ~QThingSpeakReader();

    void SetChannelId(const char * szChannelId);
    void SetReadApiKey(const char * szApiKey);
    void MarkFieldForReading(int iField, bool bEnable);

    void StartReading();
    void StopReading();

private slots:
    void on_channelDataReady(TS_CHANNEL_LAST channel);
    void on_fieldDataReady(int iField, const char * data);

private:

    QThread * m_pWorkerThread;
    TSReader * m_pReader;

signals:

    void startReader();
    void stopReader();

    void fieldDataAvailable(int iField, const char * data);
    void channelDataAvailable(TS_CHANNEL_LAST channelData);


};

#endif // QTHINGSPEAKREADER_H
