#include "fmod_settings.h"

#if defined(FMOD_SUPPORT_PROFILE) && defined(FMOD_SUPPORT_PROFILE_CODEC)

#include "fmod_profile_codec.h"
#include "fmod_systemi.h"
#include "fmod_dsp_codec.h"

namespace FMOD
{

FMOD_RESULT FMOD_ProfileCodec_Create()
{
    FMOD_RESULT result = FMOD_OK;

    if (gGlobal->gProfileCodec)
    {
        return FMOD_OK;
    }

    gGlobal->gProfileCodec = FMOD_Object_Alloc(ProfileCodec);
    if (!gGlobal->gProfileCodec)
    {
        return FMOD_ERR_MEMORY;
    }

    result = gGlobal->gProfileCodec->init();
    if (result != FMOD_OK)
    {
        return result;
    }

    return gGlobal->gProfile->registerModule(gGlobal->gProfileCodec);
}


FMOD_RESULT FMOD_ProfileCodec_Release()
{
    FMOD_RESULT result = FMOD_OK;

    if (gGlobal->gProfileCodec)
    {
        result = gGlobal->gProfile->unRegisterModule(gGlobal->gProfileCodec);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = gGlobal->gProfileCodec->release();
		gGlobal->gProfileCodec = 0;
    }

    return result;
}


/*===================================================================*/


ProfileCodec::ProfileCodec()
{

}


FMOD_RESULT ProfileCodec::init()
{
    return FMOD_OK;
}


FMOD_RESULT ProfileCodec::release()
{
    FMOD_Memory_Free(this);

    return FMOD_OK;
}


FMOD_RESULT ProfileCodec::update(FMOD::SystemI *system, unsigned int delta)
{
    FMOD_RESULT                 result            = FMOD_OK;
    int                         mpegCodecsUsed    = 0;
    int                         mpegCodecsTotal   = 0;
    int                         adpcmCodecsUsed   = 0;
    int                         adpcmCodecsTotal  = 0;
    int                         xmaCodecsUsed     = 0;
    int                         xmaCodecsTotal    = 0;
    int                         rawCodecsUsed     = 0;
    int                         rawCodecsTotal    = 0;
    ProfilePacketCodecTotals    packet;

#ifdef FMOD_SUPPORT_DSPCODEC
    #ifdef FMOD_SUPPORT_MPEG
    mpegCodecsUsed      = getNumFreeCodecs(system->mDSPCodecPool_MPEG);
    mpegCodecsTotal     = system->mDSPCodecPool_MPEG.mNumDSPCodecs;
    #endif
    #ifdef FMOD_SUPPORT_IMAADPCM
    adpcmCodecsUsed     = getNumFreeCodecs(system->mDSPCodecPool_ADPCM);
    adpcmCodecsTotal    = system->mDSPCodecPool_ADPCM.mNumDSPCodecs;
    #endif
    #ifdef FMOD_SUPPORT_XMA
    xmaCodecsUsed       = getNumFreeCodecs(system->mDSPCodecPool_XMA);
    xmaCodecsTotal      = system->mDSPCodecPool_XMA.mNumDSPCodecs;
    #endif
    #ifdef FMOD_SUPPORT_RAWCODEC
    rawCodecsUsed       = getNumFreeCodecs(system->mDSPCodecPool_RAW);
    rawCodecsTotal      = system->mDSPCodecPool_RAW.mNumDSPCodecs;
    #endif
#endif
 
    /*
        Send the data packet
    */
    packet.hdr.size     = sizeof(ProfilePacketCodecTotals);
    packet.hdr.type     = FMOD_PROFILE_DATATYPE_CODEC;
    packet.hdr.subtype  = FMOD_PROFILE_DATASUBTYPE_CODEC_TOTALS;
    packet.hdr.version  = FMOD_PROFILE_CODEC_VERSION;
    
    packet.mpeg         = mpegCodecsUsed;
    packet.mpegTotal    = mpegCodecsTotal;
    packet.adpcm        = adpcmCodecsUsed;
    packet.adpcmTotal   = adpcmCodecsTotal;
    packet.xma          = xmaCodecsUsed;
    packet.xmaTotal     = xmaCodecsTotal;
    packet.raw          = rawCodecsUsed;
    packet.rawTotal     = rawCodecsTotal;

#ifdef PLATFORM_ENDIAN_BIG
	// Endian swap data
    packet.mpeg         = FMOD_SWAPENDIAN_DWORD(packet.mpeg);
    packet.mpegTotal    = FMOD_SWAPENDIAN_DWORD(packet.mpegTotal);
    packet.adpcm        = FMOD_SWAPENDIAN_DWORD(packet.adpcm);
    packet.adpcmTotal   = FMOD_SWAPENDIAN_DWORD(packet.adpcmTotal);
    packet.xma          = FMOD_SWAPENDIAN_DWORD(packet.xma);
    packet.xmaTotal     = FMOD_SWAPENDIAN_DWORD(packet.xmaTotal);
    packet.raw          = FMOD_SWAPENDIAN_DWORD(packet.raw);
    packet.rawTotal     = FMOD_SWAPENDIAN_DWORD(packet.rawTotal);
#endif

    result = gGlobal->gProfile->addPacket((ProfilePacketHeader *)&packet);
    if (result != FMOD_OK)
    {
        return result;
    }

    return FMOD_OK;    
}

#ifdef FMOD_SUPPORT_SOFTWARE
int ProfileCodec::getNumFreeCodecs(const DSPCodecPool &codecPool) const
{
    int usedCodecCount = 0;

    for (int i = 0; i < codecPool.mNumDSPCodecs; i++)
    {
        bool finished;
        
        codecPool.mPool[i]->getFinished(&finished);

        if (!codecPool.mAllocated[i] && finished)
        {
            usedCodecCount++;
        }
    }

    return (codecPool.mNumDSPCodecs - usedCodecCount);
}
#endif

}  // namespace FMOD

#endif  // FMOD_SUPPORT_PROFILE && FMOD_SUPPORT_PROFILE_CODEC
