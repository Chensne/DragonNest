#ifndef _FMOD_PROFILE_CPU_H
#define _FMOD_PROFILE_CPU_H

#ifndef _FMOD_SETTINGS_H
#include "fmod_settings.h"
#endif

#if defined(FMOD_SUPPORT_PROFILE) && defined(FMOD_SUPPORT_PROFILE_CPU)

#ifndef _FMOD_PROFILE_H
#include "fmod_profile.h"
#endif
#ifndef _FMOD_PROFILE_CPU_PKT_H
#include "fmod_profile_cpu_pkt.h"
#endif

namespace FMOD
{
    class ProfileCpu : public ProfileModule
    {
      public :

        ProfileCpu();

        FMOD_RESULT init();
        FMOD_RESULT release();
        FMOD_RESULT update(SystemI *system, unsigned int delta);
    };

    FMOD_RESULT FMOD_ProfileCpu_Create();
    FMOD_RESULT FMOD_ProfileCpu_Release();

    extern ProfileCpu *g_profile_cpu;
}

#endif  // FMOD_SUPPORT_PROFILE && FMOD_SUPPORT_PROFILE_CPU

#endif  // _FMOD_PROFILE_CPU_H
