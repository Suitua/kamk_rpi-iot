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

#ifndef FIFOBUFFER_H
#define FIFOBUFFER_H

//#define DEFAULT_BUFFER_SIZE     10240       // 10k
#define MAX_BUFFER_ITEMS        128

class FIFOBuffer
{
public:
    //Constructor
    FIFOBuffer(unsigned long  ulBufferSize,
               unsigned long  ulMaxItems = MAX_BUFFER_ITEMS);
    //Destructor
    ~FIFOBuffer();
    //Adds new data into the buffer
    bool AddData(const void * data, unsigned long ulDataLen);
    //Gets the data
    long unsigned  GetData(void * data, unsigned long ulDataLen);
    //Gets the data length of the next item
    long unsigned GetDataLength();
    //Gets the number of items in the buffer
    unsigned long GetItemCount();
    //Gets the total amount of data in the buffer (bytes)
    unsigned long int GetTotalDataLength();
    //Clears the buffer
    void Clear();
    //Gets the last error message
    char * GetLastError();
private:

    char m_szLastError[256];        //Contains the last error message
    bool m_bNewErrMsg;              //Indicates if there is a new, unread error message
    //How many items in buffer so far
    unsigned long m_uItemsInBuffer;
    //Head
    unsigned long m_uHead;
    //Tail
    unsigned long m_uTail;
    //Size of each item
    unsigned long *m_ulItemSize;
    //Start location in buffer for each item
    unsigned long *m_ulItemStart;
    //Data buffer total size
    unsigned long m_ulBufferSize;
    //Max items in buffer
    unsigned long m_uMaxItemsInBuffer;
    //The total size of data now in buffer
    unsigned long m_ulCurrentDataSize;
    //Memory segment for the buffer data
    unsigned char * m_pucBufferMmemory;

};

#endif // FIFOBUFFER_H
