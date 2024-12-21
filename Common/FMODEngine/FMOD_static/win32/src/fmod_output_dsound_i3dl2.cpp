#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_I3DL2

#include "fmod.h"
#include "fmod_3dl2.h"
#include "fmod_localcriticalsection.h"
#include "fmod_output_dsound.h"
#include "fmod_sample_dsound.h"
#include "fmod_systemi.h"

namespace FMOD
{


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
bool OutputDSound::querySupportI3DL2(unsigned int ulQuery)
{
	unsigned long ulSupport = 0;
    HRESULT       hr;
	GUID          guid;

    FLOG((FMOD_DEBUG_LEVEL_ALL, __FILE__, __LINE__, "OutputDSound::querySupportI3DL2", "check properties %08X\n", ulQuery));

    FMOD_memcpy(&guid, &FMOD_DSPROPSETID_I3DL2_ListenerProperties, sizeof(GUID));

    hr = mBufferReverb->QuerySupport(guid, ulQuery, &ulSupport);
	if ( FAILED(hr) ) 
    {
		return false;
    }

	if ( (ulSupport&(KSPROPERTY_SUPPORT_GET|KSPROPERTY_SUPPORT_SET)) == 
                    (KSPROPERTY_SUPPORT_GET|KSPROPERTY_SUPPORT_SET) )
	{
        mFeaturesReverb |= (DWORD)(1 << ulQuery);

		FLOG((FMOD_DEBUG_LEVEL_ALL, __FILE__, __LINE__, "OutputDSound::querySupportI3DL2", "success\n"));

		return true;
	}

	return false;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
bool OutputDSound::initI3DL2()
{
    FMOD_RESULT             result;
	ULONG					support = 0;
	HRESULT					hr = DS_OK;
    GUID                    guid;
    FMOD_CODEC_WAVEFORMAT   waveformat;

	if (mBufferReverb)
	{
		mBufferReverb->Release();
		mBufferReverb = 0;
	}

    /*
        Create I3DL2 sample
    */

    FMOD_memset(&waveformat, 0, sizeof(FMOD_CODEC_WAVEFORMAT));
    waveformat.frequency = 44100;
    waveformat.lengthpcm = 1024;
    waveformat.channels  = 1;
    waveformat.format    = FMOD_SOUND_FORMAT_PCM16;

    result = createSample(FMOD_3D | FMOD_UNIQUE, &waveformat, (Sample **)&mSampleReverb);
    if (result != FMOD_OK)
    {
        return false;
    }

    /*
	    OBTAIN I3DL2 INTERFACE TO BUFFER 
	*/
    FMOD_memcpy(&guid, &FMOD_IID_IKsPropertySet, sizeof(GUID));

    if(mSampleReverb->mBuffer3D->QueryInterface(guid, (void **)&mBufferReverb) != DS_OK)
	{
		mBufferReverb = 0;		
		return false;
	}

	/*
	    QUERY FOR I3DL2 support
	*/
    support = querySupportI3DL2(DSPROPERTY_I3DL2LISTENER_ALL);
    if (support) support = querySupportI3DL2(DSPROPERTY_I3DL2LISTENER_ROOM);
    if (support) support = querySupportI3DL2(DSPROPERTY_I3DL2LISTENER_ROOMHF);
    if (support) support = querySupportI3DL2(DSPROPERTY_I3DL2LISTENER_ROOMROLLOFFFACTOR);
    if (support) support = querySupportI3DL2(DSPROPERTY_I3DL2LISTENER_DECAYTIME);
    if (support) support = querySupportI3DL2(DSPROPERTY_I3DL2LISTENER_DECAYHFRATIO);
    if (support) support = querySupportI3DL2(DSPROPERTY_I3DL2LISTENER_REFLECTIONS);
    if (support) support = querySupportI3DL2(DSPROPERTY_I3DL2LISTENER_REFLECTIONSDELAY);
    if (support) support = querySupportI3DL2(DSPROPERTY_I3DL2LISTENER_REVERB);
    if (support) support = querySupportI3DL2(DSPROPERTY_I3DL2LISTENER_REVERBDELAY);
    if (support) support = querySupportI3DL2(DSPROPERTY_I3DL2LISTENER_DIFFUSION);
    if (support) support = querySupportI3DL2(DSPROPERTY_I3DL2LISTENER_DENSITY);
    if (support) support = querySupportI3DL2(DSPROPERTY_I3DL2LISTENER_HFREFERENCE);

    if (support) 
	{
		FLOG((FMOD_DEBUG_LEVEL_ALL, __FILE__, __LINE__, "OutputDSound::initI3DL2", "I3DL2 - success\n"));
	}
    else
    {
		FLOG((FMOD_DEBUG_LEVEL_ALL, __FILE__, __LINE__, "OutputDSound::initI3DL2", "I3DL2 - FAIL\n"));
    }

	if (!mFeaturesReverb)
	{
		FLOG((FMOD_DEBUG_LEVEL_ALL, __FILE__, __LINE__, "OutputDSound::initI3DL2", "failed I3DL2 check, releasing reverb interface\n"));
		return false;
	}

	/*
        Turn down reverb to start with.
    */
    if ((mFeaturesReverb & (DWORD)1 << DSPROPERTY_I3DL2LISTENER_ROOM))
    {
        DWORD Room = I3DL2LISTENER_MINROOM;

        FMOD_memcpy(&guid, &FMOD_DSPROPSETID_I3DL2_ListenerProperties, sizeof(GUID));

        hr = mBufferReverb->Set(guid, DSPROPERTY_I3DL2LISTENER_ROOM, 0, 0, &Room, sizeof(DWORD));
    }

	return true;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputDSound::closeI3DL2()
{
	if (mBufferReverb) 
	{
        /*
            Turn down reverb again
        */
        if ((mFeaturesReverb & (DWORD)1 << DSPROPERTY_I3DL2LISTENER_ROOM))
        {
            GUID    guid;
            DWORD   Room = I3DL2LISTENER_MINROOM;

            FMOD_memcpy(&guid, &FMOD_DSPROPSETID_I3DL2_ListenerProperties, sizeof(GUID));

	        mBufferReverb->Set(guid, DSPROPERTY_I3DL2LISTENER_ROOM, 0, 0, &Room, sizeof(DWORD));
        }

		mBufferReverb->Release();
		mBufferReverb = 0;
	}

    if (mSampleReverb)
    {
	    mSampleReverb->release();
        mSampleReverb = 0;
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputDSound::setPropertiesI3DL2(const FMOD_REVERB_PROPERTIES *prop)
{
    if (!prop)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

	FLOG((FMOD_DEBUG_LEVEL_ALL, __FILE__, __LINE__, "OutputDSound::setPropertiesI3DL2", "Entering \n"));

	if ( !(mFeaturesReverb & (DWORD)1 << DSPROPERTY_I3DL2LISTENER_ALL) ) 
	{
        FLOG((FMOD_DEBUG_LEVEL_ALL, __FILE__, __LINE__, "OutputDSound::setPropertiesI3DL2", "failed - apparently DSPROPERTY_I3DL2LISTENER_ALL isnt supported\n"));

		return FMOD_ERR_UNSUPPORTED;
	}

	if (mFeaturesReverb & (DWORD)1 << DSPROPERTY_I3DL2LISTENER_ALL)
    {
		HRESULT hr;
        I3DL2_LISTENERPROPERTIES   dsprop = {0};
        GUID    guid;

    	FLOG((FMOD_DEBUG_LEVEL_ALL, __FILE__, __LINE__, "OutputDSound::setPropertiesI3DL2", "setting listener properties\n"));
    
        dsprop.lRoom                    = prop->Room;              
        dsprop.lRoomHF                  = prop->RoomHF;
        dsprop.flRoomRolloffFactor      = prop->RoomRolloffFactor;
        dsprop.flDecayTime              = prop->DecayTime;         
        dsprop.flDecayHFRatio           = prop->DecayHFRatio;      
        dsprop.lReflections             = prop->Reflections;       
        dsprop.flReflectionsDelay       = prop->ReflectionsDelay;  
        dsprop.lReverb                  = prop->Reverb;            
        dsprop.flReverbDelay            = prop->ReverbDelay;       
        dsprop.flDiffusion              = prop->Diffusion;
        dsprop.flDensity                = prop->Density;
        dsprop.flHFReference            = prop->HFReference;

        FMOD_memcpy(&guid, &FMOD_DSPROPSETID_I3DL2_ListenerProperties, sizeof(GUID));

        hr = mBufferReverb->Set(guid, DSPROPERTY_I3DL2LISTENER_ALL, 0, 0, &dsprop, sizeof(I3DL2_LISTENERPROPERTIES));
        if (FAILED(hr))
        {
        	FLOG((FMOD_DEBUG_LEVEL_ALL, __FILE__, __LINE__, "OutputDSound::setPropertiesEAX2", "setting listener properties FAILED\n"));

            return FMOD_ERR_INVALID_PARAM;
        }
    }

	return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
FMOD_RESULT	OutputDSound::getPropertiesI3DL2(FMOD_REVERB_PROPERTIES *prop)
{
	ULONG   ulReceived;

    if (!prop)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

	if ( !(mFeaturesReverb & (DWORD)1 << DSPROPERTY_I3DL2LISTENER_ALL) ) 
	{
		return FMOD_ERR_UNSUPPORTED;
	}

	if ((mFeaturesReverb & (DWORD)1 << DSPROPERTY_I3DL2LISTENER_ALL))
    {
        GUID    guid;
    	HRESULT hr;
        I3DL2_LISTENERPROPERTIES   dsprop = {0};

        FMOD_memcpy(&guid, &FMOD_DSPROPSETID_I3DL2_ListenerProperties, sizeof(GUID));

		hr = mBufferReverb->Get(guid, DSPROPERTY_I3DL2LISTENER_ALL, 0, 0, &dsprop, sizeof(I3DL2_LISTENERPROPERTIES), &ulReceived);
        if (FAILED(hr))
        {
     		return FMOD_ERR_UNSUPPORTED;
        }

        prop->Environment       = 0;
        prop->EnvSize           = 0;
        prop->EnvDiffusion      = 0;
        prop->Room              = dsprop.lRoom;
        prop->RoomHF            = dsprop.lRoomHF;
        prop->RoomLF            = 0;
        prop->DecayTime         = dsprop.flDecayTime;
        prop->DecayHFRatio      = dsprop.flDecayHFRatio;
        prop->DecayLFRatio      = 0;
        prop->Reflections       = dsprop.lReflections;
        prop->ReflectionsDelay  = dsprop.flReflectionsDelay;
        prop->ReflectionsPan[0] = 0;
        prop->ReflectionsPan[1] = 0;
        prop->ReflectionsPan[2] = 0;
        prop->Reverb            = dsprop.lReverb;
        prop->ReverbDelay       = dsprop.flReverbDelay;
        prop->ReverbPan[0]      = 0;
        prop->ReverbPan[1]      = 0;
        prop->ReverbPan[2]      = 0;
        prop->EchoTime          = 0;
        prop->EchoDepth         = 0;
        prop->ModulationTime    = 0;
        prop->ModulationDepth   = 0;
        prop->AirAbsorptionHF   = 0;
        prop->HFReference       = dsprop.flHFReference;
        prop->LFReference       = 0;
        prop->RoomRolloffFactor = dsprop.flRoomRolloffFactor;
        prop->Flags             = 0;
        prop->Density           = dsprop.flDensity;
        prop->Diffusion         = dsprop.flDiffusion;
    }

	return FMOD_OK;
}

}

#endif  /* FMOD_SUPPORT_I3DL2 */