#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_WINMM

#include "fmod.h"
#include "fmod_codec_wav.h"
#include "fmod_downmix.h"
#include "fmod_output_winmm.h"
#include "fmod_memory.h"
#include "fmod_soundi.h"
#include "fmod_stringw.h"
#include "fmod_systemi.h"

#include <windows.h>
#include <mmsystem.h>

namespace FMOD
{

static const FMOD_GUID KSDATAFORMAT_SUBTYPE_PCM        = { 0x00000001, 0x0000, 0x0010, { 0x80,0x00,0x00,0xaa,0x00,0x38,0x9b,0x71 } };
static const FMOD_GUID KSDATAFORMAT_SUBTYPE_IEEE_FLOAT = { 0x00000003, 0x0000, 0x0010, { 0x80,0x00,0x00,0xaa,0x00,0x38,0x9b,0x71 } };

FMOD_OUTPUT_DESCRIPTION_EX winmmoutput;

#ifdef PLUGIN_EXPORTS

#ifdef __cplusplus
extern "C" {
#endif

    /*
        FMODGetOutputDescription is mandantory for every fmod plugin.  This is the symbol the registerplugin function searches for.
        Must be declared with F_API to make it export as stdcall.
    */
    F_DECLSPEC F_DLLEXPORT FMOD_OUTPUT_DESCRIPTION_EX * F_API FMODGetOutputDescriptionEx()
    {
        return OutputWinMM::getDescriptionEx();
    }

#ifdef __cplusplus
}
#endif

#endif  /* PLUGIN_EXPORTS */


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_OUTPUT_DESCRIPTION_EX *OutputWinMM::getDescriptionEx()
{
    FMOD_memset(&winmmoutput, 0, sizeof(FMOD_OUTPUT_DESCRIPTION_EX));

    winmmoutput.name                    = "FMOD WinMM Output";
    winmmoutput.version                 = 0x00010100;
    winmmoutput.polling                 = true;
    winmmoutput.mType                   = FMOD_OUTPUTTYPE_WINMM;
    winmmoutput.mSize                   = sizeof(OutputWinMM);

    winmmoutput.getnumdrivers           = &OutputWinMM::getNumDriversCallback;
    winmmoutput.getdriverinfo           = &OutputWinMM::getDriverInfoCallback;
    winmmoutput.getdriverinfow          = &OutputWinMM::getDriverInfoWCallback;
    winmmoutput.getdrivercaps           = &OutputWinMM::getDriverCapsCallback;
    winmmoutput.init                    = &OutputWinMM::initCallback;
    winmmoutput.close                   = &OutputWinMM::closeCallback;
    winmmoutput.start                   = &OutputWinMM::startCallback;
    winmmoutput.stop                    = &OutputWinMM::stopCallback;
    winmmoutput.gethandle               = &OutputWinMM::getHandleCallback;
    winmmoutput.getposition             = &OutputWinMM::getPositionCallback;
    winmmoutput.lock                    = &OutputWinMM::lockCallback;

#ifdef FMOD_SUPPORT_RECORDING
    winmmoutput.record_getnumdrivers    = &OutputWinMM::recordGetNumDriversCallback;
    winmmoutput.record_getdriverinfo    = &OutputWinMM::recordGetDriverInfoCallback;
    winmmoutput.record_getdriverinfow   = &OutputWinMM::recordGetDriverInfoWCallback;
    winmmoutput.record_start            = &OutputWinMM::recordStartCallback;
    winmmoutput.record_stop             = &OutputWinMM::recordStopCallback;
    winmmoutput.record_getposition      = &OutputWinMM::recordGetPositionCallback;
    winmmoutput.record_lock             = &OutputWinMM::recordLockCallback;
    winmmoutput.record_unlock           = 0;
#endif

    return &winmmoutput;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWinMM::getNumDrivers(int *numdrivers)
{
    *numdrivers = waveOutGetNumDevs();

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWinMM::getDriverInfo(int id, char *name, int namelen, FMOD_GUID *guid)
{
	WAVEOUTCAPSA waveoutcaps;

    if (name && namelen >= 1)
    {
        waveOutGetDevCapsA(id, &waveoutcaps, sizeof(waveoutcaps));

        FMOD_strncpy(name, waveoutcaps.szPname, namelen);
        name[namelen - 1] = 0;
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWinMM::getDriverInfoW(int id, short *name, int namelen, FMOD_GUID *guid)
{    
	WAVEOUTCAPSW waveoutcaps;

    if (name && namelen >= 1)
    {
        waveOutGetDevCapsW(id, &waveoutcaps, sizeof(waveoutcaps));

        FMOD_strncpyW(name, (short *)waveoutcaps.szPname, namelen);
        name[namelen - 1] = 0;
    }	

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWinMM::testFormat(int id, FMOD_SOUND_FORMAT format, int channels)
{
    HWAVEOUT                handle;
	WAVE_FORMATEXTENSIBLE   pcmwf;
	UINT			        hr;
    int                     bits;

    SoundI::getBitsFromFormat(format, &bits);

    /*
        Initialize waveout.
    */
    FMOD_memset(&pcmwf,0,sizeof(WAVE_FORMATEXTENSIBLE));

    if (bits > 16 || channels > 2)
    {
        pcmwf.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
    }
    else
    {
        pcmwf.Format.wFormatTag = WAVE_FORMAT_PCM;
    }

    pcmwf.Format.nChannels            = channels;
    pcmwf.Format.wBitsPerSample       = bits;
    pcmwf.Format.nBlockAlign          = pcmwf.Format.nChannels * pcmwf.Format.wBitsPerSample / 8;
    pcmwf.Format.nSamplesPerSec       = 44100;
    pcmwf.Format.nAvgBytesPerSec	  = pcmwf.Format.nSamplesPerSec * pcmwf.Format.nBlockAlign;

    if (pcmwf.Format.wFormatTag == WAVE_FORMAT_EXTENSIBLE)
    {
        pcmwf.Format.cbSize               = 22;
        pcmwf.Samples.wValidBitsPerSample = bits;
        pcmwf.dwChannelMask               = 0;      /* FIXME - this should be set according to mulchaud.rtf */
        if (format == FMOD_SOUND_FORMAT_PCMFLOAT)
        {
            FMOD_memcpy(&pcmwf.SubFormat, &KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, sizeof(GUID));
        }
        else
        {
            FMOD_memcpy(&pcmwf.SubFormat, &KSDATAFORMAT_SUBTYPE_PCM, sizeof(GUID));
        }        
    }

    hr = waveOutOpen(&handle, id, (WAVEFORMATEX *)&pcmwf.Format, 0, 0, 0);
    
    waveOutClose(handle);

	if (hr)
    {
		return FMOD_ERR_FORMAT;
    }        

    return FMOD_OK;
}

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWinMM::getDriverCaps(int id, FMOD_CAPS *caps)
{
    if (testFormat(id, FMOD_SOUND_FORMAT_PCM16, 8) == FMOD_OK)
    {
        *caps = (FMOD_CAPS)(*caps | FMOD_CAPS_OUTPUT_MULTICHANNEL);
    }
    if (testFormat(id, FMOD_SOUND_FORMAT_PCM8, 2) == FMOD_OK)
    {
        *caps = (FMOD_CAPS)(*caps | FMOD_CAPS_OUTPUT_FORMAT_PCM8);
    }
    if (testFormat(id, FMOD_SOUND_FORMAT_PCM16, 2) == FMOD_OK)
    {
        *caps = (FMOD_CAPS)(*caps | FMOD_CAPS_OUTPUT_FORMAT_PCM16);
    }
    if (testFormat(id, FMOD_SOUND_FORMAT_PCM24, 2) == FMOD_OK)
    {
        *caps = (FMOD_CAPS)(*caps | FMOD_CAPS_OUTPUT_FORMAT_PCM24);
    }
    if (testFormat(id, FMOD_SOUND_FORMAT_PCM32, 2) == FMOD_OK)
    {
        *caps = (FMOD_CAPS)(*caps | FMOD_CAPS_OUTPUT_FORMAT_PCM32);
    }
    if (testFormat(id, FMOD_SOUND_FORMAT_PCMFLOAT, 2) == FMOD_OK)
    {
        *caps = (FMOD_CAPS)(*caps | FMOD_CAPS_OUTPUT_FORMAT_PCMFLOAT);
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWinMM::init(int selecteddriver, FMOD_INITFLAGS flags, int *outputrate, int outputchannels, FMOD_SOUND_FORMAT *outputformat, int dspbufferlength, int dspnumbuffers, void *extradriverdata)
{
	WAVE_FORMATEXTENSIBLE   pcmwf;
    int                     bits;
	UINT			        hr;
    
    Plugin::init(); /* Set up gGlobal - for debug / file / memory access by this plugin. */

	FLOG((FMOD_DEBUG_LEVEL_ALL, __FILE__, __LINE__, "OutputWinMM::init", "Initializing.\n"));

	if (!mCoInitialized)
	{
		hr = CoInitialize(NULL);
        if (hr == S_OK || S_FALSE)
        {
		    mCoInitialized = true;
        }
	}

    SoundI::getBitsFromFormat(*outputformat, &bits);

    /*
        Initialize waveout.
    */
    FMOD_memset(&pcmwf,0,sizeof(WAVE_FORMATEXTENSIBLE));
    if (bits > 16 || outputchannels > 2)
    {
        pcmwf.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
    }
    else
    {
        pcmwf.Format.wFormatTag = WAVE_FORMAT_PCM;
    }

    pcmwf.Format.nChannels            = outputchannels;
    pcmwf.Format.wBitsPerSample       = bits;
    pcmwf.Format.nBlockAlign          = pcmwf.Format.nChannels * pcmwf.Format.wBitsPerSample / 8;
    pcmwf.Format.nSamplesPerSec       = *outputrate;
    pcmwf.Format.nAvgBytesPerSec	  = pcmwf.Format.nSamplesPerSec * pcmwf.Format.nBlockAlign;

    if (pcmwf.Format.wFormatTag == WAVE_FORMAT_EXTENSIBLE)
    {
        pcmwf.Format.cbSize               = 22;
        pcmwf.Samples.wValidBitsPerSample = bits;
        pcmwf.dwChannelMask               = 0;      /* FIXME - this should be set according to mulchaud.rtf */
        if (*outputformat == FMOD_SOUND_FORMAT_PCMFLOAT)
        {
            FMOD_memcpy(&pcmwf.SubFormat, &KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, sizeof(GUID));
        }
        else
        {
            FMOD_memcpy(&pcmwf.SubFormat, &KSDATAFORMAT_SUBTYPE_PCM, sizeof(GUID));
        }        
    }

    hr = waveOutOpen(&mHandle, selecteddriver, (WAVEFORMATEX *)&pcmwf.Format, 0, 0, 0);
	if (hr) 
	{
#ifdef FMOD_DEBUG
		switch (hr) 
		{
			case MMSYSERR_BADDEVICEID :
				FLOG((FMOD_DEBUG_LEVEL_ALL, __FILE__, __LINE__, "OutputWinMM::init", "Specified device ID is out of range.\n"));
				break;
			case MMSYSERR_ALLOCATED :
				FLOG((FMOD_DEBUG_LEVEL_ALL, __FILE__, __LINE__, "OutputWinMM::init", "Specified resource is already allocated.\n"));
				break;
			case MMSYSERR_NOMEM :
				FLOG((FMOD_DEBUG_LEVEL_ALL, __FILE__, __LINE__, "OutputWinMM::init", "Unable to allocate or lock memory.\n"));
				break;
			case WAVERR_BADFORMAT :
				FLOG((FMOD_DEBUG_LEVEL_ALL, __FILE__, __LINE__, "OutputWinMM::init", "Attempted to open with an unsupported wave format.\n"));
				break;
			case WAVERR_SYNC :
				FLOG((FMOD_DEBUG_LEVEL_ALL, __FILE__, __LINE__, "OutputWinMM::init", "Attempted to open a synchronous driver without specifying the WAVE_ALLOWSYNC flag.\n"));
				break;
			default :
				FLOG((FMOD_DEBUG_LEVEL_ALL, __FILE__, __LINE__, "OutputWinMM::init", "Unknown Error trying to open wave device!.\n"));
				break;
		};
#endif
		if (hr == WAVERR_BADFORMAT)
        {
			return FMOD_ERR_FORMAT;
        }        
		if (hr == MMSYSERR_ALLOCATED)
        {
			return FMOD_ERR_OUTPUT_ALLOCATED;
        }

        return FMOD_ERR_OUTPUT_INIT;
	}
    else
    {
        FLOG((FMOD_DEBUG_LEVEL_ALL, __FILE__, __LINE__, "OutputWinMM::init", "Opened wave device successfully\n"));
    }


	/*
        CREATE AND START LOOPING WAVEOUT BLOCK
    */
    {
        FMOD_RESULT  result;
	    WAVEHDR	    *wavehdr;
        HRESULT      hr;
	    int 	     count, loops;
        unsigned int bufferlength;
        int          numbuffers;
        unsigned int mode;

        result = mSystem->getDSPBufferSize(&bufferlength, &numbuffers);
        if (result != FMOD_OK)
        {
            return result;
        }

        mode        = WHDR_BEGINLOOP | WHDR_ENDLOOP;
        SoundI::getBytesFromSamples(bufferlength * numbuffers, &mBlockLengthBytes, outputchannels, *outputformat);
        loops       = -1;
        mNumBlocks  = 1;
        
        mBuffer = (char *)FMOD_Memory_Calloc(mBlockLengthBytes);
        if (!mBuffer)
        {
            return FMOD_ERR_MEMORY;
        }

        /*
            Allocate and set up waveout blocks
        */
	    for (count = 0; count < mNumBlocks; count++)
	    {
		    wavehdr = &mBlock[count].wavehdr;

		    mBlock[count].data = (char *)mBuffer + (mBlockLengthBytes / mNumBlocks * count);
		    
		    wavehdr->dwFlags			= mode;
		    wavehdr->lpData				= (LPSTR)mBlock[count].data;
		    wavehdr->dwBufferLength		= mBlockLengthBytes / mNumBlocks;
		    wavehdr->dwBytesRecorded	= 0;
		    wavehdr->dwUser				= count;
		    wavehdr->dwLoops			= loops;

	        hr = waveOutPrepareHeader(mHandle, wavehdr, sizeof(WAVEHDR));
	        if (hr)
	        {
		        mRunning = false;
		        return FMOD_ERR_OUTPUT_CREATEBUFFER;
	        }
	    }
    }

    FLOG((FMOD_DEBUG_LEVEL_ALL, __FILE__, __LINE__, "OutputWinMM::init", "Done.\n"));

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWinMM::close()
{
    int count;

    Plugin::init(); /* Set up gGlobal - for debug / file / memory access by this plugin. */

	mRunning = false;

    if (mNumBlocks)
    {
	    for (count = 0; count < mNumBlocks; count++)
	    {
            waveOutUnprepareHeader(mHandle, &mBlock[count].wavehdr, sizeof(WAVEHDR));
            mBlock[count].wavehdr.dwFlags &= ~WHDR_PREPARED;
    	    mBlock[count].data = NULL;
        }
        mNumBlocks = 0;
    }

    if (mBuffer)
    {
        FMOD_Memory_Free(mBuffer);
    }
	mBuffer = NULL;

	waveOutClose(mHandle);

    if (mCoInitialized)
    {
        CoUninitialize();
        mCoInitialized = false;
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWinMM::getHandle(void **handle)
{
    *handle = mHandle;

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWinMM::start()
{
    FMOD_RESULT result;
    MMRESULT    hr;
	WAVEHDR	   *wavehdr;
    int         count;
    int         totalblocks;

    result = OutputPolled::start();
    if (result != FMOD_OK)
    {
        return result;
    }

    totalblocks = mNumBlocks;

    mRunning = true;

	for (count=0; count < totalblocks; count++)
	{
		wavehdr = &mBlock[count].wavehdr;

	    hr = waveOutWrite(mHandle, wavehdr, sizeof(WAVEHDR));
	    if (hr)
	    {
		    mRunning = false;
		    return FMOD_ERR_OUTPUT_DRIVERCALL;
	    }
	}

    
    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWinMM::stop()
{
    FMOD_RESULT result;

    result = OutputPolled::stop();
    if (result != FMOD_OK)
    {
        return result;
    }

	mRunning = false;

	waveOutReset(mHandle);

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWinMM::getPosition(unsigned int *pcm)
{
    FMOD_RESULT         result;
	MMTIME              mmt;
    int                 bits;
    int                 outputrate;
    int                 outputchannels;
    FMOD_SOUND_FORMAT   outputformat;

	mmt.wType = TIME_BYTES;
	waveOutGetPosition(mHandle, &mmt, sizeof(MMTIME));

    /*
        Get the values FMOD had set for rate, format, channels.
    */
    result = mSystem->getSoftwareFormat(&outputrate, &outputformat, &outputchannels, 0, 0, &bits);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (mSystem->mDownmix)
    {
        mSystem->mDownmix->getOutputChannels(&outputchannels);
    }

    mmt.u.cb = mmt.u.cb * 8 / bits;
    mmt.u.cb /= outputchannels;

	*pcm = mmt.u.cb;

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWinMM::lock(unsigned int offset, unsigned int length, void **ptr1, void **ptr2, unsigned int *len1, unsigned int *len2)
{
	offset %= mBlockLengthBytes;

	if (offset + length > mBlockLengthBytes)
	{
		*ptr1 = (char *)mBuffer + offset;
		*ptr2 = (char *)mBuffer;
		*len1 = mBlockLengthBytes - offset;
		*len2 = length - (mBlockLengthBytes - offset);
	}
	else
	{
		*ptr1 = (char *)mBuffer + offset;
		*ptr2 = NULL;
		*len1 = length;
		*len2 = 0;
	}

    return FMOD_OK;
}



#ifdef FMOD_SUPPORT_RECORDING
/*
[
    [DESCRIPTION]

    [PARAMETERS]

    [RETURN_VALUE]

    [REMARKS]

    [SEE_ALSO]
]
*/
FMOD_RESULT OutputWinMM::recordThread()
{
    if (!mRecording)
    {
        return FMOD_OK;
    }
    
    while (mRecordCurrentBlock < mRecordNextBlock)
    {
        unsigned int fillblock;
        int lastblock;

        fillblock = mRecordCurrentBlock % RECORD_MAXBLOCKS;

		waveInUnprepareHeader(mRecordHandle, &mRecordBlock[fillblock].wavehdr, sizeof(WAVEHDR));

        /*
            Prepare the last block to be added.  It needs to be at least a triple buffer for this to work.
        */
        lastblock = fillblock - 1;
        if (lastblock < 0)
        {
            lastblock = RECORD_MAXBLOCKS - 1;
        }

		waveInPrepareHeader(mRecordHandle, &mRecordBlock[lastblock].wavehdr, sizeof(WAVEHDR));
		waveInAddBuffer    (mRecordHandle, &mRecordBlock[lastblock].wavehdr, sizeof(WAVEHDR));

		mRecordCurrentBlock++;
    }

    return FMOD_OK;
}


/*
[
    [DESCRIPTION]

    [PARAMETERS]

    [RETURN_VALUE]

    [REMARKS]

    [SEE_ALSO]
]
*/
void OutputWinMM::recordThreadCallback(void *data)
{
    OutputWinMM *winmm = (OutputWinMM *)data;

    winmm->recordThread();
}


/*
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
*/
void CALLBACK OutputWinMM::recordCallback(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
    OutputWinMM *winmm = (OutputWinMM *)dwInstance;

	if (!winmm->mRecording)
    {
		return;
    }

	if (uMsg == WIM_DATA)
	{
        winmm->mRecordNextBlock++;
        winmm->mRecordThread.wakeupThread();
	}
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWinMM::recordGetNumDrivers(int *numdrivers)
{
    if (!numdrivers)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *numdrivers = waveInGetNumDevs();

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]
    WinMM does not provide GUIDs

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWinMM::recordGetDriverInfo(int id, char *name, int namelen, FMOD_GUID *guid)
{
    WAVEINCAPSA waveincaps;

    if (name && namelen >= 1)
    {
        waveInGetDevCapsA(id, &waveincaps, sizeof(waveincaps));
        
	    FMOD_strncpy(name, waveincaps.szPname, namelen - 1);
        name[namelen - 1] = 0;
    }

    return FMOD_OK;
}

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]
    WinMM does not provide GUIDs

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWinMM::recordGetDriverInfoW(int id, short *name, int namelen, FMOD_GUID *guid)
{
    WAVEINCAPSW waveincaps;

    if (name && namelen >= 1)
    {
        waveInGetDevCapsW(id, &waveincaps, sizeof(waveincaps));
        
        FMOD_strncpyW(name, (short *)waveincaps.szPname, namelen - 1);
        name[namelen - 1] = 0;
    }	

    return FMOD_OK;
}

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWinMM::recordStart(FMOD_RECORDING_INFO *recordinfo, Sound *sound, bool loop)
{
    FMOD_RESULT             result;
	MMRESULT		        hr;
    WAVE_FORMATEXTENSIBLE   wfx;
	WAVEHDR			       *wavehdr;
    FMOD_MODE               mode;
	int				        count, bits;
    SoundI                 *soundi;

    soundi = (SoundI *)sound;
    if (!soundi)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

	if (soundi->mLength < 1)
	{
		return FMOD_ERR_INVALID_PARAM;
	}

    /*
        No multi recording with WinMM
    */
    if (mRecordNumActive)
    {
        return FMOD_ERR_UNSUPPORTED;
    }


 	// ========================================================================================================
	// CREATE AND START CAPTURE BUFFER
	// ========================================================================================================

    recordinfo->mRecordFormat       = soundi->mFormat;
    recordinfo->mRecordChannels     = soundi->mChannels;
    mode                            = soundi->mMode;
    recordinfo->mRecordRate         = (int)soundi->mDefaultFrequency;

    SoundI::getBitsFromFormat(recordinfo->mRecordFormat, &bits);

    FMOD_memset(&wfx, 0, sizeof(WAVE_FORMATEXTENSIBLE));
    wfx.Format.wFormatTag            = WAVE_FORMAT_EXTENSIBLE;
    wfx.Format.nChannels             = recordinfo->mRecordChannels;
    wfx.Format.wBitsPerSample        = bits;
    wfx.Format.nBlockAlign           = wfx.Format.nChannels * wfx.Format.wBitsPerSample / 8;
    wfx.Format.nSamplesPerSec        = recordinfo->mRecordRate;
    wfx.Format.nAvgBytesPerSec	     = wfx.Format.nSamplesPerSec * wfx.Format.nBlockAlign;
    wfx.Format.cbSize                = 22;  // Designates extra data
    wfx.Samples.wValidBitsPerSample  = bits;
    FMOD_memcpy(&wfx.SubFormat, (soundi->mFormat == FMOD_SOUND_FORMAT_PCMFLOAT ? &KSDATAFORMAT_SUBTYPE_IEEE_FLOAT : &KSDATAFORMAT_SUBTYPE_PCM), sizeof(GUID));

	mRecordBlockAlign = wfx.Format.nBlockAlign;

    mRecordBlockLenBytes  = RECORD_BLOCKLENMS;
    mRecordBlockLenBytes *= recordinfo->mRecordRate;
    mRecordBlockLenBytes /= 1000;

    recordinfo->mRecordBufferLength = mRecordBlockLenBytes * RECORD_MAXBLOCKS;

    mRecordBlockLenBytes *= mRecordBlockAlign;
    
	hr = waveInOpen(&mRecordHandle, recordinfo->mRecordId, (WAVEFORMATEX*)&wfx, (unsigned long)&recordCallback, (DWORD)this, CALLBACK_FUNCTION);
	if (hr) 
	{
		switch (hr) 
		{
			case MMSYSERR_BADDEVICEID :
				FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "OutputWinMM::recordStart", "Specified device ID is out of range.\n"));
				break;
			case MMSYSERR_ALLOCATED :
				FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "OutputWinMM::recordStart", "Specified resource is already allocated.\n"));
				break;
			case MMSYSERR_NOMEM :
				FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "OutputWinMM::recordStart", "Unable to allocate or lock memory.\n"));
				break;
			case WAVERR_BADFORMAT :
				FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "OutputWinMM::recordStart", "Attempted to open with an unsupported wave format.\n"));
				break;
			case WAVERR_SYNC :
				FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "OutputWinMM::recordStart", "Attempted to open a synchronous driver without specifying the WAVE_ALLOWSYNC flag.\n"));
				break;
			default :
				FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "OutputWinMM::recordStart", "Unknown Error trying to open wave device!."));
				break;
		};

		if (hr == MMSYSERR_ALLOCATED)
        {
			return FMOD_ERR_OUTPUT_ALLOCATED;
        }
		else if (hr == WAVERR_BADFORMAT)
        {
            return FMOD_ERR_OUTPUT_FORMAT;
        }
        else
        {
			return FMOD_ERR_OUTPUT_CREATEBUFFER;
        }
	}

    /*
        Allocate and set up wavein blocks
    */

    mRecordData = (char *)FMOD_Memory_Calloc(mRecordBlockLenBytes * RECORD_MAXBLOCKS);

	for (count=0; count < RECORD_MAXBLOCKS; count++)
	{
		wavehdr = &mRecordBlock[count].wavehdr;

		mRecordBlock[count].data = mRecordData + (mRecordBlockLenBytes * count);
		
		wavehdr->dwFlags			= 0;
		wavehdr->lpData				= (LPSTR)mRecordBlock[count].data;
		wavehdr->dwBufferLength		= mRecordBlockLenBytes;
		wavehdr->dwBytesRecorded	= mRecordBlockLenBytes;
		wavehdr->dwUser				= count;
		wavehdr->dwLoops			= 0;
	}

	for (count=0; count < RECORD_MAXBLOCKS - 1; count++)
	{
		wavehdr = &mRecordBlock[count].wavehdr;

	    hr = waveInPrepareHeader(mRecordHandle, wavehdr, sizeof(WAVEHDR));
	    if (hr)
	    {
		    mRecording = false;
		    return FMOD_ERR_OUTPUT_CREATEBUFFER;
	    }
	    hr = waveInAddBuffer(mRecordHandle, wavehdr, sizeof(WAVEHDR));
	    if (hr)
	    {
		    mRecording = false;
		    return FMOD_ERR_OUTPUT_CREATEBUFFER;
	    }
	}

	mRecordCurrentBlock = 0;
    mRecordNextBlock = 0;
	mRecording = true;

    result = mRecordThread.initThread("FMOD Record thread", recordThreadCallback, this, Thread::PRIORITY_CRITICAL, 0, 0, true, 0, mSystem);
    if (result != FMOD_OK)
    {
        return result;
    }

	hr = waveInStart(mRecordHandle);
	if (hr)
	{
		mRecording = false;
		return FMOD_ERR_RECORD;
	}

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWinMM::recordStop(FMOD_RECORDING_INFO *recordinfo)
{
	int count;

	mRecording = FALSE;

    mRecordThread.closeThread();
            
	waveInReset(mRecordHandle);
	waveInClose(mRecordHandle);

	for (count = 0; count < RECORD_MAXBLOCKS; count++)
	{
		waveInUnprepareHeader(mRecordHandle, &mRecordBlock[count].wavehdr, sizeof(WAVEHDR));
		mRecordBlock[count].data = NULL;
	}

	if (mRecordData)
	{
		FMOD_Memory_Free(mRecordData);
		mRecordData = NULL;
	}

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWinMM::recordGetPosition(FMOD_RECORDING_INFO *recordinfo, unsigned int *pcm)
{
    int currblock;

    if (!pcm)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

	if (!mRecordBlockAlign)
    {
        return FMOD_ERR_INTERNAL;
    }

    currblock = mRecordCurrentBlock % RECORD_MAXBLOCKS;

	*pcm = currblock * mRecordBlockLenBytes / mRecordBlockAlign;

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWinMM::recordLock(FMOD_RECORDING_INFO *recordinfo, unsigned int offset, unsigned int length, void **ptr1, void **ptr2, unsigned int *len1, unsigned int *len2)
{
	unsigned int lenbytes = mRecordBlockLenBytes * RECORD_MAXBLOCKS;

	/*
        Cant lock more than the sample length
    */
	if (length > lenbytes) 
    {
		length = lenbytes;
    }

	if (offset >= lenbytes || offset < 0 || length < 0) 
	{
		*ptr1 = NULL;
		*ptr2 = NULL;
		*len1 = 0;
		*len2 = 0;
		return FMOD_ERR_INVALID_PARAM;
	}

	/*
        If it is not a wrapping lock just fill out ptr1
    */
	if (offset + length <= lenbytes)
	{
		*ptr1 = ((char *)mRecordData + offset);
		*len1 = length;
		*ptr2 = NULL;
		*len2 = 0;
	}

	/*
        Otherwise return wrapped pointers in pt1 and ptr2
    */
	else
	{
		*ptr1 = ((char *)mRecordData + offset);
		*len1 = lenbytes - offset;
		*ptr2 = mRecordData;
		*len2 = length - (lenbytes - offset);
	}

    return FMOD_OK;
}
#endif


/*
    ==============================================================================================================

    CALLBACK INTERFACE

    ==============================================================================================================
*/


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK OutputWinMM::getNumDriversCallback(FMOD_OUTPUT_STATE *output, int *numdrivers)
{
    OutputWinMM *winmm = (OutputWinMM *)output;

    return winmm->getNumDrivers(numdrivers);
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK OutputWinMM::getDriverInfoCallback(FMOD_OUTPUT_STATE *output, int id, char *name, int namelen, FMOD_GUID *guid)
{
    OutputWinMM *winmm = (OutputWinMM *)output;

    return winmm->getDriverInfo(id, name, namelen, guid);
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK OutputWinMM::getDriverInfoWCallback(FMOD_OUTPUT_STATE *output, int id, short *name, int namelen, FMOD_GUID *guid)
{
    OutputWinMM *winmm = (OutputWinMM *)output;

    return winmm->getDriverInfoW(id, name, namelen, guid);
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT OutputWinMM::getDriverCapsCallback(FMOD_OUTPUT_STATE *output, int id, FMOD_CAPS *caps)
{
    OutputWinMM *winmm = (OutputWinMM *)output;

    return winmm->getDriverCaps(id, caps);
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK OutputWinMM::initCallback(FMOD_OUTPUT_STATE *output, int selecteddriver, FMOD_INITFLAGS flags, int *outputrate, int outputchannels, FMOD_SOUND_FORMAT *outputformat, int dspbufferlength, int dspnumbuffers, void *extradriverdata)
{
    OutputWinMM *winmm = (OutputWinMM *)output;

    return winmm->init(selecteddriver, flags, outputrate, outputchannels, outputformat, dspbufferlength, dspnumbuffers, extradriverdata);
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK OutputWinMM::closeCallback(FMOD_OUTPUT_STATE *output)
{
    OutputWinMM *winmm = (OutputWinMM *)output;

    return winmm->close();
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK OutputWinMM::startCallback(FMOD_OUTPUT_STATE *output)
{
    OutputWinMM *winmm = (OutputWinMM *)output;

    return winmm->start();
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK OutputWinMM::stopCallback(FMOD_OUTPUT_STATE *output)
{
    OutputWinMM *winmm = (OutputWinMM *)output;

    return winmm->stop();
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK OutputWinMM::getHandleCallback(FMOD_OUTPUT_STATE *output, void **handle)
{
    OutputWinMM *winmm = (OutputWinMM *)output;

    return winmm->getHandle(handle);
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK OutputWinMM::getPositionCallback(FMOD_OUTPUT_STATE *output, unsigned int *pcm)
{
    OutputWinMM *winmm = (OutputWinMM *)output;

    return winmm->getPosition(pcm);
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK OutputWinMM::lockCallback(FMOD_OUTPUT_STATE *output, unsigned int offset, unsigned int length, void **ptr1, void **ptr2, unsigned int *len1, unsigned int *len2)
{
    OutputWinMM *winmm = (OutputWinMM *)output;

    return winmm->lock(offset, length, ptr1, ptr2, len1, len2);
}

#ifdef FMOD_SUPPORT_RECORDING
/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK OutputWinMM::recordGetNumDriversCallback(FMOD_OUTPUT_STATE *output, int *numdrivers)
{
    OutputWinMM *winmm = (OutputWinMM *)output;

    return winmm->recordGetNumDrivers(numdrivers);
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK OutputWinMM::recordGetDriverInfoCallback(FMOD_OUTPUT_STATE *output, int id, char *name, int namelen, FMOD_GUID *guid)
{
    OutputWinMM *winmm = (OutputWinMM *)output;

    return winmm->recordGetDriverInfo(id, name, namelen, guid);
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK OutputWinMM::recordGetDriverInfoWCallback(FMOD_OUTPUT_STATE *output, int id, short *name, int namelen, FMOD_GUID *guid)
{
    OutputWinMM *winmm = (OutputWinMM *)output;

    return winmm->recordGetDriverInfoW(id, name, namelen, guid);
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK OutputWinMM::recordStartCallback(FMOD_OUTPUT_STATE *output, FMOD_RECORDING_INFO *recordinfo, FMOD_SOUND *sound, int loop)
{
    OutputWinMM *winmm = (OutputWinMM *)output;

    return winmm->recordStart(recordinfo, (Sound *)sound, loop ? true : false);
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK OutputWinMM::recordStopCallback(FMOD_OUTPUT_STATE *output, FMOD_RECORDING_INFO *recordinfo)
{
    OutputWinMM *winmm = (OutputWinMM *)output;

    return winmm->recordStop(recordinfo);
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK OutputWinMM::recordGetPositionCallback(FMOD_OUTPUT_STATE *output, FMOD_RECORDING_INFO *recordinfo, unsigned int *pcm)
{
    OutputWinMM *winmm = (OutputWinMM *)output;

    return winmm->recordGetPosition(recordinfo, pcm);
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64

	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK OutputWinMM::recordLockCallback(FMOD_OUTPUT_STATE *output, FMOD_RECORDING_INFO *recordinfo, unsigned int offset, unsigned int length, void **ptr1, void **ptr2, unsigned int *len1, unsigned int *len2)
{
    OutputWinMM *winmm = (OutputWinMM *)output;

    return winmm->recordLock(recordinfo, offset, length, ptr1, ptr2, len1, len2);
}
#endif

}

#endif  /* #ifdef FMOD_SUPPORT_WINMM */
