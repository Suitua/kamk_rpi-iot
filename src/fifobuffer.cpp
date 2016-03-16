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

#include "fifobuffer.h"
#include <string.h>
#include <stdlib.h>

FIFOBuffer::FIFOBuffer(unsigned long ulBufferSize, unsigned long ulMaxItems)
{
    memset(&m_szLastError,0,sizeof(m_szLastError));
    m_uItemsInBuffer = 0;
    m_ulBufferSize = ulBufferSize;
    m_uMaxItemsInBuffer = ulMaxItems;
    m_ulCurrentDataSize = 0;
    m_bNewErrMsg = false;
    m_pucBufferMmemory = new unsigned char[m_ulBufferSize];

    m_uHead = 0;
    m_uTail = 0;

    m_ulItemSize = new long unsigned [m_uMaxItemsInBuffer];
    m_ulItemStart = new long unsigned [m_uMaxItemsInBuffer];

    for(unsigned long i = 0; i < m_uMaxItemsInBuffer; i++)
    {
        m_ulItemSize[i] = 0;
        m_ulItemStart[i] = 0;


    }
}

FIFOBuffer::~FIFOBuffer()
{
    delete[] m_pucBufferMmemory;

    delete[] m_ulItemSize;
    delete[]  m_ulItemStart;


}

bool FIFOBuffer::AddData(const void *data, unsigned long ulDataLen)
{
    //This should not ever happen..... but it does
    if(m_ulCurrentDataSize + ulDataLen > m_ulBufferSize)
    {
        m_bNewErrMsg = true;
        strcpy(m_szLastError, "Cannot add more data, buffer overflow");
        return false;
    }

    if(m_uHead >= m_uMaxItemsInBuffer)
        m_uHead = 0;

    //Some checks that we don't exceed set limits
    if(m_uTail == m_uHead+1)
    {
        m_bNewErrMsg = true;
        strcpy(m_szLastError, "Cannot add more items, buffer overflow");
        return false;
    }

    if(m_uItemsInBuffer == m_uMaxItemsInBuffer)
    {
        m_bNewErrMsg = true;
        strcpy(m_szLastError, "Cannot add more items, buffer is full (item count exceeded).");
        return false;
    }

    if(m_uHead < m_uTail)
    {
        //If the buffer heads is not 0
        if(m_uHead != 0)
        {
            //If theres not enough free memory between head and tail
            if(ulDataLen > ((unsigned long)abs(m_ulItemStart[m_uHead-1]+ m_ulItemSize[m_uHead-1] - m_ulItemStart[m_uTail])))
            {
                //If there is not enough memory between the beginning of the buffer and the tail
                //if(ulDataLen >= m_ulItemStart[m_uTail])
                //{
                    m_bNewErrMsg = true;
                    strcpy(m_szLastError, "Cannot add more items, buffer is full (data size exceeded).");
                    return false;
                //}
                //m_ulItemStart[m_uHead] = 0;
            }


            if((m_ulItemStart[m_uHead-1]+m_ulItemSize[m_uHead-1]+ulDataLen) > m_ulBufferSize)
            {

                if(ulDataLen >= m_ulItemStart[m_uTail])
                {
                    m_bNewErrMsg = true;
                    strcpy(m_szLastError, "Cannot add more items, buffer is full (data size exceeded).");
                    return false;
                }
                m_ulItemStart[m_uHead] = 0;
            }

            //Else keep filling the buffer normally
            else
            {
                m_ulItemStart[m_uHead] = m_ulItemStart[m_uHead-1]+m_ulItemSize[m_uHead-1];
            }

        }
        //Else if it's 0
        else
        {
            if((ulDataLen + m_ulItemStart[m_uMaxItemsInBuffer-1]+m_ulItemSize[m_uMaxItemsInBuffer-1] >= m_ulBufferSize
                    && m_ulCurrentDataSize != 0))
            {
                if(ulDataLen > m_ulItemStart[m_uTail])
                {
                    m_bNewErrMsg = true;
                    strcpy(m_szLastError, "Cannot add more items, buffer is full (data size exceeded).");
                    return false;
                }
                 m_ulItemStart[m_uHead] = 0;
            }
            else
                m_ulItemStart[m_uHead] = m_ulItemStart[m_uMaxItemsInBuffer-1]+m_ulItemSize[m_uMaxItemsInBuffer-1];


        }
    }

    else
    {
        //If the buffer heads is not 0
        if(m_uHead != 0)
        {

            //If theres not enough free memory in the end of the buffer
            if((ulDataLen + m_ulItemStart[m_uHead-1]+ m_ulItemSize[m_uHead-1]) > m_ulBufferSize)
            {

                //If there is not enough memory between the beginning of the buffer and the tail
                if(ulDataLen > m_ulItemStart[m_uTail])
                {

                    m_bNewErrMsg = true;
                    strcpy(m_szLastError, "Cannot add more items, buffer is full (data size exceeded).");
                    return false;
                }
                m_ulItemStart[m_uHead] = 0;
            }

            //Else keep filling the buffer normally
            else
            {
                m_ulItemStart[m_uHead] = m_ulItemStart[m_uHead-1]+m_ulItemSize[m_uHead-1];
            }


        }
        //Else if it's 0
        else
        {
            if(ulDataLen >= m_ulItemStart[m_uTail] && m_ulCurrentDataSize != 0)
            {
                m_bNewErrMsg = true;
                strcpy(m_szLastError, "Cannot add more items, buffer is full (data size exceeded).");
                return false;
            }

            //If theres not enough free memory in the end of the buffer
            if((ulDataLen + m_ulItemStart[m_uMaxItemsInBuffer-1]+m_ulItemSize[m_uMaxItemsInBuffer-1]) > m_ulBufferSize)
            {
/*
                //If there is not enough memory between the beginning of the buffer and the tail
                if(ulDataLen > m_ulItemStart[m_uTail])
                {

                    m_bNewErrMsg = true;
                    strcpy(m_szLastError, "Cannot add more items, buffer is full (data size exceeded).");
                    return false;
                }
                */
                m_ulItemStart[m_uHead] = 0;
            }

            //Else keep filling the buffer normally
            else
            {
                m_ulItemStart[m_uHead] = m_ulItemStart[m_uMaxItemsInBuffer-1]+m_ulItemSize[m_uMaxItemsInBuffer-1];
            }

        }
    }
    //Store the items size and location in buffer
    m_ulItemSize[m_uHead] = ulDataLen;
    //Copy the data into the buffer
    memcpy(&m_pucBufferMmemory[m_ulItemStart[m_uHead]],data,ulDataLen);



    //Update the buffer stats
    m_uItemsInBuffer++;
    m_uHead++;
    m_ulCurrentDataSize += ulDataLen;



    return true;
}

