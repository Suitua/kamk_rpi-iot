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

#include "tcpsocket.h"
#include <netdb.h>



#define SO_REUSEPORT 15


TCPSocket::TCPSocket()
{
    m_bIsConnected = false;
    memset(m_szLastError,0,sizeof(m_szLastError));
}

TCPSocket::~TCPSocket()
{
    if(m_bIsConnected)
        Disconnect();
}

bool TCPSocket::Connect(const char *hostAddr, unsigned int port)
{
    if(((m_iSockFd = socket(PF_INET,SOCK_STREAM,PF_UNSPEC)) < 0))
    {
     GenerateErrorMessage();
     m_bNewErrMsg = true;
     return false;
    }

    else
    {
        char hAddr[32];

        //Is the host name or an IP-address

        //IP-address
        if(hostAddr[0] >= '0' && hostAddr[0] <= '9' )  //First char between 0 and 9
        {
            strcpy(hAddr,hostAddr);
        }
        //Name
        else
        {
            struct hostent *he;
            struct in_addr **addr_list;

            if ((he=gethostbyname( hostAddr ) ) == NULL)
            {
                GenerateErrorMessage();
                m_bNewErrMsg = true;
                return false;
            }

            addr_list = (struct in_addr **) he->h_addr_list;
            //Get the first one;
            strcpy(hAddr , inet_ntoa(*addr_list[0]) );

        }

        memset((void *)&m_sServAddr,sizeof(m_sServAddr),0);
        m_sServAddr.sin_family = AF_INET;
        m_sServAddr.sin_addr.s_addr = inet_addr(hAddr);
        m_sServAddr.sin_port = htons(port);

        if ( connect(m_iSockFd,(struct sockaddr *)&m_sServAddr,sizeof(m_sServAddr)) < 0 )
        {
            GenerateErrorMessage();
            m_bNewErrMsg = true;
            return false;
        }

        else
        {
            m_bIsConnected = true;
           return true;
        }
    }
}

bool TCPSocket::Disconnect()
{
    if (close(m_iSockFd))
    {
         GenerateErrorMessage();
         m_bNewErrMsg = true;
         return false;
    }

    m_bIsConnected = false;
    return true;

}

bool TCPSocket::WaitForConnection(unsigned int port)
{
    unsigned int clilen;
    int listenFd;

    if (( listenFd = socket(PF_INET,SOCK_STREAM,PF_UNSPEC)) < 0 )
    {
        GenerateErrorMessage();
        m_bNewErrMsg = true;
        return false;
    }

    // set SO_REUSEADDR and SO_REUSEPORT on the socket to true (1) to avoid error on second time binding it
    int optval = 1;
    if(setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval))
    {
        GenerateErrorMessage();
        m_bNewErrMsg = true;
        return false;
    }
    if(setsockopt(listenFd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof optval))
    {
        GenerateErrorMessage();
        m_bNewErrMsg = true;
        return false;
    }

    //Init the memory segments with zeros
    memset((void *)&m_sServAddr,sizeof(m_sServAddr),0);
    m_sServAddr.sin_family = AF_INET;
    m_sServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    m_sServAddr.sin_port = htons(port);

    if (( bind(listenFd,(struct sockaddr *)&m_sServAddr,sizeof(m_sServAddr)) < 0))
    {
        GenerateErrorMessage();
        m_bNewErrMsg = true;
        return false;
    }

    if (( listen(listenFd,5 ) < 0))
    {
        GenerateErrorMessage();
        m_bNewErrMsg = true;
        return false;
    }

    clilen = sizeof(m_sCliAddr);
    //Listen for a new connection (blocks untill someone connects)
    if (( m_iSockFd = accept(listenFd,(struct sockaddr *)&m_sCliAddr,&clilen)) < 0)
    if (( listen(m_iSockFd,5 ) < 0))
    {
        GenerateErrorMessage();
        m_bNewErrMsg = true;
        return false;
    }

    close(listenFd);

    m_bIsConnected = true;

    return true;
}

int TCPSocket::Write(const void * buf , int bufLen)
{
    int bytesWritten = -1;

    if ( ((bytesWritten = send(m_iSockFd,buf,bufLen,0)) != bufLen))
    {
        GenerateErrorMessage();
        m_bNewErrMsg = true;
        return -1;
    }

    return bytesWritten;
}

