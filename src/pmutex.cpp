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

#include "pmutex.h"
#include <sys/time.h>

PMutex::PMutex()
{
    memset(&m_szLastError,0,sizeof(m_szLastError));
    m_bIsInitialized = false;
     m_bNewErrMsg = false;
}

PMutex::~PMutex()
{

    if(m_bIsInitialized)
    {
        for (int i = 0; i<MUTEX_MAX_CONDS;i++)
        {
            pthread_cond_destroy(&m_cConditions[i]);
        }
        pthread_mutex_destroy(&m_mutex);
    }


}

bool PMutex::Initialize()
{
    if(pthread_mutex_init(&m_mutex,NULL))
    {
        GenerateErrorMessage();
        m_bNewErrMsg = true;
        return false;
    }

    for (int i = 0; i<MUTEX_MAX_CONDS;i++)
    {
        if(pthread_cond_init(&m_cConditions[i],NULL))
        {
            GenerateErrorMessage();
            m_bNewErrMsg = true;
            return false;
        }
    }

    m_bIsInitialized = true;
    return true;

}

bool PMutex::Lock()
{

    if(pthread_mutex_lock(&m_mutex))
    {
        GenerateErrorMessage();
        m_bNewErrMsg = true;
        return false;
    }

    return true;


}

bool PMutex::TryLock()
{

    if(pthread_mutex_trylock(&m_mutex))
    {
        GenerateErrorMessage();
        m_bNewErrMsg = true;
        return false;
    }
    return true;

}

bool PMutex::Release()
{
    if(pthread_mutex_unlock(&m_mutex))
    {
        GenerateErrorMessage();
        m_bNewErrMsg = true;
        return false;
    }
    return true;
}

bool PMutex::WaitForCond(int cond)
{  
    if(pthread_cond_wait(&m_cConditions[cond], &m_mutex))
    {
        GenerateErrorMessage();
        m_bNewErrMsg = true;
        return false;
    }
    return true;
}

bool PMutex::TryWaitForCond(int cond, int timeout_usec)
{

    if (999999 < timeout_usec)
        timeout_usec = 999999;

    struct timespec timeToWait;
    struct timeval now;
    gettimeofday(&now,NULL);

    timeToWait.tv_nsec = now.tv_usec + 1000*timeout_usec;

    if(pthread_cond_timedwait(&m_cConditions[cond], &m_mutex,&timeToWait))
    {
        GenerateErrorMessage();
        m_bNewErrMsg = true;
        return false;
    }
    return true;

}

bool PMutex::SignalCond(int cond)
{
    if(pthread_cond_signal(&m_cConditions[cond]))
    {
        GenerateErrorMessage();
        m_bNewErrMsg = true;
        return false;
    }
    return true;
}

bool PMutex::BroadcastCond(int cond)
{
    if(pthread_cond_broadcast(&m_cConditions[cond]))
    {
        GenerateErrorMessage();
        m_bNewErrMsg = true;
        return false;
    }
    return true;
}

bool PMutex::IsInitialized()
{
    return m_bIsInitialized;
}

/**
  @brief Gets the last error message
  @param
  @return a pointer to the NULL terminated error string, NULL if no error
*/
char * PMutex::GetLastError()
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
void PMutex::GenerateErrorMessage()
{
    switch (errno) {
    case EPERM:
    {
        strcpy(m_szLastError, "No permissions.");
        break;
    }
    case EBUSY:
    {
        strcpy(m_szLastError, "The mutex is in use.");
        break;
    }


    default:
        strcpy(m_szLastError, "Unknown error.");
        break;

    }
}
