#ifndef _FMOD_PROFILE_CODEC_H
#define _FMOD_PROFILE_CODEC_H

#ifndef _FMOD_SETTINGS_H
#include "fmod_settings.h"
#endif

#if defined(FMOD_SUPPORT_PROFILE) && defined(FMOD_SUPPORT_PROFILE_CODEC)

#ifndef _FMOD_PROFILE_H
#include "fmod_profile.h"
#endif
#ifndef _FMOD_PROFILE_CODEC_PKT_H
#include "fmod_profile_codec_pkt.h"
#endif
#ifndef _FMOD_DSP_CODECPOOL_H
#include "fmod_dsp_codecpool.h"
#endif

namespace FMOD
{
    class ProfileCodec : public ProfileModule
    {
      public :

        ProfileCodec();

        FMOD_RESULT init();
        FMOD_RESULT release();
        FMOD_RESULT update(SystemI *system, unsigned int delta);

      private :
      
#ifdef FMOD_SUPPORT_SOFTWARE
        int getNumFreeCodecs(const DSPCodecPool &codecPool) const;
#endif
    };

    FMOD_RESULT FMOD_ProfileCodec_Create();
    FMOD_RESULT FMOD_ProfileCodec_Release();

    extern ProfileCodec *g_profile_codec;
}

#endif  // FMOD_SUPPORT_PROFILE && FMOD_SUPPORT_PROFILE_CODEC

#endif  // _FMOD_PROFILE_CODEC_H
