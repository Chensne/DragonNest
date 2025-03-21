#ifndef _FMOD_CHANNELI_H
#define _FMOD_CHANNELI_H

#include "fmod_settings.h"

#include "fmod.hpp"
#include "fmod_linkedlist.h"
#ifndef _FMOD_MEMORYTRACKER_H
#include "fmod_memorytracker.h"
#endif

namespace FMOD
{

    const unsigned int SYSTEMID_BITS   = 4;
    const unsigned int CHANINDEX_BITS  = 12;
    const unsigned int REFCOUNT_BITS   = 16;
    const unsigned int SYSTEMID_SHIFT  = REFCOUNT_BITS + CHANINDEX_BITS;
    const unsigned int CHANINDEX_SHIFT = REFCOUNT_BITS;
    const unsigned int REFCOUNT_SHIFT  = 0;
    const unsigned int SYSTEMID_MASK   = (unsigned int)0xffffffff >> (32 - SYSTEMID_BITS);
    const unsigned int CHANINDEX_MASK  = (unsigned int)0xffffffff >> (32 - CHANINDEX_BITS);
    const unsigned int REFCOUNT_MASK   = (unsigned int)0xffffffff >> (32 - REFCOUNT_BITS);

    const unsigned int FMOD_CHANNEL_DEFAULTPRIORITY = 128;

    const int          FMOD_CHANNEL_MAXINPUTCHANNELS = 16;

    #if defined(PLATFORM_PS3) || defined(PLATFORM_XENON) || defined(PLATFORM_MAC) || defined(PLATFORM_LINUX)
    const int          FMOD_CHANNEL_MAXREALSUBCHANNELS = 1;     /* We know these 2 are definitely software only and the mixer supports 16ch. */
    #else    
    const int          FMOD_CHANNEL_MAXREALSUBCHANNELS = FMOD_CHANNEL_MAXINPUTCHANNELS;
    #endif

    typedef enum 
    {
        FMOD_CHANNEL_PANMODE_PAN,
        FMOD_CHANNEL_PANMODE_SPEAKERMIX,
        FMOD_CHANNEL_PANMODE_SPEAKERLEVELS,
    } FMOD_CHANNEL_PANMODE;

    class ChannelGroupI;
    class ChannelReal;
	class DSPI;
    class DSPConnectionI;
    class ReverbI;
    class SoundI;
    class Stream;
    class SyncPoint;
    class SystemI;
    class OcclusionProcessor;

    typedef unsigned int FMOD_CHANNEL_FLAG;
    typedef unsigned int FMOD_CHANNEL_STOPFLAG;

    #define CHANNELI_FLAG_PAUSED                0x00000001
    #define CHANNELI_FLAG_MUTED                 0x00000002
    #define CHANNELI_FLAG_JUSTWENTVIRTUAL       0x00000004
    #define CHANNELI_FLAG_REALMUTE              0x00000008
    #define CHANNELI_FLAG_MOVED                 0x00000010
    #define CHANNELI_FLAG_FORCEVIRTUAL          0x00000040
    #define CHANNELI_FLAG_USEDINPUTMIX          0x00000080
    #define CHANNELI_FLAG_PLAYINGPAUSED         0x00000100
    #define CHANNELI_FLAG_USEDPAUSEDELAY        0x00000200
    #define CHANNELI_FLAG_ENDDELAY              0x00000400
    #define CHANNELI_FLAG_MUSICOWNED            0x00000800

    #define CHANNELI_STOPFLAG_REFSTAMP          0x00000001
    #define CHANNELI_STOPFLAG_UPDATELIST        0x00000002
    #define CHANNELI_STOPFLAG_RESETCALLBACKS    0x00000004
    #define CHANNELI_STOPFLAG_CALLENDCALLBACK   0x00000008
    #define CHANNELI_STOPFLAG_RESETCHANNELGROUP 0x00000010
    #define CHANNELI_STOPFLAG_PROCESSENDDELAY   0x00000020
    #define CHANNELI_STOPFLAG_UPDATESYNCPOINTS  0x00000040
    #define CHANNELI_STOPFLAG_DONTFREELEVELS    0x00000080

