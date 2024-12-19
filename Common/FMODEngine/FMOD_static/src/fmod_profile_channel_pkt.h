#ifndef _FMOD_PROFILE_CHANNEL_PKT_H
#define _FMOD_PROFILE_CHANNEL_PKT_H

#ifndef _FMOD_PROFILE_PKT_H
#include "fmod_profile_pkt.h"
#endif

namespace FMOD
{
    const char FMOD_PROFILE_CHANNEL_VERSION = 0;

    enum
    {
        FMOD_PROFILE_DATASUBTYPE_CHANNEL_TOTALS = 0
    };

#ifdef FMOD_SUPPORT_PRAGMAPACK    
    #pragma pack(1)
#endif
    typedef struct    // FMOD_PROFILE_DATASUBTYPE_CHANNEL_TOTALS
    {
        ProfilePacketHeader  hdr            FMOD_PACKED_INTERNAL;
        int                  hardware       FMOD_PACKED_INTERNAL;
        int                  hardwareTotal  FMOD_PACKED_INTERNAL;
        int                  software       FMOD_PACKED_INTERNAL;
        int                  softwareTotal  FMOD_PACKED_INTERNAL;
        int                  emulated       FMOD_PACKED_INTERNAL;
        int                  maximum        FMOD_PACKED_INTERNAL;
    } FMOD_PACKED ProfilePacketChannelTotals;
#ifdef FMOD_SUPPORT_PRAGMAPACK
    #pragma pack()
#endif
}

#endif  // _FMOD_PROFILE_CHANNEL_PKT_H