unsigned long FIFOBuffer::GetDataLength()
{
    return m_ulItemSize[m_uHead];
}

long unsigned  FIFOBuffer::GetData(void *data, unsigned long ulDataLen)
{


    if(m_uItemsInBuffer == 0)
    {
        m_bNewErrMsg = true;
        strcpy(m_szLastError, "No items in buffer.");
        return false;
    }

    /*
    //Some checks that we don't exceed set limits
    if(m_uTail == m_uHead)
    {
        m_bNewErrMsg = true;
        strcpy(m_szLastError, "Cannot add more items, buffer overflow");
        return false;
    }*/


    if(m_ulItemSize[m_uTail] > ulDataLen)
    {
        m_bNewErrMsg = true;
        strcpy(m_szLastError, "Data size too big for the data buffer.");
        return false;
    }

    memcpy(data,
           &m_pucBufferMmemory[m_ulItemStart[m_uTail]],
            m_ulItemSize[m_uTail]);

    unsigned long int ulSize = m_ulItemSize[m_uTail];

    m_ulCurrentDataSize -= ulSize;
    m_uTail++;
    m_uItemsInBuffer--;

    if(m_uTail >= m_uMaxItemsInBuffer)
        m_uTail = 0;

    return ulSize;

}

unsigned long FIFOBuffer::GetItemCount()
{
    return m_uItemsInBuffer;
}

unsigned long FIFOBuffer::GetTotalDataLength()
{
    return m_ulCurrentDataSize;
}

void FIFOBuffer::Clear()
{
    for(unsigned long i = 0; i < m_uMaxItemsInBuffer; i++)
    {
        m_ulItemSize[i] = 0;
        m_ulItemStart[i] = 0;

    }
    m_uHead = 0;
    m_uTail = 0;
    m_uItemsInBuffer = 0;
    m_ulCurrentDataSize = 0;
}
/**
  @brief Gets the last error message
  @param
  @return a pointer to the NULL terminated error string, NULL if no error
*/
char * FIFOBuffer::GetLastError()
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