    typedef struct
    {
        float                          *mLevels;
        unsigned int                    mPCM,  mLoopStart,  mLoopEnd;
        ChannelReal                    *mRealChannel;
        SoundI                         *mSound;
        DSPI                           *mDSP;
        int                             mLoopCount;
        bool                            mMute, mPaused;
        unsigned int                    mDSPClockHi, mDSPClockLo;
        FMOD_MODE                       mMode;

        #ifdef PLATFORM_WII
        int                             mLowPassCutoff;
        bool                            mBiquadActive;
        unsigned short                  mBiquadB0;
        unsigned short                  mBiquadB1;
        unsigned short                  mBiquadB2;
        unsigned short                  mBiquadA1;
        unsigned short                  mBiquadA2;
        unsigned int                    mControllerSpeaker;
        #endif
    } FMOD_CHANNEL_INFO;

    class ChannelI : public LinkedListNode      /* This linked list node entry is for System::mChannelFreeListHead / mChannelUsedListHead */
    {                                                                                                           /*  16 bytes */
        DECLARE_MEMORYTRACKER

        friend class Channel;                                                                                       
        friend class OcclusionProcessor;
                                                                                                                    
      public:                                                                                                       
                                                                                                                    
        SortedLinkedListNode    mSortedListNode;                                                                /*  12 bytes */
        SortedLinkedListNode    mSoundGroupSortedListNode;                                                      /*  12 bytes */
                                                                                                                    
        int                     mIndex;                                                                         /*   4 bytes */
        void                   *mUserData;                                                                      /*   4 bytes */
        unsigned int            mHandleOriginal;    /* Handle as it was when this channel was first played */   /*   4 bytes */
        SystemI                *mSystem;                                                                        /*   4 bytes */
        int                     mNumRealChannels;                                                               /*   4 bytes */
        ChannelReal            *mRealChannel[FMOD_CHANNEL_MAXREALSUBCHANNELS];                                  /*  64 bytes <-------- */
        unsigned int            mHandleCurrent;     /* Handle as it is now */                                   /*   4 bytes */
                                                                                                                    
        /*                                                                                                          
            Misc                                                                                                    
        */
        FMOD_CHANNEL_FLAG       mFlags;                                                                         /*   4 bytes */                                                                                                     
        FMOD_CHANNEL_PANMODE    mLastPanMode;                                                                   /*   4 bytes */
        int                     mPriority;                                                                      /*   4 bytes */
        unsigned int            mListPosition;                                                                  /*   4 bytes */
        unsigned int            mSoundGroupListPosition;                                                        /*   4 bytes */
        SyncPoint              *mSyncPointCurrent;                                                              /*   4 bytes */
        unsigned int            mSyncPointLastPos;                                                              /*   4 bytes */
        ChannelGroupI          *mChannelGroup;                                                                  /*   4 bytes */
        LinkedListNode          mChannelGroupNode;                                                              /*  16 bytes */
        float                   mFadeVolume;                                                                    /*   4 bytes */
        float                   mFadeTarget;                                                                    /*   4 bytes */
        unsigned int            mEndDelay;                                                                      /*   4 bytes */
        FMOD_UINT64P            mDSPClockDelay;                                                                 /*   8 bytes */
        FMOD_UINT64P            mDSPClockEnd;                                                                   /*   8 bytes */
        FMOD_UINT64P            mDSPClockPause;                                                                 /*   8 bytes */
        float                   mLowPassGain;                                                                   /*   4 bytes */
        DSPI                   *mAddDSPHead;                                                                    /*   4 bytes */
        FMOD_SPEAKERMODE        mSpeakerMode;                                                                   /*   4 bytes */

