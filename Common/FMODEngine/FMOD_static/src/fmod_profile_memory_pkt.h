#ifndef _FMOD_PROFILE_MEMORY_PKT_H
#define _FMOD_PROFILE_MEMORY_PKT_H

#ifndef _FMOD_PROFILE_PKT_H
#include "fmod_profile_pkt.h"
#endif

namespace FMOD
{
    const char FMOD_PROFILE_MEMORY_VERSION = 0;

    enum
    {
        FMOD_PROFILE_DATASUBTYPE_MEMORY_DUMP = 0,
        FMOD_PROFILE_DATASUBTYPE_MEMORY_ALLOC,
        FMOD_PROFILE_DATASUBTYPE_MEMORY_FREE
    };

#ifdef FMOD_SUPPORT_PRAGMAPACK    
    #pragma pack(1)
#endif
    typedef struct      // FMOD_PROFILE_DATASUBTYPE_MEMORY_DUMP
    {
        ProfilePacketHeader  hdr        FMOD_PACKED_INTERNAL;
        unsigned int         blocksize  FMOD_PACKED_INTERNAL;   // bytesize of a single block
        unsigned int         sizeblocks FMOD_PACKED_INTERNAL;   // total number of blocks in heap
        unsigned int         numallocs  FMOD_PACKED_INTERNAL;   // number of allocated chunks
    } FMOD_PACKED ProfilePacketMemoryDump;

    typedef struct
    {
        int  mSize          FMOD_PACKED_INTERNAL;
        int  mNumBlocks     FMOD_PACKED_INTERNAL;
        int  mBlockOffset   FMOD_PACKED_INTERNAL;
    } FMOD_PACKED ProfilePacketMemoryBlock;

    typedef struct      // FMOD_PROFILE_DATASUBTYPE_MEMORY_ALLOC
    {
        ProfilePacketHeader  hdr        FMOD_PACKED_INTERNAL;
        unsigned int         blockptr   FMOD_PACKED_INTERNAL;
        unsigned int         size       FMOD_PACKED_INTERNAL;
        // file
        // line
    } FMOD_PACKED ProfilePacketMemoryAlloc;

    typedef struct      // FMOD_PROFILE_DATASUBTYPE_MEMORY_FREE
    {
        ProfilePacketHeader  hdr        FMOD_PACKED_INTERNAL;
        unsigned int         blockptr   FMOD_PACKED_INTERNAL;
        // file
        // line
    } FMOD_PACKED ProfilePacketMemoryFree;
#ifdef FMOD_SUPPORT_PRAGMAPACK
    #pragma pack()
#endif
}

#endif
