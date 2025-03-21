#ifndef _FMOD_SYNCPOINT_H
#define _FMOD_SYNCPOINT_H

#include "fmod_settings.h"

namespace FMOD
{
    class SoundI;

    #ifdef FMOD_SUPPORT_PRAGMAPACK
	    #pragma pack(1)
    #endif

    class SyncPoint : public SortedLinkedListNode
    {
      public:
        char           *mName              FMOD_PACKED_INTERNAL;
        SoundI         *mSound             FMOD_PACKED_INTERNAL; /* The subsound that owns it. */

	    unsigned int    mOffset            FMOD_PACKED_INTERNAL;
        unsigned short  mSubSoundIndex     FMOD_PACKED_INTERNAL; /* The subsound index that owns it. */
        unsigned short  mIndex             FMOD_PACKED_INTERNAL; /* The relative index in the subsound. (needs to be fixed if a syncpoint is deleted) */
        int             mStatic            FMOD_PACKED_INTERNAL; /* 1 = memory points to a preallocated block, so when releasing, dont free it! */
    } FMOD_PACKED;

    class SyncPointNamed : public SyncPoint
    {
      public:
	    char	        mNameMemory[FMOD_STRING_MAXNAMELEN];
    };

    #ifdef FMOD_SUPPORT_PRAGMAPACK
        #ifdef 	CODEWARRIOR
        #pragma pack(0)
        #else
        #pragma pack()
        #endif
    #endif

}

#endif


