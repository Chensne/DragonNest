#ifndef _FMOD_OUTPUT_WASAPI_H
#define _FMOD_OUTPUT_WASAPI_H

#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_WASAPI

#include "fmod_systemi.h"
#include "fmod_outputi.h"
#include "fmod_memorytracker.h"
#include "fmod_stringw.h"

// 0x0600 is Vista (safe since WASAPI output mode is only available on Vista)
#define _WIN32_WINNT 0x0601
#include <Windows.h>

#include "wasapi\fmod_audioclient.h"
#include "wasapi\fmod_mmdeviceapi.h"

namespace FMOD
{
	typedef struct FMOD_WASAPIDriver
	{
		LPWSTR   id;
		short   *name;
	} FMOD_WASAPIDriver;

    typedef struct FMOD_WASAPIRecordMembers
    {
        bool                      mRecordStereoToMono;
        IAudioClient             *mCaptureAudioClient;
        IAudioCaptureClient      *mCaptureClient;
        UINT                      mCaptureBufferLength;
        BYTE                     *mRecordBuffer;
        int                       mRecordBlockAlign;
        int                       mRecordWritePosition;
    } FMOD_WASAPIRecordMembers;

    class OutputWASAPI : public Output
    {
        DECLARE_MEMORYTRACKER_NONVIRTUAL

      private:

        /*
            General variables
        */
        bool                      mCoInitialized;
        bool                      mInitialised;
        bool                      mExclusiveMode;
          
        /*
            Render variables
        */
        int                       mNumRenderDrivers;
        FMOD_WASAPIDriver         mRenderDrivers[FMOD_OUTPUT_MAXDRIVERS];
        IAudioClient             *mRenderAudioClient;
        IAudioRenderClient       *mRenderClient;
        UINT                      mRenderBufferLength;
        int                       mRenderBlockAlign;
        FMOD_SOUND_FORMAT         mRenderFormat;
        int                       mRenderChannels;
        HANDLE                    mFeederEventHandle;
        bool                      mFeederThreadElevated;
        Thread                    mFeederThread;
        unsigned int              mFeederTimeout;
        Thread                    mMixerThread;
        BYTE                     *mMixerBuffer;
        int                       mMixerBufferLength;
        UINT                      mMixerBufferBlockSize;
        int                       mMixerBufferNumBlocks;
        int                       mMixerReadPosition;
        int                       mMixerWritePosition;
        DSPResampler             *mMixerResamplerDSP;
        float                    *mMixerFormatConversionBuffer;
        unsigned int              mMixerFormatConversionBufferSizeBytes;
        float                    *mMixerChannelConversionBuffer;
        unsigned int              mMixerChannelConversionBufferSizeBytes;

        /*
            Capture variables
        */
        int                       mNumCaptureDrivers;
        FMOD_WASAPIDriver         mCaptureDrivers[FMOD_OUTPUT_MAXDRIVERS];
#ifdef FMOD_SUPPORT_RECORDING
        HANDLE                    mRecordEventHandle;
        bool                      mRecordThreadElevated;
        Thread                    mRecordThread;

        bool                      mRecordInitialised;
#endif

        /*
            Private methods
        */
        FMOD_RESULT            enumerateDefaultDevices      (IMMDeviceEnumerator *pEnumerator);
        FMOD_RESULT            enumerate                    ();
        FMOD_RESULT            parseUUIDString              (WCHAR *srcString, FMOD_GUID *destGUID);
        FMOD_RESULT            feederUpdate                 ();       
        FMOD_RESULT            mixerUpdate                  ();
        FMOD_RESULT            channelConvert               (float *outbuffer, float *inbuffer, int outchannels, int inchannels, int length);
        FMOD_RESULT            cleanUpEnumeration           ();

#ifdef FMOD_SUPPORT_RECORDING
        FMOD_RESULT            recordUpdate                 ();
#endif

 static void                   feederThreadCallback         (void *userdata);
 static void                   mixerThreadCallback          (void *userdata);
 
#ifdef FMOD_SUPPORT_RECORDING
 static void                   recordThreadCallback         (void *userdata);
#endif

      public:
        
        static FMOD_OUTPUT_DESCRIPTION_EX *getDescriptionEx();  

