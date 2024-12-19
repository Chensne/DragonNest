#ifndef _FMOD_CODEC_SF2_H
#define _FMOD_CODEC_SF2_H

#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_SF2

#include "fmod_codeci.h"

namespace FMOD
{
    class CodecSF2 : public Codec
    {
      private:

        unsigned int                  *mDataOffset;        /* array of offsets to raw sample data */
        int                            mCurrentIndex;      /* current SF2 index */

        FMOD_RESULT openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO *userexinfo);
        FMOD_RESULT closeInternal();
        FMOD_RESULT readInternal(void *buffer, unsigned int size, unsigned int *read);
        FMOD_RESULT setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype);
        FMOD_RESULT soundcreateInternal(int subsound, FMOD_SOUND *sound);

      public:

        static FMOD_RESULT F_CALLBACK openCallback(FMOD_CODEC_STATE *codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO *userexinfo);
        static FMOD_RESULT F_CALLBACK closeCallback(FMOD_CODEC_STATE *codec);
        static FMOD_RESULT F_CALLBACK readCallback(FMOD_CODEC_STATE *codec, void *buffer, unsigned int sizebytes, unsigned int *bytesread);
        static FMOD_RESULT F_CALLBACK setPositionCallback(FMOD_CODEC_STATE *codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype);
        static FMOD_RESULT F_CALLBACK soundcreateCallback(FMOD_CODEC_STATE *codec, int subsound, FMOD_SOUND *sound);

        static FMOD_CODEC_DESCRIPTION_EX *getDescriptionEx();        
    };
}

#endif  /* FMOD_SUPPORT_SF2 */

#endif

