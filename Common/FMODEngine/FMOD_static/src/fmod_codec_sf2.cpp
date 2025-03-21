#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_SF2

#include "fmod.h"
#include "fmod_codec_sf2.h"
#include "fmod_debug.h"
#include "fmod_file.h"
#include "fmod_metadata.h"
#include "fmod_soundi.h"
#include "fmod_string.h"

#include <stdio.h>

namespace FMOD
{

FMOD_CODEC_DESCRIPTION_EX sf2codec;


#ifdef PLUGIN_EXPORTS

#ifdef __cplusplus
extern "C" {
#endif

    /*
        FMODGetCodecDescription is mandatory for every fmod plugin.  This is the symbol the registerplugin function searches for.
        Must be declared with F_API to make it export as stdcall.
    */
    F_DECLSPEC F_DLLEXPORT FMOD_CODEC_DESCRIPTION_EX * F_API FMODGetCodecDescriptionEx()
    {
        return CodecSF2::getDescriptionEx();
    }

#ifdef __cplusplus
}
#endif

#endif  /* PLUGIN_EXPORTS */

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_CODEC_DESCRIPTION_EX *CodecSF2::getDescriptionEx()
{
    FMOD_memset(&sf2codec, 0, sizeof(FMOD_CODEC_DESCRIPTION_EX));

    sf2codec.name        = "FMOD SoundFont 2.0 Codec";
    sf2codec.version     = 0x00010100;
    sf2codec.timeunits   = FMOD_TIMEUNIT_PCM;
    sf2codec.open        = &CodecSF2::openCallback;
    sf2codec.close       = &CodecSF2::closeCallback;
    sf2codec.read        = &CodecSF2::readCallback;
    sf2codec.setposition = &CodecSF2::setPositionCallback;
    sf2codec.soundcreate = &CodecSF2::soundcreateCallback;

    sf2codec.mType       = FMOD_SOUND_TYPE_SF2;
    sf2codec.mSize       = sizeof(CodecSF2);

    return &sf2codec;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT CodecSF2::openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO *userexinfo)
{
    FMOD_RESULT     result = FMOD_OK;

    init(FMOD_SOUND_TYPE_SF2);

    FLOG((LOG_NORMAL, __FILE__, __LINE__, "CodecSF2::openInternal", "attempting to open as SF2..\n"));

	result = mFile->seek(0, SEEK_SET);
    if (result != FMOD_OK)
    {
        return result;
    }


	mSrcDataOffset = 0;

//    if (mHeader.numsamples < 1)
    {
        return FMOD_ERR_FORMAT;
    }

//    mShdr = (FMOD_SF2_SAMPLE_HEADER **)FMOD_Memory_Calloc(sizeof(void *) * mHeader.numsamples);
  //  if (!mShdr)
    {
        return FMOD_ERR_MEMORY;
    }

//    numsubsounds = mHeader.numsamples;
    waveformat = mWaveFormat;

	/*
        Get size of file in bytes
    */
	result = mFile->getSize(&mSrcDataSize);
    if (result != FMOD_OK)
    {
        return result;
    }

    return result;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT CodecSF2::closeInternal()
{
    if (waveformat)
    {
        FMOD_Memory_Free(waveformat);
        waveformat = 0;
    }

    if (mDataOffset)
    {
        FMOD_Memory_Free(mDataOffset);
        mDataOffset = 0;
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
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT CodecSF2::readInternal(void *buffer, unsigned int sizebytes, unsigned int *bytesread)
{
    FMOD_RESULT result = FMOD_OK;

    result = mFile->read(buffer, 1, sizebytes, bytesread);
	if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
    {
        return result;
    }
    
    #ifdef PLATFORM_ENDIAN_BIG
    {
        unsigned int count;
        signed short *wptr = (signed short *)buffer;
        
        for (count=0; count < *bytesread >> 1; count++)
        {
            wptr[count] = FMOD_SWAPENDIAN_WORD(wptr[count]);
        }
    
    }
    #endif

    return result;	
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT CodecSF2::setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    FMOD_RESULT result = FMOD_OK;

    if (subsound < 0 || (numsubsounds && subsound >= numsubsounds))
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (mFile->mSeekable)
    {
        unsigned int posbytes;

        if (subsound != mCurrentIndex)
        {
            mCurrentIndex = subsound;
        }

        result = SoundI::getBytesFromSamples(position, &posbytes, waveformat[mCurrentIndex].channels, waveformat[mCurrentIndex].format);
        if (result != FMOD_OK)
        {
            return result;
        }

        posbytes += mDataOffset[mCurrentIndex];

        result = mFile->seek(posbytes, SEEK_SET);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    return result;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT CodecSF2::soundcreateInternal(int subsound, FMOD_SOUND *sound)
{
    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK CodecSF2::openCallback(FMOD_CODEC_STATE *codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO *userexinfo)
{
    CodecSF2 *sf2 = (CodecSF2 *)codec;    

    return sf2->openInternal(usermode, userexinfo);
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK CodecSF2::closeCallback(FMOD_CODEC_STATE *codec)
{
    CodecSF2 *sf2 = (CodecSF2 *)codec;    

    return sf2->closeInternal();
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK CodecSF2::readCallback(FMOD_CODEC_STATE *codec, void *buffer, unsigned int sizebytes, unsigned int *bytesread)
{
    CodecSF2 *sf2 = (CodecSF2 *)codec;    

    return sf2->readInternal(buffer, sizebytes, bytesread);
}

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK CodecSF2::setPositionCallback(FMOD_CODEC_STATE *codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    CodecSF2 *sf2 = (CodecSF2 *)codec;    

    return sf2->setPositionInternal(subsound, position, postype);
}

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK CodecSF2::soundcreateCallback(FMOD_CODEC_STATE *codec, int subsound, FMOD_SOUND *sound)
{
    CodecSF2 *sf2 = (CodecSF2 *)codec;    

    return sf2->soundcreateInternal(subsound, sound);
}

}

#endif

