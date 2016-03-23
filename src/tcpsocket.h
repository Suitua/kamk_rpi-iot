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
