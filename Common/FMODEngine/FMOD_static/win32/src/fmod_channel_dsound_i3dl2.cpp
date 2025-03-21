#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_I3DL2

#include "fmod_3dl2.h"
#include "fmod_channel_dsound_i3dl2.h"
#include "fmod_output_dsound.h"


namespace FMOD
{

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelDSoundI3DL2::setReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES *prop)
{
    HRESULT                hr;
    I3DL2_BUFFERPROPERTIES dsprops;
    GUID                   guid;

    if (!mOutputDSound)
    {
        return FMOD_ERR_UNINITIALIZED;
    }

    if (!prop)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mBuffer3D)
    {
        return FMOD_ERR_UNSUPPORTED;
    }

	/*
		OBTAIN REVERB INTERFACE
	*/
	if (!mBufferReverb)
	{
  		if (mFlags & CHANNELREAL_FLAG_PAUSED && mOutputDSound->mDirectXVersion >= 8)
  		{
            mBuffer->AcquireResources(DSBPLAY_LOCHARDWARE, 0, NULL);
        }

        FMOD_memcpy(&guid, &FMOD_IID_IKsPropertySet, sizeof(GUID));

        hr = mBuffer3D->QueryInterface(guid, (void **)&mBufferReverb);
	    if (hr != DS_OK)
	    {
		    return FMOD_ERR_UNSUPPORTED;
	    }
	}

	/*
	    NOW SET CHANNEL PROPERTIES
	*/
    dsprops.lDirect                = prop->Direct;             
    dsprops.lDirectHF              = prop->DirectHF;           
    dsprops.lRoom                  = prop->Room;               
    dsprops.lRoomHF                = prop->RoomHF;             
    dsprops.flRoomRolloffFactor    = prop->RoomRolloffFactor;
    dsprops.Obstruction.flLFRatio  = prop->ObstructionLFRatio;
    dsprops.Obstruction.lHFLevel   = prop->Obstruction;
    dsprops.Occlusion.flLFRatio    = prop->OcclusionLFRatio;
    dsprops.Occlusion.lHFLevel     = prop->Occlusion;

    FMOD_memcpy(&guid, &FMOD_DSPROPSETID_I3DL2_BufferProperties, sizeof(GUID));

    hr = mBufferReverb->Set(guid, DSPROPERTY_I3DL2BUFFER_ALL, NULL, 0, &dsprops, sizeof(I3DL2_BUFFERPROPERTIES));
    if (FAILED(hr))
    {
	    return FMOD_ERR_INVALID_PARAM;
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelDSoundI3DL2::getReverbProperties(FMOD_REVERB_CHANNELPROPERTIES *prop)
{
    HRESULT                hr;
    I3DL2_BUFFERPROPERTIES dsprops;
    GUID                   guid;
    ULONG                  ulReceived;

    if (!mOutputDSound)
    {
        return FMOD_ERR_UNINITIALIZED;
    }

    if (!prop)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mBuffer3D)
    {
        return FMOD_ERR_UNSUPPORTED;
    }

	/*
		OBTAIN REVERB INTERFACE
	*/
	if (!mBufferReverb)
	{
  		if (mFlags & CHANNELREAL_FLAG_PAUSED && mOutputDSound->mDirectXVersion >= 8)
  		{
            mBuffer->AcquireResources(DSBPLAY_LOCHARDWARE, 0, NULL);
        }

        FMOD_memcpy(&guid, &FMOD_IID_IKsPropertySet, sizeof(GUID));

        hr = mBuffer3D->QueryInterface(guid, (void **)&mBufferReverb);
	    if (hr != DS_OK)
	    {
		    return FMOD_ERR_UNSUPPORTED;
	    }
	}

    /*
        Get reverb properties
    */
    FMOD_memcpy(&guid, &FMOD_DSPROPSETID_I3DL2_BufferProperties, sizeof(GUID));

    hr = mBufferReverb->Get(guid, DSPROPERTY_I3DL2BUFFER_ALL, NULL, 0, &dsprops, sizeof(I3DL2_BUFFERPROPERTIES), &ulReceived);
    if (FAILED(hr))
    {
        return FMOD_ERR_UNSUPPORTED;
    }

    prop->Direct                = dsprops.lDirect;         
    prop->DirectHF              = dsprops.lDirectHF;
    prop->Room                  = dsprops.lRoom;
    prop->RoomHF                = dsprops.lRoomHF;
    prop->Obstruction           = dsprops.Obstruction.lHFLevel;
    prop->ObstructionLFRatio    = dsprops.Obstruction.flLFRatio;
    prop->Occlusion             = dsprops.Occlusion.lHFLevel;
    prop->OcclusionLFRatio      = dsprops.Occlusion.flLFRatio;
    prop->OcclusionRoomRatio    = 0;
    prop->OcclusionDirectRatio  = 0;
    prop->Exclusion             = 0; 
    prop->ExclusionLFRatio      = 0;
    prop->OutsideVolumeHF       = 0;
    prop->DopplerFactor         = 0;
    prop->RolloffFactor         = 0;
    prop->RoomRolloffFactor     = dsprops.flRoomRolloffFactor;
    prop->AirAbsorptionFactor   = 0;
    prop->Flags                 = 0;

    return FMOD_OK;
}

}

#endif
