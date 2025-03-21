#ifndef _FMOD_DSP_FILTER_H
#define _FMOD_DSP_FILTER_H

#include "fmod_settings.h"

#include "fmod_dspi.h"
#include "fmod_os_misc.h"

#ifndef _FMOD_MEMORYTRACKER_H
#include "fmod_memorytracker.h"
#endif

namespace FMOD
{
    class DSPFilter : public DSPI
    {
        DECLARE_MEMORYTRACKER_NONVIRTUAL

      private:

        float          *mHistoryBuffer;
        unsigned int    mHistoryPosition;
        int             mBufferChannels;

      protected:                                            
        
        FMOD_RESULT     release(bool freethis = true);
        
#ifndef FMOD_SUPPORT_MIXER_NONRECURSIVE
        virtual FMOD_RESULT read(float **outbuffer, int *outchannels, unsigned int *length, FMOD_SPEAKERMODE speakermode, int speakermodechannels, unsigned int tick);
#endif

      public:
                  
        #if !defined(PLATFORM_PS3) && !defined(PLATFORM_WINDOWS_PS3MODE)

        FMOD_RESULT     startBuffering();
        FMOD_RESULT     getHistoryBuffer(float **buffer, unsigned int *position, unsigned int *length);
        FMOD_RESULT     stopBuffering();

        #endif
    };
}

#endif  

