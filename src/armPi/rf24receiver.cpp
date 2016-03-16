/*
*  Copyright (C) 2016, Timo Leinonen <timojt.leinonen@gmail.com>
*  
*  Description: "The RF24Receiver for Raspberry Pi is based on RF24 library made by 
*  J. Coliz (2011) <maniacbug@ymail.com> and modified by Charles-Henri Hallard (03/17/2013)
*  (http://hallard.me)." 
*  
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

#include "rf24receiver.h"
//#define TRANSMITTER               //Comment / uncomment to compile transmitter module (not working at the moment)

RF24Receiver::RF24Receiver(uint8_t _cepin, uint8_t _cspin, uint32_t spispeed )
{
    m_iBlockSize = 32;

    m_pRadio = new RF24(_cepin, _cspin, spispeed);

    for(int i = 0; i < PIPE_MAX_COUNT; i++)
    {
        m_szRecvBuffer[i] = new char [INTERNAL_BUFFER_SIZE];
        m_iMessageLen[i] = 0;
        m_bNewMessage[i] = false;
        m_bRecvBufferOverflow[i] = false;
        m_pMessageBuffer[i] = new FIFOBuffer(MESSAGE_BUFFER_SIZE,MESSAGE_BUFFER_ITEMS);
#ifdef TRANSMITTER
        m_szAckSendBuffer[i] = new char [m_iBlockSize];
        m_bSendAckMessage[i] = false;
#endif
    }

    m_pBufferMutex = new PMutex();
    m_pBufferMutex->Initialize();
    m_pBufferMutex->Release();
    m_pRadioMutex = new PMutex();
    m_pRadioMutex->Initialize();
    m_pRadioMutex->Release();
    m_pFirstAvailableMutex = new PMutex();
    m_pFirstAvailableMutex->Initialize();
    m_pFirstAvailableMutex->Release();


    m_pReceiverThread = new Thread();
    m_pReceiverThread->SetWork(receptionloop,this);

    m_bStopReception = false;
    m_bPauseReception = false;


    m_pRadio->begin();
    m_pRadio->setAutoAck(1);                    // Set auto ack enabled
    m_pRadio->setRetries(1,15);                 // Smallest time between retries, max no. of retries
    m_pRadio->setPayloadSize(m_iBlockSize);

    strcpy((char*)&m_uiPipeAddr[0][0],PIPE1_DEFAULT_ADDR);
    strcpy((char*)&m_uiPipeAddr[1][0],PIPE2_DEFAULT_ADDR);
    strcpy((char*)&m_uiPipeAddr[2][0],PIPE3_DEFAULT_ADDR);
    strcpy((char*)&m_uiPipeAddr[3][0],PIPE4_DEFAULT_ADDR);
    strcpy((char*)&m_uiPipeAddr[4][0],PIPE5_DEFAULT_ADDR);
    strcpy((char*)&m_uiPipeAddr[5][0],PIPE6_DEFAULT_ADDR);

    m_iLastReceivedMessage = -1;

#ifdef TRANSMITTER
    m_bAckMessagesInUse = false;
    m_bTransmitterEnabled = false;
    m_bStopTransmitter = false;
#endif


}

RF24Receiver::~RF24Receiver()
{
    m_pReceiverThread->Cancel();

    for(int i = 0; i < PIPE_MAX_COUNT; i++)
    {
        delete[] m_szRecvBuffer[i];
        delete m_pMessageBuffer[i];
#ifdef TRANSMITTER
        delete[] m_szAckSendBuffer[i];
#endif
    }

#ifdef TRANSMITTER
    if (true == m_bTransmitterEnabled)
    {
        m_pTransmitterThread->Cancel();
        delete[] m_pTransmitterThread;

        for(int i = 0; i < PIPE_MAX_COUNT; i++)
        {
            delete[] m_szSendBuffer[i];
        }
    }
#endif

    delete m_pReceiverThread;
    delete m_pBufferMutex;
    delete m_pRadioMutex;
    delete m_pFirstAvailableMutex;
    delete m_pRadio;
}

bool RF24Receiver::StartReceiving(unsigned int iPipeMask)
{

    m_iReadingPipeMask = iPipeMask;

    m_bPauseReception = false;
    m_bStopReception = false;

    if(false == m_pReceiverThread->Run())
        return false;

    return true;
}

bool RF24Receiver::StopReceiving()
{

    m_bStopReception = true;
    return true;
}

bool RF24Receiver::SetPipeAddress(unsigned int iPipe, const char *address)
{
    if(iPipe > 6)
        return false;

    for(int i = 0; i < 5; i++)
    {
        m_uiPipeAddr[iPipe-1][i] = address[i];
    }

    m_uiPipeAddr[iPipe-1][5] = '\0';
    return true;
}

const char *RF24Receiver::GetPipeAddress(unsigned int iPipe)
{
    if(iPipe > 6)
        return NULL;
    return (char*)m_uiPipeAddr[iPipe-1];
}


int RF24Receiver::NewMessageAvailable(unsigned int iPipe)
{
    if(iPipe > PIPE_MAX_COUNT)
        return false;
    m_pBufferMutex->Lock();

    return m_bNewMessage[iPipe-1];

    m_pBufferMutex->Release();
}

int RF24Receiver::ReadMessage(char *buf, unsigned long ulBufLen, unsigned int iPipe)
{
    if(iPipe > PIPE_MAX_COUNT)
        return 0;

    if(m_bNewMessage[iPipe-1] == 0)
        return 0;

    m_pBufferMutex->Lock();
    if(ulBufLen < m_pMessageBuffer[iPipe-1]->GetDataLength())
    {
        m_pBufferMutex->Release();
        return 0;
    }

    int bytesRead = m_pMessageBuffer[iPipe-1]->GetData(buf,ulBufLen);
    if(bytesRead == 0)
    {
         m_pBufferMutex->Release();
        return 0;
    }
    m_bNewMessage[iPipe-1]--;


    m_pBufferMutex->Release();
    return bytesRead;
}

bool RF24Receiver::SetRadioChannel(unsigned uChannel)
{
    if(false == m_bStopReception)
    {
        m_bPauseReception = true;
        m_pRadioMutex->Lock();
        m_pRadio->setChannel(uChannel);
        m_pRadioMutex->Release();
        m_pRadioMutex->BroadcastCond(COND_RESUME_RECEPTION);
        return true;
    }
    else
        m_pRadio->setChannel(uChannel);

    return true;
}

bool RF24Receiver::SetPowerAmplifierLevel(unsigned uLevel)
{
    if(false == m_bStopReception)
    {
        m_bPauseReception = true;
        m_pRadioMutex->Lock();
        m_pRadio->setPALevel(uLevel);
        m_pRadioMutex->Release();
        m_pRadioMutex->BroadcastCond(COND_RESUME_RECEPTION);
        return true;
    }
    else
        m_pRadio->setPALevel(uLevel);

    return true;
}

unsigned RF24Receiver::GetPowerAmpliferLevel()
{

    if(false == m_bStopReception)
    {
        m_bPauseReception = true;
        m_pRadioMutex->Lock();
        unsigned uRes = m_pRadio->getPALevel();
        m_pRadioMutex->Release();
        m_bPauseReception = false;
        m_pRadioMutex->BroadcastCond(COND_RESUME_RECEPTION);
        return uRes;
    }
    else
       return  m_pRadio->getPALevel();

}

bool RF24Receiver::SetDataRate(rf24_datarate_e eDataRate)
{
    if(false == m_bStopReception)
    {
        m_bPauseReception = true;
        m_pRadioMutex->Lock();
        m_pRadio->setDataRate(eDataRate);
        m_pRadioMutex->Release();
        m_bPauseReception = false;
        m_pRadioMutex->BroadcastCond(COND_RESUME_RECEPTION);
        return true;
    }
    else
        m_pRadio->getDataRate();
   return  true;
}

rf24_datarate_e RF24Receiver::GetDataRate()
{
    if(false == m_bStopReception)
    {
        m_bPauseReception = true;
        m_pRadioMutex->Lock();
        rf24_datarate_e eRes = m_pRadio->getDataRate();
        m_pRadioMutex->Release();
        m_bPauseReception = false;
        m_pRadioMutex->BroadcastCond(COND_RESUME_RECEPTION);
        return eRes;
    }
    else
       return  m_pRadio->getDataRate();
}


int RF24Receiver::GetFirstAvailablePipe()
{
    m_pBufferMutex->Lock();
    for(int i = 0; i < PIPE_MAX_COUNT; i++)
    {
        if(m_bNewMessage[i] > 0)
        {
            m_pBufferMutex->Release();
            return i+1;
        }
    }
    m_pBufferMutex->Release();

    m_pFirstAvailableMutex->WaitForCond(COND_FIRST_AVAILABLE);
    int iRes = m_iLastReceivedMessage;
    m_pFirstAvailableMutex->Release();

    return iRes+1;
}

int RF24Receiver::ReadFirstMessage(char *buf, int iBufLen)
{
    m_pBufferMutex->Lock();
    for(int i = 0; i < PIPE_MAX_COUNT; i++)
    {
        if(m_bNewMessage[i] > 0)
        {
            m_pBufferMutex->Release();
            return ReadMessage(buf,iBufLen,i+1);
        }
    }
    m_pBufferMutex->Release();

    m_pFirstAvailableMutex->WaitForCond(COND_FIRST_AVAILABLE);
    int iRes = m_iLastReceivedMessage;
    m_pFirstAvailableMutex->Release();
    return ReadMessage(buf,iBufLen,iRes+1);
}

void *RF24Receiver::receptionloop(void *data)
{
    RF24Receiver * pReceiver = (RF24Receiver*)data;

    pReceiver->m_pRadioMutex->Lock();
    pReceiver-> m_pRadio->openWritingPipe(&pReceiver->m_uiPipeAddr[0][0]);

   //Checking the bit mask seems to be useless since the radio
   //receives data from all pipes regardless of them being open for reading or not
    for(unsigned int i = 0; i < PIPE_MAX_COUNT; i++)
    {
        if((pReceiver->m_iReadingPipeMask) & (1<<i))
        {
            pReceiver->m_pRadio->openReadingPipe(i,&pReceiver->m_uiPipeAddr[i][0]);
        }
    }

    pReceiver->m_pRadio->startListening();

    while(false == pReceiver->m_bStopReception)
    {

        if(pReceiver->m_bPauseReception)
        {
            pReceiver->m_pRadio->stopListening();
            /*
            pReceiver->m_pRadioMutex->Release();

            while(pReceiver->m_bPauseReception)
                usleep(1000);   //Sleeping here is not a good practice, maybe replace this with cond_signal or message queve

            pReceiver-> m_pRadioMutex->Lock();
            */

            pReceiver->m_pRadioMutex->WaitForCond(COND_RESUME_RECEPTION);
            pReceiver->m_pRadio->startListening();
        }

        usleep(10000);      //Check for new messages every 10 milliseconds
                            //(using sleep is a waste of CPU cycles but I don't see any other option here)

        uint8_t pipeNo;
        char received[32];
        memset(received,0,sizeof(received));
        if( pReceiver->m_pRadio->available(&pipeNo))
        {
            //Enforce the bit mask checking here since it has no effect when opening the pipes
            //We still have to read the data and discard it
            pReceiver->m_pRadio->flush_tx();
            pReceiver->m_pRadio->read(received, sizeof(received));


            //Only store the data if the pipe is marked for reading
            if((pReceiver->m_iReadingPipeMask) & (1<<pipeNo))
                pReceiver->addToBuffer(received,pipeNo);
       }
