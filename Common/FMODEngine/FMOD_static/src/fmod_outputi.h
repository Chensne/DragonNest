#ifndef _FMOD_OUTPUTI_H
#define _FMOD_OUTPUTI_H

#include "fmod_settings.h"

#include "fmod_channelpool.h"
#include "fmod_channelgroupi.h"
#include "fmod_dsp_resampler.h"
#include "fmod_output.h"
#include "fmod_linkedlist.h"
#include "fmod_os_misc.h"
#include "fmod_plugin.h"

#define FMOD_OUTPUT_MAXDRIVERS  32

namespace FMOD
{
    class Sample;
    class MemoryTracker;
    struct FMOD_RECORDING_INFO;

    static const FMOD_OUTPUTTYPE FMOD_OUTPUTTYPE_EMULATED = (FMOD_OUTPUTTYPE)-1;
    static const FMOD_OUTPUTTYPE FMOD_OUTPUTTYPE_SOFTWARE = (FMOD_OUTPUTTYPE)-2;

    typedef int         (F_CALLBACK *FMOD_OUTPUT_GETSAMPLEMAXCHANNELS)       (FMOD_OUTPUT_STATE *output, FMOD_MODE mode, FMOD_SOUND_FORMAT format); 
    typedef FMOD_RESULT (F_CALLBACK *FMOD_OUTPUT_GETDRIVERINFOCALLBACK)      (FMOD_OUTPUT_STATE *output, int id, char *name, int namelen, FMOD_GUID *guid);
    typedef FMOD_RESULT (F_CALLBACK *FMOD_OUTPUT_GETDRIVERINFOWCALLBACK)     (FMOD_OUTPUT_STATE *output, int id, short *name, int namelen, FMOD_GUID *guid);
    typedef FMOD_RESULT (F_CALLBACK *FMOD_OUTPUT_GETDRIVERCAPSEXCALLBACK)    (FMOD_OUTPUT_STATE *output, int id, FMOD_CAPS *caps, int *minfrequency, int *maxfrequency, FMOD_SPEAKERMODE *controlpanelspeakermode);
    typedef FMOD_RESULT (F_CALLBACK *FMOD_OUTPUT_GETDRIVERCAPSEX2CALLBACK)   (FMOD_OUTPUT_STATE *output, int id, FMOD_CAPS *caps, int *minfrequency, int *maxfrequency, FMOD_SPEAKERMODE *controlpanelspeakermode, int *num2dchannels, int *num3dchannels, int *totalchannels);
    typedef FMOD_RESULT (F_CALLBACK *FMOD_OUTPUT_INITEXCALLBACK)             (FMOD_OUTPUT_STATE *output, int selecteddriver, FMOD_INITFLAGS flags, int *outputrate, int outputchannels, FMOD_SOUND_FORMAT *outputformat, FMOD_SPEAKERMODE *speakermode, int dspbufferlength, int dspnumbuffers, int max2dchannels, int max3dchannels, void *extradriverdata);
    typedef FMOD_RESULT (F_CALLBACK *FMOD_OUTPUT_STARTCALLBACK)              (FMOD_OUTPUT_STATE *output);
    typedef FMOD_RESULT (F_CALLBACK *FMOD_OUTPUT_STOPCALLBACK)               (FMOD_OUTPUT_STATE *output);
    typedef FMOD_RESULT (F_CALLBACK *FMOD_OUTPUT_UPDATEFINISHEDCALLBACK)     (FMOD_OUTPUT_STATE *output);
    typedef FMOD_RESULT (F_CALLBACK *FMOD_OUTPUT_CREATESAMPLECALLBACK)       (FMOD_OUTPUT_STATE *output, FMOD_MODE mode, FMOD_CODEC_WAVEFORMAT *waveformat, Sample **sample);
    typedef FMOD_RESULT (F_CALLBACK *FMOD_OUTPUT_GETSOUNDRAMCALLBACK)        (FMOD_OUTPUT_STATE *output, int *currentalloced, int *maxalloced, int *total);
    typedef FMOD_RESULT (F_CALLBACK *FMOD_OUTPUT_RECORDGETDRIVERCAPSCALLBACK)(FMOD_OUTPUT_STATE *output, int id, FMOD_CAPS *caps, int *minfrequency, int *maxfrequency);
    typedef FMOD_RESULT (F_CALLBACK *FMOD_OUTPUT_RECORDSTARTCALLBACK)        (FMOD_OUTPUT_STATE *output, FMOD_RECORDING_INFO *recordinfo, FMOD_SOUND *sound, int loop);
    typedef FMOD_RESULT (F_CALLBACK *FMOD_OUTPUT_RECORDSTOPCALLBACK)         (FMOD_OUTPUT_STATE *output, FMOD_RECORDING_INFO *recordinfo);
    typedef FMOD_RESULT (F_CALLBACK *FMOD_OUTPUT_RECORDGETPOSITIONCALLBACK)  (FMOD_OUTPUT_STATE *output, FMOD_RECORDING_INFO *recordinfo, unsigned int *pcm);
    typedef FMOD_RESULT (F_CALLBACK *FMOD_OUTPUT_RECORDLOCKCALLBACK)         (FMOD_OUTPUT_STATE *output, FMOD_RECORDING_INFO *recordinfo, unsigned int offset, unsigned int length, void **ptr1, void **ptr2, unsigned int *len1, unsigned int *len2);
    typedef FMOD_RESULT (F_CALLBACK *FMOD_OUTPUT_RECORDUNLOCKCALLBACK)       (FMOD_OUTPUT_STATE *output, FMOD_RECORDING_INFO *recordinfo, void *ptr1, void *ptr2, unsigned int len1, unsigned int len2);    
    typedef FMOD_RESULT (F_CALLBACK *FMOD_OUTPUT_SETREVERBCALLBACK)          (FMOD_OUTPUT_STATE *output, const FMOD_REVERB_PROPERTIES *prop);
    typedef FMOD_RESULT (F_CALLBACK *FMOD_OUTPUT_GETREVERBCALLBACK)          (FMOD_OUTPUT_STATE *output, FMOD_REVERB_PROPERTIES *prop);
    typedef FMOD_RESULT (F_CALLBACK *FMOD_OUTPUT_POSTMIXCALLBACK)            (FMOD_OUTPUT_STATE *output);
    typedef FMOD_RESULT (F_CALLBACK *FMOD_OUTPUT_GETMEMORYUSED)              (FMOD_OUTPUT_STATE *output, MemoryTracker *tracker);
    


