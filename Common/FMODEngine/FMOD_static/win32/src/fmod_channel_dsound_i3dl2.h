#ifndef _FMOD_CHANNEL_DSOUND_I3DL2_H
#define _FMOD_CHANNEL_DSOUND_I3DL2_H

#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_I3DL2

#include "fmod_channel_dsound.h"

namespace FMOD
{
    class ChannelDSoundI3DL2 : public ChannelDSound
    {   
      public:
        
        ChannelDSoundI3DL2() {};

        FMOD_RESULT  setReverbProperties  (const FMOD_REVERB_CHANNELPROPERTIES *prop);
        FMOD_RESULT  getReverbProperties  (FMOD_REVERB_CHANNELPROPERTIES *prop);
    };
}

#endif /* FMOD_SUPPORT_I3DL2 */

#endif