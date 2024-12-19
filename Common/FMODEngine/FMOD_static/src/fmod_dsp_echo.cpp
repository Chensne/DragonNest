#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_ECHO

#include "fmod.h"
#include "fmod_dspi.h"
#include "fmod_dsp_echo.h"
#include "fmod_systemi.h"

#include <stdio.h>

namespace FMOD
{

FMOD_DSP_DESCRIPTION_EX dspecho_desc;

#ifdef PLUGIN_EXPORTS

#ifdef __cplusplus
extern "C" {
#endif

    /*
        FMODGetDSPDescription is mandantory for every fmod plugin.  This is the symbol the registerplugin function searches for.
        Must be declared with F_API to make it export as stdcall.
    */
    F_DECLSPEC F_DLLEXPORT FMOD_DSP_DESCRIPTION_EX * F_API FMODGetDSPDescriptionEx()
    {
        return DSPEcho::getDescriptionEx();
    }

#ifdef __cplusplus
}
#endif

#endif  /* PLUGIN_EXPORTS */


FMOD_DSP_PARAMETERDESC dspecho_param[5] =
{
    { 1.0f,  5000.0f,   500.0f, "Delay",        "ms",       "Echo delay in ms.  10  to 5000.  Default = 500." },
    { 0.0f,     1.0f,     0.5f, "Decay",        "%",        "Echo decay per delay.  0 to 1.  1.0 = No decay, 0.0 = total decay.  Default = 0.5." },
    { 0.0f,    16.0f,     0.0f, "Max channels", "channels", "Maximum channels supported.  0 to 16.  0 = same as fmod's default output polyphony, 1 = mono, 2 = stereo etc.  Default = 0.  It is suggested to leave at 0!" },
    { 0.0f,     1.0f,     1.0f, "Drymix",       "%",        "Volume of original signal to pass to output.  0.0 to 1.0. Default = 1.0." },
    { 0.0f,     1.0f,     1.0f, "Wetmix",       "%",        "Volume of echo delay signal to pass to output.  0.0 to 1.0. Default = 1.0." },
};

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
FMOD_DSP_DESCRIPTION_EX *DSPEcho::getDescriptionEx()
{
    FMOD_memset(&dspecho_desc, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));

    FMOD_strcpy(dspecho_desc.name, "FMOD Echo");
    dspecho_desc.version       = 0x00010100;
    dspecho_desc.create        = DSPEcho::createCallback;
    dspecho_desc.release       = DSPEcho::releaseCallback;
    dspecho_desc.reset         = DSPEcho::resetCallback;
    dspecho_desc.read          = DSPEcho::readCallback;

    dspecho_desc.numparameters = sizeof(dspecho_param) / sizeof(dspecho_param[0]);
    dspecho_desc.paramdesc     = dspecho_param;
    dspecho_desc.setparameter  = DSPEcho::setParameterCallback;
    dspecho_desc.getparameter  = DSPEcho::getParameterCallback;
#ifdef FMOD_SUPPORT_MEMORYTRACKER
    dspecho_desc.getmemoryused = &DSPEcho::getMemoryUsedCallback;
#endif

    dspecho_desc.mType         = FMOD_DSP_TYPE_ECHO;
    dspecho_desc.mCategory     = FMOD_DSP_CATEGORY_FILTER;
    dspecho_desc.mSize         = sizeof(DSPEcho);

