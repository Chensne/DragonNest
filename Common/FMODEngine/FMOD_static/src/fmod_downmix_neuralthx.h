#ifndef _FMOD_DOWNMIX_NEURAL_H
#define _FMOD_DOWNMIX_NEURAL_H

#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_NEURAL

#include "fmod.h"

#include "fmod_downmix.h"
#include "../lib/neural_thx/Neural_THX_Interface.h"

namespace FMOD
{
    class DownmixNeural : public Downmix
    {
      public:
          FMOD_RESULT init                  (unsigned int dspbuffersize, unsigned int rate, FMOD_SPEAKERMODE outputspeakermode, void *extradata = 0);
          FMOD_RESULT shutdown              ();
          FMOD_RESULT encode                (float *inbuffer, float *outbuffer, unsigned int numsamples);
          
          typedef struct NeuralExtraData
          {
              bool    usefinallimiting;
              float   lfecutoff;
          };

      private:
          NEURAL_THX_ENCODER            mNeuralEncoder;
          Neural_THX_Encoder_Settings   mNeuralSettings;
          Neural_THX_Encoder_Params     mNeuralParameters;

          unsigned int                  mNeuralMode;
    };
}

#endif

#endif // _FMOD_DOWNMIX_NEURAL_H
