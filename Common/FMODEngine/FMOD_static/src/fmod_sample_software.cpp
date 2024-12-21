#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_SOFTWARE

#include "fmod_dsp_codec.h"
#include "fmod_memory.h"
#include "fmod_output.h"
#include "fmod_output_software.h"
#include "fmod_sample_software.h"
#include "fmod_systemi.h"

#ifdef PLATFORM_PS3
#include "fmod_output_ps3.h"
#endif

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
SampleSoftware::SampleSoftware()
{
    mBuffer = 0;
    mBufferMemory = 0;
    mLoopPointDataEndOffset = 0;
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
FMOD_RESULT SampleSoftware::release(bool freethis)
{
    FMOD_RESULT result;
    
    if (mFlags & FMOD_SOUND_FLAG_PRELOADEDFSB)
    {
        return FMOD_ERR_PRELOADED;
    }
    
    if (mFlags & FMOD_SOUND_FLAG_PROGRAMMERSOUND)
    {
        return FMOD_ERR_PROGRAMMERSOUND;
    }

    /*
        The reason there is a sleep loop here and not a criticalsection is that we don't want one sound release stalling on other non blocking sounds, 
        and we also don't want the non blocking thread stalling on release calls (not such a huge issue but it slows loading down unescessarily).
    */
    while ((mOpenState != FMOD_OPENSTATE_READY && mOpenState != FMOD_OPENSTATE_ERROR) || (mFlags & FMOD_SOUND_FLAG_DONOTRELEASE))
    {
        FMOD_OS_Time_Sleep(2);
    }

    if (mSystem)
    {
        result = mSystem->stopSound(this);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

	if (mBufferMemory)
	{
        if (0)
        {}
        #ifdef PLATFORM_PS3
        else if ((mMode & FMOD_LOADSECONDARYRAM) && OutputPS3::mRSXPoolInitialised)
        {
            OutputPS3::mRSXPool.free(mBufferMemory, __FILE__, __LINE__);
        }
        #endif
        else if ((mMode & FMOD_LOADSECONDARYRAM) && (FMOD::gGlobal->gMemoryTypeFlags & FMOD_MEMORY_SECONDARY))
        {
            FMOD_Memory_FreeType(mBufferMemory, FMOD_MEMORY_SECONDARY);
        }
        else
        {
            FMOD_Memory_Free(mBufferMemory);
        }
        mBufferMemory = 0;
	}
	
	if (mLoopPointDataEnd && mLoopPointDataEnd != mLoopPointDataEndMemory)
	{
	    FMOD_Memory_Free(mLoopPointDataEnd);
	    mLoopPointDataEnd = 0;
	}

    mBuffer = 0;

    return Sample::release(freethis);
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
FMOD_RESULT SampleSoftware::setMode(FMOD_MODE mode)
{
    FMOD_RESULT result;

    result = Sample::setMode(mode);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = setLoopPointData();
    if (result != FMOD_OK)
    {
        return result;
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
FMOD_RESULT SampleSoftware::lockInternal(unsigned int offset, unsigned int length, void **ptr1, void **ptr2, unsigned int *len1, unsigned int *len2)
{
    FMOD_RESULT result;
    char *src = 0;
    unsigned int overflowbytes, pointB;
        
    /*
        If we're locking part of the sample that has been modified by the loop point stuff, restore the old data first.
    */
    result = getBytesFromSamples(FMOD_DSP_RESAMPLER_OVERFLOWLENGTH, &overflowbytes);
    if (result != FMOD_OK)
    {
        return result;
    }
  
    result = getBytesFromSamples(mLoopStart + mLoopLength, &pointB);
    if (result != FMOD_OK)
    {
        return result;
    }
 
    if (offset >= pointB && offset < pointB + overflowbytes)
    {
        result = restoreLoopPointData();
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    src = (char *)mBuffer;
        
    if (offset >= mLengthBytes || offset < 0 || length < 0 || length > mLengthBytes) 
	{
		*ptr1 = 0;
        if (ptr2)
        {
		    *ptr2 = 0;
        }
		*len1 = 0;
        if (len2)
        {
		    *len2 = 0;
        }
		return FMOD_ERR_INVALID_PARAM;
	}

	if (offset + length <= mLengthBytes)
	{
        *ptr1 = src + offset;
		*len1 = length;
        if (ptr2)
        {
		    *ptr2 = 0;
        }
        if (len2)
        {
		    *len2 = 0;
        }
	}
	/*
        Otherwise return wrapped pointers in pt1 and ptr2
    */
	else
	{
        *ptr1 = src + offset;
		*len1 = mLengthBytes - offset;
        if (ptr2)
        {
            *ptr2 = src;
        }
        if (len2)
        {
            *len2 = length - (mLengthBytes - offset);
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
FMOD_RESULT SampleSoftware::unlockInternal(void *ptr1, void *ptr2, unsigned int len1, unsigned int len2)
{
    FMOD_RESULT result;

    result = setLoopPointData();
    if (result != FMOD_OK)
    {
        return result;
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
FMOD_RESULT SampleSoftware::setBufferData(void *data) 
{ 
    #ifdef PLATFORM_PS3
    if ((unsigned int)data % 16)
    {
        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "SampleSoftware::setBufferData", "data: %08x not 16 byte aligned!\n", data));
        return FMOD_ERR_MEMORY_CANTPOINT;
    }
    #endif

    mBuffer = data; 
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
FMOD_RESULT SampleSoftware::setLoopPoints(unsigned int loopstart, unsigned int looplength)
{
    if (loopstart >= mLength || loopstart + looplength > mLength)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    restoreLoopPointData();

    mLoopStart = loopstart;
    mLoopLength = looplength;

    setLoopPointData();

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
FMOD_RESULT SampleSoftware::setLoopPointData()
{
    FMOD_RESULT result;
    unsigned int overflowbytes,  pointA, pointB;

    if ((mFormat != FMOD_SOUND_FORMAT_PCM8 &&
         mFormat != FMOD_SOUND_FORMAT_PCM16 &&
         mFormat != FMOD_SOUND_FORMAT_PCM24 &&
         mFormat != FMOD_SOUND_FORMAT_PCM32 &&
         mFormat != FMOD_SOUND_FORMAT_PCMFLOAT) || !mLoopPointDataEnd)
    {
        return FMOD_OK;
    }

    result = getBytesFromSamples(FMOD_DSP_RESAMPLER_OVERFLOWLENGTH, &overflowbytes);
    if (result != FMOD_OK)
    {
        return result;
    }
    result = getBytesFromSamples(mLoopStart, &pointA);
    if (result != FMOD_OK)
    {
        return result;
    }
    result = getBytesFromSamples(mLoopStart + mLoopLength, &pointB);
    if (result != FMOD_OK)
    {
        return result;
    }
    
    if (mMode & FMOD_LOOP_BIDI)
    {
        int count;

        FMOD_memcpy(mLoopPointDataEnd, (char *)mBuffer + pointB, overflowbytes); /* backup the data first */
        mLoopPointDataEndOffset = pointB;

        switch (mFormat)
        {
            case FMOD_SOUND_FORMAT_PCM8:
            {
                char *srcptr, *destptr;

                srcptr = destptr = (char *)mBuffer + pointB;
                srcptr -= mChannels;
                for (count=0; count < FMOD_DSP_RESAMPLER_OVERFLOWLENGTH * mChannels; count++)
                {
                    *destptr = *srcptr;
                    destptr++;
                    srcptr--;
                }
                break;
            }
            case FMOD_SOUND_FORMAT_PCM16:
            {
                short *srcptr, *destptr;

                srcptr = destptr = (short *)((char *)mBuffer + pointB);
                srcptr -= mChannels;
                for (count=0; count < FMOD_DSP_RESAMPLER_OVERFLOWLENGTH * mChannels; count++)
                {
                    *destptr = *srcptr;
                    destptr++;
                    srcptr--;
                }
                break;
            }
            case FMOD_SOUND_FORMAT_PCM24:
            {
                char *srcptr, *destptr;

                srcptr = destptr = (char *)mBuffer + pointB;
                srcptr -= (mChannels * 3);
                for (count=0; count < FMOD_DSP_RESAMPLER_OVERFLOWLENGTH * mChannels; count++)
                {
                    destptr[0] = srcptr[0];
                    destptr[1] = srcptr[1];
                    destptr[2] = srcptr[2];
                    destptr += 3;
                    srcptr  -= 3;
                }
                break;
            }
            case FMOD_SOUND_FORMAT_PCM32:
            case FMOD_SOUND_FORMAT_PCMFLOAT:
            {
                int *srcptr, *destptr;

                srcptr = destptr = (int *)((char *)mBuffer + pointB);
                srcptr -= mChannels;
                for (count=0; count < FMOD_DSP_RESAMPLER_OVERFLOWLENGTH * mChannels; count++)
                {
                    *destptr = *srcptr;
                    destptr++;
                    srcptr--;
                }
                break;
            }
            default:
            {
                break;
            }
        }
    }
    else if (mMode & FMOD_LOOP_NORMAL)
    {
        if (mLoopPointDataEndOffset)
        {
            FMOD_memcpy((char *)mBuffer + mLoopPointDataEndOffset, mLoopPointDataEnd, overflowbytes);
        }

        FMOD_memcpy(mLoopPointDataEnd, (char *)mBuffer + pointB, overflowbytes);
        mLoopPointDataEndOffset = pointB;

        FMOD_memcpy((char *)mBuffer + pointB, (char *)mBuffer + pointA, overflowbytes);
    }
    else if (mMode & FMOD_LOOP_OFF)
    {
        if (mLoopPointDataEndOffset)
        {
            FMOD_memcpy((char *)mBuffer + mLoopPointDataEndOffset, mLoopPointDataEnd, overflowbytes);
            mLoopPointDataEndOffset = 0;
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
FMOD_RESULT SampleSoftware::restoreLoopPointData()
{
    FMOD_RESULT result;
    unsigned int overflowbytes, pointA, pointB;

    if ((mFormat != FMOD_SOUND_FORMAT_PCM8 &&
         mFormat != FMOD_SOUND_FORMAT_PCM16 &&
         mFormat != FMOD_SOUND_FORMAT_PCM24 &&
         mFormat != FMOD_SOUND_FORMAT_PCM32 &&
         mFormat != FMOD_SOUND_FORMAT_PCMFLOAT) || !mLoopPointDataEnd)
    {
        return FMOD_OK;
    }

    result = getBytesFromSamples(FMOD_DSP_RESAMPLER_OVERFLOWLENGTH, &overflowbytes);
    if (result != FMOD_OK)
    {
        return result;
    }
    result = getBytesFromSamples(mLoopStart, &pointA);
    if (result != FMOD_OK)
    {
        return result;
    }
    result = getBytesFromSamples(mLoopStart + mLoopLength, &pointB);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (mLoopPointDataEndOffset)
    {
        FMOD_memcpy((char *)mBuffer + mLoopPointDataEndOffset, mLoopPointDataEnd, overflowbytes);
        mLoopPointDataEndOffset = 0;
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

FMOD_RESULT SampleSoftware::getMemoryUsedImpl(MemoryTracker *tracker)
{
    tracker->add(false, FMOD_MEMBITS_SOUND, sizeof(SampleSoftware) - sizeof(Sample));

    if (mBuffer)
    {
        unsigned int overflowbytes;

        if (mFormat == FMOD_SOUND_FORMAT_IMAADPCM || mFormat == FMOD_SOUND_FORMAT_XMA || mFormat == FMOD_SOUND_FORMAT_MPEG)
        {
            overflowbytes = 0;
        }
        else
        {
            SoundI::getBytesFromSamples(FMOD_DSP_RESAMPLER_OVERFLOWLENGTH, &overflowbytes, mChannels, mFormat);
        }
        
        if (0){}
        #ifdef PLATFORM_PS3
        else if ((mMode & FMOD_LOADSECONDARYRAM) && OutputPS3::mRSXPoolInitialised)
        {
            tracker->add(false, FMOD_MEMBITS_SOUND_SECONDARYRAM, mLengthBytes + (overflowbytes * 2) + 16);
        }
        #endif
        else if ((mMode & FMOD_LOADSECONDARYRAM) && (FMOD::gGlobal->gMemoryTypeFlags & FMOD_MEMORY_SECONDARY))
        {
            tracker->add(false, FMOD_MEMBITS_SOUND_SECONDARYRAM, mLengthBytes + (overflowbytes * 2) + 16);
        }
        else if (!(mMode & FMOD_OPENMEMORY_POINT))
        {
            tracker->add(false, FMOD_MEMBITS_SOUND, mLengthBytes + (overflowbytes * 2) + 16);
        }
    }

    if (mLoopPointDataEnd != mLoopPointDataEndMemory)
    {
        unsigned int overflowbytes = 0;

        SoundI::getBytesFromSamples(FMOD_DSP_RESAMPLER_OVERFLOWLENGTH, &overflowbytes, mChannels, mFormat);

        tracker->add(false, FMOD_MEMBITS_SOUND, overflowbytes);
    }

    return Sample::getMemoryUsedImpl(tracker);
}

#endif

}

#endif
