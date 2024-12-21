#include "fmod_settings.h"

#if defined(FMOD_SUPPORT_PROFILE) && defined(FMOD_SUPPORT_PROFILE_CHANNEL)

#include "fmod_profile_channel.h"
#include "fmod_systemi.h"
#include "fmod_output_software.h"
#include "fmod_output_emulated.h"

namespace FMOD
{

FMOD_RESULT FMOD_ProfileChannel_Create()
{
    FMOD_RESULT result = FMOD_OK;

    if (gGlobal->gProfileChannel)
    {
        return FMOD_OK;
    }

    gGlobal->gProfileChannel = FMOD_Object_Alloc(ProfileChannel);
    if (!gGlobal->gProfileChannel)
    {
        return FMOD_ERR_MEMORY;
    }

    result = gGlobal->gProfileChannel->init();
    if (result != FMOD_OK)
    {
        return result;
    }

    return gGlobal->gProfile->registerModule(gGlobal->gProfileChannel);
}


FMOD_RESULT FMOD_ProfileChannel_Release()
{
    FMOD_RESULT result = FMOD_OK;

    if (gGlobal->gProfileChannel)
    {
        result = gGlobal->gProfile->unRegisterModule(gGlobal->gProfileChannel);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = gGlobal->gProfileChannel->release();
		gGlobal->gProfileChannel = 0;
    }

    return result;
}


/*===================================================================*/


ProfileChannel::ProfileChannel()
{

}


FMOD_RESULT ProfileChannel::init()
{
    return FMOD_OK;
}


FMOD_RESULT ProfileChannel::release()
{
    FMOD_Memory_Free(this);

    return FMOD_OK;
}


FMOD_RESULT ProfileChannel::update(FMOD::SystemI *system, unsigned int delta)
{
    FMOD_RESULT                 result                  = FMOD_OK;
    int                         softwareChannels        = 0; 
    int                         totalSoftwareChannels   = 0;
    int                         hardwareChannels        = 0; 
    int                         totalHardwareChannels   = 0; 
    int                         emulatedChannels        = 0;
    int                         maximumChannels         = 0;
    ProfilePacketChannelTotals  packet;

    /*
        Get number of used software channels
    */
#ifdef FMOD_SUPPORT_SOFTWARE
    if (system->mSoftware->mChannelPool)
    {
        result = system->mSoftware->mChannelPool->getChannelsUsed(&softwareChannels);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = system->mSoftware->mChannelPool->getNumChannels(&totalSoftwareChannels);
        if (result != FMOD_OK)
        {
            return result;
        }
    }
#endif

    /*
        Get number of used hardware channels
    */
    if (system->mOutput)
    {
        int totalCount  = 0;
        int usedCount   = 0;

        if (system->mOutput->mChannelPool)
        {
            result = system->mOutput->mChannelPool->getChannelsUsed(&usedCount);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = system->mOutput->mChannelPool->getNumChannels(&totalCount);
            if (result != FMOD_OK)
            {
                return result;
            }

            hardwareChannels += usedCount;
            totalHardwareChannels += totalCount;
        }

        if (system->mOutput->mChannelPool3D && (system->mOutput->mChannelPool != system->mOutput->mChannelPool3D))
        {
            result = system->mOutput->mChannelPool3D->getChannelsUsed(&usedCount);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = system->mOutput->mChannelPool3D->getNumChannels(&totalCount);
            if (result != FMOD_OK)
            {
                return result;
            }

            hardwareChannels += usedCount;
            totalHardwareChannels += totalCount;
        }
    }
    
    /*
        Get number of used emulated channels
    */
    if (system->mEmulated && system->mEmulated->mChannelPool)
    {
        result = system->mEmulated->mChannelPool->getChannelsUsed(&emulatedChannels);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    /*
        Get total number of channels allocated
    */
    maximumChannels = system->mNumChannels;

    /*
        Send the data packet
    */
    packet.hdr.size     = sizeof(ProfilePacketChannelTotals);
    packet.hdr.type     = FMOD_PROFILE_DATATYPE_CHANNEL;
    packet.hdr.subtype  = FMOD_PROFILE_DATASUBTYPE_CHANNEL_TOTALS;
    packet.hdr.version  = FMOD_PROFILE_CHANNEL_VERSION;
    
    packet.software         = softwareChannels;
    packet.softwareTotal    = totalSoftwareChannels; 
    packet.hardware         = hardwareChannels;
    packet.hardwareTotal    = totalHardwareChannels;
    packet.emulated         = emulatedChannels;
    packet.maximum          = maximumChannels;

#ifdef PLATFORM_ENDIAN_BIG
	// Endian swap data
    packet.software         = FMOD_SWAPENDIAN_DWORD(packet.software);
    packet.softwareTotal    = FMOD_SWAPENDIAN_DWORD(packet.softwareTotal);
    packet.hardware         = FMOD_SWAPENDIAN_DWORD(packet.hardware);
    packet.hardwareTotal    = FMOD_SWAPENDIAN_DWORD(packet.hardwareTotal);
    packet.emulated         = FMOD_SWAPENDIAN_DWORD(packet.emulated);
    packet.maximum          = FMOD_SWAPENDIAN_DWORD(packet.maximum);
#endif

    result = gGlobal->gProfile->addPacket((ProfilePacketHeader *)&packet);
    if (result != FMOD_OK)
    {
        return result;
    }

    return FMOD_OK;    
}

}  // namespace FMOD

#endif  // FMOD_SUPPORT_PROFILE && FMOD_SUPPORT_PROFILE_CHANNEL
