#include "fmod_settings.h"

#include "fmod.h"
#include "fmod_memory.h"
#include "fmod_output.h"
#include "fmod_output_asio.h"
#include "fmod_output_wasapi.h"
#include "fmod_output_dsound.h"
#include "fmod_output_winmm.h"
#include "fmod_output_openal.h"
#include "fmod_pluginfactory.h"


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT FMOD_OS_Output_Register(FMOD::PluginFactory *pluginfactory)
{
#ifdef FMOD_USE_PLUGINS

    CHECK_RESULT(pluginfactory->tryLoadPlugin("output_dsound"));
    CHECK_RESULT(pluginfactory->tryLoadPlugin("output_winmm"));
    CHECK_RESULT(pluginfactory->tryLoadPlugin("output_wasapi"));
    CHECK_RESULT(pluginfactory->tryLoadPlugin("output_asio"));

#else

    #ifdef FMOD_SUPPORT_DSOUND
    CHECK_RESULT(pluginfactory->registerOutput(FMOD::OutputDSound::getDescriptionEx()));
    #endif
    #ifdef FMOD_SUPPORT_WINMM
    CHECK_RESULT(pluginfactory->registerOutput(FMOD::OutputWinMM::getDescriptionEx()));
    #endif
    #ifdef FMOD_SUPPORT_WASAPI
    CHECK_RESULT(pluginfactory->registerOutput(FMOD::OutputWASAPI::getDescriptionEx()));
    #endif
    #ifdef FMOD_SUPPORT_ASIO
    CHECK_RESULT(pluginfactory->registerOutput(FMOD::OutputASIO::getDescriptionEx()));
    #endif
    #ifdef FMOD_SUPPORT_OPENAL
    CHECK_RESULT(pluginfactory->registerOutput(FMOD::OutputOpenAL::getDescriptionEx()));
    #endif

#endif

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT FMOD_OS_Output_GetDefault(FMOD_OUTPUTTYPE *outputtype)
{  
    if (!outputtype)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *outputtype = FMOD_OUTPUTTYPE_DSOUND;

#ifdef FMOD_SUPPORT_WASAPI
    /*
        Check COM support for WASAPI, override default output mode if supported
    */
    {
        IMMDeviceEnumerator *enumerator = NULL;
        HRESULT              hResult    = S_OK;
        
        hResult = CoInitialize(NULL);	

        if (SUCCEEDED(CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&enumerator)))
        {
            *outputtype = FMOD_OUTPUTTYPE_WASAPI;
            enumerator->Release();
        }

        if (hResult == S_OK || hResult == S_FALSE)
        {
            CoUninitialize();
        }
    }
#endif

    return FMOD_OK;
}
