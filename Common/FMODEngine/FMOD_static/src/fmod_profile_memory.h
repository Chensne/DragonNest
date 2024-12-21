#ifndef _FMOD_PROFILE_MEMORY_H
#define _FMOD_PROFILE_MEMORY_H

#ifndef _FMOD_SETTINGS_H
#include "fmod_settings.h"
#endif

#if defined(FMOD_SUPPORT_PROFILE) && defined(FMOD_SUPPORT_PROFILE_MEMORY)

#ifndef _FMOD_PROFILE_H
#include "fmod_profile.h"
#endif
#ifndef _FMOD_PROFILE_MEMORY_PKT_H
#include "fmod_profile_memory_pkt.h"
#endif

namespace FMOD
{
    class ProfileMemory : public ProfileModule
    {
        private :

        char *mBuffer;


        public :

        ProfileMemory();

        FMOD_RESULT init();
        FMOD_RESULT release();
        FMOD_RESULT update(SystemI *system, unsigned int dt);
    };

    FMOD_RESULT FMOD_ProfileMemory_Create();
    FMOD_RESULT FMOD_ProfileMemory_Release();

    extern ProfileMemory *g_profile_memory;
}

#endif  // FMOD_SUPPORT_PROFILE && FMOD_SUPPORT_PROFILE_MEMORY

#endif
