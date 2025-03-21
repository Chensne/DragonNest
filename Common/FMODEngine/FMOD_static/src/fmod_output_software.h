#ifndef _FMOD_OUTPUT_SOFTWARE_H
#define _FMOD_OUTPUT_SOFTWARE_H

#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_SOFTWARE

#include "fmod_outputi.h"

#ifndef _FMOD_MEMORYTRACKER_H
#include "fmod_memorytracker.h"
#endif

namespace FMOD
{
    class ChannelSoftware;

    class OutputSoftware : public Output
    {
        DECLARE_MEMORYTRACKER_NONVIRTUAL

        friend class SystemI;
        friend class Output;

        #ifdef PLATFORM_PS3
        friend class OutputPS3;
        #endif

      private:
        
        ChannelSoftware *mChannel;

        int getSampleMaxChannels(FMOD_MODE mode, FMOD_SOUND_FORMAT format);	        
        
      public:
        
        OutputSoftware();

	    virtual FMOD_RESULT     init(int maxsoftwarechannels);
	    virtual FMOD_RESULT     release();
                FMOD_RESULT     createSample(FMOD_MODE mode, FMOD_CODEC_WAVEFORMAT *waveformat, Sample **sample);

        static int F_CALLBACK   getSampleMaxChannelsCallback (FMOD_OUTPUT_STATE *output, FMOD_MODE mode, FMOD_SOUND_FORMAT format);
    };
}

#endif

#endif