#ifdef TRANSMITTER
       if(pReceiver->m_bAckMessagesInUse)
       {
           //Now check if we have to send ack messages back
           for(int i = 0; i < PIPE_MAX_COUNT; i++)
           {
               if(true == pReceiver->m_bSendAckMessage[i])
               {
                    pReceiver->sendMessage(i,&pReceiver->m_szAckSendBuffer[i][0],true);
                    pReceiver->m_bSendAckMessage[i] = false;
                    pReceiver->m_szAckSendBuffer[i][0] = '\0';
               }
           }
       }
#endif
    }
    pReceiver->m_pRadio->stopListening();
    pReceiver->m_pRadioMutex->Release();

    return NULL;
}

bool RF24Receiver::addToBuffer(const char *msg, int iPipe)
{
    m_pBufferMutex->Lock();

    if(m_iMessageLen[iPipe] + m_iBlockSize >= INTERNAL_BUFFER_SIZE)
    {
        m_pBufferMutex->Release();
        return false;
    }

    for(unsigned int i = 0; i <m_iBlockSize;i++)
    {
        m_szRecvBuffer[iPipe][m_iMessageLen[iPipe]] = msg[i];
        m_iMessageLen[iPipe]++;
        if(msg[i] == 0)
        {         
            if(false == m_pMessageBuffer[iPipe]->AddData(&m_szRecvBuffer[iPipe][0],strlen(&m_szRecvBuffer[iPipe][0])+1))
            {
                m_bRecvBufferOverflow[iPipe] = true;
                if(DEBUG)
                    printf("Pipe%d: Buffer overflow.\n",iPipe+1);

                m_iMessageLen[iPipe] = 0;
                m_szRecvBuffer[iPipe][0] = '\0';
                m_pBufferMutex->Release();
                return false;
            }

            m_bNewMessage[iPipe]++;
            m_iLastReceivedMessage = iPipe;
            m_iMessageLen[iPipe] = 0;
            m_szRecvBuffer[iPipe][0] = '\0';
            m_pFirstAvailableMutex->BroadcastCond(COND_FIRST_AVAILABLE);
            break;
        }

    }

    m_pBufferMutex->Release();

    return true;

}

