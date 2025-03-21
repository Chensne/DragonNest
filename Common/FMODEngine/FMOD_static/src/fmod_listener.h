#ifndef _FMOD_LISTENER_H
#define _FMOD_LISTENER_H

#include "fmod_settings.h"

#include "fmod.h"
#include "fmod_types.h"

namespace FMOD
{
    const int LISTENER_MAX = 4;

    class Listener
    {
      public:

        FMOD_VECTOR mPosition;
        FMOD_VECTOR mLastPosition;
        FMOD_VECTOR mVelocity;
        FMOD_VECTOR mLastVelocity;
        FMOD_VECTOR mUp;
        FMOD_VECTOR mLastUp;
        FMOD_VECTOR mFront;
        FMOD_VECTOR mLastFront;
        FMOD_VECTOR mRight;
        bool        mMoved;
        bool        mRotated;

      public:

        Listener();

    };
}

#endif

