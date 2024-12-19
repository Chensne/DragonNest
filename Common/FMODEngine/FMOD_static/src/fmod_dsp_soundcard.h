#ifndef _FMOD_DSP_SOUNDCARD_H
#define _FMOD_DSP_SOUNDCARD_H

#include "fmod_settings.h"

#include "fmod_dsp_filter.h"

#ifndef _FMOD_MEMORYTRACKER_H
#include "fmod_memorytracker.h"
#endif

namespace FMOD
{
    class DSPSoundCard : public DSPFilter
    {            
        FMOD_RESULT     alloc(FMOD_DSP_DESCRIPTION_EX *description);
        FMOD_RESULT     read(void *outbuffer, unsigned int *length, FMOD_SPEAKERMODE speakermode, int speakermodechannels, unsigned int tick);
    };
}

#endif  
