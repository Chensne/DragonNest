#ifndef _FMOD_SYSTEMI_H
#define _FMOD_SYSTEMI_H

#include "fmod_settings.h"

#include "fmod.hpp"
#include "fmod_channeli.h"
#include "fmod_channel_stream.h"
#include "fmod_channelgroupi.h"
#include "fmod_dsp_connectionpool.h"
#include "fmod_dsp_codecpool.h"
#include "fmod_dspi.h"
#include "fmod_listener.h"
#include "fmod_soundi.h"
#include "fmod_soundgroupi.h"
#include "fmod_speakerlevels_pool.h"
#include "fmod_historybuffer_pool.h"
#include "fmod_reverbi.h"
#include "fmod_geometry_mgr.h"
#include "fmod_thread.h"
#include "fmod_time.h"
#ifndef _FMOD_MEMORYTRACKER_H
#include "fmod_memorytracker.h"
#endif

#define FMOD_NEED_DSPCODECPOOLINITCRIT \
    defined(FMOD_SUPPORT_DSPCODEC) && \
    (defined(FMOD_SUPPORT_MPEG) || defined(FMOD_SUPPORT_IMAADPCM) || \
     defined(FMOD_SUPPORT_XMA) || defined(FMOD_SUPPORT_RAWCODEC) || defined(FMOD_SUPPORT_CELT)

namespace FMOD
{
    class Output;
    class OutputSoftware;
    class OutputEmulated;
	class PluginFactory;
	class GeometryI;
    class MemPool;
    class ReverbI;
    class Downmix;

    typedef struct
    {
        FMOD_SPEAKER    mSpeaker;
        FMOD_VECTOR     mPosition;
        FMOD_VECTOR     mXZNormal;
        float           mXZAngle;
        float           mDistance;
        bool            mActive;
        bool            mPairUseVBAP;
        float           mPairVBAPSign;
    } FMOD_SPEAKERCONFIG;

    const FMOD_SPEAKERMODE FMOD_SPEAKERMODE_STEREO_LINEAR       = (FMOD_SPEAKERMODE)1000; /* The speakers are stereo with non constant power (linear) panning. */
    const int              FMOD_MAXPRIORITIES                   = 256;
    const int              FMOD_MAXAUDIBIILITY                  = 1000;
    const int              FMOD_STREAMDECODEBUFFERSIZE_DEFAULT  = 400;  /* 400ms */

    const int              FMOD_ADVANCEDSETTINGS_MAXXMACODECS   = 32;
    const int              FMOD_ADVANCEDSETTINGS_MAXADPCMCODECS = 32;
    const int              FMOD_ADVANCEDSETTINGS_MAXMPEGCODECS  = 16;
    const int              FMOD_ADVANCEDSETTINGS_MAXCELTCODECS  = 16;
    const int              FMOD_ADVANCEDSETTINGS_MAXPCMCODECS   = 16;
    const int              FMOD_ADVANCEDSETTINGS_MAX3DREVERBDSPS  = 0;

    const int              FMOD_RECORD_TEMPBUFFERSIZE           = 2048;

    /*
        ==========================================================================================
        WARNING!!!  Don't #ifdef stuff (ABOVE THE LINE - see below) here that might be included in 
        the static version of fmodex.dll and not in the plugin version of fmodex.dll!  If you do 
        the shared class will be out of sync and it will crash!
        ==========================================================================================
    */
    class SystemI : public LinkedListNode
    {
        DECLARE_MEMORYTRACKER_NONVIRTUAL_EXPORT

      public:
        bool                            mInitialized;
        bool                            mPluginsLoaded;
        FMOD_UINT_NATIVE                mMainThreadID;                                       
        FMOD_INITFLAGS                  mFlags;                                                                            
        SoundI                          mSoundListHead;
        static FMOD_OS_CRITICALSECTION *gSoundListCrit;     // This protects mSoundListHead, mSoundGroupUsedHead, mSoundGroupFreeHead


        /*
            Channels & channel management
        */
        int                             mNumChannels;
        ChannelI                       *mChannel;                                  
        ChannelI                        mChannelUsedListHead;
        ChannelI                        mChannelFreeListHead;
        SortedLinkedListNode            mChannelSortedListHead;

        /*
            Output variables.
        */
        Output                         *mOutput;
        FMOD_OUTPUTTYPE                 mOutputType;                                      
        FMOD_SOUND_FORMAT               mOutputFormat;
        int                             mOutputRate;
        int                             mOutputHandle;
        int                             mMaxInputChannels;
        int                             mMaxOutputChannels;
        int                             mSelectedDriver;
        OutputEmulated                 *mEmulated;
                                       
        /*                             
            DSP system                 
        */                             
        unsigned int                    mDSPBlockSize;
        unsigned int                    mDSPBufferSize;
#ifdef FMOD_SUPPORT_SOFTWARE
        float                          *mDSPTempBuff;
        float                          *mDSPTempBuffMem;
        float                          *mDSPMixBuff[FMOD_DSP_MAXTREEDEPTH];
        DSPConnectionPool               mDSPConnectionPool;
        FMOD_OS_CRITICALSECTION        *mDSPCrit;
        FMOD_OS_CRITICALSECTION        *mDSPLockCrit;
        FMOD_OS_CRITICALSECTION        *mDSPConnectionCrit;
        /* one crit for all codec pools; this means e.g. ADPCM can block MPEG
           unnecessarily, but this is a very rare situation anyway
        */
        FMOD_OS_CRITICALSECTION        *mDSPCodecPoolInitCrit;
        bool                            mDSPActive;
        DSPI                           *mDSPSoundCard;
        DSPI                           *mDSPChannelGroupTarget;
        TimeStamp                       mDSPTimeStamp;
        DSPConnectionRequest            mConnectionRequest[FMOD_DSP_CONNECTION_REQUEST_MAX];
        DSPConnectionRequest            mConnectionRequestUsedHead;
        DSPConnectionRequest            mConnectionRequestFreeHead;
        bool                            mConnectionRequestFlushing;
        OutputSoftware                 *mSoftware;
#endif
        
        FMOD_PPCALIGN16(FMOD_UINT64P    mDSPClock);
                                      
        /*                             
            3D Controls                
        */                             
        Listener                        mListener[LISTENER_MAX];
        int                             mNumListeners;
        float                           mDistanceScale;
        float                           mRolloffScale;
        float                           mDopplerScale;
        FMOD_3D_ROLLOFFCALLBACK         mRolloffCallback;
                                       
        /*                             
            Plugins
        */
        PluginFactory                  *mPluginFactory;
        char                            mPluginPath[FMOD_STRING_MAXPATHLEN];
        unsigned int                    mFSBPluginHandle;
        unsigned int                    mWAVPluginHandle;
        unsigned int                    mMPEGPluginHandle;
        unsigned int                    mCELTPluginHandle;
                                       
        /*                             
            Miscellaneous              
        */
        FMOD_ADVANCEDSETTINGS           mAdvancedSettings;
        void                           *mUserData;
        TimeStamp                       mUpdateTimeStamp;
        unsigned int                    mLastTimeStamp;
        unsigned int                    mIndex;
        int                             mNumSoftwareChannels;
        int                             mMinHardwareChannels2D;
        int                             mMaxHardwareChannels2D;
        int                             mMinHardwareChannels3D;
        int                             mMaxHardwareChannels3D;
        unsigned int                    mDeviceListLastCheckedTime;
        bool                            mDeviceListChanged;
        bool                            mCreatedHardwareSample;
        ChannelGroupI                  *mChannelGroup;
        ChannelGroupI                   mChannelGroupHead;
        SoundGroupI                    *mSoundGroup;
        LinkedListNode                  mSoundGroupUsedHead;
        LinkedListNode                  mSoundGroupFreeHead;
        FMOD_DSP_RESAMPLER              mResampleMethod;
        MemSingleton                    mMultiSubSampleLockBuffer;
        FMOD_OS_CRITICALSECTION        *mMultiSubSampleLockBufferCrit;
        bool                            mUsesUserCallbacks;
        unsigned int                    mBufferSize;

        FMOD_FILE_OPENCALLBACK          mOpenCallback;
        FMOD_FILE_CLOSECALLBACK         mCloseCallback;
        FMOD_FILE_READCALLBACK          mReadCallback;
        FMOD_FILE_SEEKCALLBACK          mSeekCallback;

        FMOD_FILE_OPENCALLBACK          mOpenRiderCallback;
        FMOD_FILE_CLOSECALLBACK         mCloseRiderCallback;
        FMOD_FILE_READCALLBACK          mReadRiderCallback;
        FMOD_FILE_SEEKCALLBACK          mSeekRiderCallback;

        FMOD_SYSTEM_CALLBACK            mCallback;

        SpeakerLevelsPool               mSpeakerLevelsPool;
        HistoryBufferPool               mHistoryBufferPool;

        /*
            Stream thread handles etc.
        */
        unsigned int                    mStreamFileBufferSize;
        FMOD_TIMEUNIT                   mStreamFileBufferSizeType;

        /*
            Speaker Variables
        */
        FMOD_SPEAKERMODE                mSpeakerMode;
        FMOD_SPEAKERCONFIG              mSpeaker[FMOD_SPEAKER_MAX];
        FMOD_SPEAKERCONFIG             *mSpeakerList[FMOD_SPEAKER_MAX];

        /*
            Standard global reverb 
        */
        ReverbI                         mReverbGlobal;

        Downmix                        *mDownmix;

        /*
            /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\

            =======================================================
            WARNING!!!  Don't #ifdef stuff (ABOVE THIS LINE) here 
            that might be included in  the static version of 
            fmodex.dll and not in the plugin version of fmodex.dll!  
            If you do the shared class will be out of sync and it 
            will crash!
            =======================================================

            \/ \/ \/ \/ \/ \/ ok to ifdef stuff \/ \/ \/ \/ \/ \/
        */
#ifdef FMOD_SUPPORT_STREAMING
        static void                     streamThread(void *data);
        LinkedListNode                  mStreamListChannelHead;
        LinkedListNode                 *mStreamListChannelCurrent;
        LinkedListNode                 *mStreamListChannelNext;
        LinkedListNode                  mStreamListSoundHead;
        Thread                          mStreamThread;
        bool                            mStreamThreadActive;
        FMOD_OS_CRITICALSECTION        *mStreamRealchanCrit;
        FMOD_OS_CRITICALSECTION        *mStreamUpdateCrit;
        FMOD_OS_CRITICALSECTION        *mStreamListCrit;
        TimeStamp                       mStreamTimeStamp;
#endif

        /*
            Output variables.
        */
#ifdef FMOD_SUPPORT_ASIO
        FMOD_SPEAKER                    mASIOSpeakerList[DSP_MAXLEVELS_OUT];
#endif

        /*
            DSP Codec pools.
        */        

#ifdef FMOD_SUPPORT_DSPCODEC
        #ifdef FMOD_SUPPORT_MPEG
        DSPCodecPool                   mDSPCodecPool_MPEG;
        #endif
        #ifdef FMOD_SUPPORT_IMAADPCM
        DSPCodecPool                   mDSPCodecPool_ADPCM;
        #endif
        #ifdef FMOD_SUPPORT_XMA
        DSPCodecPool                   mDSPCodecPool_XMA;
        #endif
        #ifdef FMOD_SUPPORT_CELT
        DSPCodecPool                   mDSPCodecPool_CELT;
        #endif
        #ifdef FMOD_SUPPORT_RAWCODEC
        DSPCodecPool                   mDSPCodecPool_RAW;
        #endif
#endif

        /*                             
            Geometry                 
        */ 
#ifdef FMOD_SUPPORT_GEOMETRY
		GeometryI					   *mGeometryList;
		GeometryMgr                     mGeometryMgr;
#ifdef FMOD_SUPPORT_GEOMETRY_THREADED
        TimeStamp                       mGeometryTimeStamp;
#endif
#endif // FMOD_SUPPORT_GEOMETRY

#ifdef FMOD_SUPPORT_MULTIREVERB
        /*
            3D reverbs
        */

        ReverbI                         mReverb3D;

        ReverbI                         mReverb3DHead;
        bool                            mReverb3DActive;
        FMOD_REVERB_PROPERTIES          mReverb3DAmbientProperties;
        FMOD_RESULT                     set3DReverbProperties(const FMOD_REVERB_PROPERTIES *prop, bool force_create=false);
        FMOD_RESULT                     get3DReverbProperties(FMOD_REVERB_PROPERTIES *prop);
        FMOD_RESULT                     set3DReverbProperties  (int instance, const FMOD_REVERB_PROPERTIES *prop, const FMOD_VECTOR *pos, const float mindistance, const float maxdistance);
        FMOD_RESULT                     get3DReverbProperties  (int instance, FMOD_REVERB_PROPERTIES *prop, FMOD_VECTOR *pos, float *mindistance, float *maxdistance);
        FMOD_RESULT                     setReverbAmbientProperties(FMOD_REVERB_PROPERTIES* prop);
        FMOD_RESULT                     getReverbAmbientProperties(FMOD_REVERB_PROPERTIES* prop);
        FMOD_RESULT                     update3DReverbs();
        FMOD_RESULT                     set3DReverbActive(bool reverb_active);
        FMOD_RESULT                     get3DReverbActive(bool *reverb_active);
        unsigned int                    count3DPhysicalReverbs();
        unsigned int                    count3DVirtualReverbs();
        
#else 
        FMOD_RESULT                     setReverbAmbientProperties(FMOD_REVERB_PROPERTIES *prop) { return FMOD_ERR_UNSUPPORTED; }
        FMOD_RESULT                     getReverbAmbientProperties(FMOD_REVERB_PROPERTIES *prop) { return FMOD_ERR_UNSUPPORTED; }
#endif

        /*
            Internal member functions.
        */
        static FMOD_RESULT              getInstance(unsigned int index, SystemI **sys);
        static FMOD_RESULT              validate(System *sound, SystemI **systemi);

        FMOD_RESULT                     updateStreams();
        FMOD_RESULT                     updateChannels(int delta);
        FMOD_RESULT                     updateSoundGroups(int delta);
#ifdef FMOD_SUPPORT_VSTPLUGIN
        FMOD_RESULT                     updateVSTPlugins();
        LinkedListNode                  mVSTPluginsListHead;
#endif
        FMOD_RESULT                     findChannel(FMOD_CHANNELINDEX id, SoundI *sound, ChannelI **channel);
        FMOD_RESULT                     findChannel(FMOD_CHANNELINDEX id, DSPI *dsp, ChannelI **channel);
        FMOD_RESULT                     createSample(FMOD_MODE mode, FMOD_CODEC_WAVEFORMAT *waveformat, Sample **sample);
        FMOD_RESULT                     createDSP(FMOD_DSP_DESCRIPTION_EX *description, DSPI **dsp, bool allocate = true);
        FMOD_RESULT                     createSoundInternal(const char *name_or_data, FMOD_MODE mode, unsigned int buffersize, FMOD_TIMEUNIT buffersizetype, FMOD_CREATESOUNDEXINFO *exinfo, bool calledfromasync, SoundI **sound);
        FMOD_RESULT                     checkDriverList(bool fromsystemupdate);
        FMOD_RESULT                     setUpPlugins();
        FMOD_RESULT                     sortSpeakerList();
        FMOD_RESULT                     prepareSpeakerPairs();
#ifdef FMOD_SUPPORT_DSPCODEC
        FMOD_RESULT                     allocateDSPCodec(FMOD_SOUND_FORMAT format, DSPCodec **dsp);
#endif
        FMOD_RESULT                     getSoftwareFormat(int *samplerate, FMOD_SOUND_FORMAT *format, int *numoutputchannels, int *maxinputchannels, FMOD_DSP_RESAMPLER *resamplemethod, int *bits)
        {
            if (samplerate)
            {
                *samplerate = mOutputRate;
            }
            if (format)
            {
                *format = mOutputFormat;
            }
            if (numoutputchannels)
            {
                *numoutputchannels = mMaxOutputChannels;
            }
            if (maxinputchannels)
            {
                *maxinputchannels = mMaxInputChannels;
            }
            if (resamplemethod)
            {
                *resamplemethod = mResampleMethod;
            }
            if (bits)
            {
                SoundI::getBitsFromFormat(mOutputFormat, bits);
            }
            return FMOD_OK;
        }


        SystemI();
        virtual ~SystemI() {}

        FMOD_RESULT       release                ();
                                                 
        // Pre-init functions.                   
        FMOD_RESULT       setOutput              (FMOD_OUTPUTTYPE output);
        FMOD_RESULT       getOutput              (FMOD_OUTPUTTYPE *output);
        FMOD_RESULT       getNumDrivers          (int *numdrivers);
        FMOD_RESULT       getDriverInfo          (int id, char *name, int namelen, FMOD_GUID *guid);
        FMOD_RESULT       getDriverInfoW         (int id, short *name, int namelen, FMOD_GUID *guid);
        FMOD_RESULT       getDriverCaps          (int id, FMOD_CAPS *caps, int *minfrequency, int *maxfrequency, FMOD_SPEAKERMODE *controlpanelspeakermode);
        FMOD_RESULT       setDriver              (int driver);
        FMOD_RESULT       getDriver              (int *driver);
        FMOD_RESULT       setHardwareChannels    (int min2d, int max2d, int min3d, int max3d);
        FMOD_RESULT       getHardwareChannels    (int *num2d, int *num3d, int *total);
        FMOD_RESULT       setSoftwareChannels    (int numsoftwarechannels);
        FMOD_RESULT       getSoftwareChannels    (int *numsoftwarechannels);
        FMOD_RESULT       setSoftwareFormat      (int samplerate, FMOD_SOUND_FORMAT format, int numoutputchannels, int maxinputchannels, FMOD_DSP_RESAMPLER resamplemethod);
        //FMOD_RESULT       getSoftwareFormat      // see above
        FMOD_RESULT       setDSPBufferSize       (unsigned int bufferlength, int numbuffers);
        FMOD_RESULT       getDSPBufferSize       (unsigned int *bufferlength, int *numbuffers);
        FMOD_RESULT       setFileSystem          (FMOD_FILE_OPENCALLBACK useropen, FMOD_FILE_CLOSECALLBACK userclose, FMOD_FILE_READCALLBACK userread, FMOD_FILE_SEEKCALLBACK userseek, int buffersize);
        FMOD_RESULT       attachFileSystem       (FMOD_FILE_OPENCALLBACK useropen, FMOD_FILE_CLOSECALLBACK userclose, FMOD_FILE_READCALLBACK userread, FMOD_FILE_SEEKCALLBACK userseek);
        FMOD_RESULT       setAdvancedSettings    (FMOD_ADVANCEDSETTINGS *settings);
        FMOD_RESULT       getAdvancedSettings    (FMOD_ADVANCEDSETTINGS *settings);
        FMOD_RESULT       setSpeakerMode         (FMOD_SPEAKERMODE speakermode);
        FMOD_RESULT       getSpeakerMode         (FMOD_SPEAKERMODE *speakermode) { if (!speakermode) { return FMOD_ERR_INVALID_PARAM; } *speakermode = mSpeakerMode; return FMOD_OK; }
        FMOD_RESULT       setCallback            (FMOD_SYSTEM_CALLBACK callback);

        // Plug-in support                       
        FMOD_RESULT       setPluginPath          (const char *path);
        FMOD_RESULT       loadPlugin             (const char *filename, unsigned int *handle, unsigned int priority);
        FMOD_RESULT       unloadPlugin           (unsigned int handle);
        FMOD_RESULT       getNumPlugins          (FMOD_PLUGINTYPE plugintype, int *numplugins);
        FMOD_RESULT       getPluginHandle        (FMOD_PLUGINTYPE plugintype, int index, unsigned int *handle);
        FMOD_RESULT       getPluginInfo          (unsigned int handle, FMOD_PLUGINTYPE *plugintype, char *name, int namelen, unsigned int *version);
        FMOD_RESULT       setOutputByPlugin      (unsigned int handle);
        FMOD_RESULT       getOutputByPlugin      (unsigned int *handle);
        FMOD_RESULT       createDSPByPlugin      (unsigned int handle, DSPI **dsp);
        FMOD_RESULT       createCodec            (FMOD_CODEC_DESCRIPTION *description, unsigned int priority);
                                                 
        // Init/Close                            
        FMOD_RESULT       init                   (int maxchannels, FMOD_INITFLAGS flags, void *extradriverdata);
        FMOD_RESULT       close                  ();
        FMOD_RESULT       closeEx                (bool calledfrominit);                                                 
        // Post-init system functions
        FMOD_RESULT       update                 ();

        FMOD_RESULT       set3DSettings          (float dopplerscale, float distancefactor, float rolloffscale);
        FMOD_RESULT       get3DSettings          (float *dopplerscale, float *distancefactor, float *rolloffscale);
        FMOD_RESULT       set3DNumListeners      (int numlisteners);
        FMOD_RESULT       get3DNumListeners      (int *numlisteners);
        FMOD_RESULT       set3DListenerAttributes(int listener, const FMOD_VECTOR *pos, const FMOD_VECTOR *vel, const FMOD_VECTOR *forward, const FMOD_VECTOR *up);
        FMOD_RESULT       get3DListenerAttributes(int listener, FMOD_VECTOR *pos, FMOD_VECTOR *vel, FMOD_VECTOR *forward, FMOD_VECTOR *up);
        FMOD_RESULT       set3DRolloffCallback   (FMOD_3D_ROLLOFFCALLBACK callback);
        FMOD_RESULT       set3DSpeakerPosition   (FMOD_SPEAKER speaker, float x, float y, bool active = true);
        FMOD_RESULT       get3DSpeakerPosition   (FMOD_SPEAKER speaker, float *x, float *y, bool *active = 0);

        FMOD_RESULT       setStreamBufferSize    (unsigned int filebuffersize, FMOD_TIMEUNIT filebuffersizetype);
        FMOD_RESULT       getStreamBufferSize    (unsigned int *filebuffersize, FMOD_TIMEUNIT *filebuffersizetype);
                                                 
        // System information functions.  
        FMOD_RESULT       getVersion             (unsigned int *version);
        FMOD_RESULT       getOutputHandle        (void **handle);
        FMOD_RESULT       getChannelsPlaying     (int *channels);
        FMOD_RESULT       getCPUUsage            (float *dsp, float *stream, float *geometry, float *update, float *total);
        FMOD_RESULT       getSoundRAM            (int *currentalloced, int *maxalloced, int *total);
        FMOD_RESULT       getNumCDROMDrives      (int *numdrives);
        FMOD_RESULT       getCDROMDriveName      (int drive, char *drivename, int drivenamelen, char *scsiname, int scsinamelen, char *devicename, int devicenamelen);
        FMOD_RESULT       getSpectrum            (float *spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype);
        FMOD_RESULT       getWaveData            (float *wavearray, int numvalues, int channeloffset);
                                                                        
        // Sound/DSP/Channel/FX creation and retrieval.        
        FMOD_RESULT       createSound            (const char *name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO *exinfo, SoundI **sound);
        FMOD_RESULT       createStream           (const char *name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO *exinfo, SoundI **sound);
        FMOD_RESULT       createDSP              (FMOD_DSP_DESCRIPTION *description, DSPI **dsp);
        FMOD_RESULT       createDSPByType        (FMOD_DSP_TYPE type, DSPI **dsp);
        FMOD_RESULT       createChannelGroup     (const char *name, ChannelGroupI **channelgroup);
        FMOD_RESULT       createSoundGroup       (const char *name, SoundGroupI **soundgroup);
        FMOD_RESULT       createReverb           (ReverbI **reverb);
                                                 
        FMOD_RESULT       playSound              (FMOD_CHANNELINDEX channelid, SoundI *sound, bool paused, ChannelI **channel);
        FMOD_RESULT       playDSP                (FMOD_CHANNELINDEX channelid, DSPI *dsp, bool paused, ChannelI **channel);
        FMOD_RESULT       getChannel             (int channelid, ChannelI **channel);
        FMOD_RESULT       getMasterChannelGroup  (ChannelGroupI **channelgroup);
        FMOD_RESULT       getMasterSoundGroup    (SoundGroupI **soundgroup);
                                                 
        // Reverb api                            
        FMOD_RESULT       setReverbProperties    (const FMOD_REVERB_PROPERTIES *prop, bool force_create=false);
        FMOD_RESULT       getReverbProperties    (FMOD_REVERB_PROPERTIES *prop);
                                                   
        // System level DSP access.
        FMOD_RESULT       getDSPHead             (DSPI **dsp);
        FMOD_RESULT       addDSP                 (DSPI *dsp, DSPConnectionI **connection);
		FMOD_RESULT       lockDSP                ();
		FMOD_RESULT       unlockDSP              ();
        FMOD_RESULT       getDSPClock            (unsigned int *hi, unsigned int *lo);

        // Recording api                         
        FMOD_RESULT       getRecordNumDrivers    (int *numdrivers);
        FMOD_RESULT       getRecordDriverInfo    (int id, char *name, int namelen, FMOD_GUID *guid);
        FMOD_RESULT       getRecordDriverInfoW   (int id, short *name, int namelen, FMOD_GUID *guid);
        FMOD_RESULT       getRecordDriverCaps    (int id, FMOD_CAPS *caps, int *minfrequency, int *maxfrequency);
        FMOD_RESULT       getRecordPosition      (int id, unsigned int *position);  

        FMOD_RESULT       recordStart            (int id, SoundI *sound, bool loop);
        FMOD_RESULT       recordStop             (int id);
        FMOD_RESULT       isRecording            (int id, bool *recording);

		// Geometry api
		FMOD_RESULT       createGeometry		 (int maxpolygons, int maxvertices, GeometryI **geometry);
		FMOD_RESULT       setGeometrySettings    (float maxworldsize);
		FMOD_RESULT       getGeometrySettings    (float *maxworldsize);
        FMOD_RESULT       loadGeometry           (const void *data, int datasize, GeometryI **geometry);
        FMOD_RESULT       getGeometryOcclusion   (const FMOD_VECTOR *listener, const FMOD_VECTOR *source, float *direct, float *reverb);
                                                 
        // Network functions.
        FMOD_RESULT       setNetworkProxy        (const char *proxy);
        FMOD_RESULT       getNetworkProxy        (char *proxy, int proxylen);
        FMOD_RESULT       setNetworkTimeout      (int timeout);
        FMOD_RESULT       getNetworkTimeout      (int *timeout);
                                                 
        // Userdata set/get.
        FMOD_RESULT       setUserData            (void *userdata);
        FMOD_RESULT       getUserData            (void **userdata);

        FMOD_RESULT       getMemoryInfo          (unsigned int memorybits, unsigned int event_memorybits, unsigned int *memoryused, FMOD_MEMORY_USAGE_DETAILS *memoryused_details);

        // ===================================================================================================
        // Functions not exposed in FMOD::System.
        // ===================================================================================================

        FMOD_RESULT F_API getSoundList                  (SoundI   **sound);
        FMOD_RESULT F_API createChannelGroupInternal    (const char *name, ChannelGroupI **channelgroup, bool createdsp, bool storenameinchannelgroup);
                                                 
        FMOD_RESULT       stopSound              (SoundI *sound);
        FMOD_RESULT       stopDSP                (DSPI   *dsp);
        FMOD_RESULT       getListenerObject      (int listener, Listener **listenerobject);

        FMOD_RESULT       setGlobalUserCallbacks (FMOD_FILE_OPENCALLBACK open, FMOD_FILE_CLOSECALLBACK close, FMOD_FILE_READCALLBACK read, FMOD_FILE_SEEKCALLBACK seek);

#ifdef FMOD_SUPPORT_SOFTWARE
        FMOD_RESULT       flushDSPConnectionRequests(bool calledfrommainthread = true);
#endif

#ifndef FMOD_STATICFORPLUGINS
        static FMOD_RESULT F_API getGlobals(Global **global);
#endif
    };
}

#endif

