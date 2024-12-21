#include "fmod_settings.h"

#include "fmod_channelgroupi.h"
#include "fmod_systemi.h"
#include "fmod_dsp_fft.h"
#include "fmod_dsp_filter.h"
#include "fmod_outputi.h"

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
FMOD_RESULT ChannelGroupI::validate(ChannelGroup *channelgroup, ChannelGroupI **channelgroupi)
{
    FMOD::ChannelGroupI *cg = (FMOD::ChannelGroupI *)channelgroup;

    if (!channelgroup)
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (!channelgroupi)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *channelgroupi = cg;

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

FMOD_RESULT ChannelGroupI::release()
{  
    if (this == mSystem->mChannelGroup)
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    return releaseInternal();
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
FMOD_RESULT ChannelGroupI::releaseInternal(bool releasechildren)
{  
    if (mGroupHead && releasechildren)
    {
        ChannelGroupI *currentgroup = (ChannelGroupI *)(mGroupHead->getNext());

        while (currentgroup != mGroupHead)
        {
            ChannelGroupI *next = (ChannelGroupI *)currentgroup->getNext();
        
            currentgroup->releaseInternal(true);
        
            currentgroup = next;
        }
    }

    /*
        First move all channels back to the main group
    */
    if (mSystem->mChannelGroup && this != mSystem->mChannelGroup)
    {
        while (mChannelHead.getNext() != &mChannelHead)
        {
            ChannelI *channel = (ChannelI *)mChannelHead.getNext()->getData();
            
            channel->setChannelGroup(mSystem->mChannelGroup);
        }
    }

    if (mDSPHead && mDSPMixTarget && mDSPMixTarget != mDSPHead)
    {
        /*
            If mDSPMixTarget != mDSPHead then the user added their own DSP unit to this ChannelGroup using addDSP and we had 
            to create a new mDSPHead on the fly. In this case, we need to free that mDSPHead's memory (mDSPHead->release())
            but not free the mDSPMixTarget's memory (mDSPMixTarget->release(false)) because mDSPMixTarget is pointing to the
            original mDSPHead which is a pointer to a DSPFilter object statically declared in the ChannelGroup class. It's like
            that to save on memory allocs.

            AJS 07/07/08
        */

        mDSPMixTarget->release(false);
        mDSPMixTarget = 0;

        if (mDSPHead)
        {
            mDSPHead->release();
            mDSPHead = 0;
        }
    }
    else
    {
        if (mDSPHead)
        {
            mDSPHead->release(false);
            mDSPHead = 0;
        }

        mDSPMixTarget = 0;
    }

    if (mName)
    {
        FMOD_Memory_Free(mName);
    }

    if (mGroupHead)
    {
        ChannelGroupI *currentgroup = (ChannelGroupI *)(mGroupHead->getNext());
        ChannelGroupI *mastergroup;

        mSystem->getMasterChannelGroup(&mastergroup);

        /*
            If it is not the master, put any children groups back onto the master.
        */
        if (mastergroup && this != mastergroup)
        {
            while (currentgroup != mGroupHead)
            {
                ChannelGroupI *next = (ChannelGroupI *)currentgroup->getNext();
            
                mastergroup->addGroup(currentgroup);
            
                currentgroup = next;
            }
        }
        
        FMOD_Memory_Free(mGroupHead);
    }

    if (this == mSystem->mOutput->mMusicChannelGroup)
    {
        mSystem->mOutput->mMusicChannelGroup = 0;
    }

    removeNode();

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

FMOD_RESULT ChannelGroupI::getSystemObject(System **system)
{
    if (!system)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *system = (System *)mSystem;

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
FMOD_RESULT ChannelGroupI::setVolume(float volume)
{
    FMOD_RESULT result;

    if (volume < 0)
    {
        volume = 0;
    }
    if (volume > 1)
    {
        volume = 1;
    }

    mVolume = volume;

    result = setVolumeInternal();
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
FMOD_RESULT ChannelGroupI::setVolumeInternal()
{
    LinkedListNode *current;
    bool forceupdatepos = false;
    float newvolume;

    newvolume = (mParent ? mParent->mRealVolume : 1.0f) * mVolume;

    if (newvolume != mRealVolume)
    {
        forceupdatepos = true;
    }

    mRealVolume = newvolume;

    if (mGroupHead)
    {
        ChannelGroupI *currentgroup = (ChannelGroupI *)(mGroupHead->getNext());

        while (currentgroup != mGroupHead)
        {
            currentgroup->setVolumeInternal();

            currentgroup = (ChannelGroupI *)currentgroup->getNext();
        }
    }

    current = mChannelHead.getNext();

    while (current != &mChannelHead)
    {
        ChannelI *channel = (ChannelI *)current->getData();
        float v;

        channel->getVolume(&v);
        channel->setVolume(v, forceupdatepos);

        current = current->getNext();
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
FMOD_RESULT ChannelGroupI::getVolume(float *volume)
{   
    if (!volume)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *volume = mVolume;

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
FMOD_RESULT ChannelGroupI::setPitch(float pitch)
{
    FMOD_RESULT result;

    if (pitch < 0)
    {
        pitch = 0;
    }

    mPitch = pitch;

    result = setPitchInternal();
    if (result != FMOD_OK)
    {
        return result;
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
FMOD_RESULT ChannelGroupI::setPitchInternal()
{
    LinkedListNode *current;
    
    mRealPitch = (mParent ? mParent->mRealPitch : 1.0f) * mPitch;

    if (mGroupHead)
    {
        ChannelGroupI *currentgroup = (ChannelGroupI *)(mGroupHead->getNext());

        while (currentgroup != mGroupHead)
        {
            currentgroup->setPitchInternal();

            currentgroup = (ChannelGroupI *)currentgroup->getNext();
        }
    }

    current = mChannelHead.getNext();

    while (current != &mChannelHead)
    {
        ChannelI *channel = (ChannelI *)current->getData();
        float freq;

        channel->getFrequency(&freq);
        channel->setFrequency(freq);

        current = current->getNext();
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
FMOD_RESULT ChannelGroupI::getPitch(float *pitch)
{   
    if (!pitch)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *pitch = mPitch;

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
FMOD_RESULT	ChannelGroupI::set3DOcclusion(float direct, float reverb)
{
    FMOD_RESULT result;

    direct = FMOD_MAX(0, FMOD_MIN(direct, 1));
    reverb = FMOD_MAX(0, FMOD_MIN(reverb, 1));

    mDirectOcclusion = direct;
    mReverbOcclusion = reverb;

    result = set3DOcclusionInternal();
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
FMOD_RESULT	ChannelGroupI::set3DOcclusionInternal()
{
    LinkedListNode *current;

    mRealDirectOcclusionVolume = (mParent ? mParent->mRealDirectOcclusionVolume : 1.0f) * (1.0f - mDirectOcclusion);
    mRealReverbOcclusionVolume = (mParent ? mParent->mRealReverbOcclusionVolume : 1.0f) * (1.0f - mReverbOcclusion);

    if (mGroupHead)
    {
        ChannelGroupI *currentgroup = (ChannelGroupI *)(mGroupHead->getNext());

        while (currentgroup != mGroupHead)
        {
            currentgroup->set3DOcclusionInternal();

            currentgroup = (ChannelGroupI *)currentgroup->getNext();
        }
    }

    current = mChannelHead.getNext();

    while (current != &mChannelHead)
    {
        ChannelI *channel = (ChannelI *)current->getData();
        float direct, reverb;

        /*
            Ignore return codes, as we want to do this for all channels
            regardless of individual failures
        */
        channel->get3DOcclusionInternal(&direct, &reverb);
        channel->set3DOcclusionInternal(direct, reverb, false);

        current = current->getNext();
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
FMOD_RESULT ChannelGroupI::get3DOcclusion(float *directOcclusion, float *reverbOcclusion)
{   
    if (directOcclusion)
    {
        *directOcclusion = mDirectOcclusion;
    }
    if (reverbOcclusion)
    {
        *reverbOcclusion = mReverbOcclusion;
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
FMOD_RESULT ChannelGroupI::setPaused(bool paused, bool setpausedflag)
{
    LinkedListNode *current;
    
	if (setpausedflag)
	{
		mPaused = paused;
	}

    if (mGroupHead)
    {
        ChannelGroupI *currentgroup = (ChannelGroupI *)(mGroupHead->getNext());

        while (currentgroup != mGroupHead)
        {
            currentgroup->setPaused(paused, false);

            currentgroup = (ChannelGroupI *)currentgroup->getNext();
        }
    }
    
    current = mChannelHead.getNext();

    while (current != &mChannelHead)
    {
        ChannelI *channel = (ChannelI *)current->getData();

        bool paused = false;

        // ignore return values here in case we get FMOD_ERR_INVALID_HANDLE
        channel->getPaused(&paused);
        channel->setPaused(paused);

        current = current->getNext();
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
FMOD_RESULT ChannelGroupI::getPaused(bool *paused)
{
    if (!paused)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *paused = mPaused;

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
FMOD_RESULT ChannelGroupI::setMute(bool mute, bool setmuteflag)
{
    LinkedListNode *current;

	if (setmuteflag)
	{
		mMute = mute;
	}

    if (mGroupHead)
    {
        ChannelGroupI *currentgroup = (ChannelGroupI *)(mGroupHead->getNext());

        while (currentgroup != mGroupHead)
        {
            currentgroup->setMute(mute, false);

            currentgroup = (ChannelGroupI *)currentgroup->getNext();
        }
    }
       
    current = mChannelHead.getNext();

    while (current != &mChannelHead)
    {
        ChannelI *channel = (ChannelI *)current->getData();

        channel->setMute(channel->mFlags & CHANNELI_FLAG_MUTED ? true : false);

        current = current->getNext();
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
FMOD_RESULT ChannelGroupI::getMute(bool *mute)
{
    if (!mute)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *mute = mMute;

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
FMOD_RESULT ChannelGroupI::stop()
{
    LinkedListNode *current;
    
    if (mGroupHead)
    {
        ChannelGroupI *currentgroup = (ChannelGroupI *)(mGroupHead->getNext());

        while (currentgroup != mGroupHead)
        {
            currentgroup->stop();

            currentgroup = (ChannelGroupI *)currentgroup->getNext();
        }
    }
    
    current = mChannelHead.getNext();

    while (current != &mChannelHead)
    {
        ChannelI *channel = (ChannelI *)current->getData();
        LinkedListNode *next = current->getNext();

        channel->stop();

        current = next;
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
FMOD_RESULT ChannelGroupI::overrideVolume(float volume)
{
    LinkedListNode *current;

    if (mGroupHead)
    {
        ChannelGroupI *currentgroup = (ChannelGroupI *)(mGroupHead->getNext());

        while (currentgroup != mGroupHead)
        {
            currentgroup->overrideVolume(volume);

            currentgroup = (ChannelGroupI *)currentgroup->getNext();
        }
    }
    
    current = mChannelHead.getNext();

    while (current != &mChannelHead)
    {
        ChannelI *channel = (ChannelI *)current->getData();

        channel->setVolume(volume);

        current = current->getNext();
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
FMOD_RESULT ChannelGroupI::overrideFrequency(float frequency)
{
    LinkedListNode *current;

    if (mGroupHead)
    {
        ChannelGroupI *currentgroup = (ChannelGroupI *)(mGroupHead->getNext());

        while (currentgroup != mGroupHead)
        {
            currentgroup->overrideFrequency(frequency);

            currentgroup = (ChannelGroupI *)currentgroup->getNext();
        }
    }
    
    current = mChannelHead.getNext();

    while (current != &mChannelHead)
    {
        ChannelI *channel = (ChannelI *)current->getData();

        channel->setFrequency(frequency);

        current = current->getNext();
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
FMOD_RESULT ChannelGroupI::overridePan(float pan)
{
    LinkedListNode *current;

    if (mGroupHead)
    {
        ChannelGroupI *currentgroup = (ChannelGroupI *)(mGroupHead->getNext());

        while (currentgroup != mGroupHead)
        {
            currentgroup->overridePan(pan);

            currentgroup = (ChannelGroupI *)currentgroup->getNext();
        }
    }
    
    current = mChannelHead.getNext();

    while (current != &mChannelHead)
    {
        ChannelI *channel = (ChannelI *)current->getData();

        channel->setPan(pan);

        current = current->getNext();
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
FMOD_RESULT ChannelGroupI::overrideReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES *prop)
{
    LinkedListNode *current;
   
    if (!prop)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (mGroupHead)
    {
        ChannelGroupI *currentgroup = (ChannelGroupI *)(mGroupHead->getNext());

        while (currentgroup != mGroupHead)
        {
            currentgroup->overrideReverbProperties(prop);

            currentgroup = (ChannelGroupI *)currentgroup->getNext();
        }
    }
  
    current = mChannelHead.getNext();

    while (current != &mChannelHead)
    {
        ChannelI *channel = (ChannelI *)current->getData();

        channel->setReverbProperties(prop);

        current = current->getNext();
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
FMOD_RESULT ChannelGroupI::override3DAttributes(const FMOD_VECTOR *pos, const FMOD_VECTOR *vel)
{
    LinkedListNode *current;
    
    if (mGroupHead)
    {
        ChannelGroupI *currentgroup = (ChannelGroupI *)(mGroupHead->getNext());

        while (currentgroup != mGroupHead)
        {
            currentgroup->override3DAttributes(pos, vel);

            currentgroup = (ChannelGroupI *)currentgroup->getNext();
        }
    }

    current = mChannelHead.getNext();

    while (current != &mChannelHead)
    {
        ChannelI *channel = (ChannelI *)current->getData();

        channel->set3DAttributes(pos, vel);

        current = current->getNext();
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
FMOD_RESULT ChannelGroupI::overrideSpeakerMix(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright)
{
    LinkedListNode *current;
    
    if (mGroupHead)
    {
        ChannelGroupI *currentgroup = (ChannelGroupI *)(mGroupHead->getNext());

        while (currentgroup != mGroupHead)
        {
            currentgroup->overrideSpeakerMix(frontleft, frontright, center, lfe, backleft, backright, sideleft, sideright);

            currentgroup = (ChannelGroupI *)currentgroup->getNext();
        }
    }

    current = mChannelHead.getNext();

    while (current != &mChannelHead)
    {
        ChannelI *channel = (ChannelI *)current->getData();

        channel->setSpeakerMix(frontleft, frontright, center, lfe, backleft, backright, sideleft, sideright);

        current = current->getNext();
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
FMOD_RESULT ChannelGroupI::updateChildMixTarget(DSPI *dsp)
{
    FMOD_RESULT result;
    DSPI *olddsp = mDSPMixTarget;

    if (mDSPHead)
    {
        return FMOD_OK;         /* Child groups should already be pointing to this dsp.  No need to recurse down */
    }
    else
    {
        mDSPMixTarget = dsp;    /* set this mixtarget to be the parent's dsp. */
    }

    if (mGroupHead)
    {
        for (LinkedListNode *node = mGroupHead->getNext(); node != mGroupHead; node = node->getNext())
        {
            ChannelGroupI *c = (ChannelGroupI *)node;
        
            result = c->updateChildMixTarget(dsp);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
    }

    /* Fix up channels. */
    {
        LinkedListNode *current;
    
        current = mChannelHead.getNext();

        while (current != &mChannelHead)
        {
            FMOD::DSPI *dsphead;
            // cache next because setChannelGroupInternal modifies the list
            LinkedListNode *next = current->getNext();
            ChannelI *channel = (ChannelI *)current->getData();

            result = channel->getDSPHead(&dsphead);
            if (result == FMOD_OK)
            {
                result = olddsp->disconnectFrom(dsphead);
                if (result != FMOD_OK)
                {
                    return result;
                }

                result = channel->setChannelGroupInternal(this, true, true);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }

            current = next;
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
FMOD_RESULT ChannelGroupI::addGroup(ChannelGroupI *group)
{
    FMOD_RESULT result;

    if (!group)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    /*
        Disconnect new group from where it came.
    */
    group->removeNode();

    if (group->mDSPHead)
    {
        group->mDSPHead->disconnectAll(false, true);
    }

    if (!mGroupHead)
    {
        mGroupHead = FMOD_Object_Calloc(ChannelGroupI);
        if (!mGroupHead)
        {
            return FMOD_ERR_MEMORY;
        }
    }

    /*
        Add to the new group.
    */
    group->addBefore(mGroupHead);

    if (mDSPMixTarget)
    {    
        if (group->mDSPHead)
        {
            result = mDSPMixTarget->addInputQueued(group->mDSPHead, false, 0, 0);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        else
        {
            result = group->updateChildMixTarget(mDSPMixTarget);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
    }

    group->mParent = this;

    result = group->setPaused(mPaused, false);
    if (result != FMOD_OK)
    {
        return result;
    }
    result = group->setMute(mMute, false);
    if (result != FMOD_OK)
    {
        return result;
    }
    result = group->setVolumeInternal();
    if (result != FMOD_OK)
    {
        return result;
    }
    result = group->setPitchInternal();
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
FMOD_RESULT ChannelGroupI::getNumGroups(int *numgroups)
{
    if (!numgroups)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mGroupHead)
    {
        *numgroups = 0;
    }
    else
    {
        *numgroups = mGroupHead->count();
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
FMOD_RESULT ChannelGroupI::getGroup(int index, ChannelGroupI **group)
{
    ChannelGroupI *current;
    int count, numgroups;

    if (!mGroupHead)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
        
    numgroups = mGroupHead->count();

    if (index < 0 || index >= numgroups)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!group)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    current = (ChannelGroupI *)(mGroupHead->getNext());
    for (count = 0; count < numgroups; count++)
    {
        if (count == index)
        {
            *group = current;
        }

        current = (ChannelGroupI *)(current->getNext());
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
FMOD_RESULT ChannelGroupI::getParentGroup(ChannelGroupI **group)
{
    if (!group)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *group = (ChannelGroupI *)mParent;

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
FMOD_RESULT ChannelGroupI::getDSPHead(DSPI **dsp)
{
    if (!dsp)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mDSPHead)
    {
        *dsp = 0;
        return FMOD_ERR_DSP_NOTFOUND;
    }

    *dsp = mDSPHead;
    
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
FMOD_RESULT ChannelGroupI::addDSP(DSPI *dsp, DSPConnectionI **dspconnection)
{
    FMOD_RESULT result;

    if (!dsp)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mDSPHead)
    {
        return FMOD_ERR_DSP_NOTFOUND;
    }

    /*
        Allocate a new dsp node and make it the new dsphead.  Let the old node now be a seperate mDSPMixTarget with its own memory.
    */
    if (mDSPHead == mDSPMixTarget)
    {
        DSPI *output;
        FMOD_DSP_DESCRIPTION description = mDSPHead->mDescription;

        result = mSystem->createDSP(&description, (DSPI **)&mDSPHead);
        if (result != FMOD_OK)
        {
            return result;
        }
        mDSPHead->setDefaults((float)mSystem->mOutputRate, -1, -1, -1);
        mDSPHead->setActive(true);

        /*
            Now disconnect from parent channelgroup (or maybe the dsp effect the parent may have added).
        */
        result = mDSPMixTarget->getOutput(0, &output);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = output->disconnectFrom(mDSPMixTarget);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = output->addInput(mDSPHead);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mDSPHead->addInput(mDSPMixTarget);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    result = mDSPHead->insertInputBetween(dsp, 0, false, dspconnection);
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
FMOD_RESULT ChannelGroupI::getName(char *name, int namelen)
{
    if (!name)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (namelen > FMOD_STRING_MAXNAMELEN)
    {
        namelen = FMOD_STRING_MAXNAMELEN;
    }

    if (mName)
    {
        FMOD_strncpy(name, mName, namelen);
    }
    else
    {
        FMOD_strncpy(name, "(null)", namelen);
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
FMOD_RESULT ChannelGroupI::getNumChannels(int *numchannels)
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
FMOD_RESULT ChannelGroupI::getChannel(int index, Channel **channel)
{
    int count = 0;
    LinkedListNode *current;

    if (!channel)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *channel = 0;

    current = mChannelHead.getNext();

    while (current != &mChannelHead)
    {
        if (count == index)
        {
            *channel = (Channel *)((ChannelI *)current->getData())->mHandleCurrent;
            return FMOD_OK;
        }

        count++;
        current = current->getNext();
    }

    return FMOD_ERR_INVALID_PARAM;
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
FMOD_RESULT ChannelGroupI::getSpectrum(float *spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype)
{
#ifdef FMOD_SUPPORT_GETSPECTRUM
    FMOD_RESULT     result = FMOD_OK;
    float          *buffer;
    unsigned int    position, length;
    int             numchannels, windowsize;
    static DSPFFT   fft;
    DSPFilter      *dsphead = (DSPFilter *)mDSPHead;

    if (!dsphead)
    {
        return FMOD_ERR_DSP_NOTFOUND;
    }

    windowsize = numvalues * 2;

    if (windowsize != (1 <<  7) &&
        windowsize != (1 <<  8) &&
        windowsize != (1 <<  9) &&
        windowsize != (1 << 10) &&
        windowsize != (1 << 11) &&
        windowsize != (1 << 12) &&
        windowsize != (1 << 13) &&
        windowsize != (1 << 14))
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    result = mSystem->getSoftwareFormat(0, 0, &numchannels, 0, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (channeloffset >= numchannels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    result = dsphead->startBuffering();
    if (result != FMOD_OK)
    {
        return result;
    }

    result = dsphead->getHistoryBuffer(&buffer, &position, &length);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (windowsize > (int)length)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
   
    position -= windowsize;
    if ((int)position < 0)
    {
        position += length;
    }

    mSystem->mUpdateTimeStamp.stampIn();

    result = fft.getSpectrum(buffer, position, length, spectrumarray, windowsize, channeloffset, numchannels, windowtype);

    mSystem->mUpdateTimeStamp.stampOut(95);

    return FMOD_OK;
#else
    return FMOD_ERR_NEEDSSOFTWARE;
#endif // FMOD_SUPPORT_GETSPECTRUM
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
FMOD_RESULT ChannelGroupI::getWaveData(float *wavearray, int numvalues, int channeloffset)
{
#ifdef FMOD_SUPPORT_SOFTWARE
    FMOD_RESULT     result = FMOD_OK;
    float          *buffer;
    unsigned int    position, length;
    int             numchannels, count;
    DSPFilter      *dsphead = (DSPFilter *)mDSPHead;

    if (!dsphead)
    {
        return FMOD_ERR_DSP_NOTFOUND;
    }

    result = mSystem->getSoftwareFormat(0, 0, &numchannels, 0, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (channeloffset >= numchannels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    result = dsphead->startBuffering();
    if (result != FMOD_OK)
    {
        return result;
    }

    result = dsphead->getHistoryBuffer(&buffer, &position, &length);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (numvalues > (int)length)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    
    position -= numvalues;
    if ((int)position < 0)
    {
        position += length;
    }

    for (count = 0; count < numvalues; count++)
    {
        wavearray[count] = buffer[position*numchannels+channeloffset];
        position++;
        if (position >= length)
        {
            position = 0;
        }
    }

    return FMOD_OK;
#else
    return FMOD_ERR_NEEDSSOFTWARE;
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
FMOD_RESULT ChannelGroupI::setUserData(void *userdata)
{
    mUserData = userdata;

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
FMOD_RESULT ChannelGroupI::getUserData(void **userdata)
{
    if (!userdata)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *userdata = mUserData;

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelGroupI::getMemoryInfo(unsigned int memorybits, unsigned int event_memorybits, unsigned int *memoryused, FMOD_MEMORY_USAGE_DETAILS *memoryused_details)
{
#ifdef FMOD_SUPPORT_MEMORYTRACKER
    GETMEMORYINFO_IMPL
#else
    return FMOD_ERR_UNIMPLEMENTED;
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
#ifdef FMOD_SUPPORT_MEMORYTRACKER

FMOD_RESULT ChannelGroupI::getMemoryUsedImpl(MemoryTracker *tracker)
{
    /*
        This channelgroup object
    */
    tracker->add(false, FMOD_MEMBITS_CHANNELGROUP, sizeof(*this));

    /*
        Channelgroup name, if any
    */
    if (mName)
    {
        tracker->add(false, FMOD_MEMBITS_STRING, FMOD_strlen(mName) + 1);
    }

    /*
        If there are child ChannelGroups then it will have a ChannelGroupI here just for list linkage
    */
    if (mGroupHead)
    {
        tracker->add(false, FMOD_MEMBITS_CHANNELGROUP, sizeof(ChannelGroupI));
    }

    /*
        Head DSP node
    */
    if (mDSPHead)
    {
        tracker->add(false, FMOD_MEMBITS_DSP, sizeof(DSPFilter));
    }

    // if mDSPHead != mDSPMixTarget then any units between them should be counted
//AJS        DSPI           *mDSPMixTarget;        /* By default this just points to mDSPHead unless dsp effects are added. */

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

#if defined(FMOD_SUPPORT_MEMORYTRACKER) && defined(FMOD_SUPPORT_SOFTWARE)

FMOD_RESULT ChannelGroupSoftware::getMemoryUsedImpl(MemoryTracker *tracker)
{
    /*
        This channelgroup object
    */
    tracker->add(false, FMOD_MEMBITS_CHANNELGROUP, sizeof(*this));

    /*
        Channelgroup name, if any
    */
    if (mName)
    {
        tracker->add(false, FMOD_MEMBITS_STRING, FMOD_strlen(mName) + 1);
    }

    /*
        If there are child ChannelGroups then it will have a ChannelGroupI here just for list linkage
    */
    if (mGroupHead)
    {
        tracker->add(false, FMOD_MEMBITS_CHANNELGROUP, sizeof(ChannelGroupI));

        for (LinkedListNode *node = mGroupHead->getNext(); node != mGroupHead; node = node->getNext())
        {
            ChannelGroupSoftware *c = (ChannelGroupSoftware *)node;
            CHECK_RESULT(c->getMemoryUsed(tracker));
        }
    }

    /*
        This points to mDSPHeadMemory now, which has already been counted
    */
/*AJS
    if (mDSPHead)
    {
        tracker->add(MEMTYPE_DSPUNIT, sizeof(DSPFilter));
    }
AJS*/

    // if mDSPHead != mDSPMixTarget then any units between them should be counted
//AJS        DSPI           *mDSPMixTarget;        /* By default this just points to mDSPHead unless dsp effects are added. */

    return FMOD_OK;
}

#endif

}
