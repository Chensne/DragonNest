#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_HIGHPASS

#include "fmod.h"
#include "fmod_dspi.h"
#include "fmod_dsp_highpass.h"

#ifdef PLATFORM_PS3_SPU
    #include "fmod_systemi_spu.h"
#else
    #include "fmod_systemi.h"
#endif

#include <stdio.h>

#ifdef PLATFORM_PS3
extern unsigned int _binary_spu_fmod_dsp_highpass_pic_start[];
#endif

namespace FMOD
{

FMOD_DSP_DESCRIPTION_EX dsphighpass;

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
        return DSPHighPass::getDescriptionEx();
    }

#ifdef __cplusplus
}
#endif

#endif  /* PLUGIN_EXPORTS */

#ifndef PLATFORM_PS3_SPU

FMOD_DSP_PARAMETERDESC dsphighpass_param[2] =
{
    { 1.0f,  22000.0f,  5000.0f, "Cutoff freq", "hz", "Highpass cutoff frequency in hz.   10.0 to 22000.0.  Default = 5000.0." },
    { 1.0f,     10.0f,     1.0f, "Resonance",   "",   "Highpass resonance Q value. 1.0 to 10.0.  Default = 1.0." }
};

#endif // PLATFORM_PS3_SPU

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
FMOD_DSP_DESCRIPTION_EX *DSPHighPass::getDescriptionEx()
{
#ifndef PLATFORM_PS3_SPU
    FMOD_memset(&dsphighpass, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));

    FMOD_strcpy(dsphighpass.name, "FMOD Highpass");
    dsphighpass.version       = 0x00010100;
    dsphighpass.create        = DSPHighPass::createCallback;
    dsphighpass.reset         = DSPHighPass::resetCallback;

    #ifdef PLATFORM_PS3
    dsphighpass.read          = (FMOD_DSP_READCALLBACK)_binary_spu_fmod_dsp_highpass_pic_start;    /* SPU PIC entry address */
    #else
    dsphighpass.read          = DSPHighPass::readCallback;
    #endif

    dsphighpass.numparameters = sizeof(dsphighpass_param) / sizeof(dsphighpass_param[0]);
    dsphighpass.paramdesc     = dsphighpass_param;
    dsphighpass.setparameter  = DSPHighPass::setParameterCallback;
    dsphighpass.getparameter  = DSPHighPass::getParameterCallback;
#ifdef FMOD_SUPPORT_MEMORYTRACKER
    dsphighpass.getmemoryused = &DSPHighPass::getMemoryUsedCallback;
#endif

    dsphighpass.mType         = FMOD_DSP_TYPE_HIGHPASS;
    dsphighpass.mCategory     = FMOD_DSP_CATEGORY_FILTER;
    dsphighpass.mSize         = sizeof(DSPHighPass);
#else
    dsphighpass.read          = DSPHighPass::readCallback;                  /* We only care about read function on SPU */
#endif

    return &dsphighpass;
}