        /*                                                                                                          
            2D                                                                                                      
        */                                                                                                          
        float                   mVolume;                                                                        /*   4 bytes */
        float                   mFrequency;                                                                     /*   4 bytes */
        float                   mPan;                                                                           /*   4 bytes */
        float                   mSpeakerFL, mSpeakerFR, mSpeakerC, mSpeakerLFE;                                 /*  16 bytes */
        float                   mSpeakerBL, mSpeakerBR, mSpeakerSL, mSpeakerSR;                                 /*  16 bytes */
        float                   mInputMix[FMOD_CHANNEL_MAXINPUTCHANNELS];                                       /*  64 bytes */
        float                  *mLevels;                                                                        /*   4 bytes */
                                                                                                                    
        /*                                                                                                          
            3D                                                                                                      
        */                                                                                                          
        float                   mReverbDryVolume;                                                               /*   4 bytes */
        float                   mVolume3D;   /* current relative 3d volume.    */                               /*   4 bytes */
        float                   mPitch3D;    /* current relative 3d frequency. */                               /*   4 bytes */
        FMOD_VECTOR             mPosition3D;                                                                    /*  12 bytes */
        FMOD_VECTOR             mVelocity3D;                                                                    /*  12 bytes */
        float                   mDistance;                                                                      /*   4 bytes */
        float                   mMinDistance;                                                                   /*   4 bytes */
        float                   mMaxDistance;                                                                   /*   4 bytes */                                                                                                                  
        float                   mConeVolume3D;                                                                  /*   4 bytes */
        float                   mConeInsideAngle;                                                               /*   4 bytes */
        float                   mConeOutsideAngle;                                                              /*   4 bytes */
        float                   mConeOutsideVolume;                                                             /*   4 bytes */
        FMOD_VECTOR             mConeOrientation;                                                               /*  12 bytes */                                                                                                                    
		float                   mDirectOcclusion;                                                               /*   4 bytes */
		float                   mReverbOcclusion;                                                               /*   4 bytes */
		float                   mDirectOcclusionTarget;                                                         /*   4 bytes */
		float                   mReverbOcclusionTarget;                                                         /*   4 bytes */
        float                   mUserDirectOcclusion;                                                           /*   4 bytes */
        float                   mUserReverbOcclusion;                                                           /*   4 bytes */
        float                   mDirectOcclusionRateOfChange;                                                   /*   4 bytes */
        float                   mReverbOcclusionRateOfChange;                                                   /*   4 bytes */
        FMOD_VECTOR            *mRolloffPoint;                                                                  /*   4 bytes */
        int                     mNumRolloffPoints;                                                              /*   4 bytes */
        float                   mSpread;                                                                        /*   4 bytes */
        float                   m3DPanLevel;                                                                    /*   4 bytes */
        float                   m3DDopplerLevel;                                                                /*   4 bytes */
                                                                                                                    
        FMOD_CHANNEL_CALLBACK   mCallback;                                                                      /*   4 bytes */
                                                                                                                /* --------- */
        // F_API so EventSound::getEndTime can link to it
        static FMOD_RESULT F_API validate(Channel *channel, ChannelI **channeli);                                /* ??? bytes */
                                
        FMOD_RESULT             returnToFreeList();
        FMOD_RESULT             setDefaults();
        FMOD_RESULT             referenceStamp(bool newstamp = false);
		FMOD_RESULT             updatePosition();
        FMOD_RESULT             forceVirtual(bool force);
        FMOD_RESULT             getChannelInfo(FMOD_CHANNEL_INFO *info);
        FMOD_RESULT             setChannelInfo(FMOD_CHANNEL_INFO *info);
        FMOD_RESULT             getRealChannel(ChannelReal **realchan, int *subchannels);
        FMOD_RESULT             calcVolumeAndPitchFor3D(int delta);
        FMOD_RESULT             setChannelGroupInternal(ChannelGroupI *channelgroup, bool resetattributes, bool forcedspreconnect = false);
		FMOD_RESULT	            set3DOcclusionInternal(float directOcclusion, float reverbOcclusion, bool resettarget=true);
		FMOD_RESULT	            get3DOcclusionInternal(float *directOcclusion, float *reverbOcclusion);
        FMOD_RESULT             getAudibilityInternal(float *audibility, bool usefadevolume);
        // F_API so EventSound::getEndTime can link to it
        FMOD_RESULT F_API       getFinalFrequency(float *frequency);

