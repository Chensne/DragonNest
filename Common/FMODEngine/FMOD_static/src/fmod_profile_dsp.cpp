#include "fmod_settings.h"

#if defined(FMOD_SUPPORT_PROFILE) && defined(FMOD_SUPPORT_PROFILE_DSP)

#include "fmod_profile_dsp.h"
#include "fmod_systemi.h"
#include "fmod_autocleanup.h"

namespace FMOD
{

FMOD_RESULT FMOD_ProfileDsp_Create()
{
    FMOD_RESULT result = FMOD_OK;

    if (gGlobal->gProfileDsp)
    {
        return FMOD_OK;
    }

    gGlobal->gProfileDsp = FMOD_Object_Alloc(ProfileDsp);
    if (!gGlobal->gProfileDsp)
    {
        return FMOD_ERR_MEMORY;
    }

    result = gGlobal->gProfileDsp->init();
    if (result != FMOD_OK)
    {
        gGlobal->gProfileDsp->release();
        gGlobal->gProfileDsp = 0;
        return result;
    }

    return gGlobal->gProfile->registerModule(gGlobal->gProfileDsp);
}


FMOD_RESULT FMOD_ProfileDsp_Release()
{
    FMOD_RESULT result = FMOD_OK;

    if (gGlobal->gProfileDsp)
    {
        result = gGlobal->gProfile->unRegisterModule(gGlobal->gProfileDsp);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = gGlobal->gProfileDsp->release();
        gGlobal->gProfileDsp = 0;
    }

    return result;
}


/*===================================================================*/


ProfileDsp::ProfileDsp()
{
    mNodeStack       = NULL;
    mMaxStackNodes   = 32;

    mDataPacket      = NULL;
    mPacketHeader    = NULL;
    mPacketNodes     = NULL;
    mNumPacketNodes  = 0;
    mMaxPacketNodes  = 300;
}


FMOD_RESULT ProfileDsp::init()
{
    AutoFreeClear<FMOD::DSPI **> autoNodeStack;
    AutoFreeClear<char *> autoDataPacket;

    // Stack used for iterating through DSP nodes, stack size increases when max is reached
    mNodeStack = (FMOD::DSPI **)FMOD_Memory_Alloc(sizeof(FMOD::DSPI*) * mMaxStackNodes);
    autoNodeStack = &mNodeStack;
    CHECK_RESULT(mNodeStack == NULL ? FMOD_ERR_MEMORY : FMOD_OK);

    // Packet to be sent over the network
    mDataPacket = (char *)FMOD_Memory_Calloc(sizeof(ProfilePacketDspNetwork) + (mMaxPacketNodes * sizeof(ProfileDspRawNode)));
    autoDataPacket = &mDataPacket;
    CHECK_RESULT(mDataPacket == NULL ? FMOD_ERR_MEMORY : FMOD_OK);

    mPacketHeader = (ProfilePacketDspNetwork *)mDataPacket;
    mPacketNodes  = (ProfileDspRawNode *)(mDataPacket + sizeof(ProfilePacketDspNetwork));

    autoNodeStack.releasePtr();
    autoDataPacket.releasePtr();

    return FMOD_OK;
}


FMOD_RESULT ProfileDsp::release()
{
    if (mNodeStack)
    {
        FMOD_Memory_Free(mNodeStack);
        mNodeStack = NULL;
    }

    if (mDataPacket)
    {
        FMOD_Memory_Free(mDataPacket);
        mDataPacket     = NULL;
        mPacketHeader   = NULL;
        mPacketNodes    = NULL;
    }

    FMOD_Memory_Free(this);
    return FMOD_OK;
}


FMOD_RESULT ProfileDsp::update(FMOD::SystemI *system, unsigned int delta)
{
    FMOD_RESULT     result          = FMOD_OK;
    unsigned int    numStackNodes   = 0;
    LocalCriticalSection criticalsection(system->mDSPConnectionCrit, true);

    mNumPacketNodes = 0;

    result = system->getDSPHead(&mNodeStack[numStackNodes++]);
    CHECK_RESULT(result);

    while (numStackNodes > 0)
	{
        FMOD::DSPI         *dspNode     = NULL;
        ProfileDspRawNode  *dspNetNode  = NULL;
        bool                isActive    = false;
        bool                isBypass    = false;

        if (mNumPacketNodes + 1 >= mMaxPacketNodes)
        {        
            result = growPacketSpace();
            CHECK_RESULT(result);
        }
        
        dspNode = mNodeStack[--numStackNodes];
        dspNetNode = &mPacketNodes[mNumPacketNodes++];
		
		FMOD_RESULT result = dspNode->getInfo(dspNetNode->name, NULL, NULL, NULL, NULL);
	    CHECK_RESULT(result);

        result = dspNode->getNumInputs(&dspNetNode->numInputs, false);
        CHECK_RESULT(result);

		result = dspNode->getActive(&isActive);
        CHECK_RESULT(result);

        result = dspNode->getBypass(&isBypass);
        CHECK_RESULT(result);

        dspNetNode->id                  = (FMOD_UINT64)dspNode;
        dspNetNode->active              = isActive ? 1 : 0;
        dspNetNode->bypass              = isBypass ? 1 : 0;
        dspNetNode->treeLevel           = dspNode->mTreeLevel;
        dspNetNode->exclusiveCPUTime    = isActive ? dspNode->mCPUUsage : 0;
#ifdef FMOD_SUPPORT_PROFILE_DSP_VOLUMELEVELS
        dspNetNode->numChannels         = isActive ? dspNode->mNumPeakVolumeChans : 0;
        FMOD_memcpy(dspNetNode->peakVolume, dspNode->mPeakVolume, sizeof(dspNode->mPeakVolume));
#endif
		
		if (!isNodeDuplicate(dspNetNode->id))
		{
			for (int i = dspNetNode->numInputs - 1; i >= 0; i--)
			{	
                if (numStackNodes >= mMaxStackNodes)
                {
                    result = growNodeStackSpace();
                    CHECK_RESULT(result);
                }
                
				result = dspNode->getInput(i, &mNodeStack[numStackNodes++], NULL, false);
                CHECK_RESULT(result);
			}
		}
	}

    result = sendPacket(system);
    CHECK_RESULT(result == FMOD_ERR_NET_WOULD_BLOCK ? FMOD_OK : result);

    return FMOD_OK;
}


FMOD_RESULT ProfileDsp::growPacketSpace()
{
    mMaxPacketNodes *= 2;

    mDataPacket = (char *)FMOD_Memory_ReAlloc(mDataPacket, sizeof(ProfilePacketDspNetwork) + (mMaxPacketNodes * sizeof(ProfileDspRawNode)));
    CHECK_RESULT(mDataPacket == NULL ? FMOD_ERR_MEMORY : FMOD_OK);

    mPacketHeader = (ProfilePacketDspNetwork *)mDataPacket;
    mPacketNodes  = (ProfileDspRawNode *)(mDataPacket + sizeof(ProfilePacketDspNetwork));           

    return FMOD_OK;
}


FMOD_RESULT ProfileDsp::growNodeStackSpace()
{
    mMaxStackNodes *= 2;
    
    mNodeStack = (FMOD::DSPI **)FMOD_Memory_ReAlloc(mNodeStack, sizeof(FMOD::DSPI *) * mMaxStackNodes);
    CHECK_RESULT(mNodeStack == NULL ? FMOD_ERR_MEMORY : FMOD_OK);
 
    return FMOD_OK;
}


bool ProfileDsp::isNodeDuplicate(FMOD_UINT64 nodeId)
{
    for (unsigned int i = 0; i < mNumPacketNodes - 1; i++)
	{
        if (mPacketNodes[i].id == nodeId)
		{
			return true;
		}
	}

    return false;
}


FMOD_RESULT ProfileDsp::sendPacket(FMOD::SystemI *system)
{
    FMOD_RESULT result              = FMOD_OK;
    float       dspUsage            = 0.0f;
    int         maxOutputChannels   = 0;
    int         maxInputChannels    = 0;

    result = system->getCPUUsage(&dspUsage, NULL, NULL, NULL, NULL);
	CHECK_RESULT(result);

    result = system->getSoftwareFormat(NULL, NULL, &maxOutputChannels, &maxInputChannels, NULL, NULL);
    CHECK_RESULT(result);
    
    mPacketHeader->hdr.size         = sizeof(ProfilePacketDspNetwork) + (sizeof(ProfileDspRawNode) * mNumPacketNodes);
    mPacketHeader->hdr.timestamp    = 0;
    mPacketHeader->hdr.type         = FMOD_PROFILE_DATATYPE_DSP;
    mPacketHeader->hdr.subtype      = FMOD_PROFILE_DATASUBTYPE_DSP_NETWORK;
    mPacketHeader->hdr.version      = FMOD_PROFILE_DSP_VERSION;
    mPacketHeader->hdr.flags        = 0;
    mPacketHeader->dspCPUUsage      = dspUsage / 100.0f;
    mPacketHeader->maxNumChannels   = (maxOutputChannels < maxInputChannels) ? maxInputChannels : maxOutputChannels;
    
#ifdef PLATFORM_ENDIAN_BIG
    FMOD_SWAPENDIAN_FLOAT(mPacketHeader->dspCPUUsage);

	for (unsigned int i = 0; i < mNumPacketNodes; i++)
	{
        mPacketNodes[i].id                = FMOD_SWAPENDIAN_QWORD(mPacketNodes[i].id);
        mPacketNodes[i].numInputs         = FMOD_SWAPENDIAN_DWORD(mPacketNodes[i].numInputs);
        mPacketNodes[i].treeLevel         = FMOD_SWAPENDIAN_WORD(mPacketNodes[i].treeLevel);
        mPacketNodes[i].exclusiveCPUTime  = FMOD_SWAPENDIAN_WORD(mPacketNodes[i].exclusiveCPUTime);
	}
#endif

    result = gGlobal->gProfile->addPacket((ProfilePacketHeader *)mPacketHeader);
    CHECK_RESULT(result);

    return FMOD_OK;
}

}       // namespace FMOD

#endif  // FMOD_SUPPORT_PROFILE && FMOD_SUPPORT_PROFILE_DSP
