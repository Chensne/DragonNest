#ifndef _FMOD_DSP_WAVETABLE_H
#define _FMOD_DSP_WAVETABLE_H

#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_SOFTWARE

#include "fmod_dspi.h"
#include "fmod_dsp_resampler.h"

namespace FMOD
{
    class SoundI;

    typedef enum
    {
        DSPWAVETABLE_SPEEDDIR_FORWARDS,
        DSPWAVETABLE_SPEEDDIR_BACKWARDS,
    } DSPWAVETABLE_SPEEDDIR;

    class DSPWaveTable : public DSPI
    {                               
      public:                                                

        FMOD_UINT64P                mPosition;
        unsigned int                mNewPosition;
        FMOD_SINT64P                mSpeed;
        DSPWAVETABLE_SPEEDDIR       mDirection;
        float                       mFrequency;
        ChannelReal				   *mChannel;
        SoundI                     *mSound;
        FMOD_UINT64P                mDSPClockStart;
        FMOD_UINT64P                mDSPClockEnd;
        FMOD_UINT64P                mDSPClockPause;
        unsigned int                mDSPFinishTick;
        DSPI                       *mDSPSoundCard;

      public:                                                
                               
        FMOD_RESULT     alloc(FMOD_DSP_DESCRIPTION_EX *description);
        FMOD_RESULT     addInput(DSPI *target);
        FMOD_RESULT     read(float **outbuffer, int *outchannels, unsigned int *length, FMOD_SPEAKERMODE speakermode, int speakermodechannels, unsigned int tick);
        FMOD_RESULT     setPositionInternal(unsigned int position);
        FMOD_RESULT     setParameterInternal(int index, float value);
        FMOD_RESULT     getParameterInternal(int index, float *value, char *valuestr);
        FMOD_RESULT     resetInternal() { mNewPosition = 0xFFFFFFFF; return FMOD_OK; }

        FMOD_RESULT     setFrequency(float frequency);
        FMOD_RESULT     getFrequency(float *frequency);
        FMOD_RESULT     setFinished(bool finished, bool force = false);
        FMOD_RESULT     getFinished(bool *finished);

        static FMOD_RESULT F_CALLBACK setParameterCallback(FMOD_DSP_STATE *dsp, int index, float value);
        static FMOD_RESULT F_CALLBACK getParameterCallback(FMOD_DSP_STATE *dsp, int index, float *value, char *valuestr);
        static FMOD_RESULT F_CALLBACK setPositionCallback(FMOD_DSP_STATE *dsp, unsigned int position);
        static FMOD_RESULT F_CALLBACK resetCallback(FMOD_DSP_STATE *dsp);

    };
}

#endif  

#endif
