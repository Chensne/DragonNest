#ifndef _FMOD_THREAD_H
#define _FMOD_THREAD_H

#include "fmod_settings.h"

#include "fmod_string.h"
#include "fmod_os_misc.h"

#ifndef _FMOD_MEMORYTRACKER_H
#include "fmod_memorytracker.h"
#endif

namespace FMOD
{
    class SystemI;

    class Thread
    {
        DECLARE_MEMORYTRACKER_NONVIRTUAL

      private:

        char                 mName[FMOD_STRING_MAXNAMELEN];
        void                *mHandle;
        bool                 mRunning;
        void                *mUserData;
        void                *mStack;
        FMOD_OS_SEMAPHORE   *mSema;
        FMOD_OS_SEMAPHORE   *mEndSema;
        void               (*mUserCallback)(void *userdata);
        
        static THREAD_RETURNTYPE THREAD_CALLCONV callback(void *userdata);

      protected:
        int                  mPeriod;
        virtual FMOD_RESULT  threadFunc();  /* Override this instead of callback if you like */

      public:

        typedef enum
        {
            PRIORITY_VERYLOW  = -2,
            PRIORITY_LOW      = -1,
            PRIORITY_NORMAL   = 0,
            PRIORITY_HIGH     = 1,
            PRIORITY_VERYHIGH = 2,
            PRIORITY_CRITICAL = 3
        } PRIORITY;

        Thread();
        virtual ~Thread() { }

        FMOD_RESULT initThread(const char *name, void (*func)(void *userdata), void *userdata, PRIORITY priority, void *stack, int stacksize, bool usesemaphore, int sleepperiod, SystemI *system);
        FMOD_RESULT closeThread();       
        FMOD_RESULT getCurrentThreadID(FMOD_UINT_NATIVE *id);
        FMOD_RESULT wakeupThread(bool frominterrupt = false);
        FMOD_RESULT setPeriod(int period) { mPeriod = period; return FMOD_OK; }
   };
}

#endif


