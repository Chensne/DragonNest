#include "fmod_settings.h"

#include "fmod_dsp_sfxreverb.h"
#include "fmod_pluginfactory.h"
#include "fmod_memory.h"
#include "fmod_systemi.h"
#include "fmod_reverbi.h"
#include "fmod_outputi.h"
#include "fmod_3d.h"
#include "fmod.h"


// A reverb's presence gain depends partly on the Distance Gain -
// the distance of the source/listener position to the reverb.
//
// The distance gain at (distance <= min radius) = 0 dB.
// The distance gain at (distance > min radius and < kMAXDISTANCE) = kMAXDISTANCE_GAINDB dB
// Between, the distance gain is a linear dB interpolation between these two values proportional to the
// distance.

// Presence gain is also governed by the reverb occlusion factor between
// the reverb centre and the source/listener position.


namespace FMOD
{

#define kMAXDISTANCE_GAINDB         (-60.0f)        // The gain of the reverb at max distance
#define k20TH_MAXDISTANCE_GAINDB    (-3.0f)         // 1/20th of the gain of the reverb at max distance

#define kREVERB_GAIN_COEF            0.002f         // Coefficient for warping the power law for additive gain (see below)
#define kREVERB_GAIN_COEF_INV        500.0f         // Reciprocal of kREVERB_GAIN_COEF (depends on the above)

ReverbI::ReverbI()
{
    int count;

    for (count = 0; count < FMOD_REVERB_MAXINSTANCES; count++)
    {
        mInstance[count].mDSP  = 0;
        mInstance[count].mChannelData = 0;
    }

    mSystem                 = 0;
    mPosition.x             = 0;
    mPosition.y             = 0;
    mPosition.z             = 0.0f; 
    mMinRadius              = 0.0f; 
    mMaxRadius              = 0.0f;
    mMinMaxDistance         = 0.0f;
    mGain                   = 0.0f;
    m3D                     = false;
    mDisableIfNoEnvironment = false;
    mActive                 = true;
    mUserData               = 0;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT ReverbI::validate(Reverb *reverb, ReverbI **reverbi)
{
    if (!reverbi)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!reverb)
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    *reverbi = (ReverbI *)reverb;

    return FMOD_OK;
}
/*
[
	[DESCRIPTION]
    Initialise internals

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
FMOD_RESULT ReverbI::init(SystemI* system, bool is_3d, FMOD_REVERB_MODE mode)
{
    FMOD_REVERB_PROPERTIES prop = FMOD_PRESET_OFF;             // then initialise them to 'off'
    int instance;

    release(false);                                            /* Release any resources */

    mSystem     = system;
    mMode       = mode;

    for (instance = 0; instance < FMOD_REVERB_MAXINSTANCES; instance++)
    {
        FMOD_memset(&mInstance[instance].mProps, 0xcc, sizeof(FMOD_REVERB_PROPERTIES));     /* invalidate properties */
        mInstance[instance].mProps.Instance = instance;
        setProperties(&prop);
    }


    /*
        Create the first instance's channeldata.
    */
    instance = 0;
    if (!mInstance[instance].mChannelData)
    {
        int channel;
        FMOD_REVERB_CHANNELPROPERTIES defaultprop = { 0, 0, 0, 0, 0, 0, 0, 0.25f, 1.5f, 1.0f, 0, 1.0f, 0, 0, 0, 0, 1.0f, FMOD_REVERB_CHANNELFLAGS_DEFAULT };

        defaultprop.Flags &= ~FMOD_REVERB_CHANNELFLAGS_INSTANCE0;  /* FMOD_REVERB_CHANNELFLAGS_DEFAULT has instance 0 set. */
        defaultprop.Flags |=  FMOD_REVERB_CHANNELFLAGS_INSTANCE0 << instance;

        mInstance[instance].mChannelData = (FMOD_REVERB_CHANNELDATA *)FMOD_Memory_Calloc(mSystem->mNumChannels * sizeof(FMOD_REVERB_CHANNELDATA));
        if (!mInstance[instance].mChannelData)
        {
            return FMOD_ERR_MEMORY;
        }

        for (channel=0; channel < mSystem->mNumChannels; channel++)
        {
            FMOD_memcpy(&mInstance[instance].mChannelData[channel].mChanProps, &defaultprop, sizeof(FMOD_REVERB_CHANNELPROPERTIES));
            mInstance[instance].mChannelData[channel].mDSPConnection = 0;
            mInstance[instance].mChannelData[channel].mPresenceGain = 0;
        }
    }

    mPosition.x = mPosition.y = mPosition.z = 0.0f; 
    mMinRadius      = 0.0f; 
    mMaxRadius      = 0.0f;
    mMinMaxDistance = 0.0f;
    mGain           = 0.0f;
    m3D             = is_3d;