int TCPSocket::Read(void * buf, int bufSize)
{
    int bytesRead = -1;

    if ( ((bytesRead = recv(m_iSockFd,buf,bufSize,0)) < 0))
    {
        GenerateErrorMessage();
        m_bNewErrMsg = true;
        return -1;
    }

    return bytesRead;
}

int TCPSocket::TryWrite(const void *buf, int bufLen)
{
    int bytesWritten = -1;

    if ( ((bytesWritten = send(m_iSockFd,buf,bufLen,MSG_DONTWAIT)) != bufLen))
    {
        GenerateErrorMessage();
        m_bNewErrMsg = true;
        return -1;
    }

    return bytesWritten;
}

int TCPSocket::TryRead(void *buf, int bufSize)
{
    int bytesRead = -1;

    if ( ((bytesRead = recv(m_iSockFd,buf,bufSize,MSG_DONTWAIT)) < 0))
    {
        GenerateErrorMessage();
        m_bNewErrMsg = true;
        return -1;
    }

    return bytesRead;
}

bool TCPSocket::IsConnected()
{
    return m_bIsConnected;
}

/**
  @brief Gets the last error message
  @param
  @return a pointer to the NULL terminated error string, NULL if no error
*/
char * TCPSocket::GetLastError()
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

/**
  @brief Generates a human readable error message from the current errno
*/
void TCPSocket::GenerateErrorMessage()
{
    switch (errno) {
    case EACCES:
    {
        strcpy(m_szLastError, "No permissions.");
        break;
    }

    case EADDRINUSE:
    {
        strcpy(m_szLastError, "Address already in use.");
        break;
    }

    case EAFNOSUPPORT:
    {
        strcpy(m_szLastError, "Address family not supported.");
        break;
    }

    case EAGAIN:
    {
        strcpy(m_szLastError, "No more free local ports or insufficient entries in the routing cache.");
        break;
    }

    case EALREADY:
    {
        strcpy(m_szLastError, "Previous connection attempt has not yet been completed.");
        break;
    }

    case EBADF:
    {
        strcpy(m_szLastError, "Bad file descriptor.");
        break;
    }

    case ECONNREFUSED:
    {
        strcpy(m_szLastError, "Connection refused (no server listening).");
        break;
    }

    case EFAULT:
    {
        strcpy(m_szLastError, "Socket structure is out of the user's' address space.");
        break;
    }

    case EINPROGRESS:
    {
        strcpy(m_szLastError, "Connection cannot be completed immediately.");
        break;
    }

    case EINTR:
    {
        strcpy(m_szLastError, "Interrupted by sys call.");
        break;
    }

    case EISCONN:
    {
        strcpy(m_szLastError, "Socket is already connected.");
        break;
    }

    case ENETUNREACH:
    {
        strcpy(m_szLastError, "Network is unreachable.");
        break;
    }

    case ENOTSOCK:
    {
        strcpy(m_szLastError, "The descriptor is not a socket.");
        break;
    }

    case ETIMEDOUT:
    {
        strcpy(m_szLastError, "Connection timed out.");
        break;
    }

    case ECONNABORTED:
    {
        strcpy(m_szLastError, "Connection aborted.");
        break;
    }

    case EPERM:
    {
        strcpy(m_szLastError, "Firewall has blocked the connection.");
        break;
    }

    default:
        strcpy(m_szLastError, "Unknown error.");
        break;
    }

    switch (h_errno)
    {
        case HOST_NOT_FOUND:
        {
            strcpy(m_szLastError, "The specified host is unknown.");
            break;
        }

        case NO_DATA:
        {
            strcpy(m_szLastError, "The requested name is valid but does not have an IP address.");
            break;
        }

        case NO_RECOVERY:
        {
            strcpy(m_szLastError, "A nonrecoverable name server error occurred");
            break;
        }

        case TRY_AGAIN:
        {
            strcpy(m_szLastError, "A temporary error occurred on an authoritative name server.Try again later.");
            break;
        }
        default:
        {
            strcpy(m_szLastError, "Unknown error.");
            break;
        }
    }







}
