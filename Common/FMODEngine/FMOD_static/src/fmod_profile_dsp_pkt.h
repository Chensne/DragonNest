#ifndef _FMOD_PROFILE_DSP_PKT_H
#define _FMOD_PROFILE_DSP_PKT_H

#ifndef _FMOD_PROFILE_PKT_H
#include "fmod_profile_pkt.h"
#endif

namespace FMOD
{
    const char FMOD_PROFILE_DSP_VERSION = 2;

    enum
    {
        FMOD_PROFILE_DATASUBTYPE_DSP_NETWORK = 0
    };

#ifdef FMOD_SUPPORT_PRAGMAPACK    
    #pragma pack(1)
#endif
    typedef struct      // FMOD_PROFILE_DATASUBTYPE_DSP_NETWORK header (version 0)
    {
        ProfilePacketHeader hdr FMOD_PACKED_INTERNAL;
    } FMOD_PACKED ProfilePacketDspNetworkV0;

    typedef struct      // FMOD_PROFILE_DATASUBTYPE_DSP_NETWORK header (version 1)
    {
        ProfilePacketHeader  hdr            FMOD_PACKED_INTERNAL;
        float                dspCPUUsage    FMOD_PACKED_INTERNAL;
    } FMOD_PACKED ProfilePacketDspNetworkV1;

    typedef struct      // FMOD_PROFILE_DATASUBTYPE_DSP_NETWORK header (version 2)
    {
        ProfilePacketHeader  hdr            FMOD_PACKED_INTERNAL;
        float                dspCPUUsage    FMOD_PACKED_INTERNAL;
        unsigned char        maxNumChannels FMOD_PACKED_INTERNAL;
    } FMOD_PACKED ProfilePacketDspNetwork;

    typedef struct      // FMOD_PROFILE_DATASUBTYPE_DSP_NETWORK bulk nodes (version 0)
    {
	    int             id          FMOD_PACKED_INTERNAL;
        char            name[32]    FMOD_PACKED_INTERNAL;
	    int             numInputs   FMOD_PACKED_INTERNAL;
	    char            active      FMOD_PACKED_INTERNAL;
        int             treeLevel   FMOD_PACKED_INTERNAL;
    } FMOD_PACKED ProfileDspRawNodeV0;

    typedef struct      // FMOD_PROFILE_DATASUBTYPE_DSP_NETWORK bulk nodes (version 1)
    {
	    int             id                  FMOD_PACKED_INTERNAL;
        char            name[32]            FMOD_PACKED_INTERNAL;
	    int             numInputs           FMOD_PACKED_INTERNAL;
	    char            active              FMOD_PACKED_INTERNAL;
        int             treeLevel           FMOD_PACKED_INTERNAL;
		int             exclusiveCPUTime    FMOD_PACKED_INTERNAL;
    } FMOD_PACKED ProfileDspRawNodeV1;

    typedef struct      // FMOD_PROFILE_DATASUBTYPE_DSP_NETWORK bulk nodes (version 2)
    {
        FMOD_UINT64     id                  FMOD_PACKED_INTERNAL;
        char            name[32]            FMOD_PACKED_INTERNAL;
	    int             numInputs           FMOD_PACKED_INTERNAL;
	    char            active              FMOD_PACKED_INTERNAL;
        char            bypass              FMOD_PACKED_INTERNAL;
        unsigned short  treeLevel           FMOD_PACKED_INTERNAL;
		unsigned short  exclusiveCPUTime    FMOD_PACKED_INTERNAL;
        unsigned char   numChannels         FMOD_PACKED_INTERNAL;
        unsigned char   peakVolume[10]      FMOD_PACKED_INTERNAL;
    } FMOD_PACKED ProfileDspRawNode;

#ifdef FMOD_SUPPORT_PRAGMAPACK
    #pragma pack()
#endif
}

#endif  // _FMOD_PROFILE_DSP_PKT_H
