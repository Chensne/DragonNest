#ifndef _FMOD_DOWNMIX_H
#define _FMOD_DOWNMIX_H

#include "fmod_settings.h"

#include "fmod.h"

namespace FMOD
{
    class Downmix
    {
      public:
          virtual FMOD_RESULT init                  (unsigned int dspbuffersize, unsigned int rate, FMOD_SPEAKERMODE outputspeakermode, void *extradata = 0) = 0;
          virtual FMOD_RESULT shutdown              () { return FMOD_OK; }
          virtual FMOD_RESULT encode                (float *inbuffer, float *outbuffer, unsigned int numsamples) = 0;

          FMOD_RESULT getOutputChannels     (int *outputchannels)
          {
              if (outputchannels)
              {
                  *outputchannels = mNumOutputChannels;
              }
              return FMOD_OK;
          }
          FMOD_RESULT getInputChannels     (int *inputchannels)
          {
              if (inputchannels)
              {
                  *inputchannels = mNumInputChannels;
              }
              return FMOD_OK;
          }
          FMOD_RESULT getOutputSpeakerMode  (FMOD_SPEAKERMODE *speakermode)
          {
              if (speakermode)
              {
                  *speakermode = mOutputSpeakerMode;
              }
              return FMOD_OK;
          }

      protected:
          int                           mNumInputChannels;
          int                           mNumOutputChannels;
          FMOD_SPEAKERMODE              mOutputSpeakerMode;
    };
}

#endif // _FMOD_DOWNMIX_H
