#ifndef _FMOD_DOWNMIX_MYEARS_H
#define _FMOD_DOWNMIX_MYEARS_H

#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_MYEARS

#include "fmod_downmix.h"

#include <stdio.h>

#define MYEARS_FILTER_ORDER     14
#define MYEARS_LFE_ORDER        4
#define MYEARS_SPEAKER_COUNT    8
#define MYEARS_SAMPLE_RATE      48000


namespace FMOD
{
    class DownmixMyEars : public Downmix
    {
    public:
        FMOD_RESULT init                  (unsigned int dspbuffersize, unsigned int rate, FMOD_SPEAKERMODE outputspeakermode, void *extradata = 0);
        FMOD_RESULT shutdown              ();
        FMOD_RESULT encode                (float *inbuffer, float *outbuffer, unsigned int numsamples);

        static bool dataExists            ();
        static int  getSampleRate         ();

        typedef struct MyEarsFilter
        {
            float   a[MYEARS_FILTER_ORDER + 1];
            float   b[MYEARS_FILTER_ORDER + 1];
            float   state[MYEARS_FILTER_ORDER];
            int     delay;
            float  *buffer;
            float  *bufferend;
            float  *bufferpos;
        };

        typedef struct LFEFilter
        {
            float   a[MYEARS_LFE_ORDER + 1];
            float   b[MYEARS_LFE_ORDER + 1];
            float   state[MYEARS_LFE_ORDER];
        };

        typedef struct MyEarsExtraData
        {
            // data for Downmix::init()
        };

    private:
        static const int    MYEARS_EXTERNALISE_DELAY;
        static const float  MYEARS_EXTERNALISE_GAIN;
        static const float  MYEARS_LFE_CUTOFF;
        static const float  MYEARS_LFE_GAIN;

        bool          mUseExtFilters;
        float         mLFEGain;
        float         mLFECutoff;

        MyEarsFilter *mLeftFilter[MYEARS_SPEAKER_COUNT];
        MyEarsFilter *mRightFilter[MYEARS_SPEAKER_COUNT];
        MyEarsFilter *mLeftFilterExt[MYEARS_SPEAKER_COUNT];
        MyEarsFilter *mRightFilterExt[MYEARS_SPEAKER_COUNT];
        LFEFilter     mLFEFilter;

        static bool     log(char* str, ...);

        FMOD_RESULT     loadFile(const char *filename);
        FMOD_RESULT     loadFilter(FILE *file, MyEarsFilter **filter, bool externaliser = false);
        void            unloadFilter(MyEarsFilter *filter);
        void            initFilter(MyEarsFilter *filter);
        void            initLFEFilter();
        __forceinline float applyFilter(float sample, MyEarsFilter *filter);
        __forceinline float applyLFEFilter(float sample);
    };
}

#endif

#endif // _FMOD_DOWNMIX_MYEARS_H