    if (this == &system->mReverbGlobal)
    {
        mDisableIfNoEnvironment = true;
    }
#ifdef FMOD_SUPPORT_MULTIREVERB
    if (this == &system->mReverb3D)
    {
        mDisableIfNoEnvironment = true;
    }
#endif

    return FMOD_OK;
}

/*
[
	[DESCRIPTION]
    Clear up internals

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
FMOD_RESULT ReverbI::release(bool freethis)
{
    int count;
   
    for (count = 0; count < FMOD_REVERB_MAXINSTANCES; count++)
    {
        if (mInstance[count].mChannelData)
        {
            FMOD_Memory_Free(mInstance[count].mChannelData);
            mInstance[count].mChannelData = 0;
        }
        releaseDSP(count);
    }

    removeNode();

#ifdef FMOD_SUPPORT_MULTIREVERB
    if (mSystem)
    {
        mSystem->update3DReverbs();

        /*
            If no more reverbs left feeding into 3D reverb, make sure it's not active on environment -1
        */
        if (mSystem && !mSystem->count3DPhysicalReverbs())
        {
            mSystem->mReverb3D.setDisableIfNoEnvironment(true);
        }
    
        /*
            If no more virtuals: turn morphing off.
        */
        if (!mSystem->count3DVirtualReverbs())
        {
            mSystem->set3DReverbActive(false);
        }
    }
#endif

    if (freethis)
    {
        FMOD_Memory_Free(this);
    }

    return FMOD_OK;
}

/*
[
	[DESCRIPTION]
    Initialise internals

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]
    Instead of just using createDSPByType we have to enumerate them all, due to plugins possibly being there or not.

    [PLATFORMS]

	[SEE_ALSO]
]
*/
FMOD_RESULT ReverbI::createDSP(int instance)
{
    if (instance < 0 || instance >= FMOD_REVERB_MAXINSTANCES)
    {
        return FMOD_ERR_REVERB_INSTANCE;
    }

    if (!mSystem)
    {
        return FMOD_ERR_INITIALIZATION;
    }

    if (!mInstance[instance].mChannelData)
    {
        int channel;

        mInstance[instance].mChannelData = (FMOD_REVERB_CHANNELDATA *)FMOD_Memory_Calloc(mSystem->mNumChannels * sizeof(FMOD_REVERB_CHANNELDATA));
        if (!mInstance[instance].mChannelData)
        {
            return FMOD_ERR_MEMORY;
        }

        for (channel=0; channel < mSystem->mNumChannels; channel++)
        {
            resetChanProperties(instance, channel);
            resetConnectionPointer(instance, channel);

        }
    }

#ifdef FMOD_SUPPORT_SOFTWARE
    if (!mInstance[instance].mDSP)
    {
        FMOD_RESULT result;
        int count, numdsps;

        result = mSystem->mPluginFactory->getNumDSPs(&numdsps);
        if (result != FMOD_OK)
        {
            return result;
        }

        for (count = 0; count < numdsps; count++)
        {
            FMOD_DSP_DESCRIPTION_EX *descriptionex = 0;
            unsigned int handle;

            result = mSystem->mPluginFactory->getDSPHandle(count, &handle);
            if (result != FMOD_OK)
            {
                continue;
            }

            result = mSystem->mPluginFactory->getDSP(handle, &descriptionex);
            if (result != FMOD_OK)
            {
                continue;
            }

            if (descriptionex->mType == FMOD_DSP_TYPE_SFXREVERB)
            {
                result = mSystem->mPluginFactory->createDSP(descriptionex, &mInstance[instance].mDSP);
                if (result != FMOD_OK)
                {
                    return result;
                }

                result = mInstance[instance].mDSP->setParameter(FMOD_DSP_SFXREVERB_DRYLEVEL, -10000.0f);
                if (result != FMOD_OK)
                {
                    return result;
                }

                return FMOD_OK;
            }
        }
        
        return FMOD_ERR_PLUGIN_MISSING;
    }

    return FMOD_OK;

#else

    return FMOD_ERR_UNSUPPORTED;

#endif
}

