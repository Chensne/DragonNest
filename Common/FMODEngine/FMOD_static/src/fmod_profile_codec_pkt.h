#ifndef _FMOD_PROFILE_CODEC_PKT_H
#define _FMOD_PROFILE_CODEC_PKT_H

#ifndef _FMOD_PROFILE_PKT_H
#include "fmod_profile_pkt.h"
#endif

namespace FMOD
{
    const char FMOD_PROFILE_CODEC_VERSION = 0;

    enum
    {
        FMOD_PROFILE_DATASUBTYPE_CODEC_TOTALS = 0
    };

#ifdef FMOD_SUPPORT_PRAGMAPACK    
    #pragma pack(1)
#endif
    typedef struct    // FMOD_PROFILE_DATASUBTYPE_CODEC_TOTALS
    {
        ProfilePacketHeader  hdr        FMOD_PACKED_INTERNAL;
        int                  mpeg       FMOD_PACKED_INTERNAL;
        int                  mpegTotal  FMOD_PACKED_INTERNAL;
        int                  adpcm      FMOD_PACKED_INTERNAL;
        int                  adpcmTotal FMOD_PACKED_INTERNAL;
        int                  xma        FMOD_PACKED_INTERNAL;
        int                  xmaTotal   FMOD_PACKED_INTERNAL;
        int                  raw        FMOD_PACKED_INTERNAL;
        int                  rawTotal   FMOD_PACKED_INTERNAL;
    } FMOD_PACKED ProfilePacketCodecTotals;
#ifdef FMOD_SUPPORT_PRAGMAPACK
    #pragma pack()
#endif
}

#endif  // _FMOD_PROFILE_CODEC_PKT_H
