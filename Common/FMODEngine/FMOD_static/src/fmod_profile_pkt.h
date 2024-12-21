#ifndef _FMOD_PROFILE_PKT_H
#define _FMOD_PROFILE_PKT_H

#ifndef _FMOD_TYPES_H
#include "fmod_types.h"
#endif

namespace FMOD
{
    const int FMOD_PROFILE_VERSION = 0;
    const int FMOD_PROFILE_PORT    = 9264;

    enum
    {
        FMOD_PROFILE_DATATYPE_CONTROL = 0,      // Client control
        FMOD_PROFILE_DATATYPE_DSP,              // DSP network
        FMOD_PROFILE_DATATYPE_MEMORY,           // Memory alloc/free
        FMOD_PROFILE_DATATYPE_CPU,              // CPU usage
        FMOD_PROFILE_DATATYPE_CHANNEL,          // Channel pool usage
        FMOD_PROFILE_DATATYPE_CODEC,            // Codec pool usage
        FMOD_PROFILE_DATATYPE_MAX,
        FMOD_PROFILE_DATATYPE_NONE = 255
    };

    enum
    {
        FMOD_PROFILE_DATASUBTYPE_CONTROL_REQUESTDATA = 0    // Client request for a particular data type
    };

#ifdef FMOD_SUPPORT_PRAGMAPACK    
    #pragma pack(1)
#endif
    typedef struct
    {
        unsigned int   size         FMOD_PACKED_INTERNAL;
        unsigned int   timestamp    FMOD_PACKED_INTERNAL;
        unsigned char  type         FMOD_PACKED_INTERNAL;
        unsigned char  subtype      FMOD_PACKED_INTERNAL;
        unsigned char  version      FMOD_PACKED_INTERNAL;
        unsigned char  flags        FMOD_PACKED_INTERNAL;
    } FMOD_PACKED ProfilePacketHeader;

    typedef struct      // FMOD_PROFILE_DATASUBTYPE_CONTROL_REQUESTDATA
    {
        ProfilePacketHeader  hdr        FMOD_PACKED_INTERNAL;
        unsigned char        type       FMOD_PACKED_INTERNAL;
        unsigned char        subtype    FMOD_PACKED_INTERNAL;
        unsigned int         updatetime FMOD_PACKED_INTERNAL;
    } FMOD_PACKED ProfilePacketControlRequestData;
#ifdef FMOD_SUPPORT_PRAGMAPACK
    #pragma pack()
#endif
}

#endif
