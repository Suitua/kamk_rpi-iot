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

#ifndef RF24RECEIVER_H
#define RF24RECEIVER_H

#include <RF24/RF24.h>
#include "thread.h"
#include "fifobuffer.h"

#define INTERNAL_BUFFER_SIZE    1024
#define MESSAGE_BUFFER_SIZE     2048
#define MESSAGE_BUFFER_ITEMS    16

#define PIPE_MAX_COUNT          6

#define PIPE_MASK_1                   1<<0
#define PIPE_MASK_2                   1<<1
#define PIPE_MASK_3                   1<<2
#define PIPE_MASK_4                   1<<3
#define PIPE_MASK_5                   1<<4
#define PIPE_MASK_6                   1<<5
#define PIPE_MASK_ALL                 0x3E

#define PIPE1_DEFAULT_ADDR              "1Node"
#define PIPE2_DEFAULT_ADDR              "2Node"
#define PIPE3_DEFAULT_ADDR              "3Node"
#define PIPE4_DEFAULT_ADDR              "4Node"
#define PIPE5_DEFAULT_ADDR              "5Node"
#define PIPE6_DEFAULT_ADDR              "6Node"

#define COND_NEW_DATA_TO_SEND       1
#define COND_RESUME_RECEPTION       2
#define COND_RECEPTION_PAUSED       3

#define COND_FIRST_AVAILABLE        1

#define DEBUG                       1

//#define TRANSMITTER               //Comment / uncomment to compile transmitter module (not working at the moment)

class RF24Receiver
{
public:
    //Constructor
    RF24Receiver(uint8_t _cepin, uint8_t _cspin, uint32_t spispeed);
    //Destructor
    ~RF24Receiver();
    //Starts the reception
    bool StartReceiving(unsigned int iPipeMask = PIPE_MASK_ALL);
    //Stops the reception
    bool StopReceiving();
    //Sets a 5 bit address for the pipe
    bool SetPipeAddress(unsigned int iPipe, const char * address);
    //Gets the pipe's address
    const char * GetPipeAddress(unsigned int iPipe);
    //Checks if there is a new message available
    int NewMessageAvailable(unsigned int iPipe);
    //Reads the message
    int ReadMessage(char * buf, unsigned long ulBufLen, unsigned int iPipe);

    //Sets the radio channel
    bool SetRadioChannel(unsigned uChannel);
    //Sets the RF24 power amplifier level
    bool SetPowerAmplifierLevel(unsigned uLevel);
    //Gets the PA level
    unsigned GetPowerAmpliferLevel();
    //Sets the datarate
    bool SetDataRate(rf24_datarate_e eDataRate);
    //Gets the datarate
    rf24_datarate_e GetDataRate();


    //Gets the first available pipe num
    //This function blocks until one message is received
    int GetFirstAvailablePipe();
    //Gets the first available message
    //This function blocks until one message is received
    int ReadFirstMessage(char * buf, int iBufLen);

#ifdef TRANSMITTER
    //Sends a message
    bool SendMessage(const char * msg, unsigned int iPipe);
    //Sends a message as an ack payload
    bool SendAckMessage(const char * msg, unsigned int iPipe);
    //Broacasts a message
    bool BroadcastMessage(const char * msg, unsigned int iPipeMask = PIPE_MASK_ALL);
    //Enables the ack messages
    bool EnableAckMessages();
    //Enables the transmitter
    bool EnableTransmitter();
#endif

private:
    //RF24 Radio object
    RF24 * m_pRadio;
    //Internal reading buffer for each pipe
    char *
    m_szRecvBuffer[PIPE_MAX_COUNT];

    //Message FIFO buffer for each pipe
    FIFOBuffer * m_pMessageBuffer[PIPE_MAX_COUNT];

    //Internal sending buffer for each pipe

    bool m_bRecvBufferOverflow[PIPE_MAX_COUNT];

    unsigned m_bNewMessage[PIPE_MAX_COUNT];
    int m_iMessageLen[PIPE_MAX_COUNT];

    //Block size for the radio traffic
    unsigned int m_iBlockSize;

    //Mutex used for protecting the read buffer
    PMutex * m_pBufferMutex;

    //Mutex used for protecting the radio object
    PMutex * m_pRadioMutex;

    PMutex * m_pFirstAvailableMutex;

    //Booleans to control the worker threads' execution
    bool m_bStopReception;
    bool m_bPauseReception;

    //Stores the last received message's pipe number
    int m_iLastReceivedMessage;

    //Worker thread that reads the incoming messages, combines them and puts them into buffer
    Thread * m_pReceiverThread;

    //Radio reception loop
    static void * receptionloop(void * data);

    //Adds the received stream block into buffer
    bool addToBuffer( const char * msg, int iPipe);

    //Pipe bit mask for reading
    unsigned int m_iReadingPipeMask;

    //Pipe addresses
    uint8_t m_uiPipeAddr[6][6];
#ifdef TRANSMITTER
    bool m_bNewMessageToSend[PIPE_MAX_COUNT];

    char *
    m_szSendBuffer[PIPE_MAX_COUNT];

    bool m_bSendAckMessage[PIPE_MAX_COUNT];

    char *
    m_szAckSendBuffer[PIPE_MAX_COUNT];

    bool m_bTransmitterEnabled;

    //Worker thread that sends messages
    Thread * m_pTransmitterThread;

    bool m_bAckMessagesInUse;
    bool m_bStopTransmitter;

    //Radio transmit loop
    static void * transmitloop(void * data);

    //Sends the string via radio
    void sendMessage(unsigned int iPipe, const char *msg, const bool asAckPayload = false);
#endif



};

#endif // RF24RECEIVER_H
