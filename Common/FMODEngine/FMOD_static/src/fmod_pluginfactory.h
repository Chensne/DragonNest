#ifndef _FMOD_PLUGINFACTORY_H
#define _FMOD_PLUGINFACTORY_H

#include "fmod_settings.h"

#include "fmod.h"
#include "fmod_codeci.h"
#include "fmod_dspi.h"
#include "fmod_outputi.h"
#include "fmod_string.h"

#ifndef _FMOD_MEMORYTRACKER_H
#include "fmod_memorytracker.h"
#endif

namespace FMOD
{
	class DSPI;
    class SystemI;

    class PluginFactory
    {
        DECLARE_MEMORYTRACKER_NONVIRTUAL

      private:

        char                        mPluginPath[FMOD_STRING_MAXPATHLEN];
        FMOD_DSP_DESCRIPTION_EX     mDSPHead;
        FMOD_CODEC_DESCRIPTION_EX   mCodecHead;
        FMOD_OUTPUT_DESCRIPTION_EX  mOutputHead;
        SystemI                    *mSystem;
        unsigned int                mCurrentPluginHandle;
        
      public:
        
        PluginFactory();

        FMOD_RESULT release         ();

        FMOD_RESULT setSystem       (SystemI *system);
        FMOD_RESULT getSystem       (SystemI **system);

        FMOD_RESULT setPluginPath   (const char *path);        
#ifdef FMOD_SUPPORT_DLLS
        FMOD_RESULT tryLoadPlugin   (const char *dllname, unsigned int *handle = 0, bool calledinternally = true, unsigned int priority = 0);
        FMOD_RESULT loadPlugin      (const char *dllname, unsigned int *handle = 0, bool calledinternally = true, unsigned int priority = 0);
#endif
        FMOD_RESULT unloadPlugin    (unsigned int handle);

        FMOD_RESULT registerCodec   (FMOD_CODEC_DESCRIPTION     *description, unsigned int *handle = 0, unsigned int priority = 0);
        FMOD_RESULT registerCodec   (FMOD_CODEC_DESCRIPTION_EX  *description, unsigned int *handle = 0, unsigned int priority = 0);
        FMOD_RESULT registerDSP     (FMOD_DSP_DESCRIPTION       *description, unsigned int *handle = 0);
        FMOD_RESULT registerDSP     (FMOD_DSP_DESCRIPTION_EX    *description, unsigned int *handle = 0);
        FMOD_RESULT registerOutput  (FMOD_OUTPUT_DESCRIPTION    *description, unsigned int *handle = 0);
        FMOD_RESULT registerOutput  (FMOD_OUTPUT_DESCRIPTION_EX *description, unsigned int *handle = 0);

        FMOD_RESULT getNumCodecs    (int *numcodecs);
        FMOD_RESULT getNumDSPs      (int *numdsps);
        FMOD_RESULT getNumOutputs   (int *numoutputs);
        FMOD_RESULT getCodecHandle  (int index, unsigned int *handle);
        FMOD_RESULT getDSPHandle    (int index, unsigned int *handle);
        FMOD_RESULT getOutputHandle (int index, unsigned int *handle);
        FMOD_RESULT getCodec        (unsigned int handle, FMOD_CODEC_DESCRIPTION_EX **codecdesc);
        FMOD_RESULT getDSP          (unsigned int handle, FMOD_DSP_DESCRIPTION_EX **dspdesc);
        FMOD_RESULT getOutput       (unsigned int handle, FMOD_OUTPUT_DESCRIPTION_EX **outputdesc);

        FMOD_RESULT createCodec     (FMOD_CODEC_DESCRIPTION_EX *codecdesc, Codec **codec);
        FMOD_RESULT createDSP       (FMOD_DSP_DESCRIPTION_EX *dspdesc, DSPI **dsp);
        FMOD_RESULT createOutput    (FMOD_OUTPUT_DESCRIPTION_EX *outputdesc, Output **dsp);
    };
}

#endif

