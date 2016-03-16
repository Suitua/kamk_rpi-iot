#ifndef THINGSPEAKREADER_H
#define THINGSPEAKREADER_H

#include "thingspeakclient.h"
#include "qthingspeakreader.h"
#include <QObject>


class ThingSpeakReader : public QObject
{
    Q_OBJECT
public:
    //Constructor
    explicit ThingSpeakReader(QObject * parent = 0);
    //Destructor
    virtual ~ThingSpeakReader();

public slots:
    void on_channelDataAvailable(TS_CHANNEL_LAST channel);

signals:
    void sendToQml(int temp1, int temp2, int temp3, int door1, int door2, int door3);

private:
    QThingSpeakReader * m_pQTS;

};

#endif // THINGSPEAKREADER_H
