#include "fmod_settings.h"

#ifndef PLATFORM_WINDOWS_PS3MODE

#ifdef FMOD_SUPPORT_SOFTWARE

#include "fmod_dsp_resampler_nointerp.h"
#include "fmod_dsp_resampler_linear.h"
#include "fmod_dsp_resampler_cubic.h"
#include "fmod_dsp_resampler_spline.h"
#include "fmod_dsp_resampler_multiinput.h"
#include "fmod_systemi.h"

namespace FMOD
{


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
FMOD_RESULT DSPResamplerMultiInput::read(float **outbuffer, int *outchannels, unsigned int *length, FMOD_SPEAKERMODE speakermode, int speakermodechannels, unsigned int tick)
{
    FMOD_RESULT result = FMOD_OK;

    if (tick >= mNoDMA->mDSPFinishTick)
    {
        return FMOD_OK;
    }
   
    mFlags |= FMOD_DSP_FLAG_IDLE;

    if (mDSPTick != tick)
    {
        float        *readbuffer  = mBuffer;
        int           inlength    = *length;
        int           readoffset  = 0;
	    unsigned int  starttime   = 0;
	    unsigned int  endtime     = 0;
        FMOD_SINT64P  speed;

        if (mSystem->mFlags & FMOD_INIT_ENABLE_PROFILE)
        {
            FMOD_OS_Time_GetNs(&starttime);
        }

        speed.mValue = mSpeed.mValue;      

        if (!mBuffer)
        {
            return FMOD_ERR_INTERNAL;
        }
      
        do
        {
            unsigned int        rlength;
            FMOD_RESAMPLER_END  endflag = FMOD_RESAMPLER_END_MIXBUFFER;

            while (mFill)
            {
                unsigned int readlen, oldresamplebufferpos;
                
                /*
                    Before a fill happens, write the end of the second block to the negative buffer area.
                */
                oldresamplebufferpos = mResampleBufferPos;

                /*
                    Now call the input and convert it to float.
                */
                readlen = mResampleBlockLength;
                
                {
                    float  *resamplebuff = (float *)mResampleBuffer + (mResampleBufferPos * mResampleBufferChannels);
                    float  *outbuff = 0;
                    int     outch;             

                    updateDSPTick(tick - 1);    /* Reset the visited flags so we can execute the children again. */

                    if (mSystem->mFlags & FMOD_INIT_ENABLE_PROFILE)
                    {
                        FMOD_OS_Time_GetNs(&endtime);
                        mCPUUsageTemp += endtime - starttime;
                    }

                    result = DSPFilter::read(&outbuff, &outch, &readlen, speakermode, speakermodechannels, tick);
                    if (result != FMOD_OK)
                    {
                        outbuff = resamplebuff;
                        FMOD_memset(outbuff, 0, readlen * mResampleBufferChannels * sizeof(float));
                        mResampleFinishPos = mResampleBufferPos;
                    }

                    if (mSystem->mFlags & FMOD_INIT_ENABLE_PROFILE)
                    {
                        FMOD_OS_Time_GetNs(&starttime);
                    }

                    mResampleBufferChannels = outch;
                    FMOD_memcpy(resamplebuff, outbuff, readlen * mResampleBufferChannels * sizeof(float));

                    mResampleBufferPos += readlen;
                    if (mResampleBufferPos >= mResampleBufferLength)
                    {
                        mResampleBufferPos = 0;
                    }
                }

                /*
                    Copy the first samples to the end of the buffer so when interpolation does a read past the end, the data will be valid.
                */
                if (!oldresamplebufferpos)
                {
                    unsigned int count;

                    for (count=0; count < mResampleBufferChannels * mOverflowLength * 2; count++)
                    {
                        ((float *)mResampleBuffer)[(mResampleBufferLength * mResampleBufferChannels) + count] = ((float *)mResampleBuffer)[count];
                    }
                }

                mFill--;
            }

            /*
                Work out what is going to end first.  The source data or the output?
            */
            rlength = inlength;     /* Start it off defaulting to the amount requested */
            if (speed.mValue > 0x100)
            {
                FMOD_UINT64P mixesleft, fracleft;
                int nextpos;
                bool endofsound = false;

                nextpos  = mResamplePosition.mHi - mOverflowLength;
                nextpos /= (int)mResampleBlockLength;
                nextpos ++;
                nextpos *= mResampleBlockLength;
                nextpos += mOverflowLength;

                mixesleft.mHi = nextpos;
                mixesleft.mLo = 0;
		    	mixesleft.mValue -= mResamplePosition.mValue;
/*
                if (connection->mInputUnit->mFinished)
                {
                    FMOD_UINT64P mixeslefttofinish;

                    mixeslefttofinish.mHi = mResampleFinishPos;
                    mixeslefttofinish.mLo = 0;
                    mixeslefttofinish.mValue -= mPosition.mValue;

                    if (mixeslefttofinish.mValue <= mixesleft.mValue)
                    {
                        mixesleft.mValue = mixeslefttofinish.mValue;
                        endofsound = true;
                    }
                }
*/

				fracleft.mValue = mixesleft.mValue % speed.mValue;
				mixesleft.mValue /= speed.mValue;
				
                if (fracleft.mValue)						/* round the count up (this could be done better) */
                {
					mixesleft.mValue++;
                }

				if (mixesleft.mValue <= rlength)
    			{
                    if (endofsound)
                    {
                        endflag = FMOD_RESAMPLER_END_SOUND;
                    }
                    else
                    {
                        endflag = FMOD_RESAMPLER_END_RESAMPLEBUFFER;
                    }
					rlength = mixesleft.mLo;
				}
            }
            
            /*
                Resample the src buffer into the destination.
            */
            if (speed.mHi == 1 && speed.mLo == 0)
            {
                FMOD_memcpy(readbuffer + (readoffset * mResampleBufferChannels), (float *)mResampleBuffer + (mResamplePosition.mHi * mResampleBufferChannels), rlength * sizeof(float) * mResampleBufferChannels);

                mResamplePosition.mValue += speed.mValue * rlength;
            }
            else
            {
                switch (mSystem->mResampleMethod)
                {
                    case FMOD_DSP_RESAMPLER_NOINTERP:
                    {
                        FMOD_Resampler_NoInterp(readbuffer + (readoffset * mResampleBufferChannels), rlength, mResampleBuffer, FMOD_SOUND_FORMAT_PCMFLOAT, &mResamplePosition, &speed, mResampleBufferChannels);
                        break;
                    }
                    case FMOD_DSP_RESAMPLER_LINEAR:
                    {
                        FMOD_Resampler_Linear(readbuffer + (readoffset * mResampleBufferChannels), rlength, mResampleBuffer, FMOD_SOUND_FORMAT_PCMFLOAT, &mResamplePosition, &speed, mResampleBufferChannels);
                        break;
                    }
#ifdef FMOD_SUPPORT_RESAMPLER_CUBIC
                    case FMOD_DSP_RESAMPLER_CUBIC:
                    {
                        FMOD_Resampler_Cubic(readbuffer + (readoffset * mResampleBufferChannels), rlength, mResampleBuffer, FMOD_SOUND_FORMAT_PCMFLOAT, &mResamplePosition, &speed, mResampleBufferChannels);
                        break;
                    }
#endif
#ifdef FMOD_SUPPORT_RESAMPLER_SPLINE
                    case FMOD_DSP_RESAMPLER_SPLINE:
                    {
                        FMOD_Resampler_Spline(readbuffer + (readoffset * mResampleBufferChannels), rlength, mResampleBuffer, FMOD_SOUND_FORMAT_PCMFLOAT, &mResamplePosition, &speed, mResampleBufferChannels);
                        break;
                    }
#endif
                    default:
                    {
                        FMOD_Resampler_Linear(readbuffer + (readoffset * mResampleBufferChannels), rlength, mResampleBuffer, FMOD_SOUND_FORMAT_PCMFLOAT, &mResamplePosition, &speed, mResampleBufferChannels);
                        break;
                    }
                }
            }
             
            if (mResamplePosition.mHi >= (mResampleBufferLength + mOverflowLength))
            {
                mResamplePosition.mHi -= mResampleBufferLength;
            }

            inlength   -= rlength;
            readoffset += rlength;

            mFlags &= ~FMOD_DSP_FLAG_IDLE;

            if (endflag == FMOD_RESAMPLER_END_SOUND)
            {
                mNoDMA->mDSPFinishTick = tick;
                break;
            }
            else if (endflag == FMOD_RESAMPLER_END_RESAMPLEBUFFER)
            {
                mFill++;
            }

        } while (inlength > 0);
        
        *outbuffer = readbuffer;
        *outchannels = mResampleBufferChannels;

        if (mSystem->mFlags & FMOD_INIT_ENABLE_PROFILE)
        {  
	        FMOD_OS_Time_GetNs(&endtime);
            mCPUUsageTemp += endtime - starttime;

            mCPUUsage = mCPUUsageTemp;
            mCPUUsageTemp = 0;
        }
    }
    else
    {
        *outbuffer = mBuffer;        /* Return the already processed buffer */
        *outchannels = mResampleBufferChannels;
        mFlags &= ~FMOD_DSP_FLAG_IDLE;
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

	[SEE_ALSO]
]
*/
FMOD_RESULT DSPResamplerMultiInput::addInput(DSPI *target)
{
    FMOD_RESULT result;

    result = DSPI::addInput(target);
    if (result != FMOD_OK)
    {
        return result;
    }

    mResampleBufferPos       = 0;
    mFill                    = 2;
    mPosition.mValue         = 0;
    mResamplePosition.mValue = 0;

    return FMOD_OK;
}


}

#endif
#endif