    struct FMOD_OUTPUT_DESCRIPTION_EX : public FMOD_OUTPUT_DESCRIPTION, public LinkedListNode
    {
        FMOD_OUTPUTTYPE  mType;        
        int              mSize;
        FMOD_OS_LIBRARY *mModule;
        unsigned int     mHandle;

        FMOD_OUTPUT_GETSAMPLEMAXCHANNELS        getsamplemaxchannels;
        FMOD_OUTPUT_GETDRIVERINFOCALLBACK       getdriverinfo;
        FMOD_OUTPUT_GETDRIVERINFOWCALLBACK      getdriverinfow;
        FMOD_OUTPUT_GETDRIVERCAPSEXCALLBACK     getdrivercapsex;
        FMOD_OUTPUT_GETDRIVERCAPSEX2CALLBACK    getdrivercapsex2;
        FMOD_OUTPUT_INITEXCALLBACK              initex;
        FMOD_OUTPUT_STARTCALLBACK               start;                 /* [in] Initialization function for the output device to start accepting audio data from the FMOD software mixer.  This is called from System::init. */
        FMOD_OUTPUT_STOPCALLBACK                stop;                  /* [in] Initialization function for the output device to stop accepting audio data from FMOD the software mixer.  This is called from System::close. */
        FMOD_OUTPUT_UPDATEFINISHEDCALLBACK      updatefinished;        /* [in] Update finished function.  PS2 only. */
        FMOD_OUTPUT_CREATESAMPLECALLBACK        createsample;
        FMOD_OUTPUT_GETSOUNDRAMCALLBACK         getsoundram;           /* [in] For information about sound ram usage. This callback is to give System::getSoundRam somethign to return */
		FMOD_OUTPUT_POSTMIXCALLBACK             postmixcallback;       /* [in] Called within mixer update, after the mix has happened, and within SystemI::mDSPCrit. */
        FMOD_OUTPUT_GETNUMDRIVERSCALLBACK       record_getnumdrivers;  /* [in] For recording device enumeration.  This callback is to give System::getRecordNumDrivers somthing to return. */
        FMOD_OUTPUT_GETDRIVERINFOCALLBACK       record_getdriverinfo;  /* [in] For recording device enumeration.  This callback is to give System::getRecordDriverInfo somthing to return. */
        FMOD_OUTPUT_GETDRIVERINFOWCALLBACK      record_getdriverinfow; /* [in] For recording device enumeration.  This callback is to give System::getRecordDriverInfo somthing to return. */
        FMOD_OUTPUT_RECORDGETDRIVERCAPSCALLBACK record_getdrivercaps;  /* [in] For recording device enumeration.  This callback is to give System::getRecordDriverCaps somthing to return. */
        FMOD_OUTPUT_RECORDSTARTCALLBACK         record_start;          /* [in] Initialization function for the recording device to start accepting audio data from the FMOD software mixer.  This is called from System::recordStart. */
        FMOD_OUTPUT_RECORDSTOPCALLBACK          record_stop;           /* [in] Initialization function for the recording device to stop accepting audio data from FMOD the software mixer.  This is called from System::recordStop. */
        FMOD_OUTPUT_RECORDGETPOSITIONCALLBACK   record_getposition;    /* [in] This is called from the FMOD software mixer thread.  This returns a position value in samples so that FMOD knows where and when to fill its record buffer. */
        FMOD_OUTPUT_RECORDLOCKCALLBACK          record_lock;           /* [in] This is called from the FMOD software mixer thread.  This function provides a pointer to data that FMOD can read from when updating its internal record buffer. */
        FMOD_OUTPUT_RECORDUNLOCKCALLBACK        record_unlock;         /* [in] This is called from the FMOD software mixer thread.  This optional function accepts the data that has been read does whatever post read operation nescessary. */
        FMOD_OUTPUT_SETREVERBCALLBACK           reverb_setproperties;  /* [in] This is called from System::setReverbProperties. */
        FMOD_OUTPUT_GETMEMORYUSED               getmemoryused;
    };

