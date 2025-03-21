#include "fmod_settings.h"

#include "fmod_autocleanup.h"
#include "fmod_debug.h"
#include "fmod_thread.h"
#include "fmod_memory.h"
#include "fmod_os_misc.h"
#include "fmod_time.h"

namespace FMOD
{


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
THREAD_RETURNTYPE THREAD_CALLCONV Thread::callback(void *data)
{
    FMOD_UINT_NATIVE id;
    Thread *thread = (Thread *)data;
    
    FMOD_OS_Thread_GetCurrentID(&id);

    gGlobal->gSystemPool->getCurrentThreadID(id);

    thread->mRunning = true;

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "Thread::callback", "* %s started\n", thread->mName));

    while (thread->mRunning)
    {
        if (thread->mSema)
        {
            FMOD_OS_Semaphore_Wait(thread->mSema);
        }
        
        if (thread->mRunning)
        {
            if (thread->mUserCallback)
            {
                thread->mUserCallback(thread->mUserData);
            }
            else
            {
                thread->threadFunc();
            }

            if (thread->mPeriod)
            {
                FMOD_OS_Time_Sleep(thread->mPeriod);
            }
        }
    }

    gGlobal->gSystemPool->clearThreadID(id);

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "Thread::callback", "* %s finished\n", thread->mName));

    FMOD_OS_Semaphore_Signal(thread->mEndSema, false);
    
    data = 0;
    
    THREAD_RETURN
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
FMOD_RESULT Thread::threadFunc()
{
    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
Thread::Thread()
{
    mHandle       = 0;
    mRunning      = false;
    mStack        = 0;
    mUserData     = 0;
    mSema         = 0;
    mEndSema      = 0;
    mPeriod       = 0;
    mUserCallback = 0;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
FMOD_RESULT Thread::initThread(const char *name, void (*func)(void *userdata), void *userdata, PRIORITY priority, void *stack, int stacksize, bool usesemaphore, int sleepperiod, SystemI *system)
{
    FMOD_RESULT result;
    FMOD_THREAD_PRIORITY pri;
    AutoFreeSema endsema_cleanup;
    AutoFreeSema sema_cleanup;

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "Thread::initThread", "Initializing %s.  priority %d\n", name ? name : "(null)", priority));
    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "Thread::initThread", "- Stacksize %d.  Stack pointer %p : usesemaphore = %d : sleeptime = %d\n", stacksize, stack, usesemaphore ? 1 : 0, sleepperiod));

    mUserCallback = func;
    mUserData     = userdata;
    mPeriod       = sleepperiod;
    mRunning      = false;

    result = FMOD_OS_Semaphore_Create(&mEndSema);
    if (result != FMOD_OK)
    {
        return result;
    }
    endsema_cleanup = mEndSema;

    if (usesemaphore)
    {
        result = FMOD_OS_Semaphore_Create(&mSema);
        if (result != FMOD_OK)
        {
            return result;        
        }
        sema_cleanup = mSema;
    }
   
    switch(priority)
    {
        case PRIORITY_VERYLOW:
        {
            pri = FMOD_THREAD_PRIORITY_VERYLOW;
            break;
        }
        case PRIORITY_LOW:
        {
            pri = FMOD_THREAD_PRIORITY_LOW;
            break;
        }
        case PRIORITY_NORMAL:
        {
            pri = FMOD_THREAD_PRIORITY_NORMAL;
            break;
        }
        case PRIORITY_HIGH:
        {
            pri = FMOD_THREAD_PRIORITY_HIGH;
            break;
        }
        case PRIORITY_VERYHIGH:
        {
            pri = FMOD_THREAD_PRIORITY_VERYHIGH;
            break;
        }
        case PRIORITY_CRITICAL:
        {
            pri = FMOD_THREAD_PRIORITY_CRITICAL;
            break;
        }
        default:
        {
            return FMOD_ERR_INVALID_PARAM;
        }
    }
    
    if (name)
    {
        FMOD_strncpy(mName, name, FMOD_STRING_MAXNAMELEN);
    }
    else
    {
        FMOD_strcpy(mName, "?????");
    }

    #ifdef PLATFORM_PS2_EE
    if (!stack)
    {
        #define ALIGNMENT 16
        stack = mStack = FMOD_Memory_Calloc(stacksize + ALIGNMENT);
        if (!stack)
        {
            return FMOD_ERR_MEMORY;
        }
        stack = (void *)FMOD_ALIGNPOINTER(stack, ALIGNMENT);
        #undef ALIGNMENT
        
        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "Thread::initThread", "allocated a thread from the heap.  Address = %p.\n", mStack));
    }
    #endif

    result = FMOD_OS_Thread_Create(name, callback, this, pri, stack, stacksize, &mHandle);
    if (result != FMOD_OK)
    {
        return result;        
    }

    /*
        Wait for thread to actually start before continuing.
    */
    while (!mRunning)
    {
        FMOD_OS_Time_Sleep(1);
    }

    if (FMOD::gGlobal->gSystemCallback)
    {
        FMOD::gGlobal->gSystemCallback((FMOD_SYSTEM *)system, FMOD_SYSTEM_CALLBACKTYPE_THREADCREATED, mHandle, (void*)name);
    }

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "Thread::initThread", "done.\n"));
    sema_cleanup.releasePtr();
    endsema_cleanup.releasePtr();

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
FMOD_RESULT Thread::closeThread()
{
    if (mRunning)
    {
        FMOD_RESULT result;

        mRunning = false;

        if (mSema)
        {
            result = FMOD_OS_Semaphore_Signal(mSema, false);  /* make thread execute */
            if (result != FMOD_OK)
            {
                return result;
            }
        }

        result = FMOD_OS_Semaphore_Wait(mEndSema);        /* wait for thread to exit */
        if (result != FMOD_OK)
        {
            return result;
        }

        if (mSema)
        {
            result = FMOD_OS_Semaphore_Free(mSema);
            if (result != FMOD_OK)
            {
                return result;
            }
            mSema = 0;
        }

        result = FMOD_OS_Semaphore_Free(mEndSema);
        if (result != FMOD_OK)
        {
            return result;
        }
        mEndSema = 0;

        result = FMOD_OS_Thread_Destroy(mHandle);
        if (result != FMOD_OK)
        {
            return result;
        }
        mHandle = 0;

        if (mStack)
        {
            FMOD_Memory_Free(mStack);
            mStack = 0;
        }
    
        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "Thread::close","%s thread destroyed\n", mName));
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
FMOD_RESULT Thread::getCurrentThreadID(FMOD_UINT_NATIVE *id)
{
    FMOD_OS_Thread_GetCurrentID(id);

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
FMOD_RESULT Thread::wakeupThread(bool frominterrupt)
{
    if (mSema)
    {
        return FMOD_OS_Semaphore_Signal(mSema, frominterrupt);
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/

#ifdef FMOD_SUPPORT_MEMORYTRACKER

FMOD_RESULT Thread::getMemoryUsedImpl(MemoryTracker *tracker)
{
//AJS        void                *mStack;

    if (mSema)
    {
        tracker->add(false, FMOD_MEMBITS_SYSTEM, gSizeofSemaphore);
    }

    if (mEndSema)
    {
        tracker->add(false, FMOD_MEMBITS_SYSTEM, gSizeofSemaphore);
    }

    return FMOD_OK;
}

#endif

}