        FMOD_RESULT            getNumDrivers                (int *numdrivers);
        FMOD_RESULT            getDriverInfo                (int id, char *name, int namelen, FMOD_GUID *guid);
        FMOD_RESULT            getDriverInfoW               (int id, short *name, int namelen, FMOD_GUID *guid);
        FMOD_RESULT            getDriverCapsEx              (int id, FMOD_CAPS *caps, int *minfrequency, int *maxfrequency, FMOD_SPEAKERMODE *controlpanelspeakermode);
        FMOD_RESULT            initEx                       (int selecteddriver, FMOD_INITFLAGS flags, int *outputrate, int outputchannels, FMOD_SOUND_FORMAT *outputformat, FMOD_SPEAKERMODE *speakermode, int dspbufferlength, int dspnumbuffers, int max2dchannels, int max3dchannels, void *extradriverdata);
	    FMOD_RESULT            close                        ();
        FMOD_RESULT            getHandle                    (void **handle);
        FMOD_RESULT            start                        ();
        FMOD_RESULT            stop                         ();
        FMOD_RESULT            mixerResampleRead            (float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int outchannels);

#ifdef FMOD_SUPPORT_RECORDING
        FMOD_RESULT            recordGetNumDrivers          (int *numdrivers);
        FMOD_RESULT            recordGetDriverInfo          (int id, char *name, int namelen, FMOD_GUID *guid);
        FMOD_RESULT            recordGetDriverInfoW         (int id, short *name, int namelen, FMOD_GUID *guid);
        FMOD_RESULT            recordGetDriverCaps          (int id, FMOD_CAPS *caps, int *minfrequency, int *maxfrequency);
        FMOD_RESULT            recordStart                  (FMOD_RECORDING_INFO *recordinfo, Sound *sound, bool loop);
        FMOD_RESULT            recordStop                   (FMOD_RECORDING_INFO *recordinfo);
        FMOD_RESULT            recordGetPosition            (FMOD_RECORDING_INFO *recordinfo, unsigned int *pcm);
        FMOD_RESULT            recordLock                   (FMOD_RECORDING_INFO *recordinfo, unsigned int offset, unsigned int length, void **ptr1, void **ptr2, unsigned int *len1, unsigned int *len2);
#endif

 static FMOD_RESULT F_CALLBACK getNumDriversCallback        (FMOD_OUTPUT_STATE *output, int *numdrivers);
 static FMOD_RESULT F_CALLBACK getDriverInfoCallback        (FMOD_OUTPUT_STATE *output, int id, char *name, int namelen, FMOD_GUID *guid);
 static FMOD_RESULT F_CALLBACK getDriverInfoWCallback       (FMOD_OUTPUT_STATE *output, int id, short *name, int namelen, FMOD_GUID *guid);
 static FMOD_RESULT F_CALLBACK getDriverCapsExCallback      (FMOD_OUTPUT_STATE *output, int id, FMOD_CAPS *caps, int *minfrequency, int *maxfrequency, FMOD_SPEAKERMODE *controlpanelspeakermode);
 static FMOD_RESULT F_CALLBACK initExCallback               (FMOD_OUTPUT_STATE *output, int selecteddriver, FMOD_INITFLAGS flags, int *outputrate, int outputchannels, FMOD_SOUND_FORMAT *outputformat, FMOD_SPEAKERMODE *speakermode, int dspbufferlength, int dspnumbuffers, int max2dchannels, int max3dchannels, void *extradriverdata);
 static FMOD_RESULT F_CALLBACK closeCallback                (FMOD_OUTPUT_STATE *output);
 static FMOD_RESULT F_CALLBACK startCallback                (FMOD_OUTPUT_STATE *output);
 static FMOD_RESULT F_CALLBACK stopCallback                 (FMOD_OUTPUT_STATE *output);
 static FMOD_RESULT F_CALLBACK getHandleCallback            (FMOD_OUTPUT_STATE *output, void **handle);
 static FMOD_RESULT F_CALLBACK mixerResampleReadCallback    (FMOD_DSP_STATE *dsp_state, float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int outchannels);

#ifdef FMOD_SUPPORT_RECORDING
 static FMOD_RESULT F_CALLBACK recordGetNumDriversCallback  (FMOD_OUTPUT_STATE *output, int *numdrivers);
 static FMOD_RESULT F_CALLBACK recordGetDriverInfoCallback  (FMOD_OUTPUT_STATE *output, int id, char *name, int namelen, FMOD_GUID *guid);
 static FMOD_RESULT F_CALLBACK recordGetDriverInfoWCallback (FMOD_OUTPUT_STATE *output, int id, short *name, int namelen, FMOD_GUID *guid);
 static FMOD_RESULT F_CALLBACK recordGetDriverCapsCallback  (FMOD_OUTPUT_STATE *output, int id, FMOD_CAPS *caps, int *minfrequency, int *maxfrequency);
 static FMOD_RESULT F_CALLBACK recordStartCallback          (FMOD_OUTPUT_STATE *output, FMOD_RECORDING_INFO *recordinfo, FMOD_SOUND *sound, int loop);
 static FMOD_RESULT F_CALLBACK recordStopCallback           (FMOD_OUTPUT_STATE *output, FMOD_RECORDING_INFO *recordinfo);
 static FMOD_RESULT F_CALLBACK recordGetPositionCallback    (FMOD_OUTPUT_STATE *output, FMOD_RECORDING_INFO *recordinfo, unsigned int *pcm);
 static FMOD_RESULT F_CALLBACK recordLockCallback           (FMOD_OUTPUT_STATE *output, FMOD_RECORDING_INFO *recordinfo, unsigned int offset, unsigned int length, void **ptr1, void **ptr2, unsigned int *len1, unsigned int *len2);
#endif

#ifdef FMOD_SUPPORT_MEMORYTRACKER
 static FMOD_RESULT F_CALLBACK getMemoryUsedCallback        (FMOD_OUTPUT_STATE *output, MemoryTracker *tracker);
#endif
    };
}

#endif  /* FMOD_SUPPORT_WASAPI */

#endif  /* _FMOD_OUTPUT_WASAPI_H */