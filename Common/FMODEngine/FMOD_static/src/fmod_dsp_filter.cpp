#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_SOFTWARE

#include "fmod_dsp_filter.h"
#include "fmod_localcriticalsection.h"
#include "fmod_historybuffer_pool.h"

#ifdef PLATFORM_PS3_SPU
    #include "fmod_systemi_spu.h"
#else
    #include "fmod_systemi.h"
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
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT DSPFilter::release(bool freethis)
{
    stopBuffering();

    return DSPI::release(freethis);
}

#if !defined(FMOD_SUPPORT_MIXER_NONRECURSIVE)
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
FMOD_RESULT DSPFilter::read(float **outbuffer, int *outchannels, unsigned int *length, FMOD_SPEAKERMODE speakermode, int speakermodechannels, unsigned int tick)
{
    FMOD_RESULT result = FMOD_OK;

    if (*length > mSystem->mDSPBlockSize)
    {
        *length = mSystem->mDSPBlockSize;
    }

    mFlags |= FMOD_DSP_FLAG_IDLE;
    *outbuffer = 0;
    *outchannels = 0;

    if (mDSPTick != tick)
    {
        LinkedListNode  *current           = 0;
        bool             hasmixed          = false;
        int              connectionnumber  = 0;
	    unsigned int     starttime         = 0;
	    unsigned int     endtime           = 0;

        if (mSystem->mFlags & FMOD_INIT_ENABLE_PROFILE)
        {   
            FMOD_OS_Time_GetNs(&starttime);
        }

        /*
            Loop through inputs and read from them, mixing into 'mixbuffer' as we go.
        */
        current = mInputHead.getNext();

        while (current != &mInputHead)
        {
            DSPConnectionI *connection = (DSPConnectionI *)current->getData();

            /*
                If the input is disabled, skip it.
            */
            if (!(connection->mInputUnit->mFlags & FMOD_DSP_FLAG_ACTIVE) || connection->mInputUnit->mFlags & (FMOD_DSP_FLAG_FINISHED | FMOD_DSP_FLAG_QUEUEDFORDISCONNECT))
            {
                connection->mInputUnit->mFlags |= FMOD_DSP_FLAG_IDLE;
            }
            else
            {
                /*
                    Execute is recursive, so don't time inclusive of inputs
                */
                if (mSystem->mFlags & FMOD_INIT_ENABLE_PROFILE)
                {  
			        FMOD_OS_Time_GetNs(&endtime);
                    mCPUUsageTemp += endtime - starttime;
                }
    			
			    result = connection->mInputUnit->read(outbuffer, outchannels, length, speakermode, speakermodechannels, tick);
                if (result != FMOD_OK)
                {
                    break;
                }

                if (mSystem->mFlags & FMOD_INIT_ENABLE_PROFILE)
                {  
			        FMOD_OS_Time_GetNs(&starttime);
                }

                /*
                    Check if we should mix or not.
                */
                bool hastomix = true;

                if (connection->mInputUnit->mFlags & FMOD_DSP_FLAG_IDLE)
                {
                    hastomix = false;
                }
                else 
                {
                    mFlags &= ~FMOD_DSP_FLAG_IDLE;

                    if (mNumInputs <= 1 && connection->mVolume == 1.0f)
                    {
                        hastomix = false;

                        if (mDescription.mCategory == FMOD_DSP_CATEGORY_SOUNDCARD && *outchannels != speakermodechannels)
                        {
                            hastomix = true;
                        }
                        else if (connection->mSetLevelsUsed && connection->checkUnity(*outchannels, speakermodechannels) != FMOD_OK)
                        {
                            hastomix = true;
                        }
                        else if (mDescription.read && mDescription.channels && mDescription.channels != *outchannels)
                        {
                            hastomix = true;    /* This is only because we need to need to set the channel count to that of mDescription.channels? */
                        }
                    }
                }
                if (hastomix)
                {
                    /*
                        If a new connection happened and no pan has been set, set it here.
                    */
                    if (!connection->mSetLevelsUsed)
                    {
                        if (speakermodechannels == *outchannels && connection->mVolume == 1.0f)
                        {
                            connection->setUnity();
                        }
                        else
                        {
                            connection->setPan(0.0f, speakermodechannels, *outchannels, speakermode);
                        }
                        connection->mSetLevelsUsed = true;
                    }

                    /* 
                        If it is the first input, give it a blank buffer to mix to.
                    */
                    if (!hasmixed)
                    {
                        FMOD_memset(mBuffer, 0, *length * speakermodechannels * sizeof(float));
                    }

                    /*
                        Mix from the input's buffer into this units mix destination
                    */
                    connection->mix(mBuffer, (float *)*outbuffer, speakermodechannels, *outchannels, *length);

                    hasmixed = true;
                }
                else
                {
                    if (connection->mRampCount)
                    {
                        int count, count2;

                        for (count = 0; count < connection->mMaxOutputLevels; count++)
                        {
                            for (count2 = 0; count2 < connection->mMaxInputLevels; count2++)
                            {
                                connection->mLevelCurrent[count][count2] = DSP_LEVEL_COMPRESS(DSP_LEVEL_DECOMPRESS(connection->mLevel[count][count2]) * connection->mVolume);
                                connection->mLevelDelta  [count][count2] = 0;
                            }
                        }
                        connection->mRampCount = 0;
                    }            
                }
            }
                    
            connection->mInputUnit->mDSPTick = tick;
            current = current->getNext();
            connectionnumber++;
        }

        if (hasmixed)
        {
            *outbuffer = mBuffer;
            *outchannels = speakermodechannels;
        }

        /*
            Call the plugin callback to actually process or generate the data for this unit.
        */
        if (mDescription.read && !(mFlags & FMOD_DSP_FLAG_BYPASS))
        {
            float *src = *outbuffer;    /* source is whatever was passed to us from above. */

            if (src == mBuffer || !src)         /* If the src is the same as the dest, we need to copy src into a temp buffer. */
            {
                if (src)
                {
                    FMOD_memcpy(mSystem->mDSPTempBuff, src, *length * *outchannels * sizeof(float));
                }
                src = mSystem->mDSPTempBuff;
            }

            if (mDescription.channels)
            {
                *outchannels = mDescription.channels;
                FMOD_memset(src, 0, *length * mDescription.channels * sizeof(float));
            }
            else if (!*outchannels)
            {
                *outchannels = speakermodechannels;
            }

            if (mFlags & FMOD_DSP_FLAG_IDLE)
            {
                FMOD_memset(src, 0, *length * *outchannels * sizeof(float));
            }


            instance = (FMOD_DSP *)this;           
            mDescription.read((FMOD_DSP_STATE *)this, src, mBuffer, *length, *outchannels, *outchannels);

            *outbuffer = mBuffer;

            mFlags &= ~FMOD_DSP_FLAG_IDLE;
        }

        if (mNumOutputs > 1 && mDescription.mCategory != FMOD_DSP_CATEGORY_RESAMPLER)   /* A multiinput resampler doesnt need its mbuffer stomped on. */
        {
            if (!hasmixed && *outbuffer != mBuffer)
            {
                if (*outbuffer)
                {
                    FMOD_memcpy(mBuffer, (float *)*outbuffer, *length * *outchannels * sizeof(float));
                }
                else
                {
                    FMOD_memset(mBuffer, 0, *length * *outchannels * sizeof(float));
                }

                *outbuffer = mBuffer;        /* Return the already processed buffer */
            }
            
            mBufferChannels = *outchannels;
            mFlags &= ~FMOD_DSP_FLAG_IDLE;
        }

        /*
            If the history buffer option is set, buffer off this data into a separate ring buffer.
        */
        if (mHistoryBuffer)
        {
            float *srcptr, *destptr;
            int len;
    
            destptr = mHistoryBuffer;
            srcptr = (float *)*outbuffer;

            if (!*outchannels)
            {
                *outchannels = speakermodechannels;
            }

            len = *length;
            while (len)
            { 
                int size = len;
                if (mHistoryPosition + size > FMOD_HISTORYBUFFERLEN)
                {
                    size = FMOD_HISTORYBUFFERLEN - mHistoryPosition;
                }

                if (!*outbuffer)
                {
                    FMOD_memset(destptr + (mHistoryPosition * *outchannels), 0, size * *outchannels * sizeof(float));
                }
                else
                {
                    FMOD_memcpy(destptr + (mHistoryPosition * *outchannels), srcptr, size * *outchannels * sizeof(float));
                }

                len -= size;
                srcptr += (size * *outchannels);
                
                mHistoryPosition += size;
                if (mHistoryPosition >= FMOD_HISTORYBUFFERLEN)
                {
                    mHistoryPosition = 0;
                }
            }
        }

        if (mSystem->mFlags & FMOD_INIT_ENABLE_PROFILE)
        {  
	        FMOD_OS_Time_GetNs(&endtime);
            mCPUUsageTemp += endtime - starttime;

#if defined(FMOD_SUPPORT_PROFILE_DSP_VOLUMELEVELS) && !defined(PLATFORM_PS3)            
            if (mDescription.mCategory != FMOD_DSP_CATEGORY_SOUNDCARD)
            {
                calculatePeaks(*outbuffer, *length, *outchannels);
            }
#endif
            if (mDescription.mCategory != FMOD_DSP_CATEGORY_RESAMPLER && mDescription.mCategory != FMOD_DSP_CATEGORY_SOUNDCARD)
            {
                mCPUUsage = mCPUUsageTemp;
                mCPUUsageTemp = 0;
            }
        }
    }
    else
    {
        *outbuffer = mBuffer;        /* Return the already processed buffer */
        *outchannels = mBufferChannels;
        mFlags &= ~FMOD_DSP_FLAG_IDLE;
    }

    return result;
}
#endif


