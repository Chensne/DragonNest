#ifndef _FMOD_DSPI_H
#define _FMOD_DSPI_H

#include "fmod_settings.h"

#include "fmod_plugin.h"
#include "fmod.hpp"
#include "fmod_dsp_connectioni.h"
#include "fmod_linkedlist.h"
#include "fmod_os_misc.h"

namespace FMOD
{ 
    class ChannelSoftware;
    class DSPFilter;
    class PluginFactory;
    class MemoryTracker;

    typedef enum
    {
        FMOD_DSP_CATEGORY_FILTER,           /* This is a unit type that processes incoming data. */
        FMOD_DSP_CATEGORY_DSPCODECMPEG,     /* This is a unit type that resamples data from a codec. (MPEG) */
        FMOD_DSP_CATEGORY_DSPCODECADPCM,    /* This is a unit type that resamples data from a codec. (ADPCM) */
        FMOD_DSP_CATEGORY_DSPCODECXMA,      /* This is a unit type that resamples data from a codec. (XMA) */
        FMOD_DSP_CATEGORY_DSPCODECCELT,     /* This is a unit type that resamples data from a codec. (CELT) */
        FMOD_DSP_CATEGORY_DSPCODECRAW,      /* This is a unit type that resamples data from a codec. (RAW) */
        FMOD_DSP_CATEGORY_SOUNDCARD,        /* This is a unit type that only receives data. (Not used on PS3 non-recursive SPU mixer) */
        FMOD_DSP_CATEGORY_WAVETABLE,        /* This is a unit type that only reads wave data. */
        FMOD_DSP_CATEGORY_RESAMPLER
    } FMOD_DSP_CATEGORY;

    #define FMOD_DSP_MAXTREEDEPTH  128

    const int FMOD_DSP_TYPE_CODECREADER   = 1000;

    #ifdef FMOD_SUPPORT_DLLS
    typedef FMOD_RESULT (F_CALLBACK *FMOD_DSP_DIALOGIDLECALLBACK)   (FMOD_DSP_STATE *dsp_state);
    #endif
    typedef FMOD_RESULT (F_CALLBACK *FMOD_DSP_GETMEMORYUSED)        (FMOD_DSP_STATE *dsp_state, MemoryTracker *tracker);
    typedef FMOD_RESULT (F_CALLBACK *FMOD_DSP_UPDATE)               (FMOD_DSP_STATE *dsp_state);

    struct FMOD_DSP_DESCRIPTION_EX : public FMOD_DSP_DESCRIPTION, public LinkedListNode
    {
        FMOD_SOUND_FORMAT       mFormat;     
        FMOD_DSP_TYPE           mType;
        int                     mSize;
        FMOD_DSP_CATEGORY       mCategory;
#ifdef FMOD_SUPPORT_DLLS
        FMOD_OS_LIBRARY        *mModule;
        void                   *mAEffect;
#endif
        int                     mResamplerBlockLength;
        unsigned int            mHandle;
        DSPI                   *mDSPSoundCard;  /* what to base dsp tick on. */

#ifdef FMOD_SUPPORT_DLLS
        FMOD_DSP_DIALOGIDLECALLBACK     configidle;
#endif
        FMOD_DSP_GETMEMORYUSED  getmemoryused;
        FMOD_DSP_UPDATE         update;
     };

    #define FMOD_DSP_CONNECTION_REQUEST_MAX     512

    typedef enum
    {
        DSPCONNECTION_REQUEST_ADDINPUT,
        DSPCONNECTION_REQUEST_ADDINPUT_ERRCHECK,
        DSPCONNECTION_REQUEST_DISCONNECTFROM,
        DSPCONNECTION_REQUEST_DISCONNECTALLINPUTS,
        DSPCONNECTION_REQUEST_DISCONNECTALLOUTPUTS,
        DSPCONNECTION_REQUEST_DISCONNECTALL,
        DSPCONNECTION_REQUEST_INSERTINBETWEEN,
        DSPCONNECTION_REQUEST_INSERTINBETWEEN_SEARCH,
        DSPCONNECTION_REQUEST_REVERBUPDATEPARAMETERS
    } FMOD_DSPCONNECTIONREQUEST_CMD;

