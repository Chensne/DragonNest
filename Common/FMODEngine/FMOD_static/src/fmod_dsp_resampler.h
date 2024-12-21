#ifndef _FMOD_DSP_RESAMPLER_H
#define _FMOD_DSP_RESAMPLER_H

#include "fmod_settings.h"

#include "fmod_dsp_filter.h"

namespace FMOD
{
    typedef FMOD_RESULT (F_CALLBACK *FMOD_RESAMPLE_CALLBACK)(FMOD_DSP_STATE *dsp, FMOD_UINT64P numsamples, void *userdata);

    const int FMOD_DSP_RESAMPLER_OVERFLOWLENGTH = 4;    /* Enough extra samples for the pcm wraparound */

    typedef enum
    {
        FMOD_RESAMPLER_END_MIXBUFFER,
        FMOD_RESAMPLER_END_RESAMPLEBUFFER,
        FMOD_RESAMPLER_END_SOUND,
        FMOD_RESAMPLER_END_SUBSOUND
    } FMOD_RESAMPLER_END;

    typedef enum
    {
        DSPRESAMPLER_SPEEDDIR_FORWARDS,
        DSPRESAMPLER_SPEEDDIR_BACKWARDS,
    } DSPRESAMPLER_SPEEDDIR;

    struct DSPResampler_NODMA
    {
        FMOD_UINT64P    mDSPClockStart      FMOD_PACKED_INTERNAL;    //   8
        FMOD_UINT64P    mDSPClockEnd        FMOD_PACKED_INTERNAL;    //   8
        FMOD_UINT64P    mDSPClockPause      FMOD_PACKED_INTERNAL;    //   8
        FMOD_MODE       mMode               FMOD_PACKED_INTERNAL;    //   4
        unsigned int    mLoopStart          FMOD_PACKED_INTERNAL;    //   4
        unsigned int    mLoopLength         FMOD_PACKED_INTERNAL;    //   4
        unsigned int    mDSPFinishTick      FMOD_PACKED_INTERNAL;    //   4
                 int    mNewLoopCount       FMOD_PACKED_INTERNAL;    //   4
        unsigned int    mLoopCountIncrement FMOD_PACKED_INTERNAL;    //   4
        unsigned int    mNewPosition        FMOD_PACKED_INTERNAL;    //   4
        unsigned int    mSetPosIncrement    FMOD_PACKED_INTERNAL;    //   4

        #ifdef PLATFORM_PS3
        unsigned int    mPad[2]             FMOD_PACKED_INTERNAL;    //   8
        #endif
    } FMOD_PACKED;                                          // = 64 /* Keep 16 byte aligned? */

    class DSPResampler : public DSPFilter
    {
        DECLARE_MEMORYTRACKER_NONVIRTUAL

      public:

        FMOD_UINT64P                mPosition;
        FMOD_SINT64P                mSpeed;
        float                       mFrequency;
        int                         mTargetFrequency;
        FMOD_UINT64P                mResamplePosition;
        void                       *mResampleBufferMemory;
        FMOD_PPCALIGN16(void                       *mResampleBuffer);
        FMOD_PPCALIGN16(int                         mResampleBufferChannels);
        FMOD_PPCALIGN16(unsigned int                mResampleBlockLength);
        unsigned int                mResampleBufferLength;
        unsigned int                mResampleBufferPos;
        unsigned int                mResampleFinishPos;
        unsigned int                mOverflowLength;
        unsigned int                mReadPosition;
        int                         mFill;
        DSPI                       *mDSPSoundCard;
        DSPRESAMPLER_SPEEDDIR       mDirection;

        FMOD_PPCALIGN16(unsigned int                mLength);
        FMOD_PPCALIGN16(int                         mLoopCount);
        FMOD_PPCALIGN16(DSPResampler_NODMA         *mNoDMA);
        #if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)
        DSPResampler_NODMA         *mNoDMAMemory;
        #else
        DSPResampler_NODMA          mNoDMAMemory;
        #endif
        
        DSPResampler();

        virtual FMOD_RESULT     release(bool freethis = true);
        FMOD_RESULT             alloc(FMOD_DSP_DESCRIPTION_EX *description);
        FMOD_RESULT             setFrequency(float frequency);
        FMOD_RESULT             getFrequency(float *frequency);       
        FMOD_RESULT             setPosition(unsigned int position, bool processinputs);

        #if defined(FMOD_SUPPORT_MIXER_NONRECURSIVE)
        FMOD_RESULT             update(unsigned int length, int *outchannels, void **outbuffer, unsigned int tick);
        #else
        virtual FMOD_RESULT     addInput(DSPI *target);
        virtual FMOD_RESULT     read(float **outbuffer, int *outchannels, unsigned int *length, FMOD_SPEAKERMODE speakermode, int speakermodechannels, unsigned int tick);
        #endif

        FMOD_RESULT  setFinished    (bool finished, bool force = false);
        FMOD_RESULT  getFinished    (bool *finished);
    };

    #ifdef PLATFORM_PS3
    class DSPResamplerPS3 : public DSPResampler
    {
      public:
        char                    mResampleBufferMemory[((512 + (FMOD_DSP_RESAMPLER_OVERFLOWLENGTH * 4)) * sizeof(short) * 8 * 2) + 16];      // *8 = 8 channel max.  *2 = double buffer.
    };
    #endif
}

#endif  

