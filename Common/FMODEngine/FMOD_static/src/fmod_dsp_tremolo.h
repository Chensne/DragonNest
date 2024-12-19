#ifndef _FMOD_DSP_TREMOLO_H
#define _FMOD_DSP_TREMOLO_H

#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_TREMOLO

#include "fmod.h"
#include "fmod_dsp_filter.h"

#define TREMOLO_MAX_CHANNELS		16
#define TREMOLO_MAX_CHANNEL_MAPS	8
#define TREMOLO_TABLE_SIZE			16
#define TREMOLO_TRANSITION			128

namespace FMOD
{   
    #if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)
    class DSPTremolo : public DSPI
    #else
    class DSPTremolo : public DSPFilter
    #endif
    {
        DECLARE_MEMORYTRACKER_NONVIRTUAL

      private:

        typedef struct {
            int     channelcount;
            float   phase[TREMOLO_MAX_CHANNEL_MAPS];
        } speakerPhaseMap;

        static const speakerPhaseMap phaseMap[];

        float           FMOD_PPCALIGN16(mFrequency);
        float           FMOD_PPCALIGN16(mFrequencyUpdate);
        float           FMOD_PPCALIGN16(mDepth);
        float           FMOD_PPCALIGN16(mDepthUpdate);
        float           FMOD_PPCALIGN16(mShape);
        float           FMOD_PPCALIGN16(mShapeUpdate);
        float           FMOD_PPCALIGN16(mSkew);
        float           FMOD_PPCALIGN16(mSkewUpdate);
        float           FMOD_PPCALIGN16(mDuty);
        float           FMOD_PPCALIGN16(mDutyUpdate);
        float           FMOD_PPCALIGN16(mSquare);
        float           FMOD_PPCALIGN16(mSquareUpdate);
        float           FMOD_PPCALIGN16(mPhase);
        float           FMOD_PPCALIGN16(mPhaseUpdate);
        bool            FMOD_PPCALIGN16(mUpdatePhase);
        float           FMOD_PPCALIGN16(mSpread);
        float           FMOD_PPCALIGN16(mSpreadUpdate);

        float           FMOD_PPCALIGN16(mKUp);
        float           FMOD_PPCALIGN16(mKHigh);
        float           FMOD_PPCALIGN16(mKDown);
        float           FMOD_PPCALIGN16(mKLow);

        int             FMOD_PPCALIGN16(mPosUp);
        int             FMOD_PPCALIGN16(mPosHigh);
        int             FMOD_PPCALIGN16(mPosDown);
        int             FMOD_PPCALIGN16(mPosLow);

        float           FMOD_PPCALIGN16(mPeriod);
        int             FMOD_PPCALIGN16(mPeriodSamples);

        float           FMOD_PPCALIGN16(mPeriodAdjust);
        float           FMOD_PPCALIGN16(mPeriodOffset);
        int             FMOD_PPCALIGN16(mPeriodAddSample);

        float           FMOD_PPCALIGN16(mTableUpPeriod);
        float           FMOD_PPCALIGN16(mTableDownPeriod);
        float           FMOD_PPCALIGN16(mUpRampFactor);
        float           FMOD_PPCALIGN16(mDownRampFactor);
        float           FMOD_PPCALIGN16(mTransitionRampFactor);
        float           FMOD_PPCALIGN16(mLFOMin);

        float           FMOD_PPCALIGN16(mInvPeriod);
        float           FMOD_PPCALIGN16(mInvTableSize);

        int             FMOD_PPCALIGN16(mLFOPos[TREMOLO_MAX_CHANNELS]);
        float           FMOD_PPCALIGN16(mLFOLevel[TREMOLO_MAX_CHANNELS]);
        float           FMOD_PPCALIGN16(mLFORamp[TREMOLO_MAX_CHANNELS]);
        int             FMOD_PPCALIGN16(mLFONextRamp[TREMOLO_MAX_CHANNELS]);

        float           FMOD_PPCALIGN16(mLFOTable[TREMOLO_TABLE_SIZE+1]);

        int             FMOD_PPCALIGN16(mOutputRate);
        int             FMOD_PPCALIGN16(mChannels);

        unsigned short	mOldSpeakerMask;

        FMOD_RESULT     createInternal();
        FMOD_RESULT     releaseInternal();
        FMOD_RESULT     resetInternal();
        FMOD_RESULT     readInternal(float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int outchannels);
        FMOD_RESULT     setParameterInternal(int index, float value);
        FMOD_RESULT     getParameterInternal(int index, float *value, char *valuestr);

        void createLFOTable();
        void updateWaveform();
        void updateTiming();
        void applyPhase();

        void getRampValues(int samplepos, float *p_lfolevel, float *p_lforamp, int *p_nextramppos);
        float getLFOLevel(int samplepos);
        float readLFOTable(int pos, bool forward, float *ramp);

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