    return &dspecho_desc;
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
FMOD_RESULT DSPEcho::createInternal()
{
    int count;

    init();
    
    mChannels = 0;
	mOldSpeakerMask = 0xFFFF;

    for (count = 0; count < mDescription.numparameters; count++)
    {
        FMOD_RESULT result;

        result = setParameter(count, mDescription.paramdesc[count].defaultval);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    /*
        Do a forced update here to make sure everything is calculated correctly first.
    */
    mChannels   = mChannelsUpdate;
    mDelay      = mDelayUpdate;
    mDecayRatio = mDecayRatioUpdate;
    mDryMix     = mDryMixUpdate;
    mWetMix     = mWetMixUpdate;
    {
        int outputrate = 0;

        mSystem->getSoftwareFormat(&outputrate, 0, 0, 0, 0, 0);

        mEchoLength = (int)((float)outputrate * mDelay) / 1000;

        if (mEchoBufferMemory)
        {
            FMOD_Memory_Free(mEchoBufferMemory);
            mEchoBufferMemory = mEchoBuffer = 0;
        }

        mEchoBufferLengthBytes = mEchoLength;
        mEchoBufferLengthBytes *= mChannels;

        if (!mEchoBufferMemory)
        {
            #ifdef ECHO_USEFLOAT
            mEchoBufferLengthBytes *= sizeof(float);

            mEchoBufferMemory = (float *)FMOD_Memory_Calloc(mEchoBufferLengthBytes + 16);
            mEchoBuffer = (float *)FMOD_ALIGNPOINTER(mEchoBufferMemory, 16);
            #else
            mEchoBufferLengthBytes *= sizeof(signed short);

            mEchoBufferMemory = (signed short *)FMOD_Memory_Calloc(mEchoBufferLengthBytes + 16);
            mEchoBuffer = (signed short *)FMOD_ALIGNPOINTER(mEchoBufferMemory, 16);
            #endif
        }
        if (!mEchoBufferMemory)
        {
            return FMOD_ERR_MEMORY;
        }

        resetInternal();
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
FMOD_RESULT DSPEcho::releaseInternal()
{
    if (mEchoBufferMemory)
    {
        FMOD_Memory_Free(mEchoBufferMemory);
        mEchoBufferMemory = mEchoBuffer = 0;
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
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT DSPEcho::resetInternal()
{
    mEchoPosition = 0;

    if (mEchoBuffer)
    {
        FMOD_memset(mEchoBuffer, 0, mEchoBufferLengthBytes);
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
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT DSPEcho::readInternal(float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int outchannels)
{   
    unsigned int count;
    bool reallocbuffer = false;

    #define OO_32767 (1.0f / 32767.0f)

    if (!inbuffer)
    {
        return FMOD_OK;
    }

    /*
        Update parameters
    */
    if (mChannels != mChannelsUpdate)
    {
        mChannels = mChannelsUpdate;

        reallocbuffer = true;
    }
    if (mDelay != mDelayUpdate)
    {
        mDelay = mDelayUpdate;

        reallocbuffer = true;
    }
    if (mDecayRatio != mDecayRatioUpdate)
    {
        mDecayRatio = mDecayRatioUpdate;
    }
    if (mDryMix != mDryMixUpdate)
    {
        mDryMix = mDryMixUpdate;
    }
    if (mWetMix != mWetMixUpdate)
    {
        mWetMix = mWetMixUpdate;
    }

    if (reallocbuffer || !mEchoBuffer)
    {
        int outputrate = 0;

        mSystem->getSoftwareFormat(&outputrate, 0, 0, 0, 0, 0);

        mEchoLength = (int)((float)outputrate * mDelay) / 1000;

        if (mEchoBufferMemory)
        {
            FMOD_Memory_Free(mEchoBufferMemory);
            mEchoBufferMemory = mEchoBuffer = 0;
        }

        mEchoBufferLengthBytes = mEchoLength;
        mEchoBufferLengthBytes *= mChannels;

        if (!mEchoBufferMemory)
        {
            #ifdef ECHO_USEFLOAT
            mEchoBufferLengthBytes *= sizeof(float);

            mEchoBufferMemory = (float *)FMOD_Memory_Calloc(mEchoBufferLengthBytes + 16);
            mEchoBuffer = (float *)FMOD_ALIGNPOINTER(mEchoBufferMemory, 16);
            #else
            mEchoBufferLengthBytes *= sizeof(signed short);

            mEchoBufferMemory = (signed short *)FMOD_Memory_Calloc(mEchoBufferLengthBytes + 16);
            mEchoBuffer = (signed short *)FMOD_ALIGNPOINTER(mEchoBufferMemory, 16);
            #endif
        }
        if (!mEchoBufferMemory)
        {
            return FMOD_ERR_MEMORY;
        }

        resetInternal();
    }


	if (speakermask != mOldSpeakerMask) /* A speaker has been disabled: clear the echo buffer for that speaker to prevent artifacts if/when the speaker is reenabled   */
	{
		int count2;
		unsigned short diff = (mOldSpeakerMask ^ speakermask);

		for (count = 0; count < (unsigned int)inchannels; count++)
		{
			if (diff & (1 << count))
			{
				int len = mEchoLength * inchannels;
				for(count2 = count; count2 < len; count2 += inchannels)
				{
					mEchoBuffer[count2] = 0;
				}
			}
		}
		mOldSpeakerMask = speakermask;
	}

    if ((inchannels > mChannels) || !mEchoBuffer || !(speakermask & ((1 << inchannels)-1)) )
    {
        FMOD_memcpy(outbuffer, inbuffer, length * outchannels * sizeof(float));
        return FMOD_OK;
    }

    if (inchannels == 1 && (speakermask & 1))
    {
        unsigned int len;
        float *out, *in;
        
        in  = inbuffer;
        out = outbuffer;

        len = length;
        while (len)
        {
            unsigned int len2 = len;
#ifdef ECHO_USEFLOAT
            float *echobuff;
#else
            short *echobuff;
#endif
            
            echobuff = mEchoBuffer + (mEchoPosition * 1);

            if (mEchoPosition + len > mEchoLength)
            {
                len2 = mEchoLength - mEchoPosition;
            }

            for (count = 0; count < len2; count++) 
            {
                float in0 = in[0];

#ifdef ECHO_USEFLOAT
                out[0]      = (in0 * mDryMix) + (echobuff[0] * mWetMix);
                echobuff[0] = (in0          ) + (echobuff[0] * mDecayRatio);
#else
                float e0 = (float)echobuff[0] * OO_32767; 

                out[0]      =                (in0 * mDryMix) + (e0 * mWetMix);

                e0 = in0 + (e0 * mDecayRatio);
                echobuff[0] = e0 > 1.0f ? 32767 : e0 < -1.0f ? -32768: (signed short)(e0 * 32767.0f);
#endif
                in++;
                out++;
                echobuff++;
	        }

		    mEchoPosition += len2;
		    if (mEchoPosition >= mEchoLength)
            {
                mEchoPosition = 0;
            }
            len -= len2;
        }
    }
    else if (inchannels == 2 && (speakermask & 0x3)==0x3)
    {
        unsigned int len;
        float *out, *in;
        
        in  = inbuffer;
        out = outbuffer;

        len = length;
        while (len)
        {
            unsigned int len2 = len;
#ifdef ECHO_USEFLOAT
            float *echobuff;
#else
            short *echobuff;
#endif     
            echobuff = mEchoBuffer + (mEchoPosition * 2);

            if (mEchoPosition + len > mEchoLength)
            {
                len2 = mEchoLength - mEchoPosition;
            }

            for (count = 0; count < len2; count++) 
            {
                float in0 = in[0];
                float in1 = in[1];

#ifdef ECHO_USEFLOAT
                out[0] = (in0 * mDryMix) + (echobuff[0] * mWetMix);
                out[1] = (in1 * mDryMix) + (echobuff[1] * mWetMix);
                echobuff[0] = in0 + (echobuff[0] * mDecayRatio);
                echobuff[1] = in1 + (echobuff[1] * mDecayRatio);
#else
                float e0 = (float)echobuff[0] * OO_32767;
                float e1 = (float)echobuff[1] * OO_32767;

                out[0]      =                (in0 * mDryMix) + (e0 * mWetMix);
                out[1]      =                (in1 * mDryMix) + (e1 * mWetMix);
                e0 = in0 + (e0 * mDecayRatio);
                e1 = in1 + (e1 * mDecayRatio);
                echobuff[0] = e0 > 1.0f ? 32767 : e0 < -1.0f ? -32768: (signed short)(e0 * 32767.0f);;
                echobuff[1] = e1 > 1.0f ? 32767 : e1 < -1.0f ? -32768: (signed short)(e1 * 32767.0f);;
#endif

                in+=2;
                out+=2;
                echobuff+=2;
	        }

		    mEchoPosition += len2;
		    if (mEchoPosition >= mEchoLength)
            {
                mEchoPosition = 0;
            }
            len -= len2;
        }
    }
    else if (inchannels == 6 && (speakermask & 0x3F)==0x3F)
    {
        unsigned int len;
        float *out, *in;
        
        in  = inbuffer;
        out = outbuffer;

        len = length;
        while (len)
        {
            unsigned int len2 = len;
#ifdef ECHO_USEFLOAT
            float *echobuff;
#else
            short *echobuff;
#endif
            echobuff = mEchoBuffer + (mEchoPosition * 6);

            if (mEchoPosition + len > mEchoLength)
            {
                len2 = mEchoLength - mEchoPosition;
            }

            for (count = 0; count < len2; count++) 
            {
                float in0 = in[0];
                float in1 = in[1];
                float in2 = in[2];
                float in3 = in[3];
                float in4 = in[4];
                float in5 = in[5];

#ifdef ECHO_USEFLOAT
                out[0] = (in0 * mDryMix) + (echobuff[0] * mWetMix);
                out[1] = (in1 * mDryMix) + (echobuff[1] * mWetMix);
                out[2] = (in2 * mDryMix) + (echobuff[2] * mWetMix);
                out[3] = (in3 * mDryMix) + (echobuff[3] * mWetMix);
                out[4] = (in4 * mDryMix) + (echobuff[4] * mWetMix);
                out[5] = (in5 * mDryMix) + (echobuff[5] * mWetMix);
                echobuff[0] = in0 + (echobuff[0] * mDecayRatio);
                echobuff[1] = in1 + (echobuff[1] * mDecayRatio);
                echobuff[2] = in2 + (echobuff[2] * mDecayRatio);
                echobuff[3] = in3 + (echobuff[3] * mDecayRatio);
                echobuff[4] = in4 + (echobuff[4] * mDecayRatio);
                echobuff[5] = in5 + (echobuff[5] * mDecayRatio);
#else
                float e0 = (float)echobuff[0] * OO_32767;
                float e1 = (float)echobuff[1] * OO_32767;
                float e2 = (float)echobuff[2] * OO_32767;
                float e3 = (float)echobuff[3] * OO_32767;
                float e4 = (float)echobuff[4] * OO_32767;
                float e5 = (float)echobuff[5] * OO_32767;

                out[0] = (in0 * mDryMix) + (e0 * mWetMix);
                out[1] = (in1 * mDryMix) + (e1 * mWetMix);
                out[2] = (in2 * mDryMix) + (e2 * mWetMix);
                out[3] = (in3 * mDryMix) + (e3 * mWetMix);
                out[4] = (in4 * mDryMix) + (e4 * mWetMix);
                out[5] = (in5 * mDryMix) + (e5 * mWetMix);

                e0 = in0 + (e0 * mDecayRatio);
                e1 = in1 + (e1 * mDecayRatio);
                e2 = in2 + (e2 * mDecayRatio);
                e3 = in3 + (e3 * mDecayRatio);
                e4 = in4 + (e4 * mDecayRatio);
                e5 = in5 + (e5 * mDecayRatio);
                echobuff[0] = e0 > 1.0f ? 32767 : e0 < -1.0f ? -32768: (signed short)(e0 * 32767.0f);
                echobuff[1] = e1 > 1.0f ? 32767 : e1 < -1.0f ? -32768: (signed short)(e1 * 32767.0f);
                echobuff[2] = e2 > 1.0f ? 32767 : e2 < -1.0f ? -32768: (signed short)(e2 * 32767.0f);
                echobuff[3] = e3 > 1.0f ? 32767 : e3 < -1.0f ? -32768: (signed short)(e3 * 32767.0f);
                echobuff[4] = e4 > 1.0f ? 32767 : e4 < -1.0f ? -32768: (signed short)(e4 * 32767.0f);
                echobuff[5] = e5 > 1.0f ? 32767 : e5 < -1.0f ? -32768: (signed short)(e5 * 32767.0f);
#endif

                in+=6;
                out+=6;
                echobuff+=6;
	        }

		    mEchoPosition += len2;
		    if (mEchoPosition >= mEchoLength)
            {
                mEchoPosition = 0;
            }
            len -= len2;
        }
    }
    else if (inchannels == 8 && (speakermask & 0xFF)==0xFF)
    {
        unsigned int len;
        float *out, *in;
        
        in  = inbuffer;
        out = outbuffer;

        len = length;
        while (len)
        {
            unsigned int len2 = len;
#ifdef ECHO_USEFLOAT
            float *echobuff;
#else
            short *echobuff;
#endif     
            echobuff = mEchoBuffer + (mEchoPosition * 8);

            if (mEchoPosition + len > mEchoLength)
            {
                len2 = mEchoLength - mEchoPosition;
            }

            for (count = 0; count < len2; count++) 
            {
                float in0 = in[0];
                float in1 = in[1];
                float in2 = in[2];
                float in3 = in[3];
                float in4 = in[4];
                float in5 = in[5];
                float in6 = in[6];
                float in7 = in[7];

#ifdef ECHO_USEFLOAT
                out[0] = (in0 * mDryMix) + (echobuff[0] * mWetMix);
                out[1] = (in1 * mDryMix) + (echobuff[1] * mWetMix);
                out[2] = (in2 * mDryMix) + (echobuff[2] * mWetMix);
                out[3] = (in3 * mDryMix) + (echobuff[3] * mWetMix);
                out[4] = (in4 * mDryMix) + (echobuff[4] * mWetMix);
                out[5] = (in5 * mDryMix) + (echobuff[5] * mWetMix);
                out[6] = (in6 * mDryMix) + (echobuff[6] * mWetMix);
                out[7] = (in7 * mDryMix) + (echobuff[7] * mWetMix);

                echobuff[0] = in0 + (echobuff[0] * mDecayRatio);
                echobuff[1] = in1 + (echobuff[1] * mDecayRatio);
                echobuff[2] = in2 + (echobuff[2] * mDecayRatio);
                echobuff[3] = in3 + (echobuff[3] * mDecayRatio);
                echobuff[4] = in4 + (echobuff[4] * mDecayRatio);
                echobuff[5] = in5 + (echobuff[5] * mDecayRatio);
                echobuff[6] = in6 + (echobuff[6] * mDecayRatio);
                echobuff[7] = in7 + (echobuff[7] * mDecayRatio);
#else
                float e0 = (float)echobuff[0] * OO_32767;
                float e1 = (float)echobuff[1] * OO_32767;
                float e2 = (float)echobuff[2] * OO_32767;
                float e3 = (float)echobuff[3] * OO_32767;
                float e4 = (float)echobuff[4] * OO_32767;
                float e5 = (float)echobuff[5] * OO_32767;
                float e6 = (float)echobuff[6] * OO_32767;
                float e7 = (float)echobuff[7] * OO_32767;

                out[0]      =                (in0 * mDryMix) + (e0 * mWetMix);
                out[1]      =                (in1 * mDryMix) + (e1 * mWetMix);
                out[2]      =                (in2 * mDryMix) + (e2 * mWetMix);
                out[3]      =                (in3 * mDryMix) + (e3 * mWetMix);
                out[4]      =                (in4 * mDryMix) + (e4 * mWetMix);
                out[5]      =                (in5 * mDryMix) + (e5 * mWetMix);
                out[6]      =                (in6 * mDryMix) + (e6 * mWetMix);
                out[7]      =                (in7 * mDryMix) + (e7 * mWetMix);

                e0 = in0 + (e0 * mDecayRatio);
                e1 = in1 + (e1 * mDecayRatio);
                e2 = in2 + (e2 * mDecayRatio);
                e3 = in3 + (e3 * mDecayRatio);
                e4 = in4 + (e4 * mDecayRatio);
                e5 = in5 + (e5 * mDecayRatio);
                e6 = in6 + (e6 * mDecayRatio);
                e7 = in7 + (e7 * mDecayRatio);

                echobuff[0] = e0 > 1.0f ? 32767 : e0 < -1.0f ? -32768: (signed short)(e0 * 32767.0f);
                echobuff[1] = e1 > 1.0f ? 32767 : e1 < -1.0f ? -32768: (signed short)(e1 * 32767.0f);
                echobuff[2] = e2 > 1.0f ? 32767 : e2 < -1.0f ? -32768: (signed short)(e2 * 32767.0f);
                echobuff[3] = e3 > 1.0f ? 32767 : e3 < -1.0f ? -32768: (signed short)(e3 * 32767.0f);
                echobuff[4] = e4 > 1.0f ? 32767 : e4 < -1.0f ? -32768: (signed short)(e4 * 32767.0f);
                echobuff[5] = e5 > 1.0f ? 32767 : e5 < -1.0f ? -32768: (signed short)(e5 * 32767.0f);
                echobuff[6] = e6 > 1.0f ? 32767 : e6 < -1.0f ? -32768: (signed short)(e6 * 32767.0f);
                echobuff[7] = e7 > 1.0f ? 32767 : e7 < -1.0f ? -32768: (signed short)(e7 * 32767.0f);
#endif
                in+=8;
                out+=8;
                echobuff+=8;
	        }

		    mEchoPosition += len2;
		    if (mEchoPosition >= mEchoLength)
            {
                mEchoPosition = 0;
            }
            len -= len2;
        }
    }
    else
    {
        unsigned int len;
        float *out, *in;
        
        in  = inbuffer;
        out = outbuffer;

        len = length;
        while (len)
        {
            int count2;
            unsigned int len2 = len;
#ifdef ECHO_USEFLOAT
            float *echobuff;
#else
            short *echobuff;
#endif
            
            echobuff = mEchoBuffer + (mEchoPosition * inchannels);

            if (mEchoPosition + len > mEchoLength) /* if (position + echo length) exceed the size of the echo buffer.. */
            {
                len2 = mEchoLength - mEchoPosition;
            }

            for (count = 0; count < len2; count++) 
            {
                for (count2 = 0; count2 < inchannels; count2++)
                {
					if (!((1 << count2) & speakermask)) 
					{
						out[count2] = in[count2];
					}
					else
					{
						float in0 = in[count2];

#ifdef ECHO_USEFLOAT
						out[count2]      =                (in0 * mDryMix) + (echobuff[count2] * mWetMix);
						echobuff[count2] =                (in0          ) + (echobuff[count2] * mDecayRatio);
#else
						float e0 = (float)echobuff[count2] * OO_32767;

						out[count2]      =                (in0 * mDryMix) + (e0 * mWetMix);
						e0 = in0 + (e0 * mDecayRatio);
						echobuff[count2] = e0 > 1.0f ? 32767 : e0 < -1.0f ? -32768: (signed short)(e0 * 32767.0f);
#endif
					}
                }

                in+=inchannels;
                out+=inchannels;
                echobuff+=inchannels;
	        }

		    mEchoPosition += len2;
		    if (mEchoPosition >= mEchoLength)
            {
                mEchoPosition = 0;
            }
            len -= len2;
        }    
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
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT DSPEcho::setParameterInternal(int index, float value)
{
    FMOD_RESULT result;

    result = mSystem->getSoftwareFormat(0, 0, &mChannelsUpdate, 0, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    switch (index)
    {
        case FMOD_DSP_ECHO_DELAY:
        {
            mDelayUpdate = value;
            break;
        }
        case FMOD_DSP_ECHO_DECAYRATIO:
        {
            mDecayRatioUpdate = value;
            break;
        }
        case FMOD_DSP_ECHO_MAXCHANNELS:
        {
            mMaxChannels = (int)value;
            if (mMaxChannels)
            {
                mChannelsUpdate = mMaxChannels;
            }
            break;
        }
        case FMOD_DSP_ECHO_DRYMIX:
        {
            mDryMixUpdate = value;
            break;
        }
        case FMOD_DSP_ECHO_WETMIX:
        {
            mWetMixUpdate = value;
            break;
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
FMOD_RESULT DSPEcho::getParameterInternal(int index, float *value, char *valuestr)
{
    switch (index)
    {
        case FMOD_DSP_ECHO_DELAY:
        {
            *value = mDelayUpdate;
            sprintf(valuestr, "%.02f", mDelayUpdate);
            break;
        }
        case FMOD_DSP_ECHO_DECAYRATIO:
        {
            *value = mDecayRatioUpdate;
            sprintf(valuestr, "%.1f", mDecayRatioUpdate * 100.0f);
            break;
        }
        case FMOD_DSP_ECHO_MAXCHANNELS:
        {
            *value = (float)mMaxChannels;
            sprintf(valuestr, "%d", mMaxChannels);
            break;
        }
        case FMOD_DSP_ECHO_DRYMIX:
        {
            *value = mDryMixUpdate;
            sprintf(valuestr, "%.1f", mDryMixUpdate * 100.0f);
            break;
        }
        case FMOD_DSP_ECHO_WETMIX:
        {
            *value = mWetMixUpdate;
            sprintf(valuestr, "%.1f", mWetMixUpdate * 100.0f);
            break;
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

#ifdef FMOD_SUPPORT_MEMORYTRACKER

FMOD_RESULT DSPEcho::getMemoryUsedImpl(MemoryTracker *tracker)
{
    // Size of this class is already accounted for (via description.mSize). Just add extra allocated memory here.

    if (mEchoBufferMemory)
    {
        tracker->add(false, FMOD_MEMBITS_DSP, mEchoBufferLengthBytes + 16);
    }  

    return FMOD_OK;
}

#endif


/*
    ==============================================================================================================

    CALLBACK INTERFACE

    ==============================================================================================================
*/


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
FMOD_RESULT F_CALLBACK DSPEcho::createCallback(FMOD_DSP_STATE *dsp)
{
    DSPEcho *echo = (DSPEcho *)dsp;

    return echo->createInternal();
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
FMOD_RESULT F_CALLBACK DSPEcho::releaseCallback(FMOD_DSP_STATE *dsp)
{
    DSPEcho *echo = (DSPEcho *)dsp;

    return echo->releaseInternal();
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
FMOD_RESULT F_CALLBACK DSPEcho::resetCallback(FMOD_DSP_STATE *dsp)
{
    DSPEcho *echo = (DSPEcho *)dsp;

    return echo->resetInternal();
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
FMOD_RESULT F_CALLBACK DSPEcho::readCallback(FMOD_DSP_STATE *dsp, float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int outchannels)
{
    DSPEcho *echo = (DSPEcho *)dsp;

    return echo->readInternal(inbuffer, outbuffer, length, inchannels, outchannels);
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
FMOD_RESULT F_CALLBACK DSPEcho::setParameterCallback(FMOD_DSP_STATE *dsp, int index, float value)
{
    DSPEcho *echo = (DSPEcho *)dsp;

    return echo->setParameterInternal(index, value);
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
FMOD_RESULT F_CALLBACK DSPEcho::getParameterCallback(FMOD_DSP_STATE *dsp, int index, float *value, char *valuestr)
{
    DSPEcho *echo = (DSPEcho *)dsp;

    return echo->getParameterInternal(index, value, valuestr);
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
FMOD_RESULT F_CALLBACK DSPEcho::getMemoryUsedCallback(FMOD_DSP_STATE *dsp, MemoryTracker *tracker)
{
    DSPEcho *echo = (DSPEcho *)dsp;    

    return echo->DSPEcho::getMemoryUsed(tracker);
}
#endif


}

#endif
