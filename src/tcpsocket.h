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

#ifndef TCPSOCKET_H
#define TCPSOCKET_H
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


class TCPSocket
{
public:
    //Constructor
    TCPSocket();
    //Desctructor
    ~TCPSocket();
    //Connects to a socket
    bool Connect(const char * hostAddr, unsigned int port);
    //Disconnects the socket
    bool Disconnect();
    //Wait for the client to connect
    bool WaitForConnection(unsigned int port);
    //Write data into the socket
    int Write(const void * buf, int bufLen);
    //Read data from the socket
    int Read(void * buf, int bufSize);
    //Try to write data into the socket (non blocking)
    int TryWrite(const void * buf, int bufLen);
    //Try to read data from the socket (non blocking)
    int TryRead(void * buf, int bufSize);
    //Checks if the socket is connected
    bool IsConnected();
    //Gets the last error message
    char * GetLastError();

private:
    int m_iSockFd;              //Read / Write file descriptor for the socket
    bool m_bIsConnected;             //Is the socket connected
    struct sockaddr_in m_sServAddr; //Server addr struct
    struct sockaddr_in m_sCliAddr; //Client addr struct
    char m_szLastError[256];   //Contains the last error message
    bool m_bNewErrMsg;          //Indicates if there is a new, unread error message
    void GenerateErrorMessage();//Generates a human readable error message from the current errno
};

#endif // TCPSOCKET_H
