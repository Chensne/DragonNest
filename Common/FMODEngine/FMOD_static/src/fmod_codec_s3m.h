#ifndef _FMOD_CODEC_S3M_H
#define _FMOD_CODEC_S3M_H

#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_S3M

#include "fmod_music.h"

namespace FMOD
{
    class DSPI;
    class ChannelPool;

    enum FMUSIC_S3MCOMMANDS
    {
	    FMUSIC_S3M_SETSPEED = 1,
	    FMUSIC_S3M_PATTERNJUMP,
	    FMUSIC_S3M_PATTERNBREAK,
	    FMUSIC_S3M_VOLUMESLIDE,
	    FMUSIC_S3M_PORTADOWN,
	    FMUSIC_S3M_PORTAUP,
	    FMUSIC_S3M_PORTATO,
	    FMUSIC_S3M_VIBRATO,
	    FMUSIC_S3M_TREMOR,
	    FMUSIC_S3M_ARPEGGIO,
	    FMUSIC_S3M_VIBRATOVOLSLIDE,
	    FMUSIC_S3M_PORTATOVOLSLIDE,
	    FMUSIC_S3M_M,
	    FMUSIC_S3M_N,
	    FMUSIC_S3M_SETSAMPLEOFFSET,
	    FMUSIC_S3M_P,
	    FMUSIC_S3M_RETRIGVOLSLIDE,
	    FMUSIC_S3M_TREMOLO,
	    FMUSIC_S3M_SPECIAL,
	    FMUSIC_S3M_SETTEMPO,
	    FMUSIC_S3M_FINEVIBRATO,
	    FMUSIC_S3M_GLOBALVOLUME,
	    FMUSIC_S3M_W,
	    FMUSIC_S3M_SETPAN,
	    FMUSIC_S3M_Y,
	    FMUSIC_S3M_Z
    };

    enum FMUSIC_S3MCOMMANDSSPECIAL
    {
	    FMUSIC_S3M_SETFILTER,
	    FMUSIC_S3M_SETGLISSANDO,
	    FMUSIC_S3M_SETFINETUNE,
	    FMUSIC_S3M_SETVIBRATOWAVE,
	    FMUSIC_S3M_SETTREMOLOWAVE,
	    FMUSIC_S3M_S5,
	    FMUSIC_S3M_S6,
	    FMUSIC_S3M_S7,
	    FMUSIC_S3M_SETPANPOSITION16,
	    FMUSIC_S3M_S9,
	    FMUSIC_S3M_STEREOCONTROL,
	    FMUSIC_S3M_PATTERNLOOP,
	    FMUSIC_S3M_NOTECUT,
	    FMUSIC_S3M_NOTEDELAY,
	    FMUSIC_S3M_PATTERNDELAY,
	    FMUSIC_S3M_FUNKREPEAT
    };
   
    const int S3M_MAXROWS    = 64;
    const int S3M_MAXSAMPLES = 99;

    class MusicChannelS3M : public MusicChannel
    {
      public:
        FMOD_RESULT volumeSlide();
        FMOD_RESULT portamento();        
        FMOD_RESULT vibrato();
        FMOD_RESULT tremolo();
        FMOD_RESULT fineVibrato();
    };

    class CodecS3M : public MusicSong
    {
      private:
        
        MusicSample      mSample[S3M_MAXSAMPLES];

        FMOD_RESULT      calculateLength();

        FMOD_RESULT      updateNote(bool audible);                        
        FMOD_RESULT      updateEffects();
        FMOD_RESULT      update(bool audible);

        FMOD_RESULT      openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO *userexinfo);
        FMOD_RESULT      closeInternal();
        FMOD_RESULT      readInternal(void *buffer, unsigned int size, unsigned int *read);
        FMOD_RESULT      setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype);

      public:

        static FMOD_RESULT F_CALLBACK openCallback(FMOD_CODEC_STATE *codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO *userexinfo);
        static FMOD_RESULT F_CALLBACK closeCallback(FMOD_CODEC_STATE *codec);
        static FMOD_RESULT F_CALLBACK readCallback(FMOD_CODEC_STATE *codec, void *buffer, unsigned int sizebytes, unsigned int *bytesread);
        static FMOD_RESULT F_CALLBACK setPositionCallback(FMOD_CODEC_STATE *codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype);

        static FMOD_CODEC_DESCRIPTION_EX *getDescriptionEx();  
        
    };
}

#endif  /* FMOD_SUPPORT_S3M */

#endif


