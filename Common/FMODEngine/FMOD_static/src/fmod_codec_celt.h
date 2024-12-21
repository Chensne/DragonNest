#ifndef _FMOD_CODEC_CELT_H
#define _FMOD_CODEC_CELT_H

/*
    Changes made to CELT code:

    - inline changed to FMOD_INLINE

    - add fmod_celt_decoder_create_only() function
    - add fmod_celt_decoder_setmode()     function

*/


#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_CELT

#include "fmod_codeci.h"
#include "../lib/libcelt/celt.h"
#include "../lib/libcelt/modes.h"
#include "../lib/libcelt/rate.h"        /* For MAX_PULSES    */
#include "../lib/libcelt/mdct.h"        /* For mdct_lookup   */
#include "../lib/libcelt/kiss_fftr.h"   /* For kiss_fftr_cfg */

#define FMOD_CELT_MAGIC             0xf30dc317
#define FMOD_CELT_FRAMESIZESAMPLES  512
#define FMOD_CELT_MAXFRAMESIZEBYTES 1024
#define FMOD_CELT_RATE              44100

/*
    Make sure to double check these values on each CELT update
*/
#define FMOD_CELT_MAX_EBANDS        24
#define FMOD_CELT_MAX_PBANDS        8
#define FMOD_CELT_MAX_ALLOCVECTORS  12
#define FMOD_CELT_MAX_OVERLAP       128

/*
    CELTDecoder memory usage:

    FMOD: MemPool::alloc                 :     76 bytes (01A7FD40) (alloc 387) - 32 bit
        or
    FMOD: MemPool::alloc                 :    136 bytes (01A7FD40) (alloc 387) - 64 bit

    FMOD: MemPool::alloc                 :   5120 bytes (0214FA38) (alloc 388)
    FMOD: MemPool::alloc                 :    192 bytes (02150E88) (alloc 389)
    FMOD: MemPool::alloc                 :      8 bytes (0121FFB8) (alloc 386)

    =                                        5456 bytes, round up to 5520 bytes
*/
#define FMOD_CELT_DECODERBUFFERSIZE 5520

namespace FMOD
{
    typedef struct
    {
        unsigned int magic;
        unsigned int framesize;

    } FMOD_CELT_FRAMEHEADER;


    class CodecCELT : public Codec
    {
        friend class CodecFSB;
        friend class ChannelSoftware;

        DECLARE_MEMORYTRACKER_NONVIRTUAL

      private:


        //char                mCELTDecoderBuffer[5408];   /* For multichannel, will need multiples of this */

        char               *mCELTDecoderBuffer;
        CELTDecoder        *mCELTDecoder[8]; /* Need on for each stereo stream in a multichannel sound */

        static CELTMode    *mCELTModeMono;
        static int          mCELTModeMonoRef;
        static CELTMode    *mCELTModeStereo;
        static int          mCELTModeStereoRef;

        FMOD_CODEC_WAVEFORMAT mWaveFormat;

        FMOD_RESULT openInternal        (FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO *userexinfo);
        FMOD_RESULT closeInternal       ();
        FMOD_RESULT readInternal        (void *buffer, unsigned int size, unsigned int *read);
        FMOD_RESULT setPositionInternal (int subsound, unsigned int position, FMOD_TIMEUNIT postype);

        FMOD_RESULT decodeHeader        (void *buffer, unsigned int *framesize);

      public:

        unsigned int        mPCMFrameLengthBytes;

        FMOD_RESULT CELTinit            (int numstreams);

        static FMOD_RESULT createCELTModeMono();
        static FMOD_RESULT createCELTModeStereo();
        static int         getBitstreamVersion();

        static FMOD_RESULT F_CALLBACK openCallback(FMOD_CODEC_STATE *codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO *userexinfo);
        static FMOD_RESULT F_CALLBACK closeCallback(FMOD_CODEC_STATE *codec);
        static FMOD_RESULT F_CALLBACK readCallback(FMOD_CODEC_STATE *codec, void *buffer, unsigned int size, unsigned int *read);
        static FMOD_RESULT F_CALLBACK setPositionCallback(FMOD_CODEC_STATE *codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype);

        #ifdef FMOD_SUPPORT_MEMORYTRACKER
        static FMOD_RESULT F_CALLBACK getMemoryUsedCallback(FMOD_CODEC_STATE *codec, MemoryTracker *tracker);
        #endif

        static FMOD_RESULT F_CALLBACK initCallback(FMOD_CODEC_STATE *codec, int numstreams);

        static FMOD_CODEC_DESCRIPTION_EX *getDescriptionEx();       
    };
}

#endif // FMOD_SUPPORT_CELT
#endif // _FMOD_CODEC_CELT_H
