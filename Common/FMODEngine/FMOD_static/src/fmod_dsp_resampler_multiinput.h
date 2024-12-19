#ifndef _FMOD_DSP_RESAMPLER_MULTIINPUT_H
#define _FMOD_DSP_RESAMPLER_MULTIINPUT_H

#include "fmod_settings.h"

#include "fmod_dsp_filter.h"
#include "fmod_dsp_resampler.h"

namespace FMOD
{
    class DSPResamplerMultiInput : public DSPResampler
    {            
      public:

        FMOD_RESULT     read(float **outbuffer, int *outchannels, unsigned int *length, FMOD_SPEAKERMODE speakermode, int speakermodechannels, unsigned int tick);
        FMOD_RESULT     addInput(DSPI *target);
    };
}

#endif  

