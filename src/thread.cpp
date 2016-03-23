#include "thread.h"

Thread::Thread()
{
    m_bIsInitialized = false;
    extThreadFunc = NULL;
    extDataPtr = NULL;
    m_iThreadRunCount = 0;
    m_bContinousExecution = false;
    m_bShutdown = true;
    m_bIsRunning = false;
    m_bIsPaused = false;
    memset(&m_szLastError,0,sizeof(m_szLastError));
    m_bPause = false;
    m_bHasExited = false;
    m_bNewErrMsg = false;
    m_bPauseAutomatically = false;
}

Thread::~Thread()
{
#ifdef PTHREAD_CANCEL_ENABLE
    if(true == m_bIsRunning)
        Cancel();
#endif

}

bool Thread::SetWork(void *(*threadFunc)(void *), void * data)
{

    if(!m_mRunMutex.IsInitialized())
        if(!m_mRunMutex.Initialize())
        {
            strcpy(m_szLastError, "Failed to initialized run mutex.");
            m_bNewErrMsg = true;
            return false;
        }

    if (pthread_attr_init(&m_aAttr))
    {
        strcpy(m_szLastError, "Failed to initialized run mutex.");
        m_bNewErrMsg = true;
        return false;
    }


    extThreadFunc = threadFunc;
    extDataPtr = data;
    m_bIsInitialized = true;
    return true;
}

bool Thread::Uninitialize()
{
    if (!IsInitialized())
    {
        strcpy(m_szLastError, "Thread is not initialized.");
        m_bNewErrMsg = true;
        return false;
    }

    if (IsRunning())
    {
        strcpy(m_szLastError, "Thread is still running.");
        m_bNewErrMsg = true;
        return false;
    }

    m_bIsInitialized = false;
    extThreadFunc = NULL;
    extDataPtr = NULL;
    return true;
}

bool Thread::Run(int count, bool bPauseAutomatically)
{

    if (!IsInitialized())
    {
        strcpy(m_szLastError, "Thread is not initialized.");
        m_bNewErrMsg = true;
        return false;
    }

    if (IsRunning())
    {
        strcpy(m_szLastError, "Thread is already running.");
        m_bNewErrMsg = true;
        return false;
    }

    if (count == 0)
    {
        m_iThreadRunCount = 0;
        m_bContinousExecution = true;
    }

    else if(count >0)
    {
        m_iThreadRunCount = count;
        m_bContinousExecution = false;
    }

    else
    {
        return false;
    }

    m_bShutdown = false;
    m_bPause = false;
    m_bHasExited = false;


#ifdef PTHREAD_CANCEL_ENABLE
    if(pthread_attr_setinheritsched(&m_aAttr, PTHREAD_EXPLICIT_SCHED))
    {
        GenerateErrorMessage();
        m_bNewErrMsg = true;
        return false;
    }
#endif

    if (pthread_create(&m_tWorkerThread, &m_aAttr, startThreadFuncion, this))
    {
        strcpy(m_szLastError, "Failed to create thread.");
        m_bNewErrMsg = true;
        m_bShutdown = true;
        return false;
    }

    else
    {
        m_bPauseAutomatically = bPauseAutomatically;

        m_mRunMutex.Release();
        usleep(1000);
        return true;
    }



}

bool Thread::RunDetached(int count)
{
    return Run(count);
}

bool Thread::Resume()
{
    if (!IsInitialized())
    {
        strcpy(m_szLastError, "Thread is not initialized.");
        m_bNewErrMsg = true;
        return false;
    }

    if (!IsRunning())
    {
        strcpy(m_szLastError, "Thread is not running.");
        m_bNewErrMsg = true;
        return false;
    }
    if (!IsPaused())
    {
        strcpy(m_szLastError, "Thread is not paused.");
        m_bNewErrMsg = true;
        return false;
    }

    m_mRunMutex.Lock();
    m_mRunMutex.SignalCond(0);
    m_bPause = false;
    m_mRunMutex.Release();
    m_bIsPaused = false;
    return true;
}

bool Thread::Pause()
{
    if (!IsInitialized())
    {
        strcpy(m_szLastError, "Thread is not initialized.");
        m_bNewErrMsg = true;
        return false;
    }

    if (!IsRunning())
    {
        strcpy(m_szLastError, "Thread is not running.");
        m_bNewErrMsg = true;
        return false;
    }

    if (IsPaused())
    {
        strcpy(m_szLastError, "Thread is already paused.");
        m_bNewErrMsg = true;
        return false;
    }

    m_mRunMutex.Lock();
    m_bPause = true;
    m_mRunMutex.Release();
    return true;
}

bool Thread::Stop()
{
    if (!IsInitialized())
    {
        strcpy(m_szLastError, "Thread is not initialized.");
        m_bNewErrMsg = true;
        return false;
    }

    if (!IsRunning())
    {
        strcpy(m_szLastError, "Thread is not running.");
        m_bNewErrMsg = true;
        return false;
    }

    m_mRunMutex.Lock();
    m_bShutdown = true;
    m_mRunMutex.Release();
    return true;
}
#ifdef PTHREAD_CANCEL_ENABLE
bool Thread::Cancel()
{
    if (!IsInitialized())
    {
        strcpy(m_szLastError, "Thread is not initialized.");
        m_bNewErrMsg = true;
        return false;
    }

    if (!IsRunning())
    {
        strcpy(m_szLastError, "Thread is not running.");
        m_bNewErrMsg = true;
        return false;
    }

    if (pthread_cancel(m_tWorkerThread))
    {
        strcpy(m_szLastError, "Cancel thread failed.");
        m_bNewErrMsg = true;
        return false;
    }

    else
    {
        m_iThreadRunCount = 0;
        m_bContinousExecution = false;
        m_bShutdown = true;
        return true;
    }

}