#if !defined(PLATFORM_PS3) && !defined(PLATFORM_WINDOWS_PS3MODE)
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
FMOD_RESULT DSPFilter::startBuffering()
{
    FMOD_RESULT result;
    int channels;
    LocalCriticalSection  criticalsection(mSystem->mDSPCrit);

    if (mHistoryBuffer)
    {
        return FMOD_OK;
    }

    criticalsection.enter();

    result = mSystem->getSoftwareFormat(0, 0, &channels, 0, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (mHistoryBuffer)
    {
        result = releaseHistoryBuffer(mHistoryBuffer);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    mHistoryPosition = 0;
   
    if (channels < mSystem->mMaxInputChannels)
    {
        channels = mSystem->mMaxInputChannels;
    }

    result = createHistoryBuffer(&mHistoryBuffer, channels);
    if (result != FMOD_OK)
    {
        return result;
    }

    criticalsection.leave();

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
FMOD_RESULT DSPFilter::getHistoryBuffer(float **buffer, unsigned int *position, unsigned int *length)
{
    if (buffer)
    {
        *buffer = mHistoryBuffer;
    }

    if (position)
    {
        *position = mHistoryPosition;
    }

    if (length)
    {
        *length = FMOD_HISTORYBUFFERLEN;
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
FMOD_RESULT DSPFilter::stopBuffering()
{
    if (mHistoryBuffer)
    {
        FMOD_RESULT result;

        LocalCriticalSection  criticalsection(mSystem->mDSPCrit, true);
        result = releaseHistoryBuffer(mHistoryBuffer);
        if (result != FMOD_OK)
        {
            return FMOD_OK;
        }
        mHistoryBuffer = 0;
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

FMOD_RESULT DSPFilter::getMemoryUsedImpl(MemoryTracker *tracker)
{
    if (mHistoryBuffer)
    {
        int channels = 0;

        CHECK_RESULT(mSystem->getSoftwareFormat(0, 0, &channels, 0, 0, 0));

        if (channels < mSystem->mMaxInputChannels)
        {
            channels = mSystem->mMaxInputChannels;
        }
        tracker->add(false, FMOD_MEMBITS_DSP, FMOD_HISTORYBUFFERLEN * channels * sizeof(float));
    }
   
    return FMOD_OK;
}

#endif

}

#endif
