#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_LOWPASS2

#include "fmod.h"
#include "fmod_dspi.h"
#include "fmod_dsp_lowpass2.h"

#ifdef PLATFORM_PS3_SPU
    #include "fmod_systemi_spu.h"
#else
    #include "fmod_systemi.h"
#endif

#include <stdio.h>
#include <math.h>

#ifdef PLATFORM_PS3
extern unsigned int _binary_spu_fmod_dsp_lowpass2_pic_start[];
#endif

namespace FMOD
{

FMOD_DSP_DESCRIPTION_EX dsplowpass2;

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
        return DSPLowPass2::getDescriptionEx();
    }

#ifdef __cplusplus
}
#endif

#endif  /* PLUGIN_EXPORTS */

#ifndef PLATFORM_PS3_SPU

FMOD_DSP_PARAMETERDESC dsplowpass2_param[2] =
{
    {  1.0f, 22000.0f,  5000.0f, "Cutoff freq", "hz", "Lowpass cutoff frequency in hz.  1.0 to 22000.0.  Default = 5000.0" },
    {  1.0f,   127.0f,     1.0f, "Resonance",   "",   "Lowpass resonance Q value. 0.0 to 127.0.  Default = 1.0" }
};

#endif // PLATFORM_PS3_SPU

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
FMOD_DSP_DESCRIPTION_EX *DSPLowPass2::getDescriptionEx()
{
#ifndef PLATFORM_PS3_SPU
    FMOD_memset(&dsplowpass2, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));

    FMOD_strcpy(dsplowpass2.name, "FMOD IT Lowpass");
    dsplowpass2.version       = 0x00010100;
    dsplowpass2.create        = DSPLowPass2::createCallback;
    dsplowpass2.reset         = DSPLowPass2::resetCallback;

    #ifdef PLATFORM_PS3
    dsplowpass2.read          = (FMOD_DSP_READCALLBACK)_binary_spu_fmod_dsp_lowpass2_pic_start;    /* SPU PIC entry address */
    #else
    dsplowpass2.read          = DSPLowPass2::readCallback;
    #endif

    dsplowpass2.numparameters = sizeof(dsplowpass2_param) / sizeof(dsplowpass2_param[0]);
    dsplowpass2.paramdesc     = dsplowpass2_param;
    dsplowpass2.setparameter  = DSPLowPass2::setParameterCallback;
    dsplowpass2.getparameter  = DSPLowPass2::getParameterCallback;
#ifdef FMOD_SUPPORT_MEMORYTRACKER
    dsplowpass2.getmemoryused = &DSPLowPass2::getMemoryUsedCallback;
#endif

    dsplowpass2.mType         = FMOD_DSP_TYPE_ITLOWPASS;
    dsplowpass2.mCategory     = FMOD_DSP_CATEGORY_FILTER;
    dsplowpass2.mSize         = sizeof(DSPLowPass2);
#else
    dsplowpass2.read          = DSPLowPass2::readCallback;                  /* We only care about read function on SPU */
#endif
    return &dsplowpass2;
}

