#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_DELAY

#include "fmod.h"
#include "fmod_dspi.h"
#include "fmod_dsp_delay.h"
#include "fmod_systemi.h"

#include <stdio.h>

namespace FMOD
{

FMOD_DSP_DESCRIPTION_EX dspdelay_desc;

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
        return DSPDelay::getDescriptionEx();
    }

#ifdef __cplusplus
}
#endif

#endif  /* PLUGIN_EXPORTS */


FMOD_DSP_PARAMETERDESC dspdelay_param[DELAY_MAX_CHANNELS+1] =
{
    { 0.0f,  10000.0f,     0.0f, "Delay ch0",       "ms",       "Channel #0 delay in ms.  0  to 10000.  Default = 0." },
    { 0.0f,  10000.0f,     0.0f, "Delay ch1",       "ms",       "Channel #1 delay in ms.  0  to 10000.  Default = 0." },
    { 0.0f,  10000.0f,     0.0f, "Delay ch2",       "ms",       "Channel #2 delay in ms.  0  to 10000.  Default = 0." },
    { 0.0f,  10000.0f,     0.0f, "Delay ch3",       "ms",       "Channel #3 delay in ms.  0  to 10000.  Default = 0." },
    { 0.0f,  10000.0f,     0.0f, "Delay ch4",       "ms",       "Channel #4 delay in ms.  0  to 10000.  Default = 0." },
    { 0.0f,  10000.0f,     0.0f, "Delay ch5",       "ms",       "Channel #5 delay in ms.  0  to 10000.  Default = 0." },
    { 0.0f,  10000.0f,     0.0f, "Delay ch6",       "ms",       "Channel #6 delay in ms.  0  to 10000.  Default = 0." },
    { 0.0f,  10000.0f,     0.0f, "Delay ch7",       "ms",       "Channel #7 delay in ms.  0  to 10000.  Default = 0." },
    { 0.0f,  10000.0f,     0.0f, "Delay ch8",       "ms",       "Channel #8 delay in ms.  0  to 10000.  Default = 0." },
    { 0.0f,  10000.0f,     0.0f, "Delay ch9",       "ms",       "Channel #9 delay in ms.  0  to 10000.  Default = 0." },
    { 0.0f,  10000.0f,     0.0f, "Delay ch10",      "ms",       "Channel #10 delay in ms.  0  to 10000.  Default = 0." },
    { 0.0f,  10000.0f,     0.0f, "Delay ch11",      "ms",       "Channel #11 delay in ms.  0  to 10000.  Default = 0." },
    { 0.0f,  10000.0f,     0.0f, "Delay ch12",      "ms",       "Channel #12 delay in ms.  0  to 10000.  Default = 0." },
    { 0.0f,  10000.0f,     0.0f, "Delay ch13",      "ms",       "Channel #13 delay in ms.  0  to 10000.  Default = 0." },
    { 0.0f,  10000.0f,     0.0f, "Delay ch14",      "ms",       "Channel #14 delay in ms.  0  to 10000.  Default = 0." },
    { 0.0f,  10000.0f,     0.0f, "Delay ch15",      "ms",       "Channel #15 delay in ms.  0  to 10000.  Default = 0." },
    { 1.0f,  10000.0f,    10.0f, "Max delay",       "ms",       "Maximum delay in ms.  1  to 10000.  Default = 10." },
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
FMOD_DSP_DESCRIPTION_EX *DSPDelay::getDescriptionEx()
{
    FMOD_memset(&dspdelay_desc, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));

    FMOD_strcpy(dspdelay_desc.name, "FMOD Delay");
    dspdelay_desc.version       = 0x00010100;
    dspdelay_desc.create        = DSPDelay::createCallback;
    dspdelay_desc.release       = DSPDelay::releaseCallback;
    dspdelay_desc.reset         = DSPDelay::resetCallback;
    dspdelay_desc.read          = DSPDelay::readCallback;

    dspdelay_desc.numparameters = sizeof(dspdelay_param) / sizeof(dspdelay_param[0]);
    dspdelay_desc.paramdesc     = dspdelay_param;
    dspdelay_desc.setparameter  = DSPDelay::setParameterCallback;
    dspdelay_desc.getparameter  = DSPDelay::getParameterCallback;
#ifdef FMOD_SUPPORT_MEMORYTRACKER
    dspdelay_desc.getmemoryused = &DSPDelay::getMemoryUsedCallback;
#endif

    dspdelay_desc.mType         = FMOD_DSP_TYPE_DELAY;
    dspdelay_desc.mCategory     = FMOD_DSP_CATEGORY_FILTER;
    dspdelay_desc.mSize         = sizeof(DSPDelay);

    return &dspdelay_desc;
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
FMOD_RESULT DSPDelay::createInternal()
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
    mMaxDelay   = mMaxDelayUpdate;
    {
        int outputrate = 0;

        mSystem->getSoftwareFormat(&outputrate, 0, &mChannels, 0, 0, 0);

        for (count = 0; count < DELAY_MAX_CHANNELS; count++)
        {
            mDelay[count] = (mDelayUpdate[count] < mMaxDelay) ? mDelayUpdate[count] : mMaxDelay;
            mOffset[count] = (int)(((float)outputrate * mDelay[count] / 1000.0f ) + 0.5f);
        }
        /* 
            need one extra sample as input is written to buffer before output is read
            allowing for a delay of zero samples
        */
        mDelayBufferLength = (int)(((float)outputrate * mMaxDelay / 1000.0f) + 0.5f) + 1;

        if (mDelayBufferMemory)
        {
            FMOD_Memory_Free(mDelayBufferMemory);
            mDelayBufferMemory = mDelayBuffer = 0;
        }

        mDelayBufferLengthBytes = mDelayBufferLength;
        mDelayBufferLengthBytes *= mChannels;

        if (!mDelayBufferMemory)
        {
#ifdef DELAY_USEFLOAT
            mDelayBufferLengthBytes *= sizeof(float);

            mDelayBufferMemory = (float *)FMOD_Memory_Calloc(mDelayBufferLengthBytes + 16);
            mDelayBuffer = (float *)FMOD_ALIGNPOINTER(mDelayBufferMemory, 16);
#else
            mDelayBufferLengthBytes *= sizeof(signed short);

            mDelayBufferMemory = (signed short *)FMOD_Memory_Calloc(mDelayBufferLengthBytes + 16);
            mDelayBuffer = (signed short *)FMOD_ALIGNPOINTER(mDelayBufferMemory, 16);
#endif
        }
        if (!mDelayBufferMemory)
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
FMOD_RESULT DSPDelay::releaseInternal()
{
    if (mDelayBufferMemory)
    {
        FMOD_Memory_Free(mDelayBufferMemory);
        mDelayBufferMemory = mDelayBuffer = 0;
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
FMOD_RESULT DSPDelay::resetInternal()
{
#ifdef DELAY_INTERLEAVED
    mWritePosition = 0;
    for (int count = 0; count < DELAY_MAX_CHANNELS; count++)
    {
        mReadPosition[count] = (mOffset[count] == 0) ? 0 : mDelayBufferLength - mOffset[count];
    }
#else
    for (int count = 0; count < DELAY_MAX_CHANNELS; count++)
    {
        mBufferStart[count] = mDelayBufferLength * count;
        mWritePosition[count] = mReadPosition[count] = mBufferStart[count];
        mReadPosition[count] += (mOffset[count] == 0) ? 0 : mDelayBufferLength - mOffset[count];
    }
    mBufferStart[DELAY_MAX_CHANNELS] = mDelayBufferLength * DELAY_MAX_CHANNELS;
#endif

    if (mDelayBuffer)
    {
        FMOD_memset(mDelayBuffer, 0, mDelayBufferLengthBytes);
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
FMOD_RESULT DSPDelay::readInternal(float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int outchannels)
{   
    unsigned int count;
    bool reallocbuffer = false;

    #define OO_32767 (1.0f / 32767.0f)

    if (!inbuffer)
    {
        return FMOD_OK;
    }

    int outputrate = 0;

    mSystem->getSoftwareFormat(&outputrate, 0, 0, 0, 0, 0);

    /*
        Update parameters
    */
    if (mChannels != inchannels)
    {
        mChannels = inchannels;

        reallocbuffer = true;
    }
    if (mMaxDelay != mMaxDelayUpdate)
    {
        mMaxDelay = mMaxDelayUpdate;

        reallocbuffer = true;
    }

    if (reallocbuffer || !mDelayBuffer)
    {
        /* 
            need one extra sample as input is written to buffer before output is read
            allowing for a delay of zero samples
        */
        mDelayBufferLength = (int)(((float)outputrate * mMaxDelay / 1000.0f) + 0.5f) + 1;

        if (mDelayBufferMemory)
        {
            FMOD_Memory_Free(mDelayBufferMemory);
            mDelayBufferMemory = mDelayBuffer = 0;
        }

        mDelayBufferLengthBytes = mDelayBufferLength;
        mDelayBufferLengthBytes *= mChannels;

        if (!mDelayBufferMemory)
        {
#ifdef DELAY_USEFLOAT
            mDelayBufferLengthBytes *= sizeof(float);

            mDelayBufferMemory = (float *)FMOD_Memory_Calloc(mDelayBufferLengthBytes + 16);
            mDelayBuffer = (float *)FMOD_ALIGNPOINTER(mDelayBufferMemory, 16);
#else
            mDelayBufferLengthBytes *= sizeof(signed short);

            mDelayBufferMemory = (signed short *)FMOD_Memory_Calloc(mDelayBufferLengthBytes + 16);
            mDelayBuffer = (signed short *)FMOD_ALIGNPOINTER(mDelayBufferMemory, 16);
#endif
        }
        if (!mDelayBufferMemory)
        {
            return FMOD_ERR_MEMORY;
        }
    }

    for (count = 0; count < (unsigned int)inchannels; count++ )
    {
        if (mDelay[count] != mDelayUpdate[count])
        {
            if (mDelayUpdate[count] > mMaxDelay)
            {
                mDelayUpdate[count] = mMaxDelay;
            }
            mDelay[count]        = mDelayUpdate[count];
            mOffset[count]       = (int)(((float)outputrate * mDelay[count] / 1000.f) + 0.5f);
#ifdef DELAY_INTERLEAVED
            mReadPosition[count] = mWritePosition - mOffset[count];
            if (mReadPosition[count] < 0)
            {
                mReadPosition[count] += mDelayBufferLength;
            }
#else
            mReadPosition[count] = mWritePosition[count] - mOffset[count];
            if (mReadPosition[count] < mBufferStart[count])
            {
                mReadPosition[count] += mDelayBufferLength;
            }
#endif
        }
    }

    if (reallocbuffer || !mDelayBuffer)
    {
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
				int len = mDelayBufferLength * inchannels;
				for(count2 = count; count2 < len; count2 += inchannels)
				{
					mDelayBuffer[count2] = 0;
				}
			}
		}
		mOldSpeakerMask = speakermask;
	}

    if (!mDelayBuffer || !(speakermask & ((1 << inchannels)-1)) )
    {
        FMOD_memcpy(outbuffer, inbuffer, length * outchannels * sizeof(float));
        return FMOD_OK;
    }

#ifdef DELAY_INTERLEAVED
    if (inchannels == 1 && (speakermask & 1))
    {
        unsigned int len;
        float *out, *in;
#ifdef DELAY_USEFLOAT
        float *delaybuffwrite;
        float *delaybuffread;
#else
        short *delaybuffwrite;
        short *delaybuffread;
#endif
        in  = inbuffer;
        out = outbuffer;
        
        delaybuffwrite = mDelayBuffer + (mWritePosition * 1);
        delaybuffread = (mDelayBuffer+0) + (mReadPosition[0] * 1);

        len = length;
        while (len)
        {
            unsigned int len2 = len;

            if (mWritePosition + (int)len > mDelayBufferLength)
            {
                len2 = mDelayBufferLength - mWritePosition;
            }

            for (count = 0; count < len2; count++)
            {
                float in0 = in[0];

#ifdef DELAY_USEFLOAT
                delaybuffwrite[0] = in0;
                out[0] = delaybuffread[0];
#else
                delaybuffwrite[0] = in0 > 1.0f ? 32767 : in0 < -1.0f ? -32768: (signed short)(in0 * 32767.0f);
                out[0] = (float)delaybuffread[0] * OO_32767;
#endif
                in++;
                out++;
                delaybuffwrite++;
                if (++mReadPosition[0] >= mDelayBufferLength)
                {
                    mReadPosition[0] = 0;
                    delaybuffread = (mDelayBuffer+0);
                }
                else
                {
                    delaybuffread++;
                }
	        }

		    mWritePosition += len2;
		    if (mWritePosition >= mDelayBufferLength)
            {
                mWritePosition = 0;
                delaybuffwrite = mDelayBuffer;
            }
            len -= len2;
        }
    }
    else if (inchannels == 2 && (speakermask & 0x3)==0x3)
    {
        unsigned int len;
        float *out, *in;
#ifdef DELAY_USEFLOAT
        float *delaybuffwrite;
        float *delaybuffread[2];
#else
        short *delaybuffwrite;
        short *delaybuffread[2];
#endif
        in  = inbuffer;
        out = outbuffer;
        
        delaybuffwrite = mDelayBuffer + (mWritePosition * 2);
        delaybuffread[0] = (mDelayBuffer+0) + (mReadPosition[0] * 2);
        delaybuffread[1] = (mDelayBuffer+1) + (mReadPosition[1] * 2);

        len = length;
        while (len)
        {
            unsigned int len2 = len;

            if (mWritePosition + (int)len > mDelayBufferLength)
            {
                len2 = mDelayBufferLength - mWritePosition;
            }

            for (count = 0; count < len2; count++)
            {
                float in0 = in[0];
                float in1 = in[1];

#ifdef DELAY_USEFLOAT
                delaybuffwrite[0] = in0;
                delaybuffwrite[1] = in1;
                out[0] = *delaybuffread[0];
                out[1] = *delaybuffread[1];
#else
                delaybuffwrite[0] = in0 > 1.0f ? 32767 : in0 < -1.0f ? -32768: (signed short)(in0 * 32767.0f);
                delaybuffwrite[1] = in1 > 1.0f ? 32767 : in1 < -1.0f ? -32768: (signed short)(in1 * 32767.0f);
                out[0] = (float)*delaybuffread[0] * OO_32767;
                out[1] = (float)*delaybuffread[1] * OO_32767;
#endif
                in += 2;
                out += 2;
                delaybuffwrite += 2;
                if (++mReadPosition[0] >= mDelayBufferLength)
                {
                    mReadPosition[0] = 0;
                    delaybuffread[0] = (mDelayBuffer+0);
                }
				else
				{
					delaybuffread[0] += 2;
				}
                if (++mReadPosition[1] >= mDelayBufferLength)
                {
                    mReadPosition[1] = 0;
                    delaybuffread[1] = (mDelayBuffer+1);
                }
				else
				{
					delaybuffread[1] += 2;
				}
	        }

		    mWritePosition += len2;
		    if (mWritePosition >= mDelayBufferLength)
            {
                mWritePosition = 0;
                delaybuffwrite = mDelayBuffer;
            }
            len -= len2;
        }
    }
    else if (inchannels == 6 && (speakermask & 0x3F)==0x3F)
    {
        unsigned int len;
        float *out, *in;
        int ch;
#ifdef DELAY_USEFLOAT
        float *delaybuffwrite;
        float *delaybuffread[6];
#else
        short *delaybuffwrite;
        short *delaybuffread[6];
#endif
        in  = inbuffer;
        out = outbuffer;
        
        delaybuffwrite = mDelayBuffer + (mWritePosition * 6);
        delaybuffread[0] = (mDelayBuffer+0) + (mReadPosition[0] * 6);
        delaybuffread[1] = (mDelayBuffer+1) + (mReadPosition[1] * 6);
        delaybuffread[2] = (mDelayBuffer+2) + (mReadPosition[2] * 6);
        delaybuffread[3] = (mDelayBuffer+3) + (mReadPosition[3] * 6);
        delaybuffread[4] = (mDelayBuffer+4) + (mReadPosition[4] * 6);
        delaybuffread[5] = (mDelayBuffer+5) + (mReadPosition[5] * 6);

        len = length;
        while (len)
        {
            unsigned int len2 = len;

            if (mWritePosition + (int)len > mDelayBufferLength)
            {
                len2 = mDelayBufferLength - mWritePosition;
            }

            for (count = 0; count < len2; count++)
            {
                float in0 = in[0];
                float in1 = in[1];
                float in2 = in[2];
                float in3 = in[3];
                float in4 = in[4];
                float in5 = in[5];

#ifdef DELAY_USEFLOAT
                delaybuffwrite[0] = in0;
                delaybuffwrite[1] = in1;
                delaybuffwrite[2] = in2;
                delaybuffwrite[3] = in3;
                delaybuffwrite[4] = in4;
                delaybuffwrite[5] = in5;
                out[0] = *delaybuffread[0];
                out[1] = *delaybuffread[1];
                out[2] = *delaybuffread[2];
                out[3] = *delaybuffread[3];
                out[4] = *delaybuffread[4];
                out[5] = *delaybuffread[5];
#else
                delaybuffwrite[0] = in0 > 1.0f ? 32767 : in0 < -1.0f ? -32768: (signed short)(in0 * 32767.0f);
                delaybuffwrite[1] = in1 > 1.0f ? 32767 : in1 < -1.0f ? -32768: (signed short)(in1 * 32767.0f);
                delaybuffwrite[2] = in2 > 1.0f ? 32767 : in2 < -1.0f ? -32768: (signed short)(in2 * 32767.0f);
                delaybuffwrite[3] = in3 > 1.0f ? 32767 : in3 < -1.0f ? -32768: (signed short)(in3 * 32767.0f);
                delaybuffwrite[4] = in4 > 1.0f ? 32767 : in4 < -1.0f ? -32768: (signed short)(in4 * 32767.0f);
                delaybuffwrite[5] = in5 > 1.0f ? 32767 : in5 < -1.0f ? -32768: (signed short)(in5 * 32767.0f);
                out[0] = (float)*delaybuffread[0] * OO_32767;
                out[1] = (float)*delaybuffread[1] * OO_32767;
                out[2] = (float)*delaybuffread[2] * OO_32767;
                out[3] = (float)*delaybuffread[3] * OO_32767;
                out[4] = (float)*delaybuffread[4] * OO_32767;
                out[5] = (float)*delaybuffread[5] * OO_32767;
#endif
                in += 6;
                out += 6;
                delaybuffwrite += 6;
                for (ch = 0; ch < 6; ch++)
                {
                    if (++mReadPosition[ch] >= mDelayBufferLength)
                    {
                        mReadPosition[ch] = 0;
                        delaybuffread[ch] = (mDelayBuffer+ch);
                    }
					else
					{
						delaybuffread[ch] += 6;
					}
                }
            }

		    mWritePosition += len2;
		    if (mWritePosition >= mDelayBufferLength)
            {
                mWritePosition = 0;
                delaybuffwrite = mDelayBuffer;
            }
            len -= len2;
        }
    }
    else if (inchannels == 8 && (speakermask & 0xFF)==0xFF)
    {
        unsigned int len;
        float *out, *in;
        int ch;
#ifdef DELAY_USEFLOAT
        float *delaybuffwrite;
        float *delaybuffread[8];
#else
        short *delaybuffwrite;
        short *delaybuffread[8];
#endif
        in  = inbuffer;
        out = outbuffer;
        
        delaybuffwrite = mDelayBuffer + (mWritePosition * 8);
        delaybuffread[0] = (mDelayBuffer+0) + (mReadPosition[0] * 8);
        delaybuffread[1] = (mDelayBuffer+1) + (mReadPosition[1] * 8);
        delaybuffread[2] = (mDelayBuffer+2) + (mReadPosition[2] * 8);
        delaybuffread[3] = (mDelayBuffer+3) + (mReadPosition[3] * 8);
        delaybuffread[4] = (mDelayBuffer+4) + (mReadPosition[4] * 8);
        delaybuffread[5] = (mDelayBuffer+5) + (mReadPosition[5] * 8);
        delaybuffread[6] = (mDelayBuffer+6) + (mReadPosition[6] * 8);
        delaybuffread[7] = (mDelayBuffer+7) + (mReadPosition[7] * 8);

        len = length;
        while (len)
        {
            unsigned int len2 = len;

            if (mWritePosition + (int)len > mDelayBufferLength)
            {
                len2 = mDelayBufferLength - mWritePosition;
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

#ifdef DELAY_USEFLOAT
                delaybuffwrite[0] = in0;
                delaybuffwrite[1] = in1;
                delaybuffwrite[2] = in2;
                delaybuffwrite[3] = in3;
                delaybuffwrite[4] = in4;
                delaybuffwrite[5] = in5;
                delaybuffwrite[6] = in6;
                delaybuffwrite[7] = in7;
                out[0] = *delaybuffread[0];
                out[1] = *delaybuffread[1];
                out[2] = *delaybuffread[2];
                out[3] = *delaybuffread[3];
                out[4] = *delaybuffread[4];
                out[5] = *delaybuffread[5];
                out[6] = *delaybuffread[6];
                out[7] = *delaybuffread[7];
#else
                delaybuffwrite[0] = in0 > 1.0f ? 32767 : in0 < -1.0f ? -32768: (signed short)(in0 * 32767.0f);
                delaybuffwrite[1] = in1 > 1.0f ? 32767 : in1 < -1.0f ? -32768: (signed short)(in1 * 32767.0f);
                delaybuffwrite[2] = in2 > 1.0f ? 32767 : in2 < -1.0f ? -32768: (signed short)(in2 * 32767.0f);
                delaybuffwrite[3] = in3 > 1.0f ? 32767 : in3 < -1.0f ? -32768: (signed short)(in3 * 32767.0f);
                delaybuffwrite[4] = in4 > 1.0f ? 32767 : in4 < -1.0f ? -32768: (signed short)(in4 * 32767.0f);
                delaybuffwrite[5] = in5 > 1.0f ? 32767 : in5 < -1.0f ? -32768: (signed short)(in5 * 32767.0f);
                delaybuffwrite[6] = in6 > 1.0f ? 32767 : in6 < -1.0f ? -32768: (signed short)(in6 * 32767.0f);
                delaybuffwrite[7] = in7 > 1.0f ? 32767 : in7 < -1.0f ? -32768: (signed short)(in7 * 32767.0f);
                out[0] = (float)*delaybuffread[0] * OO_32767;
                out[1] = (float)*delaybuffread[1] * OO_32767;
                out[2] = (float)*delaybuffread[2] * OO_32767;
                out[3] = (float)*delaybuffread[3] * OO_32767;
                out[4] = (float)*delaybuffread[4] * OO_32767;
                out[5] = (float)*delaybuffread[5] * OO_32767;
                out[6] = (float)*delaybuffread[6] * OO_32767;
                out[7] = (float)*delaybuffread[7] * OO_32767;
#endif
                in += 8;
                out += 8;
                delaybuffwrite += 8;
                for (ch = 0; ch < 8; ch++)
                {
                    if (++mReadPosition[ch] >= mDelayBufferLength)
                    {
                        mReadPosition[ch] = 0;
                        delaybuffread[ch] = (mDelayBuffer+ch);
                    }
                    else
                    {
                        delaybuffread[ch] += 8;
                    }
                }
            }

		    mWritePosition += len2;
		    if (mWritePosition >= mDelayBufferLength)
            {
                mWritePosition = 0;
                delaybuffwrite = mDelayBuffer;
            }
            len -= len2;
        }
    }
    else
    {
        unsigned int len;
        float *out, *in;
        int ch;

#ifdef DELAY_USEFLOAT
        float *delaybuffwrite;
        float *delaybuffread[DELAY_MAX_CHANNELS];
#else
        short *delaybuffwrite;
        short *delaybuffread[DELAY_MAX_CHANNELS];
#endif
        in  = inbuffer;
        out = outbuffer;
        
        delaybuffwrite = mDelayBuffer + (mWritePosition * inchannels);
        for (ch = 0; ch < inchannels; ch++)
        {
            delaybuffread[ch] = (mDelayBuffer+ch) + (mReadPosition[ch] * inchannels);
        }

        len = length;
        while (len)
        {
            unsigned int len2 = len;

            if (mWritePosition + (int)len > mDelayBufferLength)
            {
                len2 = mDelayBufferLength - mWritePosition;
            }

            for (count = 0; count < len2; count++)
            {
#ifdef DELAY_USEFLOAT
                for (ch = 0; ch < inchannels; ch++)
                {
                    delaybuffwrite[ch] = in[ch];
                    out[ch] = *delaybuffread[ch];
                }
#else
                for (ch = 0; ch < inchannels; ch++)
                {
                    delaybuffwrite[ch] = in[ch] > 1.0f ? 32767 : in[ch] < -1.0f ? -32768: (signed short)(in[ch] * 32767.0f);
                    out[ch] = (float)*delaybuffread[ch] * OO_32767;
                }
#endif
                in += inchannels;
                out += inchannels;
                delaybuffwrite += inchannels;
                for (ch = 0; ch < inchannels; ch++)
                {
                    if (++mReadPosition[ch] >= mDelayBufferLength)
                    {
                        mReadPosition[ch] = 0;
                        delaybuffread[ch] = (mDelayBuffer+ch);
                    }
                    else
                    {
                        delaybuffread[ch] += inchannels;
                    }
                }
            }

		    mWritePosition += len2;
		    if (mWritePosition >= mDelayBufferLength)
            {
                mWritePosition = 0;
                delaybuffwrite = mDelayBuffer;
            }
            len -= len2;
        }
    }
#else // !DELAY_INTERLEAVED
    if (inchannels == 1 && (speakermask & 1))
    {
        unsigned int len;
        float *out, *in;
#ifdef DELAY_USEFLOAT
        float *delaybuffwrite;
        float *delaybuffread;
#else
        short *delaybuffwrite;
        short *delaybuffread;
#endif
        in  = inbuffer;
        out = outbuffer;
        
        delaybuffwrite = mDelayBuffer + mWritePosition[0];
        delaybuffread = mDelayBuffer + mReadPosition[0];

        len = length;
        while (len)
        {
            unsigned int len2 = len;

            if (mWritePosition[0] + (int)len > mDelayBufferLength)
            {
                len2 = mDelayBufferLength - mWritePosition[0];
            }

            for (count = 0; count < len2; count++)
            {
                float in0 = in[0];

#ifdef DELAY_USEFLOAT
                *(delaybuffwrite++) = in0;
                out[0] = *delaybuffread;
#else
                *(delaybuffwrite++) = in0 > 1.0f ? 32767 : in0 < -1.0f ? -32768: (signed short)(in0 * 32767.0f);
                out[0] = (float)*delaybuffread * OO_32767;
#endif
                in++;
                out++;
                if (++mReadPosition[0] >= mBufferStart[1])
                {
                    mReadPosition[0] = 0;
                    delaybuffread = mDelayBuffer;
                }
                else
                {
                    delaybuffread++;
                }
	        }

		    mWritePosition[0] += len2;
		    if (mWritePosition[0] >= mDelayBufferLength)
            {
                mWritePosition[0] = 0;
                delaybuffwrite = mDelayBuffer;
            }
            len -= len2;
        }
    }
    else if (inchannels == 2 && (speakermask & 0x3)==0x3)
    {
        unsigned int len;
        float *out, *in;
#ifdef DELAY_USEFLOAT
        float *delaybuffwrite[2];
        float *delaybuffread[2];
#else
        short *delaybuffwrite[2];
        short *delaybuffread[2];
#endif
        in  = inbuffer;
        out = outbuffer;
        
        delaybuffwrite[0] = mDelayBuffer + mWritePosition[0];
        delaybuffwrite[1] = mDelayBuffer + mWritePosition[1];
        delaybuffread[0] = mDelayBuffer + mReadPosition[0];
        delaybuffread[1] = mDelayBuffer + mReadPosition[1];

        len = length;
        while (len)
        {
            unsigned int len2 = len;

            if (mWritePosition[0] + (int)len > mDelayBufferLength)
            {
                len2 = mDelayBufferLength - mWritePosition[0];
            }

            for (count = 0; count < len2; count++)
            {
                float in0 = in[0];
                float in1 = in[1];

#ifdef DELAY_USEFLOAT
                *(delaybuffwrite[0]++) = in0;
                *(delaybuffwrite[1]++) = in1;
                out[0] = *delaybuffread[0];
                out[1] = *delaybuffread[1];
#else
                *(delaybuffwrite[0]++) = in0 > 1.0f ? 32767 : in0 < -1.0f ? -32768: (signed short)(in0 * 32767.0f);
                *(delaybuffwrite[1]++) = in1 > 1.0f ? 32767 : in1 < -1.0f ? -32768: (signed short)(in1 * 32767.0f);
                out[0] = (float)*delaybuffread[0] * OO_32767;
                out[1] = (float)*delaybuffread[1] * OO_32767;
#endif
                in += 2;
                out += 2;
                if (++mReadPosition[0] >= mBufferStart[1])
                {
                    mReadPosition[0] = 0;
                    delaybuffread[0] = mDelayBuffer;
                }
				else
				{
					delaybuffread[0]++;
				}
                if (++mReadPosition[1] >= mBufferStart[2])
                {
                    mReadPosition[1] = mBufferStart[1];
                    delaybuffread[1] = mDelayBuffer + mBufferStart[1];
                }
				else
				{
					delaybuffread[1]++;
				}
	        }

		    if (mWritePosition[0] + (int)len2 >= mDelayBufferLength)
            {
                mWritePosition[0] = 0;
                mWritePosition[1] = mBufferStart[1];
                delaybuffwrite[0] = mDelayBuffer;
                delaybuffwrite[1] = mDelayBuffer + mBufferStart[1];
            }
            else
            {
		        mWritePosition[0] += len2;
		        mWritePosition[1] += len2;
            }
            len -= len2;
        }
    }
    else if (inchannels == 6 && (speakermask & 0x3F)==0x3F)
    {
        unsigned int len;
        float *out, *in;
        int ch;
#ifdef DELAY_USEFLOAT
        float *delaybuffwrite[6];
        float *delaybuffread[6];
#else
        short *delaybuffwrite[6];
        short *delaybuffread[6];
#endif
        in  = inbuffer;
        out = outbuffer;
        
        delaybuffwrite[0] = mDelayBuffer + mWritePosition[0];
        delaybuffwrite[1] = mDelayBuffer + mWritePosition[1];
        delaybuffwrite[2] = mDelayBuffer + mWritePosition[2];
        delaybuffwrite[3] = mDelayBuffer + mWritePosition[3];
        delaybuffwrite[4] = mDelayBuffer + mWritePosition[4];
        delaybuffwrite[5] = mDelayBuffer + mWritePosition[5];
        delaybuffread[0] = mDelayBuffer + mReadPosition[0];
        delaybuffread[1] = mDelayBuffer + mReadPosition[1];
        delaybuffread[2] = mDelayBuffer + mReadPosition[2];
        delaybuffread[3] = mDelayBuffer + mReadPosition[3];
        delaybuffread[4] = mDelayBuffer + mReadPosition[4];
        delaybuffread[5] = mDelayBuffer + mReadPosition[5];

        len = length;
        while (len)
        {
            unsigned int len2 = len;

            if (mWritePosition[0] + (int)len > mDelayBufferLength)
            {
                len2 = mDelayBufferLength - mWritePosition[0];
            }

            for (count = 0; count < len2; count++)
            {
                float in0 = in[0];
                float in1 = in[1];
                float in2 = in[2];
                float in3 = in[3];
                float in4 = in[4];
                float in5 = in[5];

#ifdef DELAY_USEFLOAT
                *(delaybuffwrite[0]++) = in0;
                *(delaybuffwrite[1]++) = in1;
                *(delaybuffwrite[2]++) = in2;
                *(delaybuffwrite[3]++) = in3;
                *(delaybuffwrite[4]++) = in4;
                *(delaybuffwrite[5]++) = in5;
                out[0] = *delaybuffread[0];
                out[1] = *delaybuffread[1];
                out[2] = *delaybuffread[2];
                out[3] = *delaybuffread[3];
                out[4] = *delaybuffread[4];
                out[5] = *delaybuffread[5];
#else
                *(delaybuffwrite[0]++) = in0 > 1.0f ? 32767 : in0 < -1.0f ? -32768: (signed short)(in0 * 32767.0f);
                *(delaybuffwrite[1]++) = in1 > 1.0f ? 32767 : in1 < -1.0f ? -32768: (signed short)(in1 * 32767.0f);
                *(delaybuffwrite[2]++) = in2 > 1.0f ? 32767 : in2 < -1.0f ? -32768: (signed short)(in2 * 32767.0f);
                *(delaybuffwrite[3]++) = in3 > 1.0f ? 32767 : in3 < -1.0f ? -32768: (signed short)(in3 * 32767.0f);
                *(delaybuffwrite[4]++) = in4 > 1.0f ? 32767 : in4 < -1.0f ? -32768: (signed short)(in4 * 32767.0f);
                *(delaybuffwrite[5]++) = in5 > 1.0f ? 32767 : in5 < -1.0f ? -32768: (signed short)(in5 * 32767.0f);
                out[0] = (float)*delaybuffread[0] * OO_32767;
                out[1] = (float)*delaybuffread[1] * OO_32767;
                out[2] = (float)*delaybuffread[2] * OO_32767;
                out[3] = (float)*delaybuffread[3] * OO_32767;
                out[4] = (float)*delaybuffread[4] * OO_32767;
                out[5] = (float)*delaybuffread[5] * OO_32767;
#endif
                in += 6;
                out += 6;
                for (ch = 0; ch < 6; ch++)
                {
                    if (++mReadPosition[ch] >= mBufferStart[ch+1])
                    {
                        mReadPosition[ch] = mBufferStart[ch];
                        delaybuffread[ch] = mDelayBuffer + mBufferStart[ch];
                    }
				    else
				    {
					    delaybuffread[ch]++;
				    }
                }
	        }

		    if (mWritePosition[0] + (int)len2 >= mDelayBufferLength)
            {
                for (ch = 0; ch < 6; ch++)
                {
                    mWritePosition[ch] = mBufferStart[ch];
                    delaybuffwrite[ch] = mDelayBuffer + mBufferStart[ch];
                }
            }
            else
            {
                for (ch = 0; ch < 6; ch++)
                {
		            mWritePosition[ch] += len2;
                }
            }
            len -= len2;
        }
    }
    else if (inchannels == 8 && (speakermask & 0xFF)==0xFF)
    {
        unsigned int len;
        float *out, *in;
        int ch;
#ifdef DELAY_USEFLOAT
        float *delaybuffwrite[8];
        float *delaybuffread[8];
#else
        short *delaybuffwrite[8];
        short *delaybuffread[8];
#endif
        in  = inbuffer;
        out = outbuffer;
        
        delaybuffwrite[0] = mDelayBuffer + mWritePosition[0];
        delaybuffwrite[1] = mDelayBuffer + mWritePosition[1];
        delaybuffwrite[2] = mDelayBuffer + mWritePosition[2];
        delaybuffwrite[3] = mDelayBuffer + mWritePosition[3];
        delaybuffwrite[4] = mDelayBuffer + mWritePosition[4];
        delaybuffwrite[5] = mDelayBuffer + mWritePosition[5];
        delaybuffwrite[6] = mDelayBuffer + mWritePosition[6];
        delaybuffwrite[7] = mDelayBuffer + mWritePosition[7];
        delaybuffread[0] = mDelayBuffer + mReadPosition[0];
        delaybuffread[1] = mDelayBuffer + mReadPosition[1];
        delaybuffread[2] = mDelayBuffer + mReadPosition[2];
        delaybuffread[3] = mDelayBuffer + mReadPosition[3];
        delaybuffread[4] = mDelayBuffer + mReadPosition[4];
        delaybuffread[5] = mDelayBuffer + mReadPosition[5];
        delaybuffread[6] = mDelayBuffer + mReadPosition[6];
        delaybuffread[7] = mDelayBuffer + mReadPosition[7];

        len = length;
        while (len)
        {
            unsigned int len2 = len;

            if (mWritePosition[0] + (int)len > mDelayBufferLength)
            {
                len2 = mDelayBufferLength - mWritePosition[0];
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

#ifdef DELAY_USEFLOAT
                *(delaybuffwrite[0]++) = in0;
                *(delaybuffwrite[1]++) = in1;
                *(delaybuffwrite[2]++) = in2;
                *(delaybuffwrite[3]++) = in3;
                *(delaybuffwrite[4]++) = in4;
                *(delaybuffwrite[5]++) = in5;
                *(delaybuffwrite[6]++) = in6;
                *(delaybuffwrite[7]++) = in7;
                out[0] = *delaybuffread[0];
                out[1] = *delaybuffread[1];
                out[2] = *delaybuffread[2];
                out[3] = *delaybuffread[3];
                out[4] = *delaybuffread[4];
                out[5] = *delaybuffread[5];
                out[6] = *delaybuffread[6];
                out[7] = *delaybuffread[7];
#else
                *(delaybuffwrite[0]++) = in0 > 1.0f ? 32767 : in0 < -1.0f ? -32768: (signed short)(in0 * 32767.0f);
                *(delaybuffwrite[1]++) = in1 > 1.0f ? 32767 : in1 < -1.0f ? -32768: (signed short)(in1 * 32767.0f);
                *(delaybuffwrite[2]++) = in2 > 1.0f ? 32767 : in2 < -1.0f ? -32768: (signed short)(in2 * 32767.0f);
                *(delaybuffwrite[3]++) = in3 > 1.0f ? 32767 : in3 < -1.0f ? -32768: (signed short)(in3 * 32767.0f);
                *(delaybuffwrite[4]++) = in4 > 1.0f ? 32767 : in4 < -1.0f ? -32768: (signed short)(in4 * 32767.0f);
                *(delaybuffwrite[5]++) = in5 > 1.0f ? 32767 : in5 < -1.0f ? -32768: (signed short)(in5 * 32767.0f);
                *(delaybuffwrite[6]++) = in6 > 1.0f ? 32767 : in6 < -1.0f ? -32768: (signed short)(in6 * 32767.0f);
                *(delaybuffwrite[7]++) = in7 > 1.0f ? 32767 : in7 < -1.0f ? -32768: (signed short)(in7 * 32767.0f);
                out[0] = (float)*delaybuffread[0] * OO_32767;
                out[1] = (float)*delaybuffread[1] * OO_32767;
                out[2] = (float)*delaybuffread[2] * OO_32767;
                out[3] = (float)*delaybuffread[3] * OO_32767;
                out[4] = (float)*delaybuffread[4] * OO_32767;
                out[5] = (float)*delaybuffread[5] * OO_32767;
                out[6] = (float)*delaybuffread[6] * OO_32767;
                out[7] = (float)*delaybuffread[7] * OO_32767;
#endif
                in += 8;
                out += 8;
                for (ch = 0; ch < 8; ch++)
                {
                    if (++mReadPosition[ch] >= mBufferStart[ch+1])
                    {
                        mReadPosition[ch] = mBufferStart[ch];
                        delaybuffread[ch] = mDelayBuffer + mBufferStart[ch];
                    }
				    else
				    {
					    delaybuffread[ch]++;
				    }
                }
	        }

		    if (mWritePosition[0] + (int)len2 >= mDelayBufferLength)
            {
                for (ch = 0; ch < 8; ch++)
                {
                    mWritePosition[ch] = mBufferStart[ch];
                    delaybuffwrite[ch] = mDelayBuffer + mBufferStart[ch];
                }
            }
            else
            {
                for (ch = 0; ch < 8; ch++)
                {
		            mWritePosition[ch] += len2;
                }
            }
            len -= len2;
        }
    }
    else
    {
        unsigned int len;
        float *out, *in;
        int ch;

#ifdef DELAY_USEFLOAT
        float *delaybuffwrite[DELAY_MAX_CHANNELS];
        float *delaybuffread[DELAY_MAX_CHANNELS];
#else
        short *delaybuffwrite[DELAY_MAX_CHANNELS];
        short *delaybuffread[DELAY_MAX_CHANNELS];
#endif
        in  = inbuffer;
        out = outbuffer;
        
        for (ch = 0; ch < inchannels; ch++)
        {
            delaybuffwrite[ch] = mDelayBuffer + mWritePosition[ch];
            delaybuffread[ch] = mDelayBuffer + mReadPosition[ch];
        }

        len = length;
        while (len)
        {
            unsigned int len2 = len;

            if (mWritePosition[0] + (int)len > mDelayBufferLength)
            {
                len2 = mDelayBufferLength - mWritePosition[0];
            }

            for (count = 0; count < len2; count++)
            {
#ifdef DELAY_USEFLOAT
                for (ch = 0; ch < inchannels; ch++)
                {
                    *(delaybuffwrite[ch]++) = in[ch];
                    out[ch] = *delaybuffread[ch];
                }
#else
                for (ch = 0; ch < inchannels; ch++)
                {
                    *(delaybuffwrite[ch]++) = in[ch] > 1.0f ? 32767 : in[ch] < -1.0f ? -32768: (signed short)(in[ch] * 32767.0f);
                    out[ch] = (float)*delaybuffread[ch] * OO_32767;
                }
#endif
                in += inchannels;
                out += inchannels;
                for (ch = 0; ch < inchannels; ch++)
                {
                    if (++mReadPosition[ch] >= mBufferStart[ch+1])
                    {
                        mReadPosition[ch] = mBufferStart[ch];
                        delaybuffread[ch] = mDelayBuffer + mBufferStart[ch];
                    }
				    else
				    {
					    delaybuffread[ch]++;
				    }
                }
            }

		    if (mWritePosition[0] + (int)len2 >= mDelayBufferLength)
            {
                for (ch = 0; ch < inchannels; ch++)
                {
                    mWritePosition[ch] = mBufferStart[ch];
                    delaybuffwrite[ch] = mDelayBuffer + mBufferStart[ch];
                }
            }
            else
            {
                for (ch = 0; ch < inchannels; ch++)
                {
		            mWritePosition[ch] += len2;
                }
            }
            len -= len2;
        }
    }
#endif // DELAY_INTERLEAVED

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
FMOD_RESULT DSPDelay::setParameterInternal(int index, float value)
{
    switch (index)
    {
        case FMOD_DSP_DELAY_MAXDELAY:
        {
            mMaxDelayUpdate = value;
            break;
        }
        default:
        {
            if ((index >= FMOD_DSP_DELAY_CH0) && (index <= FMOD_DSP_DELAY_CH15))
            {
                mDelayUpdate[index-FMOD_DSP_DELAY_CH0] = value;
            }
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
FMOD_RESULT DSPDelay::getParameterInternal(int index, float *value, char *valuestr)
{
    switch (index)
    {
        case FMOD_DSP_DELAY_MAXDELAY:
        {
            *value = mMaxDelayUpdate;
            sprintf(valuestr, "%.02f", mMaxDelayUpdate);
            break;
        }
        default:
        {
            if ((index >= FMOD_DSP_DELAY_CH0) && (index <= FMOD_DSP_DELAY_CH15))
            {
                *value = mDelayUpdate[index-FMOD_DSP_DELAY_CH0];
                sprintf(valuestr, "%.02f", mDelayUpdate[index-FMOD_DSP_DELAY_CH0]);
            }
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

FMOD_RESULT DSPDelay::getMemoryUsedImpl(MemoryTracker *tracker)
{
    // Size of this class is already accounted for (via description.mSize). Just add extra allocated memory here.

    if (mDelayBufferMemory)
    {
        tracker->add(false, FMOD_MEMBITS_DSP, mDelayBufferLengthBytes + 16);  
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
FMOD_RESULT F_CALLBACK DSPDelay::createCallback(FMOD_DSP_STATE *dsp)
{
    DSPDelay *delay = (DSPDelay *)dsp;

    return delay->createInternal();
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
FMOD_RESULT F_CALLBACK DSPDelay::releaseCallback(FMOD_DSP_STATE *dsp)
{
    DSPDelay *delay = (DSPDelay *)dsp;

    return delay->releaseInternal();
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
FMOD_RESULT F_CALLBACK DSPDelay::resetCallback(FMOD_DSP_STATE *dsp)
{
    DSPDelay *delay = (DSPDelay *)dsp;

    return delay->resetInternal();
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
FMOD_RESULT F_CALLBACK DSPDelay::readCallback(FMOD_DSP_STATE *dsp, float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int outchannels)
{
    DSPDelay *delay = (DSPDelay *)dsp;

    return delay->readInternal(inbuffer, outbuffer, length, inchannels, outchannels);
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
FMOD_RESULT F_CALLBACK DSPDelay::setParameterCallback(FMOD_DSP_STATE *dsp, int index, float value)
{
    DSPDelay *delay = (DSPDelay *)dsp;

    return delay->setParameterInternal(index, value);
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
FMOD_RESULT F_CALLBACK DSPDelay::getParameterCallback(FMOD_DSP_STATE *dsp, int index, float *value, char *valuestr)
{
    DSPDelay *delay = (DSPDelay *)dsp;

    return delay->getParameterInternal(index, value, valuestr);
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
FMOD_RESULT F_CALLBACK DSPDelay::getMemoryUsedCallback(FMOD_DSP_STATE *dsp, MemoryTracker *tracker)
{
    DSPDelay *delay = (DSPDelay *)dsp;    

    return delay->DSPDelay::getMemoryUsed(tracker);
}
#endif


}

#endif