/*
[
	[DESCRIPTION]
    Initialise internals

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
FMOD_RESULT ReverbI::releaseDSP(int instance)
{
    if (instance < 0 || instance >= FMOD_REVERB_MAXINSTANCES)
    {
        return FMOD_ERR_REVERB_INSTANCE;
    }

    if (mInstance[instance].mDSP)
    {
        FMOD_RESULT result;

        result = mInstance[instance].mDSP->disconnectFrom(0);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mInstance[instance].mDSP->release();
        if (result != FMOD_OK)
        {
            return result;
        }
        mInstance[instance].mDSP = 0;
    }  
    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
FMOD_RESULT ReverbI::getChanProperties(int instance, const int index, FMOD_REVERB_CHANNELPROPERTIES *props, DSPConnectionI **connection)
{
    if (instance < 0 || instance >= FMOD_REVERB_MAXINSTANCES)
    {
        if (connection)
        {
            *connection = 0;
        }
        return FMOD_ERR_REVERB_INSTANCE;
    }

    if (!mInstance[instance].mChannelData)
    {
        if (connection)
        {
            *connection = 0;
        }
        return FMOD_ERR_REVERB_INSTANCE;
    }

    if ((index >= 0) && (index < mSystem->mNumChannels))
    {
        if (props)
        {
            FMOD_memcpy(props, &mInstance[instance].mChannelData[index].mChanProps, sizeof(FMOD_REVERB_CHANNELPROPERTIES));
        }

        if (connection)
        {
            *connection = mInstance[instance].mChannelData[index].mDSPConnection;
        }
    }
    else
    {
        if (connection)
        {
            *connection = 0;
        }
        return FMOD_ERR_INVALID_PARAM;
    }

    return FMOD_OK;
}

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
FMOD_RESULT ReverbI::setChanProperties(int instance, const int index, const FMOD_REVERB_CHANNELPROPERTIES *props, DSPConnectionI *connection)
{
    if (instance < 0 || instance >= FMOD_REVERB_MAXINSTANCES || !mInstance[instance].mChannelData)
    {
        return FMOD_ERR_REVERB_INSTANCE;
    }

    if ((index >= 0) && (index < mSystem->mNumChannels))
    {
        if (props)
        {
            FMOD_memcpy(&mInstance[instance].mChannelData[index].mChanProps, props, sizeof(FMOD_REVERB_CHANNELPROPERTIES));

            mInstance[instance].mChannelData[index].mChanProps.Flags &= ~(FMOD_REVERB_CHANNELFLAGS_INSTANCE0 |
                                                                          FMOD_REVERB_CHANNELFLAGS_INSTANCE1 |
                                                                          FMOD_REVERB_CHANNELFLAGS_INSTANCE2 |
                                                                          FMOD_REVERB_CHANNELFLAGS_INSTANCE3);
            mInstance[instance].mChannelData[index].mChanProps.Flags |= (FMOD_REVERB_CHANNELFLAGS_INSTANCE0 << instance);
        }

        if (connection)
        {
            mInstance[instance].mChannelData[index].mDSPConnection = connection;
        }
    }
    else
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

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
FMOD_RESULT ReverbI::resetChanProperties(int instance, int channel)
{
    FMOD_REVERB_CHANNELPROPERTIES defaultprop = { 0, 0, 0, 0, 0, 0, 0, 0.25f, 1.5f, 1.0f, 0, 1.0f, 0, 0, 0, 0, 1.0f, 0 };

    if (!mInstance[instance].mChannelData)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    defaultprop.Flags &= ~FMOD_REVERB_CHANNELFLAGS_INSTANCE0;  /* FMOD_REVERB_CHANNELFLAGS_DEFAULT has instance 0 set. */
    defaultprop.Flags |=  FMOD_REVERB_CHANNELFLAGS_INSTANCE0 << instance;

    FMOD_memcpy(&mInstance[instance].mChannelData[channel].mChanProps, &defaultprop, sizeof(FMOD_REVERB_CHANNELPROPERTIES));

    mInstance[instance].mChannelData[channel].mPresenceGain = 0;

    return resetConnectionPointer(instance, channel);
}



