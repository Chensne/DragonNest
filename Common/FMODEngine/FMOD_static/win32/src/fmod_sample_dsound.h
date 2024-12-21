#ifndef _FMOD_SAMPLE_DSOUND_H
#define _FMOD_SAMPLE_DSOUND_H

#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_DSOUND

#include "fmod_memory.h"
#include "fmod_soundi.h"
#include "fmod_sound_sample.h"

#ifndef _FMOD_MEMORYTRACKER_H
#include "fmod_memorytracker.h"
#endif

struct IDirectSoundBuffer8;
struct IDirectSound3DBuffer;
struct OutputDSound;

namespace FMOD
{
    class SampleDSound : public Sample
    {
        DECLARE_MEMORYTRACKER

        friend class ChannelDSound;
        friend class OutputDSound;

        #ifdef FMOD_SUPPORT_OPENAL
        friend class OutputOpenAL;
        #endif

      private:

        IDirectSoundBuffer8  *mBuffer;
        IDirectSound3DBuffer *mBuffer3D;
        OutputDSound         *mOutput;
        bool                  mLOCSoftware;

      public:

        SampleDSound();

        FMOD_RESULT release(bool freethis = true);
        FMOD_RESULT lockInternal(unsigned int offset, unsigned int length, void **ptr1, void **ptr2, unsigned int *len1, unsigned int *len2);
        FMOD_RESULT unlockInternal(void *ptr1, void *ptr2, unsigned int len1, unsigned int len2);
    };
}

#endif /* FMOD_SUPPORT_DSOUND */

#endif