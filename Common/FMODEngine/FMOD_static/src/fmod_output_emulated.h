#ifndef _FMOD_OUTPUT_EMULATED_H
#define _FMOD_OUTPUT_EMULATED_H

#include "fmod_settings.h"

#include "fmod_outputi.h"

#ifndef _FMOD_MEMORYTRACKER_H
#include "fmod_memorytracker.h"
#endif

namespace FMOD
{
    class ChannelEmulated;
    
    class OutputEmulated : public Output
    {
        DECLARE_MEMORYTRACKER_NONVIRTUAL

        friend class SystemI;

      private:

        ChannelEmulated *mChannel;

      public:

        OutputEmulated();
        
	    FMOD_RESULT     init(int maxchannels);
	    FMOD_RESULT     release();
	    FMOD_RESULT     update();
    };
}

#endif