    class ChannelPool;
    class ChannelReal;
    class SoundI;

    struct FMOD_RECORDING_INFO : public LinkedListNode
    {
        int                     mRecordId;
        int                     mRecordDriver;  /* MGB Deprecated - This is a GUID, should be implemented via getDriverInfo */
        FMOD_GUID               mRecordGUID;
        bool                    mRecordFinished;

        int                     mRecordOffset;
        bool                    mRecordLoop;
        FMOD_SOUND_FORMAT       mRecordFormat;
        unsigned int            mRecordLastCursorPos;
        unsigned int            mRecordBufferLength;
        SoundI                 *mRecordSound;
        int                     mRecordRate;
        int                     mRecordChannels;

        DSPResampler           *mRecordResamplerDSP;
        float                  *mRecordTempBuffer;
        unsigned int            mRecordTempBufferLength;
        void                   *mRecordPlatformSpecific;
    };

    class Output : public Plugin, public FMOD_OUTPUT_STATE
    {
        DECLARE_MEMORYTRACKER_NONVIRTUAL

        friend class SystemI;
        friend class PluginFactory;
        friend class SoundI;
        friend class ChannelGroupI;
        #if defined(FMOD_SUPPORT_PROFILE) && defined(FMOD_SUPPORT_PROFILE_CHANNEL)
        friend class ProfileChannel;
        #endif

      public:

        ChannelPool             *mChannelPool;       /* Used for generic 2d/3d channels, or 2d channels only */

      protected:

        bool                     mEnumerated;
        bool                     mPolling; 
        SystemI                 *mSystem;
        ChannelPool             *mChannelPool3D;     /* Used for 3d channels only */
        int                      mMixAheadBlocks;
        ChannelGroupI           *mMusicChannelGroup; /* Music channelgroup for Xbox360 */

        int                      mNum2DChannelsFromCaps;
        int                      mNum3DChannelsFromCaps;
        int                      mTotalChannelsFromCaps;
        int                      mDSPTick;

#ifdef FMOD_SUPPORT_RECORDING
        bool                     mRecordEnumerated;
        int                      mRecordNumDrivers;
        int                      mRecordNumActive;

        FMOD_OS_CRITICALSECTION *mRecordInfoCrit;
        FMOD_RECORDING_INFO      mRecordInfoHead;
        FMOD_RECORDING_INFO     *mRecordInfoForResampler;

        FMOD_RESULT              recordRead(FMOD_RECORDING_INFO *recordinfo, float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int outchannels);
 static FMOD_RESULT F_CALLBACK   recordResamplerReadCallback(FMOD_DSP_STATE *dsp_state, float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int outchannels);
        FMOD_RESULT              recordFill(FMOD_RECORDING_INFO *recordinfo, unsigned int length);
        FMOD_RESULT              recordUpdate();
        FMOD_RESULT              recordGetInfo(int id, FMOD_RECORDING_INFO **info);
        FMOD_RESULT              recordStop(FMOD_RECORDING_INFO *recordinfo);
        FMOD_RESULT              recordStopAll(bool finishedonly);
#endif

      public:

        FMOD_OUTPUT_DESCRIPTION_EX  mDescription;

        Output();
               
        virtual FMOD_RESULT release();
        
#ifdef FMOD_SUPPORT_SOFTWARE
        FMOD_RESULT         mix(void *buffer, unsigned int numsamples);
#endif

#ifndef PLATFORM_PS3_SPU
        virtual FMOD_RESULT getFreeChannel(FMOD_MODE mode, ChannelReal **realchannel, int numchannels, int numsoundchannels, int *found, bool ignorereserved = false);
#endif

#ifdef FMOD_SUPPORT_HARDWAREXM
        FMOD_RESULT         reserveVoice(int voice, bool reserved);
#endif

#ifdef FMOD_SUPPORT_SOFTWARE
        static FMOD_RESULT F_CALLBACK mixCallback(FMOD_OUTPUT_STATE *output, void *buffer, unsigned int length);
#endif
    };
}

#endif

