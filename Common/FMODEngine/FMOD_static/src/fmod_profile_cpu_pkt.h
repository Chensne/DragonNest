#ifndef _FMOD_PROFILE_CPU_PKT_H
#define _FMOD_PROFILE_CPU_PKT_H

#ifndef _FMOD_PROFILE_PKT_H
#include "fmod_profile_pkt.h"
#endif

namespace FMOD
{
    const char FMOD_PROFILE_CPU_VERSION = 1;

    enum
    {
        FMOD_PROFILE_DATASUBTYPE_CPU_TOTALS = 0
    };

#ifdef FMOD_SUPPORT_PRAGMAPACK    
    #pragma pack(1)
#endif
    typedef struct    // FMOD_PROFILE_DATASUBTYPE_CPU_TOTALS (version 0)
    {
        ProfilePacketHeader  hdr            FMOD_PACKED_INTERNAL;
        float                dspUsage       FMOD_PACKED_INTERNAL;
        float                streamUsage    FMOD_PACKED_INTERNAL;
        float                updateUsage    FMOD_PACKED_INTERNAL;
    } FMOD_PACKED ProfilePacketCpuTotalsV0;
    
    typedef struct    // FMOD_PROFILE_DATASUBTYPE_CPU_TOTALS (version 1)
    {
        ProfilePacketHeader  hdr            FMOD_PACKED_INTERNAL;
        float                dspUsage       FMOD_PACKED_INTERNAL;
        float                streamUsage    FMOD_PACKED_INTERNAL;
        float                updateUsage    FMOD_PACKED_INTERNAL;
        float                geometryUsage  FMOD_PACKED_INTERNAL;
    } FMOD_PACKED ProfilePacketCpuTotals;
#ifdef FMOD_SUPPORT_PRAGMAPACK
    #pragma pack()
#endif
}

#endif  // _FMOD_PROFILE_CPU_PKT_H
