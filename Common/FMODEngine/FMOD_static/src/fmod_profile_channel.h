#ifndef _FMOD_PROFILE_CHANNEL_H
#define _FMOD_PROFILE_CHANNEL_H

#ifndef _FMOD_SETTINGS_H
#include "fmod_settings.h"
#endif

#if defined(FMOD_SUPPORT_PROFILE) && defined(FMOD_SUPPORT_PROFILE_CHANNEL)

#ifndef _FMOD_PROFILE_H
#include "fmod_profile.h"
#endif
#ifndef _FMOD_PROFILE_CHANNEL_PKT_H
#include "fmod_profile_channel_pkt.h"
#endif

namespace FMOD
{
    class ProfileChannel : public ProfileModule
    {
      public :

        ProfileChannel();

        FMOD_RESULT init();
        FMOD_RESULT release();
        FMOD_RESULT update(SystemI *system, unsigned int delta);
    };

    FMOD_RESULT FMOD_ProfileChannel_Create();
    FMOD_RESULT FMOD_ProfileChannel_Release();

    extern ProfileChannel *g_profile_channel;
}

#endif  // FMOD_SUPPORT_PROFILE && FMOD_SUPPORT_PROFILE_CHANNEL

#endif  // _FMOD_PROFILE_CHANNEL_H
