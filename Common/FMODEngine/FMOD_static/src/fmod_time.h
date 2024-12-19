#ifndef _FMOD_TIME_H
#define _FMOD_TIME_H

#include "fmod_settings.h"

#include "fmod.h"
#include "fmod_types.h"

namespace FMOD
{
    class TimeStamp
    {
      public:

	    unsigned int    mIn, mOut;
	    unsigned int    mPausedIn, mPausedOut;
	    unsigned int    mTotalIn, mTotalOut;
	    FMOD_UFLOAT     mPercent, mAvTotal, mAvCurrent, mCPUUsage;
        bool            mPaused;
        unsigned int    mPausedTotal;
        int             mPausedRefCount;
        bool            mTiming;
       
        TimeStamp();

        FMOD_RESULT     stampIn();
        FMOD_RESULT     stampOut(int damppercentage);
        FMOD_RESULT     getCPUUsage(FMOD_UFLOAT *cpuusage);
        FMOD_RESULT     setPaused(bool paused);
    };
}

#endif