#ifndef PLATFORM_PS3_SPU

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
FMOD_RESULT DSPLowPass2::createInternal()
{
    int count;

    init();

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
FMOD_RESULT DSPLowPass2::resetInternal()
{
    int count;

    for (count = 0; count < DSP_MAXLEVELS_MAX; count++)
    {
    	mFilter_Y[count][0] = mFilter_Y[count][1] = 0;
    }

    return FMOD_OK;
}

#endif // PLATFORM_PS3_SPU

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
FMOD_RESULT DSPLowPass2::updateCoefficients(float resonance, float cutoff)
{
    int   outputrate;
    float fc, fs;
    float fg, fb0, fb1;

#ifdef PLATFORM_PS3_SPU
    outputrate = 48000;
#else
    mSystem->getSoftwareFormat(&outputrate, 0, 0, 0, 0, 0);
#endif

    fc = (float)cutoff;
    fs = (float)outputrate;

    fc *= (float)(2.0f * 3.14159265358f / fs);

    float dmpfac = FMOD_POW(10.0f, -((24.0f / 128.0f) * resonance) / 20.0f);
    float d = (1.0f - 2.0f * dmpfac) * fc;

    if (d > 2.0f) 
    {
        d = 2.0f;
    }

    d = (2.0f * dmpfac - d)/fc;
    float e = FMOD_POW(1.0f / fc, 2.0f);

    fg  = 1.0f / (1.0f + d + e);
    fb0 = (d + e + e) / (1 + d + e);
    fb1 = -e / (1 + d + e);

    if (fg < 0.00001f)
    {
        fg = 0;
    }

	if (fb0 > 1.999f && fb1 < -0.999f)
	{
		fb0 = 2.0f;
		fb1 = -1.0f;
	}

    mFilter_A0 = fg;
    mFilter_B0 = fb0;
    mFilter_B1 = fb1;

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
FMOD_RESULT DSPLowPass2::readInternal(float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int outchannels)
{
    static float dc = (float)1E-20;

    if (!inbuffer)
    {
        return FMOD_OK;
    }

    if (mResonance != mResonanceUpdate || mCutoffHz != mCutoffHzUpdate)
    {
        mResonance = mResonanceUpdate;
        mCutoffHz  = mCutoffHzUpdate;
        updateCoefficients(mResonance, mCutoffHz);
    }

	if (!(speakermask & ((1 << inchannels)-1)))  /*No speaker channels are active, copy in buffer to out buffer and skip the DSP*/
	{
		FMOD_memcpy(outbuffer, inbuffer, sizeof(float)*length*inchannels);
		return FMOD_OK;
	}

    if (mFilter_A0 == 0.0f && mFilter_B0 == 2.0f && mFilter_B1 == -1.0f)
    {
        int count;

        FMOD_memset(outbuffer, 0, sizeof(float)*length*inchannels);
        
        for (count = 0; count < inchannels; count++)
        {
            mFilter_Y[count][0] = 0.0f;
            mFilter_Y[count][1] = 0.0f;
        }
        return FMOD_OK;
    }
              
    if (inchannels == 1 && (speakermask & 1))
    {
	    float fy1 = mFilter_Y[0][0];
        float fy2 = mFilter_Y[0][1];

        while (length)
        {
            outbuffer[0] = (inbuffer[0] + dc) * mFilter_A0 + fy1 * mFilter_B0 + fy2 * mFilter_B1;
	        fy2 = fy1;
            fy1 = outbuffer[0];

            dc = -dc;
            length--;
            outbuffer++;
            inbuffer++;
        }

	    mFilter_Y[0][0] = fy1;
	    mFilter_Y[0][1] = fy2;
    }
    else if (inchannels == 2 && (speakermask & 0x3) == 0x3)
    {
	    float fyL1 = mFilter_Y[0][0];
	    float fyL2 = mFilter_Y[0][1];
	    float fyR1 = mFilter_Y[1][0];
	    float fyR2 = mFilter_Y[1][1];
        
        while (length)
        {
	        float fyL,fyR;
            
            fyL  = (inbuffer[0] + dc) * mFilter_A0 + fyL1 * mFilter_B0 + fyL2 * mFilter_B1;
            fyR  = (inbuffer[1] + dc) * mFilter_A0 + fyR1 * mFilter_B0 + fyR2 * mFilter_B1;
	        fyL2 = fyL1; 
            fyL1 = fyL;	        
	        fyR2 = fyR1; 
            fyR1 = fyR;


	        outbuffer[0] = fyL;
	        outbuffer[1] = fyR;

            dc = -dc;
            length--;
            outbuffer+=2;
            inbuffer+=2;
        }

	    mFilter_Y[0][0] = fyL1;
	    mFilter_Y[0][1] = fyL2;
	    mFilter_Y[1][0] = fyR1;
	    mFilter_Y[1][1] = fyR2;
    }
    else if (inchannels == 6 && (speakermask & 0x3F) == 0x3F )
    {
	    float fyA1 = mFilter_Y[0][0];
	    float fyA2 = mFilter_Y[0][1];
	    float fyB1 = mFilter_Y[1][0];
	    float fyB2 = mFilter_Y[1][1];
	    float fyC1 = mFilter_Y[2][0];
	    float fyC2 = mFilter_Y[2][1];
	    float fyD1 = mFilter_Y[3][0];
	    float fyD2 = mFilter_Y[3][1];
	    float fyE1 = mFilter_Y[4][0];
	    float fyE2 = mFilter_Y[4][1];
	    float fyF1 = mFilter_Y[5][0];
	    float fyF2 = mFilter_Y[5][1];

        while (length)
        {
	        float fyA,fyB,fyC,fyD,fyE,fyF;
            
            fyA  = (inbuffer[0] + dc) * mFilter_A0 + fyA1 * mFilter_B0 + fyA2 * mFilter_B1;
            fyB  = (inbuffer[1] + dc) * mFilter_A0 + fyB1 * mFilter_B0 + fyB2 * mFilter_B1;
            fyC  = (inbuffer[2] + dc) * mFilter_A0 + fyC1 * mFilter_B0 + fyC2 * mFilter_B1;
            fyD  = (inbuffer[3] + dc) * mFilter_A0 + fyD1 * mFilter_B0 + fyD2 * mFilter_B1;
            fyE  = (inbuffer[4] + dc) * mFilter_A0 + fyE1 * mFilter_B0 + fyE2 * mFilter_B1;
            fyF  = (inbuffer[5] + dc) * mFilter_A0 + fyF1 * mFilter_B0 + fyF2 * mFilter_B1;
	        fyA2 = fyA1; 
            fyA1 = fyA;	        
	        fyB2 = fyB1; 
            fyB1 = fyB;
	        fyC2 = fyC1; 
            fyC1 = fyC;
	        fyD2 = fyD1; 
            fyD1 = fyD;
	        fyE2 = fyE1; 
            fyE1 = fyE;
	        fyF2 = fyF1; 
            fyF1 = fyF;
	        outbuffer[0] = fyA;
	        outbuffer[1] = fyB;
	        outbuffer[2] = fyC;
	        outbuffer[3] = fyD;
	        outbuffer[4] = fyE;
	        outbuffer[5] = fyF;

            dc = -dc;
            length--;
            outbuffer+=6;
            inbuffer+=6;
        }

	    mFilter_Y[0][0] = fyA1;
	    mFilter_Y[0][1] = fyA2;
	    mFilter_Y[1][0] = fyB1;
	    mFilter_Y[1][1] = fyB2;
	    mFilter_Y[2][0] = fyC1;
	    mFilter_Y[2][1] = fyC2;
	    mFilter_Y[3][0] = fyD1;
	    mFilter_Y[3][1] = fyD2;
	    mFilter_Y[4][0] = fyE1;
	    mFilter_Y[4][1] = fyE2;
	    mFilter_Y[5][0] = fyF1;
	    mFilter_Y[5][1] = fyF2;
    }    
	else if (inchannels == 8 && (speakermask & 0xFF) == 0xFF )
    {
	    float fyA1 = mFilter_Y[0][0];
	    float fyA2 = mFilter_Y[0][1];
	    float fyB1 = mFilter_Y[1][0];
	    float fyB2 = mFilter_Y[1][1];
	    float fyC1 = mFilter_Y[2][0];
	    float fyC2 = mFilter_Y[2][1];
	    float fyD1 = mFilter_Y[3][0];
	    float fyD2 = mFilter_Y[3][1];
	    float fyE1 = mFilter_Y[4][0];
	    float fyE2 = mFilter_Y[4][1];
	    float fyF1 = mFilter_Y[5][0];
	    float fyF2 = mFilter_Y[5][1];
		float fyG1 = mFilter_Y[6][0];
	    float fyG2 = mFilter_Y[6][1];
		float fyH1 = mFilter_Y[7][0];
	    float fyH2 = mFilter_Y[7][1];

        while (length)
        {
	        float fyA,fyB,fyC,fyD,fyE,fyF,fyG,fyH;
            
            fyA  = (inbuffer[0] + dc) * mFilter_A0 + fyA1 * mFilter_B0 + fyA2 * mFilter_B1;
            fyB  = (inbuffer[1] + dc) * mFilter_A0 + fyB1 * mFilter_B0 + fyB2 * mFilter_B1;
            fyC  = (inbuffer[2] + dc) * mFilter_A0 + fyC1 * mFilter_B0 + fyC2 * mFilter_B1;
            fyD  = (inbuffer[3] + dc) * mFilter_A0 + fyD1 * mFilter_B0 + fyD2 * mFilter_B1;
            fyE  = (inbuffer[4] + dc) * mFilter_A0 + fyE1 * mFilter_B0 + fyE2 * mFilter_B1;
            fyF  = (inbuffer[5] + dc) * mFilter_A0 + fyF1 * mFilter_B0 + fyF2 * mFilter_B1;
			fyG  = (inbuffer[6] + dc) * mFilter_A0 + fyG1 * mFilter_B0 + fyG2 * mFilter_B1;
            fyH  = (inbuffer[7] + dc) * mFilter_A0 + fyH1 * mFilter_B0 + fyH2 * mFilter_B1;
	        fyA2 = fyA1; 
            fyA1 = fyA;	        
	        fyB2 = fyB1; 
            fyB1 = fyB;
	        fyC2 = fyC1; 
            fyC1 = fyC;
	        fyD2 = fyD1; 
            fyD1 = fyD;
	        fyE2 = fyE1; 
            fyE1 = fyE;
	        fyF2 = fyF1; 
            fyF1 = fyF;
			fyG2 = fyG1; 
            fyG1 = fyG;
			fyH2 = fyH1; 
            fyH1 = fyH;
	        outbuffer[0] = fyA;
	        outbuffer[1] = fyB;
	        outbuffer[2] = fyC;
	        outbuffer[3] = fyD;
	        outbuffer[4] = fyE;
	        outbuffer[5] = fyF;
			outbuffer[6] = fyG;
	        outbuffer[7] = fyH;

            dc = -dc;
            length--;
            outbuffer+=8;
            inbuffer+=8;
        }

	    mFilter_Y[0][0] = fyA1;
	    mFilter_Y[0][1] = fyA2;
	    mFilter_Y[1][0] = fyB1;
	    mFilter_Y[1][1] = fyB2;
	    mFilter_Y[2][0] = fyC1;
	    mFilter_Y[2][1] = fyC2;
	    mFilter_Y[3][0] = fyD1;
	    mFilter_Y[3][1] = fyD2;
	    mFilter_Y[4][0] = fyE1;
	    mFilter_Y[4][1] = fyE2;
	    mFilter_Y[5][0] = fyF1;
	    mFilter_Y[5][1] = fyF2;
	    mFilter_Y[6][0] = fyG1;
	    mFilter_Y[6][1] = fyG2;
	    mFilter_Y[7][0] = fyH1;
	    mFilter_Y[7][1] = fyH2;
    }    
    else
    {
        int count2;

        for (count2 = 0; count2 < inchannels; count2++)
        {
            float *in = inbuffer + count2;
            float *out = outbuffer + count2;
            int len;
            
			if (!((1<<count2) & speakermask)) //DSP effect is not active on the current speaker so: out = in
			{ 
				int inc;
				int offset1, offset2, offset3;
				
				offset1 = inchannels;  
				offset2 = inchannels * 2;  
				offset3 = inchannels * 3; 

				len = length >> 2;
				inc = inchannels << 2;
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
					in += inchannels;
					out += inchannels;
				}
				continue; 
			}
			else //DSP effect is active on the current speaker so: out = lowpass(in)
			{
				float fy1 = mFilter_Y[count2][0];
				float fy2 = mFilter_Y[count2][1];
				len = length;
				while (len)
				{            
					out[0] = (in[0] + dc) * mFilter_A0 + fy1 * mFilter_B0 + fy2 * mFilter_B1;
					fy2 = fy1;
					fy1 = out[0];

					dc = -dc;
					len--;
					in += inchannels;
					out += inchannels;
				}

				mFilter_Y[count2][0] = fy1;
				mFilter_Y[count2][1] = fy2;
			}
        }
    }

    return FMOD_OK;
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
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT DSPLowPass2::setParameterInternal(int index, float value)
{
    switch (index)
    {
        case FMOD_DSP_LOWPASS_CUTOFF:
        {
            mCutoffHzUpdate  = value;
            break;
        }
        case FMOD_DSP_LOWPASS_RESONANCE:
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
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT DSPLowPass2::getParameterInternal(int index, float *value, char *valuestr)
{
    switch (index)
    {
        case FMOD_DSP_LOWPASS_CUTOFF:
        {
            *value = mCutoffHzUpdate;
            sprintf(valuestr, "%.02f", mCutoffHzUpdate);
            break;
        }
        case FMOD_DSP_LOWPASS_RESONANCE:
        {
            *value = mResonanceUpdate;
            sprintf(valuestr, "%.02f", mResonanceUpdate);
            break;
        }
    }

    return FMOD_OK;
}

#endif // !PLATFORM_PS3_SPU


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

FMOD_RESULT DSPLowPass2::getMemoryUsedImpl(MemoryTracker *tracker)
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
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK DSPLowPass2::createCallback(FMOD_DSP_STATE *dsp)
{
    DSPLowPass2 *lowpass2 = (DSPLowPass2 *)dsp;

    return lowpass2->createInternal();
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
FMOD_RESULT F_CALLBACK DSPLowPass2::resetCallback(FMOD_DSP_STATE *dsp)
{
    DSPLowPass2 *lowpass2 = (DSPLowPass2 *)dsp;

    return lowpass2->resetInternal();
}

#endif // !PLATFORM_PS3_SPU

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
FMOD_RESULT F_CALLBACK DSPLowPass2::readCallback(FMOD_DSP_STATE *dsp, float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int outchannels)
{
    DSPLowPass2 *lowpass2 = (DSPLowPass2 *)dsp;

    return lowpass2->readInternal(inbuffer, outbuffer, length, inchannels, outchannels);
}

#ifndef PLATFORM_PS3_SPU

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
FMOD_RESULT F_CALLBACK DSPLowPass2::setParameterCallback(FMOD_DSP_STATE *dsp, int index, float value)
{
    DSPLowPass2 *lowpass2 = (DSPLowPass2 *)dsp;

    return lowpass2->setParameterInternal(index, value);
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
FMOD_RESULT F_CALLBACK DSPLowPass2::getParameterCallback(FMOD_DSP_STATE *dsp, int index, float *value, char *valuestr)
{
    DSPLowPass2 *lowpass2 = (DSPLowPass2 *)dsp;

    return lowpass2->getParameterInternal(index, value, valuestr);
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
FMOD_RESULT F_CALLBACK DSPLowPass2::getMemoryUsedCallback(FMOD_DSP_STATE *dsp, MemoryTracker *tracker)
{
    DSPLowPass2 *lowpass2 = (DSPLowPass2 *)dsp;    

    return lowpass2->DSPLowPass2::getMemoryUsed(tracker);
}
#endif

#endif //!PLATFORM_PS3_SPU

}

#endif
