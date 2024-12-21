#ifndef _FMOD_OUTPUT_NOSOUND_NRT_H
#define _FMOD_OUTPUT_NOSOUND_NRT_H

#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_NOSOUND_NRT

#include "fmod_output_polled.h"

namespace FMOD
{
    class OutputNoSound_NRT : public Output
    {
      private:

        unsigned int      mBufferLength;
        void             *mBuffer;
      
      public:
        
        static FMOD_OUTPUT_DESCRIPTION_EX *getDescriptionEx();  

        FMOD_RESULT              enumerate();
        FMOD_RESULT              getNumDrivers(int *numdrivers);
        FMOD_RESULT              getDriverName(int driver, char *name, int namelen);
        FMOD_RESULT              getDriverCaps(int id, FMOD_CAPS *caps);
	    FMOD_RESULT              init(int selecteddriver, FMOD_INITFLAGS flags, int *outputrate, int outputchannels, FMOD_SOUND_FORMAT *outputformat, int dspbufferlength, int dspnumbuffers, void *extradriverdata);
	    FMOD_RESULT              close();
	    FMOD_RESULT              update();

        static FMOD_RESULT F_CALLBACK getNumDriversCallback(FMOD_OUTPUT_STATE *output, int *numdrivers);
        static FMOD_RESULT F_CALLBACK getDriverNameCallback(FMOD_OUTPUT_STATE *output, int id, char *name, int namelen);
        static FMOD_RESULT F_CALLBACK getDriverCapsCallback(FMOD_OUTPUT_STATE *output, int id, FMOD_CAPS *caps);
        static FMOD_RESULT F_CALLBACK initCallback         (FMOD_OUTPUT_STATE *output, int selecteddriver, FMOD_INITFLAGS flags, int *outputrate, int outputchannels, FMOD_SOUND_FORMAT *outputformat, int dspbufferlength, int dspnumbuffers, void *extradriverdata);
        static FMOD_RESULT F_CALLBACK closeCallback        (FMOD_OUTPUT_STATE *output);
        static FMOD_RESULT F_CALLBACK updateCallback       (FMOD_OUTPUT_STATE *output);
    };
}

#endif /* #ifdef FMOD_SUPPORT_NOSOUND_NRT */

#endif

