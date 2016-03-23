#ifndef THREAD_H
#define THREAD_H

#include <sched.h>
#include "pmutex.h"

#define MAX_NUM_OF_CPUS 8

#define CPU0    1<<0
#define CPU1    1<<1
#define CPU2    1<<2
#define CPU3    1<<3

#define CPU4    1<<4
#define CPU5    1<<5
#define CPU6    1<<6
#define CPU7    1<<7


class Thread
{
public:
    //Constructor
    Thread();
    //Destructor
    ~Thread();
    //Initializes the thread
    bool SetWork(void*(*threadFunc)(void *),void * data);
    //Uninitializes the thread
    bool Uninitialize();
    //Starts the thread and executes the thread function 'count' times
    bool Run(int count = 1, bool bPauseAutomatically = false);
    //Starts the thread as detached and executes the thread function 'count' times
    bool RunDetached(int count = 1);
    //Pauses the thread
    bool Pause();
    //Resumes the threads execution
    bool Resume();
    //Stops the thread (exits the thread function)
    bool Stop();

    //Joins the the thread (waits for it to exit)
    bool Join(void ** retData = NULL);

#ifdef PTHREAD_CANCEL_ENABLE
    //Immediately cancels the thread
    bool Cancel();
    //Sets thread priority
    bool SetPriority(int prio);
    //Gets the priority
    int GetPriority();
    //Sets affinity
    bool SetAffinity(int cpus);
    //Gets the affinity
    int GetAffinity();
#endif

    //Checks if the thread is initialized
    bool IsInitialized();
    //Checks if the thread is running
    bool IsRunning();
    //Checks if the thread is paused
    bool IsPaused();

    //Gets the last error message
    char * GetLastError();


private:

    void * loop();                    //The run loop for the thread
    PMutex m_mRunMutex;
    pthread_t m_tWorkerThread;      //worker thread
    //cpu_set_t m_cCpuset;            //Cpu set


#ifdef PTHREAD_CANCEL_ENABLE
    pthread_attr_t m_aAttr;         //Thread attributes
#endif
    struct sched_param m_sSchedPar; //Thread scheduling parameters

    int m_iThreadRunCount;          //How many times we want to execute the thread func
    bool m_bContinousExecution;     //Keep executing the function until asked to stop
    bool m_bShutdown;               //Is the shutdown requested
    bool m_bPause;                  //Pause
    bool m_bPauseAutomatically;     //Pause automatically after each function
    bool m_bHasExited;              //Has the thread exited already
    void *(*extThreadFunc)(void *);  //Ext thread function ptr
    void * extDataPtr;              //Ext data ptr for the thread function
    bool m_bIsInitialized;          //Is the thread initialized
    bool m_bIsRunning;              //Is the thread running
    bool m_bIsPaused;               //Is the thread paused (but still running)
    bool m_bNewErrMsg;              //Indicates if there is a new, unread error message
    char m_szLastError[256];       //Contains the last error message
    void GenerateErrorMessage();    //Generates a human readable error message from the current errno
    static void *startThreadFuncion(void *arg);
};

#endif // THREAD_H