      public:

        ChannelI();
        ChannelI(int index, SystemI *system);
        FMOD_RESULT  init();

                               
        FMOD_RESULT  getSystemObject      (System **system);

        FMOD_RESULT  play                 (SoundI *sound, bool paused, bool reset, bool startmuted);
        FMOD_RESULT  play                 (DSPI *dsp, bool paused, bool reset, bool startmuted);
        FMOD_RESULT  alloc                (SoundI *sound, bool reset);
        FMOD_RESULT  alloc                (DSPI *dsp, bool reset);
        FMOD_RESULT  start                ();
        FMOD_RESULT  updateSyncPoints     (bool seeking);
        FMOD_RESULT  update               (int delta, bool updategeometrynow = false);
        FMOD_RESULT  updateStream         ();

        FMOD_RESULT  stop                 ();
        FMOD_RESULT  stopEx               (FMOD_CHANNEL_STOPFLAG stopflag);
        FMOD_RESULT  setPaused            (bool paused);
        FMOD_RESULT  getPaused            (bool *paused);
        FMOD_RESULT  setVolume            (float volume, bool forceupdatepos = false);
        FMOD_RESULT  getVolume            (float *volume);
        FMOD_RESULT  setFrequency         (float frequency);
        FMOD_RESULT  getFrequency         (float *frequency);
        FMOD_RESULT  setPan               (float pan, bool calldriver = true);
        FMOD_RESULT  getPan               (float *pan);
        FMOD_RESULT  setDelay             (FMOD_DELAYTYPE delaytype, unsigned int delayhi, unsigned int delaylo);
        FMOD_RESULT  getDelay             (FMOD_DELAYTYPE delaytype, unsigned int *delayhi, unsigned int *delaylo);
        FMOD_RESULT  setSpeakerMix        (float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright, bool calldriver = true);
        FMOD_RESULT  getSpeakerMix        (float *frontleft, float *frontright, float *center, float *lfe, float *backleft, float *backright, float *sideleft, float *sideright);
        FMOD_RESULT  setSpeakerLevels     (FMOD_SPEAKER speaker, float *levels, int numlevels, bool calldriver = true);
        FMOD_RESULT  getSpeakerLevels     (FMOD_SPEAKER speaker, float *levels, int numlevels);
        FMOD_RESULT  setInputChannelMix   (float *levels, int numlevels);
        FMOD_RESULT  getInputChannelMix   (float *levels, int numlevels);
        FMOD_RESULT  setMute              (bool mute);
        FMOD_RESULT  getMute              (bool *mute);
        FMOD_RESULT  setPriority          (int priority);
        FMOD_RESULT  getPriority          (int *priority);
        FMOD_RESULT  setPosition          (unsigned int position, FMOD_TIMEUNIT postype);
        FMOD_RESULT  getPosition          (unsigned int *position, FMOD_TIMEUNIT postype);
        FMOD_RESULT  setReverbProperties  (const FMOD_REVERB_CHANNELPROPERTIES *prop);
        FMOD_RESULT  getReverbProperties  (FMOD_REVERB_CHANNELPROPERTIES *prop);
        FMOD_RESULT  setChannelGroup      (ChannelGroupI *channelgroup);
        FMOD_RESULT  getChannelGroup      (ChannelGroupI **channelgroup);
        FMOD_RESULT  setCallback          (FMOD_CHANNEL_CALLBACK callback);
        FMOD_RESULT  calculate3DReverbGain(ReverbI *reverb, FMOD_VECTOR *channelpos, float *gain);
        FMOD_RESULT  setLowPassGain       (float gain);
        FMOD_RESULT  getLowPassGain       (float *gain);