#ifndef PLATFORM_PS3_SPU


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
FMOD_RESULT DSPHighPass::createInternal()
{
    int count;

    init();
   
    /*
        Calculat the max cutoff rate
    */
	int outputrate = 0;
	FMOD_RESULT result = mSystem->getSoftwareFormat(&outputrate, 0, 0, 0, 0, 0);
	if (result != FMOD_OK)
	{
		return result;
	}
	mCutoffHzMaximum = ((float)outputrate / 2.0f) - 10.0f;

    for (count = 0; count < mDescription.numparameters; count++)
    {
        FMOD_RESULT result;

        result = setParameter(count, mDescription.paramdesc[count].defaultval);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    resetInternal();

    /*
        Do a forced update here to make sure everything is calculated correctly first.
    */
    mResonance = mResonanceUpdate;
    mCutoffHz  = mCutoffHzUpdate;
    updateCoefficients(mResonance, mCutoffHz);

    return FMOD_OK;
}

#endif // !PLATFORM_PS3_SPU

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]    

	[SEE_ALSO]
]
*/
FMOD_RESULT DSPHighPass::resetInternal()
{
    int count;

    for (count=0; count < DSP_MAXLEVELS_MAX; count++)
    {
        mIn1[count]  = mIn2[count]  = 0;
        mOut1[count] = mOut2[count] = 0;
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
    
	[SEE_ALSO]
]
*/
FMOD_RESULT DSPHighPass::updateCoefficients(float resonance, float cutoff)
{
    int   outputrate;
    float w;

#ifdef PLATFORM_PS3_SPU
    outputrate = 48000;
#else
    mSystem->getSoftwareFormat(&outputrate, 0, 0, 0, 0, 0);
#endif

    w = FMOD_PI2 * (float)cutoff / outputrate;

    mCoefficient_B0 =  (1.0f + FMOD_COS(w)) / 2.0f;
    mCoefficient_B1 = -(1.0f + FMOD_COS(w));
    mCoefficient_B2 =  (1.0f + FMOD_COS(w)) / 2.0f;
    mCoefficient_A0 =   1.0f + FMOD_SIN(w) / (2 * resonance);
    mCoefficient_A1 =  -2.0f * FMOD_COS(w);
    mCoefficient_A2 =   1.0f - FMOD_SIN(w) / (2 * resonance);

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
    
	[SEE_ALSO]
]
*/
FMOD_RESULT DSPHighPass::process(float *inbuffer, float *outbuffer, unsigned int length, int channels)
{
    unsigned int count;
    int count2;
    static float dc = (float)1E-20;

    if (channels == 1 && (speakermask & 1))
    {
        for (count = 0; count < length; count++)
        {
            float in0, out0;
            
            in0 = inbuffer[count] + dc;
            
            out0 = (mCoefficient_B0 * in0 + mCoefficient_B1 * mIn1[0] + mCoefficient_B2 * mIn2[0] - mCoefficient_A1 * mOut1[0] - mCoefficient_A2 * mOut2[0]) / mCoefficient_A0;

	        mIn2[0] = mIn1[0];
            mIn1[0] = in0;

            mOut2[0] = mOut1[0];
            mOut1[0] = out0;

            outbuffer[count] = out0;

            dc = -dc;
        }                              
    }
    else if (channels == 2 && (speakermask & 0x3) == 0x3)
    {
        for (count = 0; count < length; count++)
        {
            float in0l, out0l;
            float in0r, out0r;
            
            in0l = inbuffer[count*2 + 0] + dc;
            in0r = inbuffer[count*2 + 1] + dc;
            
            out0l = (mCoefficient_B0 * in0l + mCoefficient_B1 * mIn1[0] + mCoefficient_B2 * mIn2[0] - mCoefficient_A1 * mOut1[0] - mCoefficient_A2 * mOut2[0]) / mCoefficient_A0;
            out0r = (mCoefficient_B0 * in0r + mCoefficient_B1 * mIn1[1] + mCoefficient_B2 * mIn2[1] - mCoefficient_A1 * mOut1[1] - mCoefficient_A2 * mOut2[1]) / mCoefficient_A0;

	        mIn2[0] = mIn1[0];
            mIn1[0] = in0l;
	        mIn2[1] = mIn1[1];
            mIn1[1] = in0r;

            mOut2[0] = mOut1[0];
            mOut1[0] = out0l;
            mOut2[1] = mOut1[1];
            mOut1[1] = out0r;

            outbuffer[count*2 + 0] = out0l;
            outbuffer[count*2 + 1] = out0r;

            dc = -dc;
        }                              
    }
	else if (channels == 6 && (speakermask & 0x3F) == 0x3F)
    {
        for (count = 0; count < length; count++)
        {
            float in0, out0;
            float in1, out1;
			float in2, out2;
			float in3, out3;
			float in4, out4;
			float in5, out5;
            
            in0 = inbuffer[count*6 + 0] + dc;
            in1 = inbuffer[count*6 + 1] + dc;
			in2 = inbuffer[count*6 + 2] + dc;
			in3 = inbuffer[count*6 + 3] + dc;
			in4 = inbuffer[count*6 + 4] + dc;
			in5 = inbuffer[count*6 + 5] + dc;
            
            out0 = (mCoefficient_B0 * in0 + mCoefficient_B1 * mIn1[0] + mCoefficient_B2 * mIn2[0] - mCoefficient_A1 * mOut1[0] - mCoefficient_A2 * mOut2[0]) / mCoefficient_A0;
            out1 = (mCoefficient_B0 * in1 + mCoefficient_B1 * mIn1[1] + mCoefficient_B2 * mIn2[1] - mCoefficient_A1 * mOut1[1] - mCoefficient_A2 * mOut2[1]) / mCoefficient_A0;
			out2 = (mCoefficient_B0 * in2 + mCoefficient_B1 * mIn1[2] + mCoefficient_B2 * mIn2[2] - mCoefficient_A1 * mOut1[2] - mCoefficient_A2 * mOut2[2]) / mCoefficient_A0;
			out3 = (mCoefficient_B0 * in3 + mCoefficient_B1 * mIn1[3] + mCoefficient_B2 * mIn2[3] - mCoefficient_A1 * mOut1[3] - mCoefficient_A2 * mOut2[3]) / mCoefficient_A0;
			out4 = (mCoefficient_B0 * in4 + mCoefficient_B1 * mIn1[4] + mCoefficient_B2 * mIn2[4] - mCoefficient_A1 * mOut1[4] - mCoefficient_A2 * mOut2[4]) / mCoefficient_A0;
			out5 = (mCoefficient_B0 * in5 + mCoefficient_B1 * mIn1[5] + mCoefficient_B2 * mIn2[5] - mCoefficient_A1 * mOut1[5] - mCoefficient_A2 * mOut2[5]) / mCoefficient_A0;

	        mIn2[0] = mIn1[0];
            mIn1[0] = in0;
	        mIn2[1] = mIn1[1];
            mIn1[1] = in1;
			mIn2[2] = mIn1[2];
            mIn1[2] = in2;
			mIn2[3] = mIn1[3];
            mIn1[3] = in3;
			mIn2[4] = mIn1[4];
            mIn1[4] = in4;
			mIn2[5] = mIn1[5];
            mIn1[5] = in5;

            mOut2[0] = mOut1[0];
            mOut1[0] = out0;
            mOut2[1] = mOut1[1];
            mOut1[1] = out1;
			mOut2[2] = mOut1[2];
            mOut1[2] = out2;
			mOut2[3] = mOut1[3];
            mOut1[3] = out3;
			mOut2[4] = mOut1[4];
            mOut1[4] = out4;
			mOut2[5] = mOut1[5];
            mOut1[5] = out5;

            outbuffer[count*6 + 0] = out0;
            outbuffer[count*6 + 1] = out1;
			outbuffer[count*6 + 2] = out2;
			outbuffer[count*6 + 3] = out3;
			outbuffer[count*6 + 4] = out4;
			outbuffer[count*6 + 5] = out5;

            dc = -dc;
        }                              
    }
	else if (channels == 8 && (speakermask & 0xFF) == 0xFF)
    {
        for (count = 0; count < length; count++)
        {
            float in0, out0;
            float in1, out1;
			float in2, out2;
			float in3, out3;
			float in4, out4;
			float in5, out5;
			float in6, out6;
			float in7, out7;
            
            in0 = inbuffer[count*8 + 0] + dc;
            in1 = inbuffer[count*8 + 1] + dc;
			in2 = inbuffer[count*8 + 2] + dc;
			in3 = inbuffer[count*8 + 3] + dc;
			in4 = inbuffer[count*8 + 4] + dc;
			in5 = inbuffer[count*8 + 5] + dc;
			in6 = inbuffer[count*8 + 6] + dc;
			in7 = inbuffer[count*8 + 7] + dc;
            
            out0 = (mCoefficient_B0 * in0 + mCoefficient_B1 * mIn1[0] + mCoefficient_B2 * mIn2[0] - mCoefficient_A1 * mOut1[0] - mCoefficient_A2 * mOut2[0]) / mCoefficient_A0;
            out1 = (mCoefficient_B0 * in1 + mCoefficient_B1 * mIn1[1] + mCoefficient_B2 * mIn2[1] - mCoefficient_A1 * mOut1[1] - mCoefficient_A2 * mOut2[1]) / mCoefficient_A0;
			out2 = (mCoefficient_B0 * in2 + mCoefficient_B1 * mIn1[2] + mCoefficient_B2 * mIn2[2] - mCoefficient_A1 * mOut1[2] - mCoefficient_A2 * mOut2[2]) / mCoefficient_A0;
			out3 = (mCoefficient_B0 * in3 + mCoefficient_B1 * mIn1[3] + mCoefficient_B2 * mIn2[3] - mCoefficient_A1 * mOut1[3] - mCoefficient_A2 * mOut2[3]) / mCoefficient_A0;
			out4 = (mCoefficient_B0 * in4 + mCoefficient_B1 * mIn1[4] + mCoefficient_B2 * mIn2[4] - mCoefficient_A1 * mOut1[4] - mCoefficient_A2 * mOut2[4]) / mCoefficient_A0;
			out5 = (mCoefficient_B0 * in5 + mCoefficient_B1 * mIn1[5] + mCoefficient_B2 * mIn2[5] - mCoefficient_A1 * mOut1[5] - mCoefficient_A2 * mOut2[5]) / mCoefficient_A0;
			out6 = (mCoefficient_B0 * in6 + mCoefficient_B1 * mIn1[6] + mCoefficient_B2 * mIn2[6] - mCoefficient_A1 * mOut1[6] - mCoefficient_A2 * mOut2[6]) / mCoefficient_A0;
			out7 = (mCoefficient_B0 * in7 + mCoefficient_B1 * mIn1[7] + mCoefficient_B2 * mIn2[7] - mCoefficient_A1 * mOut1[7] - mCoefficient_A2 * mOut2[7]) / mCoefficient_A0;

	        mIn2[0] = mIn1[0];
            mIn1[0] = in0;
	        mIn2[1] = mIn1[1];
            mIn1[1] = in1;
			mIn2[2] = mIn1[2];
            mIn1[2] = in2;
			mIn2[3] = mIn1[3];
            mIn1[3] = in3;
			mIn2[4] = mIn1[4];
            mIn1[4] = in4;
			mIn2[5] = mIn1[5];
            mIn1[5] = in5;
			mIn2[6] = mIn1[6];
            mIn1[6] = in6;
			mIn2[7] = mIn1[7];
            mIn1[7] = in7;

            mOut2[0] = mOut1[0];
            mOut1[0] = out0;
            mOut2[1] = mOut1[1];
            mOut1[1] = out1;
			mOut2[2] = mOut1[2];
            mOut1[2] = out2;
			mOut2[3] = mOut1[3];
            mOut1[3] = out3;
			mOut2[4] = mOut1[4];
            mOut1[4] = out4;
			mOut2[5] = mOut1[5];
            mOut1[5] = out5;
			mOut2[6] = mOut1[6];
            mOut1[6] = out6;
			mOut2[7] = mOut1[7];
            mOut1[7] = out7;

            outbuffer[count*8 + 0] = out0;
            outbuffer[count*8 + 1] = out1;
			outbuffer[count*8 + 2] = out2;
			outbuffer[count*8 + 3] = out3;
			outbuffer[count*8 + 4] = out4;
			outbuffer[count*8 + 5] = out5;
			outbuffer[count*8 + 6] = out6;
			outbuffer[count*8 + 7] = out7;

            dc = -dc;
        }                              
    }
    else
    {
		for (count2 = 0; count2 < channels; count2++)
		{
			float *in = inbuffer + count2;
			float *out = outbuffer + count2;
			int len;
            
			if (!((1 << count2) & speakermask))
			{	
				int inc;
				int offset1, offset2, offset3;
				offset1 = channels;  
				offset2 = channels * 2;  
				offset3 = channels * 3; 
				len = length >> 2;
				inc = channels << 2;

				while (len)
				{    
					out[0] = in[0];	
					out[offset1] = in[offset1];
					out[offset2] = in[offset2];
					out[offset3] = in[offset3];
					
					len--;
					in += inc;
					out += inc;
				}
				
				len = length & 3;

				while (len)
				{    
					out[0] = in[0];
					len--;
					in += channels;
					out += channels;
				}
				continue; 
			}
			else
			{
				for (count = 0; count < length; count++)
				{				
					float in0, out0;
		
					in0 = inbuffer[(count * channels) + count2] + dc;
		        
					out0 = (mCoefficient_B0 * in0 + mCoefficient_B1 * mIn1[count2] + mCoefficient_B2 * mIn2[count2] - mCoefficient_A1 * mOut1[count2] - mCoefficient_A2 * mOut2[count2]) / mCoefficient_A0;

					mIn2[count2] = mIn1[count2];
					mIn1[count2] = in0;

					mOut2[count2] = mOut1[count2];
					mOut1[count2] = out0;

					outbuffer[(count * channels) + count2] = out0;
					
					dc = -dc;
				}
			}
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
    
	[SEE_ALSO]
]
*/
FMOD_RESULT DSPHighPass::readInternal(float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int outchannels)
{
    if (!inbuffer)
    {
        return FMOD_OK;
    }

    while (mResonance != mResonanceUpdate || mCutoffHz != mCutoffHzUpdate)
    {
        float cutoffdelta = 10.0f + (mCutoffHz / 100.0f);

        if (mResonance < mResonanceUpdate)
        {
            mResonance += 1.0f;
            if (mResonance >= mResonanceUpdate)
            {
                mResonance = mResonanceUpdate;
            }           
        }
        else if (mResonance > mResonanceUpdate)
        {
            mResonance -= 1.0f;
            if (mResonance <= mResonanceUpdate)
            {
                mResonance = mResonanceUpdate;
            }           
        }
        if (mCutoffHz < mCutoffHzUpdate)
        {
            mCutoffHz += cutoffdelta;
            if (mCutoffHz >= mCutoffHzUpdate)
            {
                mCutoffHz = mCutoffHzUpdate;
            }           
        }
        else if (mCutoffHz > mCutoffHzUpdate)
        {
            mCutoffHz -= cutoffdelta;
            if (mCutoffHz <= mCutoffHzUpdate)
            {
                mCutoffHz = mCutoffHzUpdate;
            }           
        }

        updateCoefficients(mResonance, mCutoffHz);
        
        process(inbuffer, outbuffer, 1, inchannels);

        inbuffer+=inchannels;
        outbuffer+=inchannels;
        length--;
        if (!length)
        {
            return FMOD_OK;
        }
    }

	if (!(speakermask & ((1 << inchannels)-1))) /*No channels are active, copy in buffer to out buffer and skip the DSP*/
	{
		FMOD_memcpy(outbuffer, inbuffer, sizeof(float)*length*inchannels);
		return FMOD_OK;
	}

    if (mCutoffHz < 2.0f)
    {
        FMOD_memcpy(outbuffer, inbuffer, sizeof(float)*length*inchannels);
        return FMOD_OK;
    }
	else if (mCutoffHz >= mCutoffHzMaximum)
    {
        int count;

        FMOD_memset(outbuffer, 0, sizeof(float)*length*inchannels);

        for (count = 0; count < inchannels; count++)
        {
            mIn1[count]  = mIn2[count]  = 0;
            mOut1[count] = mOut2[count] = 0;
        }
        return FMOD_OK;
    }

    return process(inbuffer, outbuffer, length, inchannels);
}

#ifndef PLATFORM_PS3_SPU

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT DSPHighPass::setParameterInternal(int index, float value)
{   
    switch (index)
    {
        case FMOD_DSP_HIGHPASS_CUTOFF:
        {
            mCutoffHzUpdate = value;
            if (mCutoffHzUpdate >= mCutoffHzMaximum)
            {
                mCutoffHzUpdate = mCutoffHzMaximum;
            }
            break;
        }
        case FMOD_DSP_HIGHPASS_RESONANCE:
        {
            mResonanceUpdate = value;
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
FMOD_RESULT DSPHighPass::getParameterInternal(int index, float *value, char *valuestr)
{
    switch (index)
    {
        case FMOD_DSP_HIGHPASS_CUTOFF:
        {
            *value = mCutoffHzUpdate;
            sprintf(valuestr, "%.02f", mCutoffHzUpdate);
            break;
        }
        case FMOD_DSP_HIGHPASS_RESONANCE:
        {
            *value = mResonanceUpdate;
            sprintf(valuestr, "%.02f", mResonanceUpdate);
            break;
        }
    }

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
 
	[SEE_ALSO]
]
*/

#ifdef FMOD_SUPPORT_MEMORYTRACKER

FMOD_RESULT DSPHighPass::getMemoryUsedImpl(MemoryTracker *tracker)
{
    // Size of this class is already accounted for (via description.mSize). Just add extra allocated memory here.

    return FMOD_OK;
}

#endif


/*
    ==============================================================================================================

    CALLBACK INTERFACE

    ==============================================================================================================
*/

#ifndef PLATFORM_PS3_SPU

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
FMOD_RESULT F_CALLBACK DSPHighPass::createCallback(FMOD_DSP_STATE *dsp)
{
    DSPHighPass *highpass = (DSPHighPass *)dsp;

    return highpass->createInternal();
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
FMOD_RESULT F_CALLBACK DSPHighPass::resetCallback(FMOD_DSP_STATE *dsp)
{
    DSPHighPass *highpass = (DSPHighPass *)dsp;

    return highpass->resetInternal();
}

#endif //!PLATFORM_PS3_SPU

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
FMOD_RESULT F_CALLBACK DSPHighPass::readCallback(FMOD_DSP_STATE *dsp, float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int outchannels)
{
    DSPHighPass *highpass = (DSPHighPass *)dsp;

    return highpass->readInternal(inbuffer, outbuffer, length, inchannels, outchannels);
}

#ifndef PLATFORM_PS3_SPU

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
FMOD_RESULT F_CALLBACK DSPHighPass::setParameterCallback(FMOD_DSP_STATE *dsp, int index, float value)
{
    DSPHighPass *highpass = (DSPHighPass *)dsp;

    return highpass->setParameterInternal(index, value);
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
FMOD_RESULT F_CALLBACK DSPHighPass::getParameterCallback(FMOD_DSP_STATE *dsp, int index, float *value, char *valuestr)
{
    DSPHighPass *highpass = (DSPHighPass *)dsp;

    return highpass->getParameterInternal(index, value, valuestr);
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
FMOD_RESULT F_CALLBACK DSPHighPass::getMemoryUsedCallback(FMOD_DSP_STATE *dsp, MemoryTracker *tracker)
{
    DSPHighPass *highpass = (DSPHighPass *)dsp;    

    return highpass->DSPHighPass::getMemoryUsed(tracker);
}
#endif

#endif //!PLATFORM_PS3_SPU

}

#endif
