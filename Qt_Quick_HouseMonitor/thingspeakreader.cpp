#include "thingspeakreader.h"

ThingSpeakReader::ThingSpeakReader(QObject *parent) : QObject(parent)
{
    m_pQTS =  new QThingSpeakReader();
    connect(m_pQTS, &QThingSpeakReader::channelDataAvailable, this, &ThingSpeakReader::on_channelDataAvailable);
    m_pQTS->SetChannelId("58423");
    m_pQTS->StartReading();
}

ThingSpeakReader::~ThingSpeakReader()
{
    m_pQTS->stopReader();
    delete m_pQTS;
}

void ThingSpeakReader::on_channelDataAvailable(TS_CHANNEL_LAST channel)
{
    emit(sendToQml(channel.IntFieldData(1),channel.IntFieldData(2),channel.IntFieldData(3)
                   ,channel.IntFieldData(4),channel.IntFieldData(5),channel.IntFieldData(6)));
}