    #define FMOD_DSP_FLAG_ACTIVE                0x00000002
    #define FMOD_DSP_FLAG_BYPASS                0x00000004
    #define FMOD_DSP_FLAG_FINISHED              0x00000008
    #define FMOD_DSP_FLAG_IDLE                  0x00000010
    #define FMOD_DSP_FLAG_USEDADDDSP            0x00000020
    #define FMOD_DSP_FLAG_FIRSTMIX              0x00000040
    #define FMOD_DSP_FLAG_QUEUEDFORDISCONNECT   0x00000100
    #define FMOD_DSP_FLAG_PARAMETERUPDATE       0x00001000

    class DSPConnectionRequest : public LinkedListNode
    {
      public:

        DSPI                           *mThis;
        DSPI                           *mTarget;
        DSPConnectionI                 *mConnection;
        FMOD_DSPCONNECTIONREQUEST_CMD   mRequest; 
        int                             mInputIndex;
    };
        
    class DSPI : public Plugin, public FMOD_DSP_STATE
    {
        DECLARE_MEMORYTRACKER_NONVIRTUAL_EXPORT

        friend class DSPFilter;
        #if defined(FMOD_SUPPORT_PROFILE) && defined(FMOD_SUPPORT_PROFILE_DSP)
        friend class ProfileDsp;
        #endif

      private:

        /*
            This stuff is timing sensitive.  It must not be referenced directly except through accessors.
        */
        LinkedListNode          mInputHead;
        LinkedListNode          mOutputHead;
        int                     mNumInputs;
        int                     mNumOutputs;

      protected:
                               
        float                  *mOutputBuffer;
        #if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)
        LinkedListNode         *mPrevious;
        #endif
        int                     mTargetFrequency;
        short                   mTreeLevel;

        #if defined(FMOD_SUPPORT_MIXER_NONRECURSIVE) || defined(PLATFORM_PS3)
        short                   mLastChannels;
        #endif

		unsigned short          mCPUUsage;
        unsigned short          mCPUUsageTemp;
#ifdef FMOD_SUPPORT_PROFILE_DSP_VOLUMELEVELS
        unsigned char           mPeakVolume[10];    /* 16 values, 5-bit packed */
        unsigned short          mNumPeakVolumeChans;
#endif

        FMOD_RESULT             doesUnitExist(DSPI *target, bool protect);
                                                                                                 
      public:                        
                    
		static FMOD_OS_CRITICALSECTION *gCrit;
        static bool                     gActive;

        static  FMOD_RESULT     convert         (void *outbuffer, void *inbuffer, FMOD_SOUND_FORMAT outformat, FMOD_SOUND_FORMAT informat, unsigned int length, int destchannelstep, int srcchannelstep, float wetmix);
        static  FMOD_RESULT     validate        (DSP *dsp, DSPI **dspi);

        virtual FMOD_RESULT     read            (float **outbuffer, int *outchannels, unsigned int *length, FMOD_SPEAKERMODE speakermode, int speakermodechannels, unsigned int tick);
        virtual FMOD_RESULT     read            (void  *outbuffer, unsigned int *length, FMOD_SPEAKERMODE speakermode, int speakermodechannels, unsigned int tick);
        virtual FMOD_RESULT     alloc           (FMOD_DSP_DESCRIPTION_EX *description);

