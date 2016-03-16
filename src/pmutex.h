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


#ifndef PMUTEX_H
#define PMUTEX_H
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <ctype.h>
#include <stdlib.h>
#define MUTEX_MAX_CONDS 5


class PMutex
{
public:

    //Constructor
    PMutex();
    //Destructor
    ~PMutex();
    //Initializes the mutex
    bool Initialize();
    //Locks the mutex
    bool Lock();
    //Tries to lock the mutex
    bool TryLock();
    //Releases the semaphore (increments the value)
    bool Release();
    //Waits for a condition to become true
    bool WaitForCond(int cond);
    //Tries of the for a condition is true
    bool TryWaitForCond(int cond,  int timeout_usec);
    //Sends a condition signal
    bool SignalCond(int cond);
    //Broadcasts a condition signal
    bool BroadcastCond(int cond);
    //Checks if the mutex is initialized
    bool IsInitialized();
    //Gets the last error message
    char * GetLastError();

private:
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cConditions[MUTEX_MAX_CONDS];//Mutex condition vars

    bool m_bIsInitialized;        //Is the mutex initialized
    bool m_bNewErrMsg;              //Indicates if there is a new, unread error message
    char m_szLastError[256];       //Contains the last error message
    void GenerateErrorMessage();    //Generates a human readable error message from the current errno

};

#endif // PMUTEX_H
