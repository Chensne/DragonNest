#ifndef _FMOD_PROFILE_DSP_H
#define _FMOD_PROFILE_DSP_H

#ifndef _FMOD_SETTINGS_H
#include "fmod_settings.h"
#endif

#if defined(FMOD_SUPPORT_PROFILE) && defined(FMOD_SUPPORT_PROFILE_DSP)

#ifndef _FMOD_PROFILE_H
#include "fmod_profile.h"
#endif
#ifndef _FMOD_PROFILE_DSP_PKT_H
#include "fmod_profile_dsp_pkt.h"
#endif

namespace FMOD
{
    class DSPI;

    class ProfileDsp : public ProfileModule
    {
        friend class Profile;

      private :

        FMOD::DSPI                **mNodeStack;
        unsigned int                mMaxStackNodes;

        char                       *mDataPacket;
        ProfilePacketDspNetwork    *mPacketHeader;
        ProfileDspRawNode          *mPacketNodes;
        unsigned int                mNumPacketNodes;
        unsigned int                mMaxPacketNodes;

        FMOD_RESULT                 growPacketSpace();
        FMOD_RESULT                 growNodeStackSpace();
        bool                        isNodeDuplicate(FMOD_UINT64 nodeId);
        FMOD_RESULT                 sendPacket(FMOD::SystemI *system);

      public :

        ProfileDsp();

        FMOD_RESULT                 init();
        FMOD_RESULT                 release();
        FMOD_RESULT                 update(SystemI *system, unsigned int delta);
    };

    FMOD_RESULT FMOD_ProfileDsp_Create();
    FMOD_RESULT FMOD_ProfileDsp_Release();

    extern ProfileDsp *g_profile_dsp;
}

#endif  // FMOD_SUPPORT_PROFILE && FMOD_SUPPORT_PROFILE_DSP

#endif  // _FMOD_PROFILE_DSP_H
