#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_CELT

#include "fmod.h"
#include "fmod_codec_celt.h"
#include "fmod_soundi.h"

extern "C" {

void * FMOD_CELT_Calloc(int size)
{
    return FMOD_Memory_Calloc(size);
}

}




namespace FMOD
{

CELTMode *CodecCELT::mCELTModeMono      = 0;
CELTMode *CodecCELT::mCELTModeStereo    = 0;
int       CodecCELT::mCELTModeMonoRef   = 0;
int       CodecCELT::mCELTModeStereoRef   = 0;

FMOD_CODEC_DESCRIPTION_EX celtcodec;

#if defined(PLUGIN_EXPORTS) && !defined(PLUGIN_FSB)

#ifdef __cplusplus
extern "C" {
#endif

    /*
        FMODGetCodecDescription is mandantory for every fmod plugin.  This is the symbol the registerplugin function searches for.
        Must be declared with F_API to make it export as stdcall.
    */
    F_DECLSPEC F_DLLEXPORT FMOD_CODEC_DESCRIPTION_EX * F_API FMODGetCodecDescriptionEx()
    {
        return CodecCELT::getDescriptionEx();
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
FMOD_CODEC_DESCRIPTION_EX *CodecCELT::getDescriptionEx()
{
    FMOD_memset(&celtcodec, 0, sizeof(FMOD_CODEC_DESCRIPTION_EX));

    celtcodec.name        = "FMOD CELT Codec";
    celtcodec.version     = 0x00010100;
    celtcodec.timeunits   = FMOD_TIMEUNIT_PCM;
    celtcodec.open        = &CodecCELT::openCallback;
    celtcodec.close       = &CodecCELT::closeCallback;
    celtcodec.read        = &CodecCELT::readCallback;
    celtcodec.setposition = &CodecCELT::setPositionCallback;
    celtcodec.init        = &CodecCELT::initCallback;

#ifdef FMOD_SUPPORT_MEMORYTRACKER
    celtcodec.getmemoryused = &CodecCELT::getMemoryUsedCallback;
#endif

    celtcodec.mSize       = sizeof(CodecCELT);

    return &celtcodec;
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
FMOD_RESULT CodecCELT::createCELTModeMono()
{
    int error;

    if (!mCELTModeMono)
    {
        mCELTModeMono = celt_mode_create(FMOD_CELT_RATE, 1, FMOD_CELT_FRAMESIZESAMPLES, &error);
        if (error != CELT_OK)
        {
            FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "CodecCELT::createCELTModeMono", "Error (%d) creating CELT mode: %d\n", error, 1));
            return FMOD_ERR_INTERNAL;
        }
    }

    mCELTModeMonoRef++;

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
FMOD_RESULT CodecCELT::createCELTModeStereo()
{
    int error;

    if (!mCELTModeStereo)
    {
        mCELTModeStereo = celt_mode_create(FMOD_CELT_RATE, 2, FMOD_CELT_FRAMESIZESAMPLES, &error);
        if (error != CELT_OK)
        {
            FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "CodecCELT::createCELTModeStereo", "Error creating CELT mode: %d\n", error));
            return FMOD_ERR_INTERNAL;
        }
    }

    mCELTModeStereoRef++;

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
int CodecCELT::getBitstreamVersion()
{
    int bitstreamversion = 0;

    if (mCELTModeMono)
    {
        celt_mode_info(mCELTModeMono, CELT_GET_BITSTREAM_VERSION, (celt_int32_t *)&bitstreamversion);
    }
    else if (mCELTModeStereo)
    {
        celt_mode_info(mCELTModeStereo, CELT_GET_BITSTREAM_VERSION, (celt_int32_t *)&bitstreamversion);
    }

    return bitstreamversion;
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
FMOD_RESULT CodecCELT::CELTinit(int numstreams)
{
    mCELTDecoderBuffer = (char *)FMOD_Memory_Calloc(numstreams * FMOD_CELT_DECODERBUFFERSIZE);
    if (!mCELTDecoderBuffer)
    {
        return FMOD_ERR_MEMORY;
    }

    for (int i = 0; i < numstreams; i++)
    {
        mCELTDecoder[i] = fmod_celt_decoder_create_only(mCELTDecoderBuffer + i * FMOD_CELT_DECODERBUFFERSIZE);
        if (!mCELTDecoder[i])
        {
            FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "CodecCELT::CELTinit", "celt_decoder_create failed\n"));
            return FMOD_ERR_INTERNAL;
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
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT CodecCELT::openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO *userexinfo)
{
    /*
        CodecCELT for fsb only
    */

    return FMOD_ERR_FORMAT;
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
FMOD_RESULT CodecCELT::closeInternal()
{       
    mCELTModeMonoRef--;
    if (mCELTModeMonoRef == 0)
    {
        celt_mode_destroy(mCELTModeMono);
    }

    mCELTModeStereoRef--;
    if (mCELTModeStereoRef == 0)
    {
        celt_mode_destroy(mCELTModeStereo);
    }

    if (mCELTDecoderBuffer)
    {
        FMOD_Memory_Free(mCELTDecoderBuffer);
        mCELTDecoderBuffer = 0;
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
FMOD_RESULT CodecCELT::decodeHeader(void *buffer, unsigned int *framesize)
{
    FMOD_CELT_FRAMEHEADER *frameheader = (FMOD_CELT_FRAMEHEADER *)buffer;

	#ifdef PLATFORM_ENDIAN_BIG
	frameheader->magic     = FMOD_SWAPENDIAN_DWORD(frameheader->magic);
	frameheader->framesize = FMOD_SWAPENDIAN_DWORD(frameheader->framesize);
	#endif

    if (frameheader->magic != FMOD_CELT_MAGIC)
    {
        return FMOD_ERR_FORMAT;
    }

    if (framesize)
    {
        *framesize = frameheader->framesize;
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
FMOD_RESULT CodecCELT::readInternal(void *buffer, unsigned int sizebytes, unsigned int *bytesread)
{
    FMOD_RESULT result;
    int error = 0;
    unsigned char readbuffer[1024];
    unsigned int framesize;
    int channel = 0;

    *bytesread = 0;

    /*
        Multichannel interleaved.
    */
    do
    {
        unsigned int bytesreadinternal = 0;

        /*
            Get next valid header
        */
        do
        {
            result = mFile->read(readbuffer, sizeof(FMOD_CELT_FRAMEHEADER), 1);
            if (result != FMOD_OK)
            {
                return result;
            }

            /*
                Decode header
            */
            result = decodeHeader(readbuffer, &framesize);
            if (result != FMOD_OK)
            {
                mFile->seek(-3, SEEK_CUR);    /* increment a byte at a time! */
            }
        }
        while (result != FMOD_OK);

        /*
            Read in a frame and decode it
        */
        result = mFile->read(readbuffer, framesize, 1);
        if (result != FMOD_OK)
        {
            return result;
        }
 
        if (waveformat[0].channels > 2)
        {
            short decodebuffer[FMOD_CELT_FRAMESIZESAMPLES * 2];

            error = celt_decode(mCELTDecoder[channel], readbuffer, framesize, (celt_int16_t *)decodebuffer);
            if (error != CELT_OK)
            {
                return FMOD_ERR_INTERNAL;
            }

            /*
                Interleave into pcm decode buffer
            */
            short *outbuffer = (short *)buffer + channel * 2;

            for (int i = 0; i < FMOD_CELT_FRAMESIZESAMPLES * 2; i+= 2)
            {
                outbuffer[0] = decodebuffer[i + 0];
                outbuffer[1] = decodebuffer[i + 1];

                outbuffer += waveformat[0].channels;
            }

            SoundI::getBytesFromSamples(FMOD_CELT_FRAMESIZESAMPLES, &bytesreadinternal, 2, FMOD_SOUND_FORMAT_PCM16);
        }
        else
        {
            error = celt_decode(mCELTDecoder[channel], readbuffer, framesize, (celt_int16_t *)buffer);
            if (error != CELT_OK)
            {
                return FMOD_ERR_INTERNAL;
            }

            SoundI::getBytesFromSamples(FMOD_CELT_FRAMESIZESAMPLES, &bytesreadinternal, waveformat[0].channels, FMOD_SOUND_FORMAT_PCM16);
        }

        *bytesread += bytesreadinternal;

        channel++;
    }
    while (channel < waveformat[0].channels / 2);

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
FMOD_RESULT CodecCELT::setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    FMOD_RESULT result;
    unsigned int frame, pcmbytes, bytespersample;
    unsigned int excessbytes = 0;
    unsigned int raw = 0;

    FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "CodecCELT::setPositionInternal", "%d\n", position));

    /*
        set the CELT mode
    */
    if (waveformat[0].channels == 1)
    {
        /*
            Mono
        */
        fmod_celt_decoder_setmode(mCELTDecoder[0], mCELTModeMono);
    }
    else
    {
        /*
            Stereo or Multichannel. (Interleaved stereo pairs)
        */
        for (int i = 0; i < waveformat[0].channels / 2; i++)
        {
            fmod_celt_decoder_setmode(mCELTDecoder[i], mCELTModeStereo);
        }
    }

    bytespersample = sizeof(signed short) * waveformat[0].channels;
    pcmbytes = position * bytespersample;
    frame = pcmbytes / mPCMFrameLengthBytes;

    if (pcmbytes)
    {
        unsigned int framerewind = 1;

        excessbytes = pcmbytes - (frame * mPCMFrameLengthBytes);

        if (frame < framerewind)
        {
            framerewind = frame;
        }

        frame -= framerewind;
        excessbytes += (mPCMFrameLengthBytes * framerewind);
    }
    else
    {
        excessbytes = 0;
        position = 0;
    }

    if (position  > (excessbytes / bytespersample))
    {
        int numframes = (int)((waveformat[0].lengthpcm * bytespersample) / mPCMFrameLengthBytes);
        int compressedframesize = waveformat[0].lengthbytes / numframes;
        raw = frame * compressedframesize;
    }
    else
    {
        raw = 0;
    }

	raw += mSrcDataOffset;

    if (raw > (unsigned int)mSrcDataOffset + (unsigned int)waveformat[0].lengthbytes)
    {
        raw = mSrcDataOffset;
    }

    result = mFile->seek(raw, SEEK_SET);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (!(mFlags & FMOD_CODEC_FROMFSB))
    {
        mFlags |= FMOD_CODEC_SEEKING;
    }

    while (excessbytes)
    {
        char buff[2048];
        unsigned int read = 0, toread = 2048;

        if (toread > excessbytes)
        {
            toread = excessbytes;
        }

        result = Codec::read(buff, toread, &read);
        if (result != FMOD_OK)
        {
            break;
        }

        /*
            During the first read after the seek, decode sometimes fails because the decoder
            hasn't built up state yet, just push on, decoded frames are dropped anyway.
        */
        if (read == 0)
        {
            read = toread;
        }

        if (excessbytes >= read)
        {
            excessbytes -= read;
        }
        else
        {
            excessbytes = 0;
        }
    }

    mFlags &= ~FMOD_CODEC_SEEKING;

    return FMOD_OK;
}


#ifdef FMOD_SUPPORT_MEMORYTRACKER

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
FMOD_RESULT CodecCELT::getMemoryUsedImpl(MemoryTracker *tracker)
{
    return FMOD_OK;
}

#endif


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
FMOD_RESULT F_CALLBACK CodecCELT::openCallback(FMOD_CODEC_STATE *codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO *userexinfo)
{
    CodecCELT *celt = (CodecCELT *)codec;

    return celt->openInternal(usermode, userexinfo);
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
FMOD_RESULT F_CALLBACK CodecCELT::closeCallback(FMOD_CODEC_STATE *codec)
{
    CodecCELT *celt = (CodecCELT *)codec;

    return celt->closeInternal();
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
FMOD_RESULT F_CALLBACK CodecCELT::readCallback(FMOD_CODEC_STATE *codec, void *buffer, unsigned int sizebytes, unsigned int *bytesread)
{
    CodecCELT *celt = (CodecCELT *)codec;

    return celt->readInternal(buffer, sizebytes, bytesread);
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
FMOD_RESULT F_CALLBACK CodecCELT::setPositionCallback(FMOD_CODEC_STATE *codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    CodecCELT *celt = (CodecCELT *)codec;

    return celt->setPositionInternal(subsound, position, postype);
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
FMOD_RESULT F_CALLBACK CodecCELT::initCallback(FMOD_CODEC_STATE *codec, int numstreams)
{
    CodecCELT *celt = (CodecCELT *)codec;

    return celt->CELTinit(numstreams);
}

#ifdef FMOD_SUPPORT_MEMORYTRACKER
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
FMOD_RESULT F_CALLBACK CodecCELT::getMemoryUsedCallback(FMOD_CODEC_STATE *codec, MemoryTracker *tracker)
{
    CodecCELT *celt = (CodecCELT *)codec;    

    return celt->getMemoryUsed(tracker);
}
#endif
}

#endif // FMOD_SUPPORT_CELT