        #ifdef FMOD_SUPPORT_MIXER_NONRECURSIVE
                FMOD_RESULT     updateHistory   (DSPI *t, float *buffer, int length, int channels);
        static  FMOD_RESULT     stepBack        (LinkedListNode *&current, DSPConnectionI *&connection, DSPI *&t, LinkedListNode *&next, bool dspionly);
        static  FMOD_RESULT     stepForwards    (LinkedListNode *&current, DSPConnectionI *&connection, DSPConnectionI *&prevconnection, DSPI *&t, LinkedListNode *&next);
        #endif
                FMOD_RESULT     run             (float **outbuffer, int *outchannels, unsigned int *length, FMOD_SPEAKERMODE speakermode, int speakermodechannels, unsigned int tick);

        virtual FMOD_RESULT     setPosition     (unsigned int position, bool processinputs);
        static  FMOD_RESULT     calculateSpeakerLevels(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright, FMOD_SPEAKERMODE speakermode, int channels, FMOD_SPEAKERMAPTYPE speakermap, float *outlevels, int *numinputlevels);
                FMOD_RESULT     updateTreeLevel (int level);
                FMOD_RESULT     updateDSPTick   (unsigned int tick);
#ifndef PLATFORM_PS3_SPU
                FMOD_RESULT     createHistoryBuffer(float **buffer, int numchannels);
                FMOD_RESULT     releaseHistoryBuffer(float *buffer);
#endif

        FMOD_DSP_DESCRIPTION_EX mDescription;
        float                   mDefaultVolume;
        float                   mDefaultFrequency;
        float                   mDefaultPan;
        int                     mDefaultPriority;
        float                  *mBuffer;
        int                     mBufferChannels;
        unsigned int            mDSPTick;
        FMOD_PPCALIGN16(unsigned int mFlags);

#if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)
        unsigned int            mFlagsMramAddress;

        FMOD_PPCALIGN16(unsigned int mMramAddress);
        DSPI                   *mMemory;
        LinkedListNode         *mInputHeadAddress;

        float                  *mHistoryBuffer;
        float                  *mHistoryBufferMemory;
        unsigned int            mHistoryPosition;
        unsigned int            mHistoryLength;

        unsigned int            mCodeMramAddress;
        unsigned int            mCodeEntryAddress;
        unsigned int            mCodeSize;
        unsigned int            mCodeLSOffset;

        unsigned int            mCodeFillLocation;
        unsigned int            mCodeFillSize;
        unsigned int            mCodeFillValue;
#else
		char                    mWantsToFinishMem;
#endif
       
        DSPI();
               
        virtual FMOD_RESULT release             (bool freethis = true);
        virtual FMOD_RESULT getSystemObject     (System **system);

        // Connection / disconnection / input and output enumeration.
                FMOD_RESULT addInputInternal           (DSPI *target, bool checkcircular, DSPConnectionI *connection = 0, DSPConnectionI **connection_out = 0, bool protect = true);
        virtual FMOD_RESULT addInput                   (DSPI *target, DSPConnectionI **connection = 0);
                FMOD_RESULT addInputQueued             (DSPI *target, bool checkcircular, DSPConnectionI *connection_old, DSPConnectionI **connection_out);

                FMOD_RESULT disconnectFromInternal     (DSPI *target, DSPConnectionI *connection, bool protect = true);
                FMOD_RESULT disconnectFrom             (DSPI *target, DSPConnectionI *connection = 0);

                FMOD_RESULT disconnectAllInternal      (bool inputs, bool outputs, bool protect = true);
                FMOD_RESULT disconnectAll              (bool inputs, bool outputs);

                FMOD_RESULT insertInputBetween         (DSPI *target, int inputindex, bool search, DSPConnectionI **connection_out);
                FMOD_RESULT insertInputBetweenInternal (DSPI *target, int inputindex, bool search, DSPConnectionI *connection, bool protect = true);

        virtual FMOD_RESULT remove                     ();
                FMOD_RESULT removeInternal             (bool protect = true);
        
        FMOD_RESULT getNumInputs                (int *numinputs, bool protect = true);
        FMOD_RESULT getNumOutputs               (int *numoutputs, bool protect = true);
        FMOD_RESULT getInput                    (int index, DSPI **input, DSPConnectionI **inputconnection = 0, bool protect = true);
        FMOD_RESULT getOutput                   (int index, DSPI **output, DSPConnectionI **outputconnection = 0, bool protect = true);

