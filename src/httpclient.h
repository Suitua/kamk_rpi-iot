#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H
#include "tcpsocket.h"
#include <iostream>

//#define HTTP_ASYNC_MODULE

#ifdef HTTP_ASYNC_MODULE
#include "thread.h"
#include "pmutex.h"
#endif

#define HTTP_HEADER_MAX_COUNT           128

#define HTTP_HEADER_MAX_SIZE    16384
#define HTTP_CONTENT_MAX_SIZE   2097152
//#define HTTP_CONTENT_MAX_SIZE   128000

#define COND_NEW_REQUEST        1
#define COND_REQUEST_COMPLETE   2

class HTTPRequest
{

    //Give HHTPClient class access to private members & methods
    friend class HTTPClient;

public:
    //Constructor
    HTTPRequest();
    //Destructor
    ~HTTPRequest();
    //Sets the http version (e.g. HTTP/1.1 )
    void setVersion(const char * szVersion);
    //Sets the http method
    void setMethod(const char * szMethod);
    //Sets the URI
    void setURI(const char * szURI);
    //Sets the host
    void setHost(const char * szHost);
    //Sets the content type
    void setContentType(const char * szType);
    //Add a header item
    void addHeaderItem(const char * szItemName, const char * itemData);
    //Adds content into the http request (the length will updated automatically)
    void addContent(const char * content);
    //Manually set the content length (bytes)
    void setContentLen(unsigned int iLen);
    //Manually set the http request string instead of auto-generating based on member data
    void setHttpRequestStr(const char * request);
    //Gets a string pointer to http request string
    const char * requestString();

    void prepareRequestStr();
    void Clear();

private:

    //A flag to indicate if the http response string is prepared
    bool m_bRequestStringPrepared;
    //http version
    std::string m_szVersion;
    //Http method
    std::string m_szMethod;
    //Http host address
    std::string m_szHostAddr;
    //http URI (Uniform Resource Identifier)
    std::string m_szURI;
    //content data
    std::string m_szContent;
    //content length as bytes
    int m_iContentLen;
    //Array for header item names
    std::string m_szHeaderName[HTTP_HEADER_MAX_COUNT];
    //Array for header data
    std::string m_szHeaderData[HTTP_HEADER_MAX_COUNT];
    //Header item count
    int m_iHeaderItemCount;
    //Http request as a single http string
    std::string m_szRequestStr;

};

class HTTPResponse
{
    //Give HTTPClient class access to private members & methods
    friend class HTTPClient;

public:
    //Constructor
    HTTPResponse();
    //Destructor
    ~HTTPResponse();
    //Gets the http response string pointer
    const char * getHttpResponseStr();
    //Sets the http response string
    void setHttpResponseStr(const char * responseStr);
    //Gets the response content length as bytes
    unsigned int contentLen();
    //Parses the response string
    void parseResponseStr();

    //Gets a string pointer to http protocol version
    const char  * version();
    //Gets a string pointer to reply status code
    const char * statusCode();
    //Gets a string pointer to  reply status as text
    const char * statusText();
    //Gets a string pointer to content data
    const char * content();
    //Gets header item count
    int headerItemCount();
    //Gets a header item (copies the name and data into given char arrays)
    bool getHeaderItem(const int item, std::string & itemName,  std::string & itemData);
    //Gets a string pointer to a header item name
    const char * headerItemName(const int item);
    //Gets a string pointer to a header item data
    const char * headerItemData(const int item);
    //Reset all data
    void Clear();

private:

    //http version
    std::string m_szVersion;
    //http status code
    std::string m_szStatusCode;
    //http status text
    std::string m_szStatusText;

    //Array for header item names
    std::string m_szHeaderName[HTTP_HEADER_MAX_COUNT];
    //Array for header data
    std::string m_szHeaderData[HTTP_HEADER_MAX_COUNT];
    //Header item count
    int m_iHeaderItemCount;

    //The content length as bytes
    unsigned int m_iContentLen;
    //The content data
    std::string m_szContent;
    //Un-parsed http response string (set outside the class)
    std::string m_szResponseStr;
};

class HTTPClient
{
public:
    //Constructor
    HTTPClient();
    //Destructor
    ~HTTPClient();
    //Gets the last error msg
    const char * GetLastError();
    //Tests if we can connect to the host and then disconnect
    bool TestConnection(const char *hostName);
    //Returns the pointer to the http request for direct modification
    HTTPRequest * httpRequest();
    //Returns the pointer to the http response for direct reading
    HTTPResponse * httpResponse();
    //Sends the http request
    bool SendRequest();

#ifdef HTTP_ASYNC_MODULE
    //Sends the http request (async) and calls the callback function if it's given
    bool SendRequest_async(void *(*onRequestCompleted)(void *) = NULL, void * data = NULL);
#endif

private:
    //Tcp socket for the connection
    TCPSocket * m_pSocket;

    char m_szLastError[1024];   //Contains the last error message
    bool m_bNewErrMsg;          //Indicates if there is a new, unread error message

    char  *m_szBuf;             //Internal buffer for HTTP response

    HTTPRequest m_httpRequest;  //http request
    HTTPResponse m_httpResponse;//http response

#ifdef HTTP_ASYNC_MODULE
    Thread * m_pRequestThread;
    bool m_bRequestCompleted;
    bool m_bShutdownRequestThread;
    bool m_bThreadRequestInProgress;
    PMutex * m_pRequestMutex;

    void *(*extCallbackFunc)(void *);  //Ext callback function ptr
    void * extDataPtr;              //Ext data ptr for the callback function

    //Thread request loop
    static void * requestLoop(void * data);
#endif
};

#endif // HTTPCLIENT_H
