#ifndef _FMOD_CHANNEL_DSOUND_EAX3_H
#define _FMOD_CHANNEL_DSOUND_EAX3_H

#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_EAX

#include "fmod_channel_dsound.h"

namespace FMOD
{
    class ChannelDSoundEAX3 : public ChannelDSound
    {
      public:
        
        ChannelDSoundEAX3() {}

        FMOD_RESULT  setReverbProperties  (const FMOD_REVERB_CHANNELPROPERTIES *prop);
        FMOD_RESULT  getReverbProperties  (FMOD_REVERB_CHANNELPROPERTIES *prop);
    };
}

#endif /* FMOD_SUPPORT_EAX */

#endif