bool Thread::SetPriority(int prio)
{
    m_sSchedPar.__sched_priority = prio;

    if(pthread_attr_setschedpolicy(&m_aAttr, SCHED_RR))
    {
        GenerateErrorMessage();
        m_bNewErrMsg = true;
        return false;
    }

    if((pthread_attr_setschedparam(&m_aAttr,&m_sSchedPar)))
    {
        GenerateErrorMessage();
        m_bNewErrMsg = true;
        return false;
    }

    return true;

}

int Thread::GetPriority()
{
    return 0;
}

bool Thread::SetAffinity(int cpus)
{


     CPU_ZERO(&m_cCpuset);
     for (int j = 0; j < MAX_NUM_OF_CPUS; j++)
         if(cpus & (1<<j))
            CPU_SET(j, &m_cCpuset);

    if (pthread_attr_setaffinity_np(&m_aAttr, sizeof(cpu_set_t), &m_cCpuset))
    {
        GenerateErrorMessage();
        m_bNewErrMsg = true;
        return false;
    }

    return true;
}

int Thread::GetAffinity()
{
   int cpuSet = 0;

   if (pthread_attr_getaffinity_np(&m_aAttr, sizeof(cpu_set_t), &m_cCpuset))
   {
       GenerateErrorMessage();
       m_bNewErrMsg = true;
       return -1;
   }

   //Construct the cpu affinity
   for(int i = 0; i<MAX_NUM_OF_CPUS;i++)
   {
       if (CPU_ISSET(i, &m_cCpuset))
           cpuSet |= 1<<i;
   }



   return cpuSet;
}
#endif
bool Thread::Join(void ** retData)
{
    if(!IsRunning() && !m_bHasExited)
    {
        strcpy(m_szLastError, "Thread not started yet.");
        m_bNewErrMsg = true;
        return false;
    }

    if (pthread_join(m_tWorkerThread,retData))
    {
        GenerateErrorMessage();
        m_bNewErrMsg = true;
        return false;
    }
    return true;
}



bool Thread::IsInitialized()
{

    return m_bIsInitialized;

}

bool Thread::IsRunning()
{
    //m_mRunMutex.Lock();
    //bool res = m_bIsRunning;
    //m_mRunMutex.Release();
    return m_bIsRunning;
}

bool Thread::IsPaused()
{
    //m_mRunMutex.Lock();
    //bool res = m_bIsPaused;
    //m_mRunMutex.Release();
    return m_bIsPaused;
}

/**
  @brief Gets the last error message
  @param
  @return a pointer to the NULL terminated error string, NULL if no error
*/
char * Thread::GetLastError()
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

void * Thread::startThreadFuncion(void *arg)
{

    return ((Thread *) arg)->loop();
}

void * Thread::loop()
{
    int loopCount = 0;

    //Run in continous mode
    if(m_bContinousExecution)
        while(1)
        {
            m_mRunMutex.Lock();
            m_bIsRunning = true;
            //If not asked to quit
            if(!m_bShutdown)
            {
                //If not asked to pause
                if (!m_bPause)
                {

                    m_bIsPaused = false;
                    m_mRunMutex.Release();
                    //Execute the work function
                    if(NULL != extThreadFunc)
                        extThreadFunc(extDataPtr);
                    //Pause to wait for the resume command if the auto pause is set
                    if(true == m_bPauseAutomatically)
                    {
                        m_bIsPaused = true;
                        m_mRunMutex.WaitForCond(0);
                        m_mRunMutex.Release();
                    }

                }

                //If asked to pause
                else
                {
                    m_bIsPaused = true;
                    m_mRunMutex.WaitForCond(0);
                    m_mRunMutex.Release();
                }
            }

            //If asked to quit
            else
            {
                m_bIsRunning = false;
                m_bHasExited = true;
                m_mRunMutex.Release();
                pthread_exit(NULL);
            }

        }
    //Run n times
    else if (m_iThreadRunCount > 0)
    {

        while(1)
        {
            m_mRunMutex.Lock();
            m_bIsRunning = true;
            if(!m_bShutdown && loopCount < m_iThreadRunCount)
            {
                if (!m_bPause)
                {
                    m_bIsPaused = false;
                    m_mRunMutex.Release();
                    extThreadFunc(extDataPtr);
                    loopCount++;
                }

                else
                {
                    m_bIsPaused = true;
                    m_mRunMutex.WaitForCond(0);
                    m_mRunMutex.Release();

                }
            }

            else
            {
                m_bIsRunning = false;
                m_bHasExited = true;
                m_mRunMutex.Release();
                pthread_exit(NULL);
            }
        }
    }

    else
    {
        m_mRunMutex.Lock();
        m_bIsRunning = true;
        m_mRunMutex.Release();
        extThreadFunc(extDataPtr);
        m_mRunMutex.Lock();
        m_bIsRunning = false;
        m_bHasExited = true;
        m_mRunMutex.Release();
        pthread_exit(NULL);
    }


}

/**
  @brief Generates a human readable error message from the current errno
*/
void Thread::GenerateErrorMessage()
{
    switch (errno) {
    case EPERM:
    {
        strcpy(m_szLastError, "No permissions.");
        break;
    }
    case EDEADLK:
    {
        strcpy(m_szLastError, "Deadlock detected.");
        break;
    }
    case EINVAL:
    {
        strcpy(m_szLastError, "Thread is not joinable type or another thread is already waiting for join.");
        break;
    }

    default:
        strcpy(m_szLastError, "Unknown error.");
        break;

    }
}
