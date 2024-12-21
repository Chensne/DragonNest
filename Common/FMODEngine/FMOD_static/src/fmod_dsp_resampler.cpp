#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_SOFTWARE

#include "fmod_channel_software.h"
#include "fmod_dsp_codec.h"
#include "fmod_dsp_resampler_nointerp.h"
#include "fmod_dsp_resampler_linear.h"
#include "fmod_dsp_resampler_cubic.h"
#include "fmod_dsp_resampler_spline.h"
#include "fmod_dsp_resampler.h"
#include "fmod_sample_software.h"

#if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE) 
#include "../ps3/src/fmod_common_spu.h"

#ifdef PLATFORM_WINDOWS_PS3MODE
    float gSourceMixBuffer[8192 * 16];
    float gTargetMixBuffer[8192 * 16];
#endif

#endif

#ifdef PLATFORM_PS3_SPU
    #include "fmod_systemi_spu.h"
    #include "fmod_spu_printf.h"

    #include <cell/dma.h>

    #define mSystem (&gSystem)
#else
    #include "fmod_systemi.h"
#endif

namespace FMOD
{

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
DSPResampler::DSPResampler()
{
    mResampleBufferMemory    = 0;
    mResampleBuffer          = 0;
    mTargetFrequency         = 0;
    mSpeed.mValue            = 0;
    mOverflowLength          = FMOD_DSP_RESAMPLER_OVERFLOWLENGTH;    

    mResampleBufferPos       = 0;
    mResampleFinishPos       = (unsigned int)-1;
    mFill                    = 2;
    mResamplePosition.mValue = 0;
    mPosition.mValue         = 0;
    mNoDMA                   = 0;

    #if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)
    mNoDMAMemory             = NULL;
    #else
    FMOD_memset(&mNoDMAMemory, 0, sizeof(DSPResampler_NODMA));
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
FMOD_RESULT DSPResampler::release(bool freethis)
{
    DSPI::release(false);

    if (mResampleBufferMemory)
    {
        FMOD_Memory_Free(mResampleBufferMemory);
        mResampleBufferMemory = 0;
    }

    #if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)
    if (mNoDMAMemory)
    {
        FMOD_Memory_Free(mNoDMAMemory);
        mNoDMAMemory = 0;
        mNoDMA = 0;
    }
    #endif

    if (freethis)
    {
        #ifdef PLATFORM_PS3
        if (mMemory)
        {
            FMOD_Memory_Free(mMemory);
        }
        else
        #endif
        {
            FMOD_Memory_Free(this);
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
FMOD_RESULT DSPResampler::alloc(FMOD_DSP_DESCRIPTION_EX *description)
{
    FMOD_RESULT result;
    int channels;
    unsigned int bytespersample = 0;

    result = DSPI::alloc(description);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mSystem->getSoftwareFormat(&mTargetFrequency, 0, 0, 0, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    #if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)

    FMOD_MEMORY_TYPE type;

    if (description->mCategory == FMOD_DSP_CATEGORY_DSPCODECMPEG ||
        description->mCategory == FMOD_DSP_CATEGORY_DSPCODECRAW  ||
        description->mCategory == FMOD_DSP_CATEGORY_DSPCODECADPCM)
    {
        type = FMOD_MEMORY_PERSISTENT;
    }
    else
    {
        type = FMOD_MEMORY_NORMAL;
    }

    mNoDMAMemory = (DSPResampler_NODMA *)FMOD_Memory_CallocType(sizeof(DSPResampler_NODMA) + 16, type);
    mNoDMA = (DSPResampler_NODMA *)(((FMOD_UINT_NATIVE)mNoDMAMemory + 15) & ~15);
    if (!mNoDMA)
    {
        return FMOD_ERR_MEMORY;
    }
    #else
    mNoDMA = &mNoDMAMemory;
    #endif

    if (description->mResamplerBlockLength)
    {
        mResampleBlockLength = description->mResamplerBlockLength;
        channels             = description->channels;
    }
    else
    {
        result = mSystem->getDSPBufferSize(&mResampleBlockLength, 0);
        if (result != FMOD_OK)
        {
            return result;
        }
        channels = mSystem->mMaxInputChannels;
    }

    mResampleBufferLength = mResampleBlockLength * 2;   /* *2 = double buffer */
    
    if (mDescription.mFormat == FMOD_SOUND_FORMAT_NONE)
    {
        mDescription.mFormat = FMOD_SOUND_FORMAT_PCMFLOAT;
    }
    
    SoundI::getBytesFromSamples(1, &bytespersample, channels, mDescription.mFormat);      

    if (0)
    {
    }
    #ifdef FMOD_SUPPORT_DSPCODEC
    #ifdef FMOD_SUPPORT_MPEG
    if (mDescription.mCategory == FMOD_DSP_CATEGORY_DSPCODECMPEG)
    {   
        DSPCodecMPEG *dspcodecmpeg = SAFE_CAST(DSPCodecMPEG, this);

        mResampleBuffer = (char *)FMOD_ALIGNPOINTER(dspcodecmpeg->mResampleBufferMemory, 16);
    }
    #endif
    #ifdef FMOD_SUPPORT_XMA
    else if (mDescription.mCategory == FMOD_DSP_CATEGORY_DSPCODECXMA)
    {
        DSPCodecXMA *dspcodecxma = SAFE_CAST(DSPCodecXMA, this);
        
        mResampleBuffer = (char *)FMOD_ALIGNPOINTER(dspcodecxma->mResampleBufferMemory, 16);
    }
    #endif
    #ifdef FMOD_SUPPORT_CELT
    else if (mDescription.mCategory == FMOD_DSP_CATEGORY_DSPCODECCELT)
    {
        DSPCodecCELT *dspcodeccelt = SAFE_CAST(DSPCodecCELT, this);
        
        mResampleBuffer = (char *)FMOD_ALIGNPOINTER(dspcodeccelt->mResampleBufferMemory, 16);
    }
    #endif
    #ifdef FMOD_SUPPORT_IMAADPCM
    else if (mDescription.mCategory == FMOD_DSP_CATEGORY_DSPCODECADPCM)
    {
        DSPCodecADPCM *dspcodecadpcm = SAFE_CAST(DSPCodecADPCM, this);
        
        mResampleBuffer = (char *)FMOD_ALIGNPOINTER(dspcodecadpcm->mResampleBufferMemory, 16);
    }
    #endif
    #ifdef FMOD_SUPPORT_RAW
    else if (mDescription.mCategory == FMOD_DSP_CATEGORY_DSPCODECRAW)
    {
        DSPCodecRaw *dspcodecraw = SAFE_CAST(DSPCodecRaw, this);

        mResampleBuffer = (char *)FMOD_ALIGNPOINTER(dspcodecraw->mResampleBufferMemory, 16);
    }
    #endif
    #endif  /* FMOD_SUPPORT_DSPCODEC */
    #ifndef PLATFORM_PS3
    else
    {
        mResampleBufferMemory = FMOD_Memory_Calloc(((mResampleBufferLength + (mOverflowLength * 4)) * bytespersample) + 16);
        if (!mResampleBufferMemory)
        {
            return FMOD_ERR_MEMORY;
        }
        mResampleBuffer = mResampleBufferMemory;
        mResampleBuffer = (char *)FMOD_ALIGNPOINTER(mResampleBuffer, 16);
    }

    mResampleBuffer = (char *)mResampleBuffer + (mOverflowLength * bytespersample);
    mResampleBuffer = (char *)FMOD_ALIGNPOINTER(mResampleBuffer, 16);
    #endif  /* !PLATFORM_PS3 */

    mResampleBufferPos       = 0;
    mResampleFinishPos       = (unsigned int)-1;
    mFill                    = 2;
    mPosition.mValue         = 0;
    mResamplePosition.mValue = 0;

    mDSPTick                 = 0;
    mNoDMA->mDSPFinishTick   = 0;

    if (description->mDSPSoundCard)
    {
        mDSPSoundCard = description->mDSPSoundCard;
    }
    else
    {
        mDSPSoundCard = mSystem->mDSPSoundCard;
    }

    return FMOD_OK;
}

#endif

#if defined(FMOD_SUPPORT_MIXER_NONRECURSIVE)


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]
    THIS FUNCTION IS PS3 ONLY

    [PLATFORMS]
    PlayStation 3

	[SEE_ALSO]
]
*/
FMOD_RESULT DSPResampler::update(unsigned int length, int *outchannels, void **outbuffer, unsigned int tick)
{
    FMOD_RESULT result = FMOD_OK;

    mFlags &= ~FMOD_DSP_FLAG_IDLE;
    mFlags &= ~FMOD_DSP_FLAG_FINISHED;

    if (mFill)
    {
        void        *resamplebuff;
        unsigned int oldresamplebufferpos;
        unsigned int bytespersample = 0;
    
        oldresamplebufferpos = mResampleBufferPos;
        
        if (mDescription.channels)
        {
            mResampleBufferChannels = mDescription.channels;
        }
        else
        {
            mResampleBufferChannels = *outchannels;
        }

        SoundI::getBytesFromSamples(1, &bytespersample, mResampleBufferChannels, mDescription.mFormat);

        resamplebuff = (char *)mResampleBuffer + (mResampleBufferPos * bytespersample);

        if (mDescription.mCategory == FMOD_DSP_CATEGORY_DSPCODECMPEG ||
            mDescription.mCategory == FMOD_DSP_CATEGORY_DSPCODECADPCM ||
            mDescription.mCategory == FMOD_DSP_CATEGORY_DSPCODECRAW ||
            mDescription.mCategory == FMOD_DSP_CATEGORY_DSPCODECXMA)
        {
            if ((int)mResampleFinishPos == -1)
            {
                instance = (FMOD_DSP *)this;   

                /*
                    Read from the dsp unit's read function.  Bring a chunk of data in to fill the back buffer of the resampler double-buffer.
                */
                result = mDescription.read((FMOD_DSP_STATE *)this, 0, (float *)resamplebuff, mResampleBlockLength, mDescription.channels, mDescription.channels);
            }
            else
            {
                FMOD_memset(resamplebuff, 0, mResampleBlockLength * bytespersample);
            }

            if (mDirection == DSPRESAMPLER_SPEEDDIR_BACKWARDS)
            {
                unsigned int pos, count;
                DSPCodec *dspcodec = SAFE_CAST(DSPCodec, this);

                /*
                    Reverse the buffer.
                */
                if (mDescription.channels == 2)
                {
                    unsigned int *buff = (unsigned int *)resamplebuff;
                    for (count = 0; count < mResampleBlockLength / 2; count++)  /* /2 is for half of a buffer. */
                    {
                        unsigned int tmp = buff[mResampleBlockLength - count - 1];
                        buff[mResampleBlockLength - count - 1] = buff[count];
                        buff[count] = tmp;
                    }
                }
                else
                {
                    signed short *buff = (signed short *)resamplebuff;
                    for (count = 0; count < mResampleBlockLength / 2; count++)  /* /2 is for half of a buffer. */
                    {
                        signed short tmp = buff[mResampleBlockLength - count - 1];
                        buff[mResampleBlockLength - count - 1] = buff[count];
                        buff[count] = tmp;
                    }
                }

                if (result == FMOD_OK)
                {
                    result = dspcodec->getPositionInternal(&pos);
                    pos -= (mResampleBlockLength * 2);                          /* Just read one forward, now jump 2 back. */
                    if ((int)pos < (int)mNoDMA->mLoopStart)
                    {
                        pos += mNoDMA->mLoopLength;
                    }
                    result = dspcodec->setPositionInternal(pos, true);
                }
            }

        }
        else
        {
            if (*outbuffer != resamplebuff)
            {
                FMOD_memcpy(resamplebuff, *outbuffer, mResampleBlockLength * bytespersample);
            }
        }

        mResampleBufferPos += mResampleBlockLength;
        if (mResampleBufferPos >= mResampleBufferLength)
        {
            mResampleBufferPos = 0;
        }

        if (mResampleFinishPos == (unsigned int)-1 && result != FMOD_OK)
        {
            mResampleFinishPos = mResampleBufferPos;
            if (!mResampleFinishPos)
            {
                mResampleFinishPos = mResampleBufferLength;
            }    
        }

        /*
            Copy the first samples to the end of the buffer so when interpolation does a read past the end, the data will be valid.
        */
        if (!oldresamplebufferpos)
        {
            unsigned int count;

            for (count=0; count < mOverflowLength * 2 * bytespersample; count++)    /* *2 = double, not sizeof(short) */
            {                        
                ((unsigned char *)mResampleBuffer)[(mResampleBufferLength * bytespersample) + count] = ((unsigned char *)mResampleBuffer)[count];
            }
        }

        mFill--;
        if (mFill)
        {
            mFlags |= FMOD_DSP_FLAG_FIRSTMIX;
            return FMOD_ERR_FILE_EOF;
        }
    }

    /*
        Now that the double buffer is up to date, do some resampling.
    */
    {
        int          inlength = length - mReadPosition;
        float       *readbuffer;
        FMOD_SINT64P speed;
        float        freq;
        
        readbuffer = (float *)gTargetMixBuffer;

        freq  = mFrequency;
        speed = mSpeed;

        if (mNoDMA->mDSPClockEnd.mValue > 0 &&
            mNoDMA->mDSPClockEnd.mValue < (mSystem->mDSPClock.mValue + mReadPosition) + inlength)
        {
            unsigned int diff = (unsigned int)((mSystem->mDSPClock.mValue + mReadPosition + inlength) - mNoDMA->mDSPClockEnd.mValue);
            if (diff > (unsigned int)inlength)
            {
                diff = inlength;
            }

            FMOD_memset(readbuffer + (mReadPosition + ((inlength - diff) * mDescription.channels)), 0, diff * sizeof(float) * mDescription.channels);
            inlength -= diff;
            mNoDMA->mDSPFinishTick = tick;
            mFlags |= FMOD_DSP_FLAG_FINISHED;
        }

        if (mNoDMA->mDSPClockPause.mValue > 0 &&
            mNoDMA->mDSPClockPause.mValue < (mSystem->mDSPClock.mValue + mReadPosition) + inlength)
        {
            unsigned int diff = (unsigned int)((mSystem->mDSPClock.mValue + mReadPosition + inlength) - mNoDMA->mDSPClockPause.mValue);
            if (diff > (unsigned int)inlength)
            {
                diff = inlength;
            }

            FMOD_memset(readbuffer + (mReadPosition + ((inlength - diff) * mDescription.channels)), 0, diff * sizeof(float) * mDescription.channels);
            inlength -= diff;
            
            mFlags &= ~FMOD_DSP_FLAG_ACTIVE;
            mNoDMA->mDSPClockPause.mValue = 0;
        }

        if (mNoDMA->mDSPClockStart.mValue > mSystem->mDSPClock.mValue && 
            mNoDMA->mDSPClockStart.mValue + inlength > mSystem->mDSPClock.mValue)
        {
            unsigned int diff = (unsigned int)(mNoDMA->mDSPClockStart.mValue - mSystem->mDSPClock.mValue);
            if (diff > (unsigned int)inlength)
            {
                diff = inlength;
            }

            if (diff)
            {
                FMOD_memset(readbuffer, 0, diff * sizeof(float) * mDescription.channels);
                inlength -= diff;
                mReadPosition += diff;
            }          
        }

        while (inlength)
        {
            unsigned int        rlength;
            FMOD_RESAMPLER_END  endflag = FMOD_RESAMPLER_END_MIXBUFFER;

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

                if (mResampleFinishPos != (unsigned int)-1)
                {
                    FMOD_UINT64P mixeslefttofinish;

                    mixeslefttofinish.mHi = mResampleFinishPos;
                    mixeslefttofinish.mLo = 0;
                    mixeslefttofinish.mValue -= mResamplePosition.mValue;

                    if (mixeslefttofinish.mValue <= mixesleft.mValue)
                    {
                        mixesleft.mValue = mixeslefttofinish.mValue;
                        endofsound = true;
                    }
                }

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
#ifdef FMOD_SUPPORT_RESAMPLER_NOINTERP
            if (speed.mHi == 1 && speed.mLo == 0)
            {
                FMOD_Resampler_NoInterp(readbuffer + (mReadPosition * mResampleBufferChannels), rlength, mResampleBuffer, mDescription.mFormat, &mResamplePosition, &speed, mResampleBufferChannels);
            }
            else
#endif
            {
                switch (mSystem->mResampleMethod)
                {
                    #ifdef FMOD_SUPPORT_RESAMPLER_NOINTERP
                    case FMOD_DSP_RESAMPLER_NOINTERP:
                    {
                        FMOD_Resampler_NoInterp(readbuffer + (mReadPosition * mResampleBufferChannels), rlength, mResampleBuffer, mDescription.mFormat, &mResamplePosition, &speed, mResampleBufferChannels);
                        break;
                    }
                    #endif
                    #ifdef FMOD_SUPPORT_RESAMPLER_LINEAR
                    case FMOD_DSP_RESAMPLER_LINEAR:
                    {
                        FMOD_Resampler_Linear(readbuffer + (mReadPosition * mResampleBufferChannels), rlength, mResampleBuffer, mDescription.mFormat, &mResamplePosition, &speed, mResampleBufferChannels);
                      
                        break;
                    }
                    #endif
                    #ifdef FMOD_SUPPORT_RESAMPLER_CUBIC
                    case FMOD_DSP_RESAMPLER_CUBIC:
                    {
                        FMOD_Resampler_Cubic(readbuffer + (mReadPosition * mResampleBufferChannels), rlength, mResampleBuffer, mDescription.mFormat, &mResamplePosition, &speed, mResampleBufferChannels);
                        break;
                    }
                    #endif
                    #ifdef FMOD_SUPPORT_RESAMPLER_SPLINE
                    case FMOD_DSP_RESAMPLER_SPLINE:
                    {
                        FMOD_Resampler_Spline(readbuffer + (mReadPosition * mResampleBufferChannels), rlength, mResampleBuffer, mDescription.mFormat, &mResamplePosition, &speed, mResampleBufferChannels);
                        break;
                    }
                    #endif
                    default:
                    {
                        FMOD_Resampler_Linear(readbuffer + (mReadPosition * mResampleBufferChannels), rlength, mResampleBuffer, mDescription.mFormat, &mResamplePosition, &speed, mResampleBufferChannels);
                        break;
                    }
                }
            }

            if (mResamplePosition.mHi >= (mResampleBufferLength + mOverflowLength))
            {
                mResamplePosition.mHi -= mResampleBufferLength;
            }

            /*
                Update mPosition and execute any loop / end logic.
                THIS INFO IS FOR DISPLAY ONLY.
            */
            if (1)
            {
                FMOD_UINT64P   len, endpoint;

                if (mLength < mNoDMA->mLoopStart + mNoDMA->mLoopLength)
                {
                    mNoDMA->mLoopLength = mLength - mNoDMA->mLoopStart;
                }

                if (mDirection == DSPRESAMPLER_SPEEDDIR_BACKWARDS)
                {
                    FMOD_UINT64P len, endpoint;

                    if (mNoDMA->mMode & FMOD_LOOP_NORMAL && mLoopCount) 
                    {
                        endpoint.mHi = mNoDMA->mLoopStart;
                        endpoint.mLo = 0;
                    }
                    else
                    {
                        endpoint.mHi = 0;
                        endpoint.mLo = 0;
                    }

                    len.mValue = speed.mValue * rlength;

                    mPosition.mValue -= len.mValue;

                    if ((signed int)mPosition.mHi <= (signed int)endpoint.mHi)
                    {
                        if ((mNoDMA->mMode & FMOD_LOOP_NORMAL) || mLength == (unsigned int)-1 && mLength)  /* 0xFFFFFFFF should mean it is an infinite netstream */
                        {
                            mPosition.mHi += mNoDMA->mLoopLength;
                        }
                        else 
                        {
                            mPosition.mHi = 0;
                        }
                    }
                }
                else
                {
                    if (mNoDMA->mMode & FMOD_LOOP_NORMAL && mLoopCount) 
                    {
                        endpoint.mHi = mNoDMA->mLoopStart + mNoDMA->mLoopLength -1;
                        endpoint.mLo = 0xFFFFFFFF;
                    }
                    else
                    {
                        endpoint.mHi = mLength - 1;
                        endpoint.mLo = 0xFFFFFFFF;
                    }

                    len.mValue = (speed.mValue * rlength);

                    mPosition.mValue += len.mValue;

                    if (mPosition.mValue > endpoint.mValue)
                    {
                        if ((mNoDMA->mMode & FMOD_LOOP_NORMAL) || mLength == (unsigned int)-1 && mLength)  /* 0xFFFFFFFF should mean it is an infinite netstream */
                        {
                            mPosition.mHi -= mNoDMA->mLoopLength;
                        }
                        else 
                        {
                            mPosition.mHi = mLength;
                        }
                    }
                }
            }

            inlength   -= rlength;
            mReadPosition += rlength;

            if (endflag == FMOD_RESAMPLER_END_SOUND)
            {
                FMOD_memset(readbuffer + (mReadPosition * mDescription.channels), 0, inlength * mDescription.channels * sizeof(float));
                mNoDMA->mDSPFinishTick = tick;
                mFlags |= FMOD_DSP_FLAG_FINISHED;
                inlength = 0;
                break;
            }
            else if (endflag == FMOD_RESAMPLER_END_RESAMPLEBUFFER)
            {
                mFill++;
                break;
            }
        }

        if (!inlength)
        {
            *outbuffer = readbuffer;
            *outchannels = mResampleBufferChannels;
            mReadPosition = 0;
        }
        else
        {
            if (mFill)
            {
                return FMOD_ERR_FILE_EOF;
            }
            else
            {
                *outbuffer = readbuffer;
                *outchannels = mResampleBufferChannels;
                mReadPosition = 0;
            }
        }
    }

    return FMOD_OK;

    /*
        This comment is here to avoid a internal compiler error in ProDG gcc compiler!
    */
}

#else

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
FMOD_RESULT DSPResampler::read(float **outbuffer, int *outchannels, unsigned int *length, FMOD_SPEAKERMODE speakermode, int speakermodechannels, unsigned int tick)
{
    FMOD_RESULT  result = FMOD_OK;
    unsigned int bytespersample;

    if (tick >= mNoDMA->mDSPFinishTick)
    {
        mFlags |= FMOD_DSP_FLAG_FINISHED;
        return FMOD_OK;
    }

    mFlags &= ~FMOD_DSP_FLAG_IDLE;
    mFlags &= ~FMOD_DSP_FLAG_FINISHED;
                
    SoundI::getBytesFromSamples(1, &bytespersample, mDescription.channels, mDescription.mFormat);

    if (mDSPTick != tick)
    {
        int             len, inlength  = *length;
        int             readoffset     = 0;
        FMOD_SINT64P    speed;
        unsigned int    starttime      = 0;
	    unsigned int    endtime        = 0;
        
        if (mSystem->mFlags & FMOD_INIT_ENABLE_PROFILE)
        {
            FMOD_OS_Time_GetNs(&starttime);
        }

        speed.mValue = mSpeed.mValue;      

        if (mNoDMA)
        {
            if (mNoDMA->mDSPClockEnd.mValue > 0 &&
                mNoDMA->mDSPClockEnd.mValue < mSystem->mDSPClock.mValue + inlength)
            {
                unsigned int diff = (unsigned int)(mSystem->mDSPClock.mValue + inlength - mNoDMA->mDSPClockEnd.mValue);
                if (diff > (unsigned int)inlength)
                {
                    diff = inlength;
                }

                FMOD_memset(mBuffer + ((inlength - diff) * mDescription.channels), 0, diff * sizeof(float) * mDescription.channels);
                inlength -= diff;

                mNoDMA->mDSPFinishTick = tick;
                mFlags |= FMOD_DSP_FLAG_FINISHED;
            }

            if (mNoDMA->mDSPClockPause.mValue > 0 &&
                mNoDMA->mDSPClockPause.mValue < (mSystem->mDSPClock.mValue + mReadPosition) + inlength)
            {
                unsigned int diff = (unsigned int)((mSystem->mDSPClock.mValue + mReadPosition + inlength) - mNoDMA->mDSPClockPause.mValue);
                if (diff > (unsigned int)inlength)
                {
                    diff = inlength;
                }

                FMOD_memset(mBuffer + (mReadPosition + ((inlength - diff) * mDescription.channels)), 0, diff * sizeof(float) * mDescription.channels);
                inlength -= diff;
            
                mFlags &= ~FMOD_DSP_FLAG_ACTIVE;
                mNoDMA->mDSPClockPause.mValue = 0;
            }

            if (mNoDMA->mDSPClockStart.mValue > mSystem->mDSPClock.mValue && 
                mNoDMA->mDSPClockStart.mValue + inlength > mSystem->mDSPClock.mValue)
            {
                unsigned int diff = (unsigned int)(mNoDMA->mDSPClockStart.mValue - mSystem->mDSPClock.mValue);
                if (diff > (unsigned int)inlength)
                {
                    diff = inlength;
                }

                if (diff)
                {
                    FMOD_memset(mBuffer, 0, diff * sizeof(float) * mDescription.channels);
                    inlength -= diff;
                    readoffset += diff;
                }          
            }
        }

        len = inlength;
        while (len)
        {
            unsigned int        rlength;
            FMOD_RESAMPLER_END  endflag = FMOD_RESAMPLER_END_MIXBUFFER;

            /*
                Fill the double buffer if nescessary.
            */
            while (mFill)
            {
                unsigned int   readlen, oldresamplebufferpos;
                void          *resamplebuff;
                
                oldresamplebufferpos = mResampleBufferPos;
                readlen              = mResampleBlockLength;               
                resamplebuff         = (char *)mResampleBuffer + (mResampleBufferPos * bytespersample);

                if ((int)mResampleFinishPos == -1)
                {
                    instance = (FMOD_DSP *)this;

                    /*
                        Read from the dsp unit's read function.  Bring a chunk of data in to fill the back buffer of the resampler double-buffer.
                    */
                    result = mDescription.read((FMOD_DSP_STATE *)this, 0, (float *)resamplebuff, readlen, mDescription.channels, mDescription.channels);
                }
                else
                {
                    FMOD_memset(resamplebuff, 0, readlen * mDescription.channels * sizeof(signed short));
                    result = FMOD_OK;
                }

                mResampleBufferPos += readlen;
                if (mResampleBufferPos >= mResampleBufferLength)
                {
                    mResampleBufferPos = 0;
                }

                if ((int)mResampleFinishPos == -1 && result != FMOD_OK)
                {
                    mResampleFinishPos = mResampleBufferPos;
                    if (!mResampleFinishPos)
                    {
                        mResampleFinishPos = mResampleBufferLength;
                    }    
                }

                
                /*
                    Copy the first samples to the end of the buffer so when interpolation does a read past the end, the data will be valid.
                */
                if (!oldresamplebufferpos)
                {
                    unsigned int count;

                    for (count=0; count < mOverflowLength * 2 * bytespersample; count++)
                    {                        
                        ((unsigned char *)mResampleBuffer)[(mResampleBufferLength * bytespersample) + count] = ((unsigned char *)mResampleBuffer)[count];
                    }
                }
                
                mFill--;
            }

            /*
                Work out what is going to end first.  The source data or the output?
            */
            rlength = len;     /* Start it off defaulting to the amount requested */
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

                if (mResampleFinishPos != (unsigned int)-1)
                {
                    FMOD_UINT64P mixeslefttofinish;

                    mixeslefttofinish.mHi = mResampleFinishPos;
                    mixeslefttofinish.mLo = 0;
                    mixeslefttofinish.mValue -= mResamplePosition.mValue;

                    if (mixeslefttofinish.mValue <= mixesleft.mValue)
                    {
                        mixesleft.mValue = mixeslefttofinish.mValue;
                        endofsound = true;
                    }
                }

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
                FMOD_Resampler_NoInterp(mBuffer + (readoffset * mDescription.channels), rlength, mResampleBuffer, mDescription.mFormat, &mResamplePosition, &speed, mDescription.channels);
            }
            else
            {
                switch (mSystem->mResampleMethod)
                {
#ifdef FMOD_SUPPORT_RESAMPLER_NOINTERP
                    case FMOD_DSP_RESAMPLER_NOINTERP:
                    {
                        FMOD_Resampler_NoInterp(mBuffer + (readoffset * mDescription.channels), rlength, mResampleBuffer, mDescription.mFormat, &mResamplePosition, &speed, mDescription.channels);
                        break;
                    }
#endif
                    case FMOD_DSP_RESAMPLER_LINEAR:
                    {
                        FMOD_Resampler_Linear(mBuffer + (readoffset * mDescription.channels), rlength, mResampleBuffer, mDescription.mFormat, &mResamplePosition, &speed, mDescription.channels);
                        break;
                    }
#ifdef FMOD_SUPPORT_RESAMPLER_CUBIC
                    case FMOD_DSP_RESAMPLER_CUBIC:
                    {
                        FMOD_Resampler_Cubic(mBuffer + (readoffset * mDescription.channels), rlength, mResampleBuffer, mDescription.mFormat, &mResamplePosition, &speed, mDescription.channels);
                        break;
                    }
#endif
#ifdef FMOD_SUPPORT_RESAMPLER_SPLINE
                    case FMOD_DSP_RESAMPLER_SPLINE:
                    {
                        FMOD_Resampler_Spline(mBuffer + (readoffset * mDescription.channels), rlength, mResampleBuffer, mDescription.mFormat, &mResamplePosition, &speed, mDescription.channels);
                        break;
                    }
#endif
                    default:
                    {
                        FMOD_Resampler_Linear(mBuffer + (readoffset * mDescription.channels), rlength, mResampleBuffer, mDescription.mFormat, &mResamplePosition, &speed, mDescription.channels);
                        break;
                    }
                }
            }                         

            if (mResamplePosition.mHi >= (mResampleBufferLength + mOverflowLength))
            {
                mResamplePosition.mHi -= mResampleBufferLength;
            }

            len   -= rlength;
            readoffset += rlength;

            if (endflag == FMOD_RESAMPLER_END_SOUND)
            {
                FMOD_memset(mBuffer + (readoffset * mDescription.channels), 0, len * mDescription.channels * sizeof(float));
                mNoDMA->mDSPFinishTick = tick;
                mFlags |= FMOD_DSP_FLAG_FINISHED;
                break;
            }
            else if (endflag == FMOD_RESAMPLER_END_RESAMPLEBUFFER)
            {
                mFill++;
            }                                            
        };

        /*
            Update mPosition and execute any loop / end logic.
            THIS INFO IS FOR DISPLAY ONLY.
        */
        if (mLength < mNoDMA->mLoopStart + mNoDMA->mLoopLength)
        {
            mNoDMA->mLoopLength = mLength - mNoDMA->mLoopStart;
        }

        if (mDirection == DSPRESAMPLER_SPEEDDIR_BACKWARDS)
        {
            FMOD_UINT64P len, endpoint;

            if (mNoDMA->mMode & FMOD_LOOP_NORMAL && mLoopCount) 
            {
                endpoint.mHi = mNoDMA->mLoopStart;
                endpoint.mLo = 0;
            }
            else
            {
                endpoint.mHi = 0;
                endpoint.mLo = 0;
            }

            len.mValue = (-speed.mValue * inlength);

            mPosition.mValue += len.mValue;

            if ((signed int)mPosition.mHi <= (signed int)endpoint.mHi)
            {
                if ((mNoDMA->mMode & FMOD_LOOP_NORMAL) || mLength == (unsigned int)-1 && mLength)  /* 0xFFFFFFFF should mean it is an infinite netstream */
                {
                    mPosition.mHi += mNoDMA->mLoopLength;
                }
                else 
                {
                    mPosition.mHi = 0;
                }
            }
        }
        else
        {
            FMOD_UINT64P len, endpoint;

            if (mLength < mNoDMA->mLoopStart + mNoDMA->mLoopLength)
            {
                mNoDMA->mLoopLength = mLength - mNoDMA->mLoopStart;
            }

            if (mNoDMA->mMode & FMOD_LOOP_NORMAL && mLoopCount) 
            {
                endpoint.mHi = mNoDMA->mLoopStart + mNoDMA->mLoopLength -1;
                endpoint.mLo = 0xFFFFFFFF;
            }
            else
            {
                endpoint.mHi = mLength - 1;
                endpoint.mLo = 0xFFFFFFFF;
            }

            len.mValue = (speed.mValue * inlength);

            mPosition.mValue += len.mValue;

            if (mPosition.mValue > endpoint.mValue)
            {
                if ((mNoDMA->mMode & FMOD_LOOP_NORMAL) || mLength == (unsigned int)-1 && mLength)  /* 0xFFFFFFFF should mean it is an infinite netstream */
                {
                    mPosition.mHi -= mNoDMA->mLoopLength;
                }
                else 
                {
                    mPosition.mHi = mLength;
                }
            }
        }
       
        if (mSystem->mFlags & FMOD_INIT_ENABLE_PROFILE)
        {
            FMOD_OS_Time_GetNs(&endtime);
            mCPUUsage = endtime - starttime;

#if defined(FMOD_SUPPORT_PROFILE_DSP_VOLUMELEVELS) && !defined(PLATFORM_PS3)
            calculatePeaks(mBuffer, *length, mDescription.channels);
#endif
        }
    }

    *outbuffer = mBuffer;      /* Return the already processed buffer */
    *outchannels = mDescription.channels;

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
FMOD_RESULT DSPResampler::addInput(DSPI *target)
{
    return FMOD_ERR_DSP_CONNECTION;
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
FMOD_RESULT DSPResampler::setFrequency(float frequency)
{
    if (frequency < 0)
    {
        mDirection = DSPRESAMPLER_SPEEDDIR_BACKWARDS;
        frequency = -frequency;
    }
    else 
    {
        mDirection = DSPRESAMPLER_SPEEDDIR_FORWARDS;
    }

    mFrequency = frequency;
 
    mSpeed.mValue = (FMOD_SINT64)(mFrequency / mTargetFrequency * 4294967296.0f);

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
FMOD_RESULT DSPResampler::getFrequency(float *frequency)
{
    if (!frequency)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *frequency = mFrequency;

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
FMOD_RESULT DSPResampler::setPosition(unsigned int position, bool processinputs)
{
    FMOD_RESULT result;

    result = DSPI::setPosition(position, processinputs);
    if (result != FMOD_OK)
    {
        return result;
    }

    mResampleBufferPos       = 0;
    mResampleFinishPos       = (unsigned int)-1;
    mFill                    = 2;
    mResamplePosition.mValue = 0;
    mPosition.mHi            = position;
    mPosition.mLo            = 0;

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
FMOD_RESULT DSPResampler::setFinished(bool finished, bool force)
{
    if (!finished)
    {
        mNoDMA->mDSPFinishTick = 0xFFFFFFFF;
        mFlags &= ~FMOD_DSP_FLAG_FINISHED;
        return FMOD_OK;
    }

    if (force)
    {
        #ifndef PLATFORM_PS3_SPU
        FMOD_OS_CriticalSection_Enter(mSystem->mDSPCrit);         /* because it came from virtual voice swap most likely and needs to block/sync. */
        #endif

        mNoDMA->mDSPFinishTick = 0;

        #ifndef PLATFORM_PS3_SPU
        FMOD_OS_CriticalSection_Leave(mSystem->mDSPCrit);
        #endif
    }
    else
    {
        if (mDSPSoundCard)
        {
            mNoDMA->mDSPFinishTick = mDSPSoundCard->mDSPTick + 1;
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
FMOD_RESULT DSPResampler::getFinished(bool *finished) 
{ 
    if (!mDSPSoundCard)
    {
        *finished = true;
        return FMOD_OK;
    }
       
    if ((mNoDMA->mDSPFinishTick >= mDSPSoundCard->mDSPTick && !(mFlags & FMOD_DSP_FLAG_FINISHED)) || (mFlags & FMOD_DSP_FLAG_QUEUEDFORDISCONNECT))
    {
        *finished = false;  /* hasn't really finished yet, still mixing. */
    }
    else
    {
        *finished = true;
    }

    return FMOD_OK; 
}

#ifdef FMOD_SUPPORT_MEMORYTRACKER

FMOD_RESULT DSPResampler::getMemoryUsedImpl(MemoryTracker *tracker)
{
    
    return FMOD_OK;
}

#endif


}

#endif