#ifdef TRANSMITTER

bool RF24Receiver::SendMessage(const char *msg, unsigned int iPipe)
{
    if(iPipe >6)
        return false;
    m_pBufferMutex->Lock();
    m_bNewMessageToSend[iPipe-1] = true;
    strcpy(&m_szSendBuffer[iPipe-1][0],msg);
    m_pBufferMutex->Release();


    m_pRadioMutex->BroadcastCond(COND_NEW_DATA_TO_SEND);
    m_bPauseReception = true;

    return true;
}

bool RF24Receiver::SendAckMessage(const char *msg, unsigned int iPipe)
{
    if(iPipe >6)
        return false;
    if(strlen(msg)>m_iBlockSize)
        return false;

    m_pBufferMutex->Lock();
    m_bNewMessageToSend[iPipe-1] = true;
    strcpy(&m_szAckSendBuffer[iPipe-1][0],msg);
    m_bSendAckMessage[iPipe-1] = true;
    m_pBufferMutex->Release();


    return true;
    return true;
}

bool RF24Receiver::BroadcastMessage(const char *msg, unsigned int iPipeMask)
{

    m_pBufferMutex->Lock();
    for(int i = 0; i < PIPE_MAX_COUNT; i++)
    {
        if((iPipeMask) & (1<<i))
        {
             m_bNewMessageToSend[i] = true;
             strcpy(&m_szSendBuffer[i][0],msg);
        }
    }
    m_pBufferMutex->Release();


    m_bPauseReception = true;
    m_pRadioMutex->BroadcastCond(COND_NEW_DATA_TO_SEND);

    return true;
}

