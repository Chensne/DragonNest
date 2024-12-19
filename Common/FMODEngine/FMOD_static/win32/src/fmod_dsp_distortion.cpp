#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_DISTORTION

#include "fmod.h"
#include "fmod_dspi.h"
#include "fmod_dsp_distortion.h"
#include "fmod_systemi.h"
#include "fmod_os_misc.h"

#include <stdio.h>

namespace FMOD
{

FMOD_DSP_DESCRIPTION_EX dspdistortion;

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
        return DSPDistortion::getDescriptionEx();
    }

#ifdef __cplusplus
}
#endif

#endif  /* PLUGIN_EXPORTS */

FMOD_DSP_PARAMETERDESC dspdistortion_param[1] =
{
    { 0.0f,     1.0f,   0.5, "Level",   "",     "Distortion value.  0.0 to 1.0.  Default = 0.5." },
};

extern "C"
{
    void FMOD_DSP_Distortion_SIMD(float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels, float k);
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
FMOD_DSP_DESCRIPTION_EX *DSPDistortion::getDescriptionEx()
{
    FMOD_memset(&dspdistortion, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));

    FMOD_strcpy(dspdistortion.name, "FMOD Distortion");
    dspdistortion.version       = 0x00010100;
    dspdistortion.create        = DSPDistortion::createCallback;
    dspdistortion.release       = DSPDistortion::releaseCallback;
    dspdistortion.reset         = DSPDistortion::resetCallback;
    dspdistortion.read          = DSPDistortion::readCallback;

    dspdistortion.numparameters = sizeof(dspdistortion_param) / sizeof(dspdistortion_param[0]);
    dspdistortion.paramdesc     = dspdistortion_param;
    dspdistortion.setparameter  = DSPDistortion::setParameterCallback;
    dspdistortion.getparameter  = DSPDistortion::getParameterCallback;
#ifdef FMOD_SUPPORT_MEMORYTRACKER
    dspdistortion.getmemoryused = &DSPDistortion::getMemoryUsedCallback;
#endif

    dspdistortion.mType         = FMOD_DSP_TYPE_DISTORTION;
    dspdistortion.mCategory     = FMOD_DSP_CATEGORY_FILTER;
    dspdistortion.mSize         = sizeof(DSPDistortion);

    return &dspdistortion;
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
FMOD_RESULT DSPDistortion::createInternal()
{
    int count;

    init();

    mSupportsSIMD = FMOD_OS_SupportsSIMD();
    
    for (count = 0; count < mDescription.numparameters; count++)
    {
        FMOD_RESULT result;
        
        result = setParameter(count, mDescription.paramdesc[count].defaultval);
        if (result != FMOD_OK)
        {
            return result;
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
FMOD_RESULT DSPDistortion::releaseInternal()
{
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
FMOD_RESULT DSPDistortion::resetInternal()
{
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
FMOD_RESULT DSPDistortion::readInternal(float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{   
    float k;
    float level = mLevel;

    if (!inbuffer)
    {
        return FMOD_OK;
    }

	if (!(speakermask & ((1 << inchannels)-1)))
	{
		FMOD_memcpy(outbuffer, inbuffer, sizeof(float)*length*inchannels);
		return FMOD_OK;
	}

    if (level < 1.0f)
    {
        k = 2.0f * level / (1.0f - level);
    }
    else
    {
        k = 2.0f * 0.9999f / (1.0f - 0.9999f);
    } 

    if (mSupportsSIMD)
    {
        FMOD_DSP_Distortion_SIMD(inbuffer, outbuffer, length, inchannels, outchannels, k);
		if (!((speakermask & ((1<<inchannels)-1)) == ((1<<inchannels)-1)))
		{
			unsigned int len;
			int count, inc;

			for (count = 0; count < inchannels; count++)
			{
				float *in = inbuffer + count;
				float *out = outbuffer + count;
				len = length >> 3;
				inc = inchannels << 3;
				int offset1, offset2, offset3, offset4, offset5, offset6, offset7;
				offset1 = inchannels;
				offset2 = inchannels * 2;
				offset3 = inchannels * 3;
				offset4 = inchannels * 4;
				offset5 = inchannels * 5;
				offset6 = inchannels * 6;
				offset7 = inchannels * 7;
					
				if (!((1 << count) & speakermask))
				{
					while (len)
					{
						out[0] = in[0];
						out[offset1] = in[offset1]; 
						out[offset2] = in[offset2]; 
						out[offset3] = in[offset3]; 
						out[offset4] = in[offset4]; 
						out[offset5] = in[offset5]; 
						out[offset6] = in[offset6]; 
						out[offset7] = in[offset7]; 
						in += inc;
						out += inc;
						len--;
					}
					
					len = length & 7;

					while (len)
					{
						outbuffer[0] = inbuffer[0];
						inbuffer += inchannels;
						outbuffer += inchannels;
						len--;
					}
				}
			}
		}
    }
    else
    {
        unsigned int len;
		int count, inc;
		int offset1, offset2, offset3, offset4, offset5, offset6, offset7;
        inc = inchannels << 3;
	    offset1 = inchannels;
	    offset2 = inchannels * 2;
	    offset3 = inchannels * 3;
	    offset4 = inchannels * 4;
	    offset5 = inchannels * 5;
	    offset6 = inchannels * 6;
	    offset7 = inchannels * 7;
		
		for (count = 0; count < inchannels; count++)
		{
			float *in = inbuffer + count;
			float *out = outbuffer + count;
			len = length >> 3;

			if (!((1 << count) & speakermask))
			{
				while (len)
				{
					out[0] = in[0];
					out[offset1] = in[offset1]; 
					out[offset2] = in[offset2]; 
					out[offset3] = in[offset3]; 
					out[offset4] = in[offset4]; 
					out[offset5] = in[offset5]; 
					out[offset6] = in[offset6]; 
					out[offset7] = in[offset7]; 
					in += inc;
					out += inc;
					len--;
				}
				
				len = (length * inchannels) & 7;

				while (len)
				{
					outbuffer[0] = inbuffer[0];
					inbuffer+=inchannels;
					outbuffer+=inchannels;
					len--;
				}
			}
			else
			{
				float kplus1 = 1.0f + k;

				while (len)
				{
					out[0] = kplus1 * in[0] / (1.0f + k * FMOD_FABS(in[0]));       
					out[offset1] = kplus1 * in[offset1] / (1.0f + k * FMOD_FABS(in[offset1]));
					out[offset2] = kplus1 * in[offset2] / (1.0f + k * FMOD_FABS(in[offset2]));
					out[offset3] = kplus1 * in[offset3] / (1.0f + k * FMOD_FABS(in[offset3]));
					out[offset4] = kplus1 * in[offset4] / (1.0f + k * FMOD_FABS(in[offset4]));
					out[offset5] = kplus1 * in[offset5] / (1.0f + k * FMOD_FABS(in[offset5]));
					out[offset6] = kplus1 * in[offset6] / (1.0f + k * FMOD_FABS(in[offset6]));
					out[offset7] = kplus1 * in[offset7] / (1.0f + k * FMOD_FABS(in[offset7]));
					in += inc;
					out += inc;
					len--;
				}

				len = (length * inchannels) & 7;

				while (len)
				{
					outbuffer[0] = kplus1 * inbuffer[0] / (1.0f + k * FMOD_FABS(inbuffer[0]));
					inbuffer+=inchannels;
					outbuffer+=inchannels;
					len--;
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
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT DSPDistortion::setParameterInternal(int index, float value)
{
    mLevel = value;

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
FMOD_RESULT DSPDistortion::getParameterInternal(int index, float *value, char *valuestr)
{
    *value = mLevel;
    sprintf(valuestr, "%.02f", mLevel);

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

FMOD_RESULT DSPDistortion::getMemoryUsedImpl(MemoryTracker *tracker)
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
FMOD_RESULT F_CALLBACK DSPDistortion::createCallback(FMOD_DSP_STATE *dsp)
{
    DSPDistortion *distortion = (DSPDistortion *)dsp;

    return distortion->createInternal();
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
FMOD_RESULT F_CALLBACK DSPDistortion::releaseCallback(FMOD_DSP_STATE *dsp)
{
    DSPDistortion *distortion = (DSPDistortion *)dsp;

    return distortion->releaseInternal();
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
FMOD_RESULT F_CALLBACK DSPDistortion::resetCallback(FMOD_DSP_STATE *dsp)
{
    DSPDistortion *distortion = (DSPDistortion *)dsp;

    return distortion->resetInternal();
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
FMOD_RESULT F_CALLBACK DSPDistortion::readCallback(FMOD_DSP_STATE *dsp, float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int outchannels)
{
    DSPDistortion *distortion = (DSPDistortion *)dsp;

    return distortion->readInternal(inbuffer, outbuffer, length, inchannels, outchannels);
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
FMOD_RESULT F_CALLBACK DSPDistortion::setParameterCallback(FMOD_DSP_STATE *dsp, int index, float value)
{
    DSPDistortion *distortion = (DSPDistortion *)dsp;

    return distortion->setParameterInternal(index, value);
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
FMOD_RESULT F_CALLBACK DSPDistortion::getParameterCallback(FMOD_DSP_STATE *dsp, int index, float *value, char *valuestr)
{
    DSPDistortion *distortion = (DSPDistortion *)dsp;

    return distortion->getParameterInternal(index, value, valuestr);
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
FMOD_RESULT F_CALLBACK DSPDistortion::getMemoryUsedCallback(FMOD_DSP_STATE *dsp, MemoryTracker *tracker)
{
    DSPDistortion *distortion = (DSPDistortion *)dsp;

    return distortion->DSPDistortion::getMemoryUsed(tracker);
}
#endif


}

#endif