/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
FMOD_RESULT ReverbI::resetConnectionPointer(int instance, int channel)
{
    if (!mInstance[instance].mChannelData)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    
    mInstance[instance].mChannelData[channel].mDSPConnection = 0;
    
    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
FMOD_RESULT ReverbI::set3DAttributes(const FMOD_VECTOR *position, float mindistance, float maxdistance)
{ 
    if (position)
    {
        mPosition = *position;
    }
    
    m3D = true;

    mMinRadius = mindistance;
    if (mMinRadius > mMaxRadius)
    {
        mMaxRadius = mMinRadius;
    }

    mMaxRadius = maxdistance;
    if (mMaxRadius < mMinRadius)
    {
        mMinRadius = mMaxRadius;
    }

    mMinMaxDistance = (mMaxRadius - mMinRadius);

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
FMOD_RESULT ReverbI::get3DAttributes(FMOD_VECTOR *position, float *mindistance, float *maxdistance)
{ 
    if (position)
    {
        *position = mPosition;
    }

    if (mindistance)
    {
        *mindistance = mMinRadius;
    }

    if (maxdistance)
    {
        *maxdistance = mMaxRadius;
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/

FMOD_RESULT ReverbI::setProperties(const FMOD_REVERB_PROPERTIES* prop_source)
{
    FMOD_REVERB_PROPERTIES oldprops;
    FMOD_REVERB_PROPERTIES *newprops;
    int instance = prop_source->Instance;

    if (instance < 0 || instance >= FMOD_REVERB_MAXINSTANCES)
    {
        return FMOD_ERR_REVERB_INSTANCE;
    }

    newprops = &mInstance[instance].mProps;
    FMOD_memcpy(&oldprops, newprops, sizeof(FMOD_REVERB_PROPERTIES));

    newprops->Instance          = prop_source->Instance          < 0       ? 0       : prop_source->Instance         > 3       ? 3       : prop_source->Instance          ; /* [in]     0     , 3     , 0      , EAX4/GameCube only. Environment Instance. 3 (2 for GameCube) seperate reverbs simultaneously are possible. This specifies which one to set. (win32/gamecube) */
    newprops->Environment       = prop_source->Environment       < -1      ? -1      : prop_source->Environment      > 25      ? 25      : prop_source->Environment       ; /* [in/out] -1    , 25    , -1     , sets all listener properties.  -1 = OFF. (win32/ps2) */
    newprops->EnvSize           = prop_source->EnvSize           < 1.0f    ? 1.0f    : prop_source->EnvSize          > 100.0f  ? 100.0f  : prop_source->EnvSize           ; /* [in/out] 1.0   , 100.0 , 7.5    , environment size in meters (win32 only) */
    newprops->EnvDiffusion      = prop_source->EnvDiffusion      < 0.0f    ? 0.0f    : prop_source->EnvDiffusion     > 1.0f    ? 1.0f    : prop_source->EnvDiffusion      ; /* [in/out] 0.0   , 1.0   , 1.0    , environment diffusion (win32/Xbox/gamecube) */
    newprops->Room              = prop_source->Room              < -10000  ? -10000  : prop_source->Room             > 0       ? 0       : prop_source->Room              ; /* [in/out] -10000, 0     , -1000  , room effect level (at mid frequencies) (win32/Xbox/gamecube) */
    newprops->RoomHF            = prop_source->RoomHF            < -10000  ? -10000  : prop_source->RoomHF           > 0       ? 0       : prop_source->RoomHF            ; /* [in/out] -10000, 0     , -100   , relative room effect level at high frequencies (win32/Xbox) */
    newprops->RoomLF            = prop_source->RoomLF            < -10000  ? -10000  : prop_source->RoomLF           > 0       ? 0       : prop_source->RoomLF            ; /* [in/out] -10000, 0     , 0      , relative room effect level at low frequencies (win32 only) */
    newprops->DecayTime         = prop_source->DecayTime         < 0.1f    ? 0.1f    : prop_source->DecayTime        > 20.0f   ? 20.0f   : prop_source->DecayTime         ; /* [in/out] 0.1   , 20.0  , 1.49   , reverberation decay time at mid frequencies (win32/Xbox/gamecube) */
    newprops->DecayHFRatio      = prop_source->DecayHFRatio      < 0.1f    ? 0.1f    : prop_source->DecayHFRatio     > 2.0f    ? 2.0f    : prop_source->DecayHFRatio      ; /* [in/out] 0.1   , 2.0   , 0.83   , high-frequency to mid-frequency decay time ratio (win32/Xbox) */
    newprops->DecayLFRatio      = prop_source->DecayLFRatio      < 0.1f    ? 0.1f    : prop_source->DecayLFRatio     > 2.0f    ? 2.0f    : prop_source->DecayLFRatio      ; /* [in/out] 0.1   , 2.0   , 1.0    , low-frequency to mid-frequency decay time ratio (win32 only) */
    newprops->Reflections       = prop_source->Reflections       < -10000  ? -10000  : prop_source->Reflections      > 1000    ? 1000    : prop_source->Reflections       ; /* [in/out] -10000, 1000  , -2602  , early reflections level relative to room effect (win32/Xbox/gamecube) */
    newprops->ReflectionsDelay  = prop_source->ReflectionsDelay  < 0.0f    ? 0.0f    : prop_source->ReflectionsDelay > 0.3f    ? 0.3f    : prop_source->ReflectionsDelay  ; /* [in/out] 0.0   , 0.3   , 0.007  , initial reflection delay time (win32/Xbox) */
    newprops->ReflectionsPan[0] = prop_source->ReflectionsPan[0];                    
    newprops->ReflectionsPan[1] = prop_source->ReflectionsPan[1];                    
    newprops->ReflectionsPan[2] = prop_source->ReflectionsPan[2];                    
    newprops->Reverb            = prop_source->Reverb            < -10000  ? -10000  : prop_source->Reverb            > 2000   ? 2000    : prop_source->Reverb            ; /* [in/out] -10000, 2000  , 200    , late reverberation level relative to room effect (win32/Xbox) */
    newprops->ReverbDelay       = prop_source->ReverbDelay       < 0.0f    ? 0.0f    : prop_source->ReverbDelay       > 0.1f   ? 0.1f    : prop_source->ReverbDelay       ; /* [in/out] 0.0   , 0.1   , 0.011  , late reverberation delay time relative to initial reflection (win32/Xbox/gamecube) */
    newprops->ReverbPan[0]      = prop_source->ReverbPan[0];
    newprops->ReverbPan[1]      = prop_source->ReverbPan[1];
    newprops->ReverbPan[2]      = prop_source->ReverbPan[2];
    newprops->EchoTime          = prop_source->EchoTime          < 0.075f  ? 0.075f  : prop_source->EchoTime          > 0.25f   ? 0.25f   : prop_source->EchoTime          ; /* [in/out] .075  , 0.25  , 0.25   , echo time (win32 only) */
    newprops->EchoDepth         = prop_source->EchoDepth         < 0.0f    ? 0.0f    : prop_source->EchoDepth         > 1.0f    ? 1.0f    : prop_source->EchoDepth         ; /* [in/out] 0.0   , 1.0   , 0.0    , echo depth (win32 only) */
    newprops->ModulationTime    = prop_source->ModulationTime    < 0.04f   ? 0.04f   : prop_source->ModulationTime    > 4.0f    ? 4.0f    : prop_source->ModulationTime    ; /* [in/out] 0.04  , 4.0   , 0.25   , modulation time (win32 only) */
    newprops->ModulationDepth   = prop_source->ModulationDepth   < 0.0f    ? 0.0f    : prop_source->ModulationDepth   > 1.0f    ? 1.0f    : prop_source->ModulationDepth   ; /* [in/out] 0.0   , 1.0   , 0.0    , modulation depth (win32/gamecube) */
    newprops->AirAbsorptionHF   = prop_source->AirAbsorptionHF   < -100.0f ? -100.0f : prop_source->AirAbsorptionHF   > 0.0f    ? 0.0f    : prop_source->AirAbsorptionHF   ; /* [in/out] -100  , 0.0   , -5.0   , change in level per meter at high frequencies (win32 only) */
    newprops->HFReference       = prop_source->HFReference       < 1000.0f ? 1000.0f : prop_source->HFReference       > 20000   ? 20000   : prop_source->HFReference       ; /* [in/out] 1000.0, 20000 , 5000.0 , reference high frequency (hz) (win32/Xbox) */
    newprops->LFReference       = prop_source->LFReference       < 20.0f   ? 20.0f   : prop_source->LFReference       > 1000.0f ? 1000.0f : prop_source->LFReference       ; /* [in/out] 20.0  , 1000.0, 250.0  , reference low frequency (hz) (win32 only) */
    newprops->RoomRolloffFactor = prop_source->RoomRolloffFactor < 0.0f    ? 0.0f    : prop_source->RoomRolloffFactor > 10.0f   ? 10.0f   : prop_source->RoomRolloffFactor ; /* [in/out] 0.0   , 10.0  , 0.0    , like FMOD_3D_Listener_SetRolloffFactor but for room effect (win32/Xbox) */
    newprops->Diffusion         = prop_source->Diffusion         < 0.0f    ? 0.0f    : prop_source->Diffusion         > 100.0f  ? 100.0f  : prop_source->Diffusion         ; /* [in/out] 0.0   , 100.0 , 100.0  , Value that controls the echo density in the late reverberation decay. (Xbox only) */
    newprops->Density           = prop_source->Density           < 0.0f    ? 0.0f    : prop_source->Density           > 100.0f  ? 100.0f  : prop_source->Density           ; /* [in/out] 0.0   , 100.0 , 100.0  , Value that controls the modal density in the late reverberation decay (Xbox only) */
    newprops->Flags             = prop_source->Flags;


#ifdef FMOD_SUPPORT_SFXREVERB
    if (mInstance[instance].mDSP)
    {
        FMOD_RESULT result;
        bool reverb3d_active = false;

        #ifdef FMOD_SUPPORT_MULTIREVERB
        if (mSystem)
        {
            mSystem->get3DReverbActive(&reverb3d_active);
        }
        #endif

        if (mDisableIfNoEnvironment)
        {
            if (newprops->Environment == -1)
            {
                mInstance[instance].mDSP->setActive(reverb3d_active);
                if (!reverb3d_active)
                {
                    mInstance[instance].mDSP->reset();
                }
            }
            else
            {
                mInstance[instance].mDSP->setActive(true);
            }
        }
        else
        {
            mInstance[instance].mDSP->setActive(true);
        }


        if (newprops->Room != oldprops.Room)
        {
            result = mInstance[instance].mDSP->setParameter(FMOD_DSP_SFXREVERB_ROOM, (float)newprops->Room);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        if (newprops->RoomHF != oldprops.RoomHF)
        {
            result = mInstance[instance].mDSP->setParameter(FMOD_DSP_SFXREVERB_ROOMHF, (float)newprops->RoomHF);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        if (newprops->RoomRolloffFactor != oldprops.RoomRolloffFactor)
        {
            result = mInstance[instance].mDSP->setParameter(FMOD_DSP_SFXREVERB_ROOMROLLOFFFACTOR, newprops->RoomRolloffFactor);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        if (newprops->DecayTime != oldprops.DecayTime)
        {
            result = mInstance[instance].mDSP->setParameter(FMOD_DSP_SFXREVERB_DECAYTIME, newprops->DecayTime);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        if (newprops->DecayHFRatio != oldprops.DecayHFRatio)
        {
            result = mInstance[instance].mDSP->setParameter(FMOD_DSP_SFXREVERB_DECAYHFRATIO, newprops->DecayHFRatio);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        if (newprops->Reflections != oldprops.Reflections)
        {
            result = mInstance[instance].mDSP->setParameter(FMOD_DSP_SFXREVERB_REFLECTIONSLEVEL, (float)newprops->Reflections);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        if (newprops->ReflectionsDelay != oldprops.ReflectionsDelay)
        {
            result = mInstance[instance].mDSP->setParameter(FMOD_DSP_SFXREVERB_REFLECTIONSDELAY, newprops->ReflectionsDelay);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        if (newprops->Reverb != oldprops.Reverb)
        {
            result = mInstance[instance].mDSP->setParameter(FMOD_DSP_SFXREVERB_REVERBLEVEL, (float)newprops->Reverb);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        if (newprops->ReverbDelay != oldprops.ReverbDelay)
        {
            result = mInstance[instance].mDSP->setParameter(FMOD_DSP_SFXREVERB_REVERBDELAY, newprops->ReverbDelay);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        if (newprops->Diffusion != oldprops.Diffusion)
        {
            result = mInstance[instance].mDSP->setParameter(FMOD_DSP_SFXREVERB_DIFFUSION, newprops->Diffusion);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        if (newprops->Density != oldprops.Density)
        {
            result = mInstance[instance].mDSP->setParameter(FMOD_DSP_SFXREVERB_DENSITY, newprops->Density);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        if (newprops->HFReference != oldprops.HFReference)
        {
            result = mInstance[instance].mDSP->setParameter(FMOD_DSP_SFXREVERB_HFREFERENCE, newprops->HFReference);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        if (newprops->RoomLF != oldprops.RoomLF)
        {
            result = mInstance[instance].mDSP->setParameter(FMOD_DSP_SFXREVERB_ROOMLF, (float)newprops->RoomLF);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        if (newprops->LFReference != oldprops.LFReference)
        {
            result = mInstance[instance].mDSP->setParameter(FMOD_DSP_SFXREVERB_LFREFERENCE, newprops->LFReference);
            if (result != FMOD_OK)
            {
                return result;
            }
        }

    }
#endif

    /*
        Modify hardware reverb settings if we're targetting the 2D reverb
    */
    if (this == &mSystem->mReverbGlobal)
    {
        if (mSystem->mOutput->mDescription.reverb_setproperties)
        {
#ifdef FMOD_SUPPORT_SOFTWARE
            mSystem->mOutput->readfrommixer = Output::mixCallback;   /* Reset 'read only' variable in FMOD_OUTPUT_STATE in case the user changed it. */
#endif
            return mSystem->mOutput->mDescription.reverb_setproperties(mSystem->mOutput, &mInstance[instance].mProps);
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

    [PLATFORMS]

	[SEE_ALSO]
]
*/
FMOD_RESULT ReverbI::getProperties(FMOD_REVERB_PROPERTIES* properties)
{
    if (!properties)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    
    if (properties->Instance < 0 || properties->Instance >= FMOD_REVERB_MAXINSTANCES)
    {
        return FMOD_ERR_REVERB_INSTANCE;
    }

    *properties = mInstance[properties->Instance].mProps;

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
FMOD_RESULT ReverbI::setActive(bool active)
{ 
    mActive = active;

    return FMOD_OK;
}

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
FMOD_RESULT ReverbI::getActive(bool *active)
{ 
    if (!active)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *active = mActive;
    
    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
FMOD_RESULT ReverbI::setUserData(void *userdata)
{ 
    mUserData = userdata;

    return FMOD_OK;
}

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
FMOD_RESULT ReverbI::getUserData(void **userdata)
{ 
    if (!userdata)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *userdata = mUserData;
    
    return FMOD_OK;
}

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
void ReverbI::setDisableIfNoEnvironment(bool dis)
{
    int count;

    mDisableIfNoEnvironment = dis; 

    for (count = 0; count < FMOD_REVERB_MAXINSTANCES; count++)
    {
        FMOD_REVERB_PROPERTIES props;

        props.Instance = count;
        getProperties(&props);
        setProperties(&props);
    }
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
void ReverbI::calculateDistanceGain(FMOD_VECTOR *p, float *linear_gain, float *linear_coeff)
{
    FMOD_VECTOR a_vector; 
    float a,g,c;

    /*
        Non-3d reverb is independent of distance
    */
    if (!m3D)
    {
        if (linear_gain)
        {
            *linear_gain = 1.0f;
        }
        if (linear_coeff)
        {
            *linear_coeff = 1.0f;
        }
        return;
    }

    /*
        Find distance from p to reverb centre
    */
    FMOD_Vector_Subtract(p, &mPosition, &a_vector);

    a = FMOD_Vector_GetLength(&a_vector);

    /*
        Calculate the input gain of the reverb based on distance
    */
    if (a <= mMinRadius)       /* Unity on and inside min distance */
    {
        g = c = 1.0f;
    } 
    else if (a >= mMaxRadius)  /* Zero at and outside max distance */
    {
        g = c = 0.0f;
    } 
    else                        /* Linear dB rolloff between min and max distances */
    {
        if (mMinMaxDistance <= 0)
        {
            g = c = 1.0f;
        }
        else
        {
            float rolloff = (a - mMinRadius) / mMinMaxDistance;

            c = 1.0f - rolloff;
            g = (float)FMOD_POW(10.0f, rolloff * k20TH_MAXDISTANCE_GAINDB);
        }
    }

    if (linear_gain)
    {
        *linear_gain = g;
    }
    if (linear_coeff)
    {
        *linear_coeff = c;
    }

}

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
FMOD_RESULT ReverbI::setPresenceGain(int instance, int index, float lingain) 
{
    if (instance < 0 || instance >= FMOD_REVERB_MAXINSTANCES)
    {
        return FMOD_ERR_REVERB_INSTANCE;
    }

    if (index < 0 || index >= mSystem->mNumChannels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mInstance[instance].mChannelData[index].mPresenceGain = lingain; 

    return FMOD_OK;
}

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
FMOD_RESULT ReverbI::getPresenceGain(int instance, int index, float *lingain)
{
    if (instance < 0 || instance >= FMOD_REVERB_MAXINSTANCES)
    {
        return FMOD_ERR_REVERB_INSTANCE;
    }

    if (index < 0 || index >= mSystem->mNumChannels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!lingain)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *lingain = mInstance[instance].mChannelData[index].mPresenceGain; 

    return FMOD_OK;   
}


/*
[
	[DESCRIPTION]

    [PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
void ReverbI::sumRoomProps(FMOD_REVERB_STDPROPERTIES *accprops, FMOD_REVERB_PROPERTIES *addprops, float factor)
{
    if (accprops && addprops)
    {
        accprops->Room += factor * FMOD_EXP((float)(addprops->Room*kREVERB_GAIN_COEF));
    }
}


/*
[
	[DESCRIPTION]
    Add this reverb's properties, multiplied by 'factor', to the props parameter.
	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
void ReverbI::sumProps(FMOD_REVERB_STDPROPERTIES *accprops, FMOD_REVERB_PROPERTIES *addprops, float factor)
{
    if (accprops && addprops)
    {
        //FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "sumProps ", "factor=%f, adding %d -> %f\n",factor,addprops->Reflections, factor * FMOD_EXP((float)(addprops->Reflections*kREVERB_GAIN_COEF))));
        accprops->Room                 += factor * FMOD_EXP((float)(addprops->Room*kREVERB_GAIN_COEF));
        accprops->RoomHF               += factor * FMOD_EXP((float)(addprops->RoomHF*kREVERB_GAIN_COEF));
        accprops->RoomLF               += factor * FMOD_EXP((float)(addprops->RoomLF*kREVERB_GAIN_COEF));
        accprops->RoomRolloffFactor    += factor * addprops->RoomRolloffFactor;
        accprops->DecayTime            += factor * addprops->DecayTime;
        accprops->DecayHFRatio         += factor * addprops->DecayHFRatio;
        accprops->Reflections          += factor * FMOD_EXP((float)(addprops->Reflections*kREVERB_GAIN_COEF));
        accprops->ReflectionsDelay     += factor * addprops->ReflectionsDelay;
        accprops->Reverb               += factor * FMOD_EXP((float)(addprops->Reverb*kREVERB_GAIN_COEF));
        accprops->ReverbDelay          += factor * addprops->ReverbDelay;
        accprops->Diffusion            += factor * addprops->Diffusion;
        accprops->Density              += factor * addprops->Density;
        accprops->HFReference          += (addprops->HFReference > 0.0f) ? factor * FMOD_LOG(addprops->HFReference) : 0.0f;
        accprops->LFReference          += (addprops->LFReference > 0.0f) ? factor * FMOD_LOG(addprops->LFReference) : 0.0f;
    }
}

/*
[
	[DESCRIPTION]

    [PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
void ReverbI::factorProps(FMOD_REVERB_PROPERTIES *quotientprops, FMOD_REVERB_STDPROPERTIES *divprops, float factor)
{
    FMOD_memset(quotientprops, 0, sizeof(FMOD_REVERB_PROPERTIES));

    if (quotientprops && divprops)
    {
        //FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "factorProps ", "factor=%f -> %d\n",factor,(divprops->Reflections>0.0f) ? (int)(0.5f + FMOD_LOG(divprops->Reflections)* kREVERB_GAIN_COEF_INV * factor) : -10000));
        quotientprops->Room                 = (divprops->Room>0.0f) ? (int)(0.5f + FMOD_LOG(divprops->Room) * kREVERB_GAIN_COEF_INV * factor) : -10000;
        quotientprops->Room                 = (divprops->Room   >0.0f) ? (int)(0.5f + FMOD_LOG(divprops->Room   * factor) * kREVERB_GAIN_COEF_INV) : -10000;
        quotientprops->RoomHF               = (divprops->RoomHF >0.0f) ? (int)(0.5f + FMOD_LOG(divprops->RoomHF * factor) * kREVERB_GAIN_COEF_INV) : -10000;
        quotientprops->RoomLF               = (divprops->RoomLF >0.0f) ? (int)(0.5f + FMOD_LOG(divprops->RoomLF * factor) * kREVERB_GAIN_COEF_INV) : -10000;
        quotientprops->RoomRolloffFactor    = divprops->RoomRolloffFactor * factor;
        quotientprops->DecayTime            = divprops->DecayTime         * factor;
        quotientprops->DecayHFRatio         = divprops->DecayHFRatio      * factor;
        quotientprops->Reflections          = (divprops->Reflections>0.0f) ? (int)(0.5f + FMOD_LOG(divprops->Reflections * factor)* kREVERB_GAIN_COEF_INV) : -10000;
        quotientprops->ReflectionsDelay     = divprops->ReflectionsDelay  * factor;
        quotientprops->Reverb               = (divprops->Reverb>0.0f) ? (int)(0.5f + FMOD_LOG(divprops->Reverb * factor)* kREVERB_GAIN_COEF_INV) : -10000;
        quotientprops->ReverbDelay          = divprops->ReverbDelay * factor;
        quotientprops->Diffusion            = divprops->Diffusion * factor;
        quotientprops->Density              = divprops->Density * factor;
        quotientprops->HFReference          = FMOD_EXP(divprops->HFReference * factor);
        quotientprops->LFReference          = FMOD_EXP(divprops->LFReference * factor);
    }
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
FMOD_RESULT ReverbI::getMemoryInfo(unsigned int memorybits, unsigned int event_memorybits, unsigned int *memoryused, FMOD_MEMORY_USAGE_DETAILS *memoryused_details)
{
#ifdef FMOD_SUPPORT_MEMORYTRACKER
    GETMEMORYINFO_IMPL
#else
    return FMOD_ERR_UNIMPLEMENTED;
#endif
}


/*
[
	[DESCRIPTION]

    [PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
#ifdef FMOD_SUPPORT_MEMORYTRACKER

FMOD_RESULT ReverbI::getMemoryUsedImpl(MemoryTracker *tracker)
{
//AJS it's static in SystemI::mReverbGlobal
//    tracker->add(MEMTYPE_REVERBI, sizeof(*this));

    for (int i=0; i < FMOD_REVERB_MAXINSTANCES; i++)
    {
        FMOD_REVERB_INSTANCE *reverbinstance = &mInstance[i];

        if (reverbinstance->mDSP)
        {
            CHECK_RESULT(reverbinstance->mDSP->getMemoryUsed(tracker));
        }

        if (reverbinstance->mChannelData)
        {
            tracker->add(false, FMOD_MEMBITS_REVERB, mSystem->mNumChannels * sizeof(FMOD_REVERB_CHANNELDATA));

            if (reverbinstance->mChannelData->mDSPConnection)
            {
                CHECK_RESULT(reverbinstance->mChannelData->mDSPConnection->getMemoryUsed(tracker));
            }
        }
    }

    return FMOD_OK;
}

#endif


} // \namespace FMOD
