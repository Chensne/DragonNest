#include "fmod_settings.h"

#include "fmod_channeli.h"
#include "fmod_channel_real.h"
#include "fmod_channelpool.h"
#include "fmod_memory.h"
#include "fmod_soundi.h"
#include "fmod_channel_software.h"

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
ChannelPool::ChannelPool()
{
    mChannel      = 0;
    mNumChannels  = 0;
    mChannelsUsed = 0;
    mSystem       = 0;
    mOutput       = 0;
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
FMOD_RESULT ChannelPool::init(SystemI *system, Output *output, int numchannels)
{
    if (numchannels < 0)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (numchannels)
    {
        mChannel = (ChannelReal **)FMOD_Memory_Calloc(numchannels * sizeof(ChannelReal *));
        if (!mChannel)
        {
            return FMOD_ERR_MEMORY;
        }
    }
    
    mNumChannels = numchannels;
    mSystem = system;
    mOutput = output;

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
FMOD_RESULT ChannelPool::release()
{
    int count;

    if (mChannel)
    {
        for (count = 0; count < mNumChannels; count++)
        {
            if (mChannel[count])
            {
                mChannel[count]->close();
            }
        }

        FMOD_Memory_Free(mChannel);
    }

    FMOD_Memory_Free(this);

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
FMOD_RESULT ChannelPool::allocateChannel(ChannelReal **realchannel, int index, int numchannels, int *found_out, bool ignorereserved)
{
    int count;
    int found = 0;

    if (!realchannel)
    {
        if (found_out)
        {
            *found_out = found;
        }
        return FMOD_ERR_INVALID_PARAM;
    } 

    if (index == FMOD_CHANNEL_FREE)
    {
        for (count = 0; count < mNumChannels; count++)
        {
            FMOD_RESULT result;
            bool playing;
        
            if (mChannel[count]->mFlags & CHANNELREAL_FLAG_ALLOCATED || 
                mChannel[count]->mFlags & CHANNELREAL_FLAG_IN_USE ||
               (mChannel[count]->mFlags & CHANNELREAL_FLAG_RESERVED && !ignorereserved))
            {
                continue;
            }
        
            result = mChannel[count]->isPlaying(&playing, true);
            if (result == FMOD_OK && !playing)
            {
                mChannel[count]->mFlags |= CHANNELREAL_FLAG_ALLOCATED;
                mChannel[count]->mFlags |= CHANNELREAL_FLAG_IN_USE;
                mChannel[count]->mFlags &= ~CHANNELREAL_FLAG_STOPPED;
                mChannel[count]->mFlags &= ~CHANNELREAL_FLAG_RESERVED;

                realchannel[found] = mChannel[count];
                found++;
                if (found == numchannels)
                {
                    if (found_out)
                    {
                        *found_out = found;
                    }
                    return FMOD_OK;
                }
            }
        }
    }
    else if (index >= 0 && index < mNumChannels)
    {  
        if (numchannels > 1)
        {
            return FMOD_ERR_CHANNEL_ALLOC;
        }

        mChannel[index]->mFlags |= CHANNELREAL_FLAG_ALLOCATED;
        mChannel[index]->mFlags |= CHANNELREAL_FLAG_IN_USE;
        mChannel[index]->mFlags &= ~CHANNELREAL_FLAG_STOPPED;

        realchannel[0] = mChannel[index];
        return FMOD_OK;
    }

    /*
        Undo the allocations that occured before just in case they are not used.
    */
    for (count = 0; count < found; count++)
    {
        if (realchannel[count])
        {
            realchannel[count]->mFlags &= ~CHANNELREAL_FLAG_ALLOCATED;
            realchannel[count]->mFlags &= ~CHANNELREAL_FLAG_IN_USE;
            realchannel[count]->mFlags |= CHANNELREAL_FLAG_STOPPED;
        }
    }

    if (found_out)
    {
        *found_out = found;
    }

    return FMOD_ERR_CHANNEL_ALLOC;
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
FMOD_RESULT ChannelPool::getNumChannels(int *numchannels)
{
    if (!numchannels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *numchannels = mNumChannels;

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
FMOD_RESULT ChannelPool::getChannelsUsed(int *numchannels)
{
    if (!numchannels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *numchannels = mChannelsUsed;
    
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
FMOD_RESULT ChannelPool::setChannel(int index, ChannelReal *channel, DSPI *dspmixtarget)
{
    if (!channel || index < 0 || index >= mNumChannels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mChannel[index] = channel;
    mChannel[index]->mPool = this;

    return mChannel[index]->init(index, mSystem, mOutput, dspmixtarget);
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
FMOD_RESULT ChannelPool::getChannel(int index, ChannelReal **channel)
{
    if (!channel || index < 0 || index >= mNumChannels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *channel = mChannel[index];

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

#ifdef FMOD_SUPPORT_MEMORYTRACKER

FMOD_RESULT ChannelPool::getMemoryUsedImpl(MemoryTracker *tracker)
{
    tracker->add(false, FMOD_MEMBITS_CHANNEL, sizeof(*this));

    if (mChannel)
    {
        tracker->add(false, FMOD_MEMBITS_CHANNEL, mNumChannels * sizeof(ChannelReal *));
    }

    return FMOD_OK;
}

#endif

}
