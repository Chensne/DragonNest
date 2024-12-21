#ifndef _FMOD_GLOBALS_H
#define _FMOD_GLOBALS_H

#include "fmod_settings.h"

#include "fmod_debug.h"
#include "fmod_types.h"
#include "fmod.h"
#include "fmod_linkedlist.h"
#include "fmod_os_misc.h"

#ifdef FMOD_SUPPORT_DLMALLOC
    #include "../lib/dlmalloc/dlmalloc.h"
#endif

namespace FMOD
{
    class SystemI;
	class MemPool;
#ifdef FMOD_SUPPORT_PROFILE
    class Profile;
    class ProfileCodec;
    class ProfileChannel;
    class ProfileCpu;
    class ProfileDsp;
#endif

    class Global
    {
        public :

        void init();
        Global() { init(); }

#ifndef PLATFORM_PS3_SPU
#ifndef PLATFORM_PS2_IOP
        SystemI                 *gSystemHead;
#endif                          
        MemPool                 *gSystemPool;
#endif                          
        int                      gSystemInitCount;
                                
#ifdef FMOD_DEBUG               
        FMOD_DEBUGLEVEL          gDebugLevel;
        FMOD_DEBUGMODE           gDebugMode;
        char                     gDebugFilename[256];
#endif                          

#if defined(PLATFORM_MAC) || defined(PLATFORM_LINUX) || defined(PLATFORM_SOLARIS) || defined(PLATFORM_IPHONE)
        unsigned int             gStartTimeSeconds;         // Common time offset that can be used by event system and low level os time functions 
#endif

        FMOD_UINT64P             gDSPClock;                 // The number of times Output::mix() has been called
        unsigned int             gDSPClockTimeStamp;        // System time in ms of when the last call to Output::mix() completed
        int                      gFileBusy;
        FMOD_SYSTEM_CALLBACK     gSystemCallback;
        FMOD_MEMORY_TYPE         gMemoryTypeFlags;
        
        LinkedListNode           gFileThreadHead;
        FMOD_OS_CRITICALSECTION *gFileCrit;

        FMOD_OS_CRITICALSECTION *gAsyncCrit;

#ifdef FMOD_SUPPORT_DLMALLOC
        struct malloc_params     gDLMalloc_mparams;
#endif

#ifdef FMOD_SUPPORT_PROFILE
        Profile              *gProfile;
        ProfileCodec         *gProfileCodec;
        ProfileChannel       *gProfileChannel;
        ProfileCpu           *gProfileCpu;
        ProfileDsp           *gProfileDsp;
#endif
        
        int                   gRandomValue;
    };

    extern Global *gGlobal;
}

#endif
