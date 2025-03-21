#include "fmod_settings.h"

#if defined(FMOD_SUPPORT_PROFILE) && defined(FMOD_SUPPORT_PROFILE_CPU)

#include "fmod_profile_cpu.h"
#include "fmod_systemi.h"

namespace FMOD
{

FMOD_RESULT FMOD_ProfileCpu_Create()
{
    FMOD_RESULT result = FMOD_OK;

    if (gGlobal->gProfileCpu)
    {
        return FMOD_OK;
    }

    gGlobal->gProfileCpu = FMOD_Object_Alloc(ProfileCpu);
    if (!gGlobal->gProfileCpu)
    {
        return FMOD_ERR_MEMORY;
    }

    result = gGlobal->gProfileCpu->init();
    if (result != FMOD_OK)
    {
        return result;
    }

    return gGlobal->gProfile->registerModule(gGlobal->gProfileCpu);
}


FMOD_RESULT FMOD_ProfileCpu_Release()
{
    FMOD_RESULT result = FMOD_OK;

    if (gGlobal->gProfileCpu)
    {
        result = gGlobal->gProfile->unRegisterModule(gGlobal->gProfileCpu);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = gGlobal->gProfileCpu->release();
		gGlobal->gProfileCpu = 0;
    }

    return result;
}


/*===================================================================*/


ProfileCpu::ProfileCpu()
{

}


FMOD_RESULT ProfileCpu::init()
{
    return FMOD_OK;
}


FMOD_RESULT ProfileCpu::release()
{
    FMOD_Memory_Free(this);

    return FMOD_OK;
}


FMOD_RESULT ProfileCpu::update(FMOD::SystemI *system, unsigned int delta)
{
    FMOD_RESULT             result        = FMOD_OK;
    float                   dspUsage      = 0.0f;
    float                   streamUsage   = 0.0f;
    float                   updateUsage   = 0.0f;
    float                   geometryUsage = 0.0f;
    ProfilePacketCpuTotals  packet;

    result = system->getCPUUsage(&dspUsage, &streamUsage, &geometryUsage, &updateUsage, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    packet.hdr.size      = sizeof(ProfilePacketCpuTotals);
    packet.hdr.type      = FMOD_PROFILE_DATATYPE_CPU;
    packet.hdr.subtype   = FMOD_PROFILE_DATASUBTYPE_CPU_TOTALS;
    packet.hdr.version   = FMOD_PROFILE_CPU_VERSION;
    
    packet.dspUsage      = dspUsage;
    packet.streamUsage   = streamUsage;
    packet.geometryUsage = geometryUsage;
    packet.updateUsage   = updateUsage;

#ifdef PLATFORM_ENDIAN_BIG
	// Endian swap usage data
    FMOD_SWAPENDIAN_FLOAT(packet.dspUsage);
    FMOD_SWAPENDIAN_FLOAT(packet.streamUsage);
    FMOD_SWAPENDIAN_FLOAT(packet.updateUsage);
#endif

    result = gGlobal->gProfile->addPacket((ProfilePacketHeader *)&packet);
    if (result != FMOD_OK)
    {
        return result;
    }

    return FMOD_OK;    
}

}  // namespace FMOD

#endif  // FMOD_SUPPORT_PROFILE && FMOD_SUPPORT_PROFILE_CPU
