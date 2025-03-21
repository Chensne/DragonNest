#ifndef _FMOD_DSP_DELAY_H
#define _FMOD_DSP_DELAY_H

#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_DELAY

#include "fmod.h"
#include "fmod_dsp_filter.h"

#if !defined(PLATFORM_PS3) && !defined(PLATFORM_WINDOWS_PS3MODE)
#define DELAY_INTERLEAVED
#define DELAY_USEFLOAT
#endif
#define DELAY_MAX_CHANNELS 16

namespace FMOD
{   
    #if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)
    class DSPDelay : public DSPI
    #else
    class DSPDelay : public DSPFilter
    #endif
    {
        DECLARE_MEMORYTRACKER_NONVIRTUAL

      private:

        float           FMOD_PPCALIGN16(mMaxDelay);
        float           FMOD_PPCALIGN16(mMaxDelayUpdate);
        float           FMOD_PPCALIGN16(mDelay[DELAY_MAX_CHANNELS]);
        float           FMOD_PPCALIGN16(mDelayUpdate[DELAY_MAX_CHANNELS]);
        int             FMOD_PPCALIGN16(mOffset[DELAY_MAX_CHANNELS]);

#ifdef DELAY_USEFLOAT
        float          *FMOD_PPCALIGN16(mDelayBuffer);
        float          *FMOD_PPCALIGN16(mDelayBufferMemory);
#else
        signed short   *FMOD_PPCALIGN16(mDelayBuffer);
        signed short   *FMOD_PPCALIGN16(mDelayBufferMemory);
#endif
        unsigned int    FMOD_PPCALIGN16(mDelayBufferLengthBytes);
        int             FMOD_PPCALIGN16(mDelayBufferLength);
#ifdef DELAY_INTERLEAVED
        int             FMOD_PPCALIGN16(mWritePosition);
#else
        int             FMOD_PPCALIGN16(mWritePosition[DELAY_MAX_CHANNELS]);
        int             FMOD_PPCALIGN16(mBufferStart[DELAY_MAX_CHANNELS+1]);
#endif
        int             FMOD_PPCALIGN16(mReadPosition[DELAY_MAX_CHANNELS]);
        int             FMOD_PPCALIGN16(mOutputRate);
        int             FMOD_PPCALIGN16(mChannels);

#ifdef PLATFORM_PS3
        short           FMOD_PPCALIGN16(mTempWriteMem[DELAY_MAX_CHANNELS][256]);    // 8192 bytes
        short           FMOD_PPCALIGN16(mTempReadMem [DELAY_MAX_CHANNELS][256]);    // 8192 bytes
#endif

        unsigned short	mOldSpeakerMask;

        FMOD_RESULT     createInternal();
        FMOD_RESULT     releaseInternal();
        FMOD_RESULT     resetInternal();
        FMOD_RESULT     readInternal(float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int outchannels);
        FMOD_RESULT     setParameterInternal(int index, float value);
        FMOD_RESULT     getParameterInternal(int index, float *value, char *valuestr);

      public:

        static FMOD_DSP_DESCRIPTION_EX *getDescriptionEx();        

        static FMOD_RESULT F_CALLBACK createCallback(FMOD_DSP_STATE *dsp);
        static FMOD_RESULT F_CALLBACK releaseCallback(FMOD_DSP_STATE *dsp);
        static FMOD_RESULT F_CALLBACK resetCallback(FMOD_DSP_STATE *dsp);
        static FMOD_RESULT F_CALLBACK readCallback(FMOD_DSP_STATE *dsp, float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int outchannels);
        static FMOD_RESULT F_CALLBACK setParameterCallback(FMOD_DSP_STATE *dsp, int index, float value);
        static FMOD_RESULT F_CALLBACK getParameterCallback(FMOD_DSP_STATE *dsp, int index, float *value, char *valuestr);
#ifdef FMOD_SUPPORT_MEMORYTRACKER
        static FMOD_RESULT F_CALLBACK getMemoryUsedCallback(FMOD_DSP_STATE *dsp, MemoryTracker *tracker);
#endif
    };
}

#endif

#endif  