        // 3D functionality.
        FMOD_RESULT  set3DAttributes      (const FMOD_VECTOR *pos, const FMOD_VECTOR *vel);
        FMOD_RESULT  get3DAttributes      (FMOD_VECTOR *pos, FMOD_VECTOR *vel);
        FMOD_RESULT  set3DMinMaxDistance  (float mindistance, float maxdistance);
        FMOD_RESULT  get3DMinMaxDistance  (float *mindistance, float *maxdistance);
        FMOD_RESULT  set3DConeSettings    (float insideconeangle, float outsideconeangle, float outsidevolume);
        FMOD_RESULT  get3DConeSettings    (float *insideconeangle, float *outsideconeangle, float *outsidevolume);
        FMOD_RESULT  set3DConeOrientation (FMOD_VECTOR *orientation);
        FMOD_RESULT  get3DConeOrientation (FMOD_VECTOR *orientation);
        FMOD_RESULT  set3DCustomRolloff   (FMOD_VECTOR *points, int numpoints);
        FMOD_RESULT  get3DCustomRolloff   (FMOD_VECTOR **points, int *numpoints);
		FMOD_RESULT	 set3DOcclusion       (float directOcclusion, float reverbOcclusion);	
		FMOD_RESULT  get3DOcclusion       (float *directOcclusion, float *reverbOcclusion);
        FMOD_RESULT  set3DSpread          (float angle);
        FMOD_RESULT  get3DSpread          (float *angle);
        FMOD_RESULT  set3DPanLevel        (float level);
        FMOD_RESULT  get3DPanLevel        (float *level);
        FMOD_RESULT  set3DDopplerLevel    (float level);
        FMOD_RESULT  get3DDopplerLevel    (float *level);

        // DSP functionality only for channels playing sounds created with FMOD_SOFTWARE.
        FMOD_RESULT  getDSPHead           (DSPI **dsp);
        FMOD_RESULT  addDSP               (DSPI *dsp, DSPConnectionI **connection);

        // Information only functions.
        FMOD_RESULT  isPlaying            (bool *isplaying);
        FMOD_RESULT  isVirtual            (bool *isvirtual);
        FMOD_RESULT  getAudibility        (float *audibility);
        FMOD_RESULT  getCurrentSound      (SoundI **sound);
        FMOD_RESULT  getCurrentDSP        (DSPI  **dsp);
        FMOD_RESULT  getSpectrum          (float *spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype);
        FMOD_RESULT  getWaveData          (float *wavearray, int numvalues, int channeloffset);
        FMOD_RESULT  getIndex             (int *index);

        // Functions also found in Sound class but here they can be set per channel.
        FMOD_RESULT  setMode              (FMOD_MODE mode);
        FMOD_RESULT  getMode              (FMOD_MODE *mode);
        FMOD_RESULT  setLoopCount         (int loopcount);
        FMOD_RESULT  getLoopCount         (int *loopcount);
        FMOD_RESULT  setLoopPoints        (unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype);
        FMOD_RESULT  getLoopPoints        (unsigned int *loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int *loopend, FMOD_TIMEUNIT loopendtype);

        // Userdata set/get.                                                
        FMOD_RESULT  setUserData          (void *userdata);
        FMOD_RESULT  getUserData          (void **userdata);

        FMOD_RESULT  getMemoryInfo        (unsigned int memorybits, unsigned int event_memorybits, unsigned int *memoryused, FMOD_MEMORY_USAGE_DETAILS *memoryused_details);

#ifdef PLATFORM_WII
        // Lowpass set/get, this is for GameCube and Wii only
        FMOD_RESULT  setLowPassFilter     (int cutoff);
        FMOD_RESULT  getLowPassFilter     (int *cutoff);

        // Biquad set/get, this is for Wii only
        FMOD_RESULT  setBiquadFilter      (bool active, unsigned short b0, unsigned short b1, unsigned short b2, unsigned short a1, unsigned short a2);
        FMOD_RESULT  getBiquadFilter      (bool *active, unsigned short *b0, unsigned short *b1, unsigned short *b2, unsigned short *a1, unsigned short *a2);

        // Controller speaker set/get, this is for Wii only
        FMOD_RESULT  setControllerSpeaker (unsigned int controller, int subchannel = -1);
        FMOD_RESULT  getControllerSpeaker (unsigned int *controller);
#endif
    };
}

#endif