        // DSP unit control
        FMOD_INLINE FMOD_RESULT setActive       (bool active)  { active ? mFlags |= FMOD_DSP_FLAG_ACTIVE : mFlags &= ~FMOD_DSP_FLAG_ACTIVE; return FMOD_OK; }
        FMOD_INLINE FMOD_RESULT getActive       (bool *active) { mFlags & FMOD_DSP_FLAG_ACTIVE ? *active = true : *active = false; return FMOD_OK; }
        FMOD_INLINE FMOD_RESULT setBypass       (bool bypass)  { bypass ? mFlags |= FMOD_DSP_FLAG_BYPASS : mFlags &= ~FMOD_DSP_FLAG_BYPASS; return FMOD_OK; }
        FMOD_INLINE FMOD_RESULT getBypass       (bool *bypass) { mFlags & FMOD_DSP_FLAG_BYPASS ? *bypass = true : *bypass = false; return FMOD_OK; }
		FMOD_INLINE FMOD_RESULT setSpeakerActive (FMOD_SPEAKER speaker, bool active) { active ? speakermask |= (1 << speaker) : speakermask &= ~(1 << speaker); return FMOD_OK; }
		FMOD_INLINE FMOD_RESULT getSpeakerActive (FMOD_SPEAKER speaker, bool *active) { speakermask & (1 << speaker) ? *active = true : *active = false; return FMOD_OK; }
        
        virtual FMOD_RESULT reset               ();

        // DSP parameter control
        virtual FMOD_RESULT setParameter        (int index, float value);
        virtual FMOD_RESULT getParameter        (int index, float *value, char *valuestr, int valuestrlen);
        virtual FMOD_RESULT getNumParameters    (int *numparams);
        virtual FMOD_RESULT getParameterInfo    (int index, char *name, char *label, char *description, int descriptionlen, float *min, float *max);
        virtual FMOD_RESULT showConfigDialog    (void *hwnd, bool show);

        // DSP attributes.  
        virtual FMOD_RESULT getInfo             (char *name, unsigned int *version, int *channels, int *configwidth, int *configheight);
        virtual FMOD_RESULT getType             (FMOD_DSP_TYPE *type);
        virtual FMOD_RESULT setDefaults         (float frequency, float volume, float pan, int priority);
        virtual FMOD_RESULT getDefaults         (float *frequency, float *volume, float *pan, int *priority);
        virtual FMOD_RESULT stopBuffering       ();
                                                  
        // Userdata set/get.
        FMOD_RESULT setUserData                 (void *userdata);
        FMOD_RESULT getUserData                 (void **userdata);

#ifdef FMOD_SUPPORT_PROFILE_DSP_VOLUMELEVELS
        FMOD_RESULT calculatePeaks              (const float *buffer, unsigned int length, unsigned int numchannels, DSPI *t = 0);
#endif
        FMOD_RESULT getMemoryInfo               (unsigned int memorybits, unsigned int event_memorybits, unsigned int *memoryused, FMOD_MEMORY_USAGE_DETAILS *memoryused_details);

        virtual FMOD_RESULT  setTargetFrequency (int frequency);
        virtual FMOD_RESULT  getTargetFrequency (int *frequency);

#ifdef FMOD_SUPPORT_MEMORYTRACKER
        static FMOD_RESULT F_CALLBACK getMemoryUsedCallback(FMOD_DSP_STATE *dsp, MemoryTracker *tracker)
        {
            tracker->add(false, FMOD_MEMBITS_DSP, ((DSPI *)dsp)->mDescription.mSize);
            return FMOD_OK;
        }
#endif

        #if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)

        FMOD_RESULT     startBuffering();
        FMOD_RESULT     getHistoryBuffer(float **buffer, unsigned int *position, unsigned int *length);

        #endif
    };
}

#endif  