bool RF24Receiver::EnableAckMessages()
{
    m_bAckMessagesInUse = true;
    m_pRadio->enableAckPayload();
    return true;
}

void *RF24Receiver::transmitloop(void *data)
{
    RF24Receiver * pReceiver = (RF24Receiver*)data;

    while(false == pReceiver->m_bStopTransmitter)
    {
        pReceiver->m_pRadioMutex->WaitForCond(COND_NEW_DATA_TO_SEND);

        pReceiver->m_pBufferMutex->Lock();

        for(int i = 0; i < PIPE_MAX_COUNT; i++)
        {

            if(true == pReceiver->m_bNewMessageToSend[i])
            {
                pReceiver->sendMessage(i,&pReceiver->m_szSendBuffer[i][0]);
                pReceiver->m_bNewMessageToSend[i] = false;
                pReceiver->m_szSendBuffer[i][0] = '\0';
            }

        }

        pReceiver->m_pBufferMutex->Release();

        if( false == pReceiver->m_bStopReception)
        {
            pReceiver->m_bPauseReception = false;
            pReceiver->m_pRadioMutex->BroadcastCond(COND_RESUME_RECEPTION);
        }

    }

    return NULL;
}
void RF24Receiver::sendMessage(unsigned int iPipe, const char *msg, const bool asAckPayload)
{

    if(true == asAckPayload)
    {
        if(strlen(msg) > 32)
            return;
        m_pRadio->writeAckPayload(iPipe,msg,strlen(msg));
        if(DEBUG) printf("Sent ack message:\n%s \n\nto Pipe%d.\n",msg,iPipe+1);
    }

    else
    {
        m_pRadio->openWritingPipe(&m_uiPipeAddr[iPipe][0]);

        unsigned int msgLen = strlen(msg);
        unsigned int fullBlocks = msgLen / m_iBlockSize;
        char lastBlock[m_iBlockSize];
        memset(lastBlock,0,sizeof(lastBlock));

        unsigned int block;

        for(block = 0; block < fullBlocks; block++)
        {
            m_pRadio->write(&msg[block*m_iBlockSize],m_iBlockSize);
        }

        strcpy(lastBlock,&msg[block*m_iBlockSize]);
        m_pRadio->write(&msg[block*m_iBlockSize],m_iBlockSize,0);

        if(DEBUG) printf("Sent message:\n%s \n\nto Pipe%d.\n",msg,iPipe+1);

    }


}

bool RF24Receiver::EnableTransmitter()
{

    if(true == m_bTransmitterEnabled)
        return true;
    else
    {
        for(int i = 0; i < PIPE_MAX_COUNT; i++)
        {
            m_szSendBuffer[i] = new char [INTERNAL_BUFFER_SIZE];
            m_bNewMessageToSend[i] = false;
        }

        m_pTransmitterThread = new Thread();
        m_pTransmitterThread->SetWork(transmitloop,this);

    }

    if(false == m_pTransmitterThread->IsRunning())
        if(false == m_pTransmitterThread->Run())
            return false;
    usleep(1000);
    m_bTransmitterEnabled = true;

    return true;
}
#endif

