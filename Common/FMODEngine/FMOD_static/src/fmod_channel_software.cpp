#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_SOFTWARE

#include "fmod_3d.h"
#include "fmod_channel_software.h"
#include "fmod_codeci.h"
#ifdef FMOD_SUPPORT_FSB
#include "fmod_codec_fsb.h"
#endif
#ifdef FMOD_SUPPORT_MPEG
#include "fmod_codec_mpeg.h"
#endif
#ifdef FMOD_SUPPORT_XMA
#include "fmod_codec_xma.h"
#endif
#if defined(FMOD_SUPPORT_WAV) && defined(FMOD_SUPPORT_IMAADPCM)
#include "fmod_codec_wav.h"
#include "fmod_codec_wav_imaadpcm.h"
#endif
#include "fmod_dsp_codec.h"
#include "fmod_dsp_fft.h"
#include "fmod_dsp_resampler.h"
#include "fmod_dsp_wavetable.h"
#include "fmod_dspi.h"
#include "fmod_sample_software.h"
#include "fmod_speakermap.h"
#include "fmod_systemi.h"

namespace FMOD
{

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
ChannelSoftware::ChannelSoftware()
{
    mDSPWaveTable     = 0;
    mDSPHead          = 0;
    mDSPResampler     = 0;
    mDSPCodec         = 0;
    mDSPLowPass       = 0;
    mDSP              = 0;
    mDSPConnection    = 0;
    mDSPReverb        = 0;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::init(int index, SystemI *system, Output *output, DSPI *dspmixtarget)
{
    FMOD_RESULT             result;
    FMOD_DSP_DESCRIPTION_EX descriptionex;

    ChannelReal::init(index, system, output, dspmixtarget);
   
    /*
        Create a head unit that things can connect to.
    */
    FMOD_memset(&descriptionex, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));
    FMOD_strcpy(descriptionex.name, "FMOD Channel DSPHead Unit");
    descriptionex.version   = 0x00010100;
    descriptionex.mCategory = FMOD_DSP_CATEGORY_FILTER;
    descriptionex.mFormat   = FMOD_SOUND_FORMAT_PCMFLOAT;

    #ifdef PLATFORM_PS3
    mDSPHead = (DSPI *)FMOD_ALIGNPOINTER(&mDSPHeadMemory, 16);
    mDSPHead = new (mDSPHead) (DSPFilter);    
    #else
    mDSPHead = &mDSPHeadMemory;
    #endif

    result = mSystem->createDSP(&descriptionex, &mDSPHead, false);
    if (result != FMOD_OK)
    {
        return result;
    }

    #ifdef PLATFORM_PS3
    mDSPHead->mMramAddress       = (unsigned int)mDSPHead;
    mDSPHead->mDescription.mSize = (sizeof(DSPFilter) + 15) & ~15;
    #endif

    /*
        Create a lowpass unit 
    */
    if (mSystem->mFlags & FMOD_INIT_SOFTWARE_OCCLUSION || mSystem->mFlags & FMOD_INIT_SOFTWARE_HRTF)
    {
        result = mSystem->createDSPByType(FMOD_DSP_TYPE_LOWPASS_SIMPLE, &mDSPLowPass);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    /*
        Create a wave table unit
    */
    FMOD_memset(&descriptionex, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));
    FMOD_strcpy(descriptionex.name, "FMOD WaveTable Unit");
    descriptionex.version       = 0x00010100;
    descriptionex.channels      = dspmixtarget->mDescription.channels;
    descriptionex.read          = 0; /* DSPWavetable uses DSPWaveTable::execute not a read callback. */

    descriptionex.setparameter  = DSPWaveTable::setParameterCallback;
    descriptionex.getparameter  = DSPWaveTable::getParameterCallback;
    descriptionex.setposition   = DSPWaveTable::setPositionCallback;
    descriptionex.reset         = DSPWaveTable::resetCallback;

    descriptionex.mCategory     = FMOD_DSP_CATEGORY_WAVETABLE;
    descriptionex.mFormat       = dspmixtarget->mDescription.mFormat;
    descriptionex.mDSPSoundCard = dspmixtarget;

    mDSPWaveTable = &mDSPWaveTableMemory;

    result = mSystem->createDSP(&descriptionex, (DSPI **)&mDSPWaveTable, false);
    if (result != FMOD_OK)
    {
        return result;
    }
    mDSPWaveTable->setFinished(true, true);

    result = mDSPWaveTable->setUserData((void **)this);
    if (result != FMOD_OK)
    {
        return result;
    }
    result = mDSPWaveTable->setTargetFrequency((int)dspmixtarget->mDefaultFrequency);
    if (result != FMOD_OK)
    {
        return result;
    }

    mMinFrequency = -mMaxFrequency;

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::close()
{
    FMOD_RESULT result;

    if (mDSPWaveTable)
    {
        mDSPWaveTable->mDSPSoundCard = 0;
    }

    if (mDSPResampler)
    {
        mDSPResampler->mDSPSoundCard = 0;
    }

    result = ChannelReal::close();
    if (result != FMOD_OK)
    {
        return result;
    }

    if (mDSPWaveTable)
    {
        mDSPWaveTable->release(false);  /* false = dont free this, as it is not alloced. */
        mDSPWaveTable = 0;
    }

    if (mDSPHead)
    {
        mDSPHead->release(false);       /* false = dont free this, as it is not alloced. */
        mDSPHead= 0;
    }

    if (mDSPResampler)
    {
        mDSPResampler->release();
        mDSPResampler = 0;
    }

    if (mDSPLowPass)
    {
        mDSPLowPass->release();
        mDSPLowPass = 0;
    }

    mDSPCodec = 0;  /* Don't free, it points to a pool codec. */

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::setupDSPCodec(DSPI *dsp)
{
#ifdef FMOD_SUPPORT_DSPCODEC
    FMOD_RESULT result;
    DSPCodec       *dspcodec;
    Codec          *codec, *soundcodec;
    SampleSoftware *sample;
    
#ifdef FMOD_SUPPORT_SENTENCING
    if (mSound->mSubSoundList)
    {
        sample = SAFE_CAST(SampleSoftware, mSound->mSubSound[mSound->mSubSoundList[mSubSoundListCurrent].mIndex]);
    }
    else
#endif
    {
        sample = SAFE_CAST(SampleSoftware, mSound);
    }

    dspcodec   = SAFE_CAST(DSPCodec, dsp);
    codec      = SAFE_CAST(Codec, dspcodec->mCodec);
    soundcodec = sample->mCodec ? sample->mCodec : sample->mSubSoundParent->mCodec;

    if (!soundcodec)
    {
        return FMOD_ERR_INTERNAL;
    }

    #ifdef PLATFORM_PS3
    /*  These need to be 0 so Codec::read doesn't try and read into mPCMBuffer on the SPU. */
    codec->mPCMBufferLength      = 0; //soundcodec->mPCMBufferLength;
    codec->mPCMBufferLengthBytes = 0; //soundcodec->mPCMBufferLength * sizeof(short) * dsp->mDescription.channels;
    #else
    codec->mPCMBufferLength      = soundcodec->mPCMBufferLength;
    codec->mPCMBufferLengthBytes = soundcodec->mPCMBufferLength * sizeof(short) * dsp->mDescription.channels;
    #endif

    if (!codec->waveformat)
    {
        return FMOD_ERR_INTERNAL;
    }

    soundcodec->mDescription.getwaveformat(soundcodec, sample->mSubSoundIndex, codec->waveformat);

    codec->mFlags = soundcodec->mFlags;
    dspcodec->mNewPosition = 0xFFFFFFFF;
    dspcodec->mNoDMA->mNewPosition = 0xFFFFFFFF;
    dspcodec->mCodec->mFile = &dspcodec->mMemoryFile;

    #ifdef PLATFORM_PS3
    dspcodec->mMemoryFile.init(mSystem, sample->mLengthBytes, PS3_DMAFILE_BLOCKSIZE);
    dspcodec->mMemoryFile.mFlags |= FMOD_FILE_BIGENDIAN;
    #else
    dspcodec->mMemoryFile.init(mSystem, sample->mLengthBytes, 0);
    #endif

    dspcodec->mMemoryFile.mPosition = 0;    
    dspcodec->mMemoryFile.mMem = sample->mBuffer;

    /*
        Copy codec specific things from the source sound to the dspcodec used in this channel.
    */
    if (0)
    {
    }
    #if defined(FMOD_SUPPORT_WAV) && defined(FMOD_SUPPORT_IMAADPCM)
    if (sample->mType == FMOD_SOUND_TYPE_WAV && sample->mFormat == FMOD_SOUND_FORMAT_IMAADPCM)
    {
        CodecWav *srcwav  = (CodecWav *)soundcodec;
        CodecWav *destwav = (CodecWav *)codec;

        destwav->mSamplesPerADPCMBlock = srcwav->mSamplesPerADPCMBlock;
        destwav->mReadBufferLength     = srcwav->mReadBufferLength;
    }
    #endif
    #if defined(FMOD_SUPPORT_WAV) && defined(FMOD_SUPPORT_RAW) && defined(FMOD_SUPPORT_IMAADPCM)
    else if (sample->mType == FMOD_SOUND_TYPE_RAW && sample->mFormat == FMOD_SOUND_FORMAT_IMAADPCM)
    {
        CodecRaw *srcwav  = (CodecRaw *)soundcodec;
        CodecWav *destwav = (CodecWav *)codec;

        destwav->mSamplesPerADPCMBlock = srcwav->mSamplesPerADPCMBlock;
        destwav->mReadBufferLength     = srcwav->mReadBufferLength;
    }
    #endif
    #if defined(FMOD_SUPPORT_FSB)
    else if (sample->mType == FMOD_SOUND_TYPE_FSB)
    {
        CodecFSB  *fsb = (CodecFSB *)soundcodec;

        if (0)
        {
        }
        #if defined(FMOD_SUPPORT_XMA)
        else if (sample->mFormat == FMOD_SOUND_FORMAT_XMA)
        {
            CodecXMA *destxma = (CodecXMA *)codec;
            int **seektable = (int **)fsb->plugindata;

            if (destxma->mXMASeekable)
            {
                destxma->mSeekTable = seektable[sample->mSubSoundIndex];
            }
            #ifdef FMOD_SUPPORT_XMA_NEWHAL
            destxma->mBlockSize = 32*1024;  // FSBank always encodes to 32kb blocks now, using XMA2.
            #endif            
        }
        #endif
        #if defined(FMOD_SUPPORT_MPEG)
        else if (sample->mFormat == FMOD_SOUND_FORMAT_MPEG)
        {
            CodecMPEG *destmpeg = (CodecMPEG *)codec;
    
            destmpeg->mPCMFrameLengthBytes = codec->waveformat[0].channels * 2304;
        }
        #endif
        #if defined(FMOD_SUPPORT_IMAADPCM)
        else if (sample->mFormat == FMOD_SOUND_FORMAT_IMAADPCM)
        {
            CodecWav *srcwav  = fsb->mADPCM;
            CodecWav *destwav = (CodecWav *)codec;

            destwav->mSamplesPerADPCMBlock = srcwav->mSamplesPerADPCMBlock;
            destwav->mReadBufferLength     = codec->waveformat[0].channels * 36;
        }
        #endif
        #if defined(FMOD_SUPPORT_CELT)
        else if (sample->mFormat == FMOD_SOUND_FORMAT_CELT)
        {
            CodecCELT *destcelt = (CodecCELT *)codec;

            destcelt->mPCMFrameLengthBytes = codec->waveformat[0].channels * FMOD_CELT_FRAMESIZESAMPLES * sizeof(short);
        }
        #endif
    
        fsb->mDescription.getwaveformat(fsb, sample->mSubSoundIndex, codec->waveformat);
    }
    #endif
    #if defined(FMOD_SUPPORT_XMA)
    else if (sample->mFormat == FMOD_SOUND_FORMAT_XMA)
    {
        CodecXMA *srcxma  = (CodecXMA *)soundcodec;
        CodecXMA *destxma = (CodecXMA *)codec;
        if (destxma->mXMASeekable)
        {
            destxma->mSeekTable = srcxma->mSeekTable;
        }
        #ifdef FMOD_SUPPORT_XMA_NEWHAL
        destxma->mBlockSize = srcxma->mBlockSize;
        #endif
    }
    #endif
    #if defined(FMOD_SUPPORT_MPEG)
    else if (sample->mFormat == FMOD_SOUND_FORMAT_MPEG)
    {
        CodecMPEG *srcmpeg  = (CodecMPEG *)soundcodec;
        CodecMPEG *destmpeg = (CodecMPEG *)codec;

        destmpeg->mPCMFrameLengthBytes = srcmpeg->mPCMFrameLengthBytes;
    }
    #endif

    /*
        If the codec is a pcm codec, fix up the blockalign and compressed bytes values.
    */
    #if defined(FMOD_SUPPORT_RAW)
    if (sample->mFormat == FMOD_SOUND_FORMAT_PCM16)
    {
        mMinFrequency = -mMaxFrequency;

        dspcodec->mWaveFormat.blockalign = sizeof(short) * dspcodec->mWaveFormat.channels;
        dspcodec->mWaveFormat.lengthbytes = dspcodec->mWaveFormat.lengthpcm * dspcodec->mWaveFormat.blockalign;
    }
    #endif

    dsp->mDescription.channels = sample->mChannels;        

    result = dsp->setTargetFrequency((int)mParent->mChannelGroup->mDSPMixTarget->mDefaultFrequency);
    if (result != FMOD_OK)
    {
        return result;
    }
 
#if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)
    if (sample->mFormat != FMOD_SOUND_FORMAT_PCM16)
#endif
    {
        mMinFrequency = 0;
    }
    
    mDSPCodec                               = dspcodec;
    mDSPCodec->mLength                      = mSound->mLength;
    mDSPCodec->mDefaultFrequency            = sample->mDefaultFrequency;
    mDSPCodec->mLoopCount                   = mLoopCount;
    mDSPCodec->mPosition                    = 0;
    mDSPCodec->DSPResampler::mPosition.mHi  = 0;
    mDSPCodec->DSPResampler::mPosition.mLo  = 0;
    mDSPCodec->mNoDMA->mLoopStart           = mLoopStart;
    mDSPCodec->mNoDMA->mLoopLength          = mLoopLength;
    mDSPCodec->mNoDMA->mMode                = mMode;
    mDSPCodec->mNoDMA->mDSPClockStart.mHi   = 0;
    mDSPCodec->mNoDMA->mDSPClockStart.mLo   = 0;
    mDSPCodec->mNoDMA->mDSPClockEnd.mHi     = 0;
    mDSPCodec->mNoDMA->mDSPClockEnd.mLo     = 0;
    mDSPCodec->mNoDMA->mDSPClockPause.mHi   = 0;
    mDSPCodec->mNoDMA->mDSPClockPause.mLo   = 0;
    mDSPCodec->mNoDMA->mNewLoopCount        = -2;

#ifdef FMOD_SUPPORT_SENTENCING
    if (mSound->mSubSoundList)
    {
        result = mDSPCodec->setUserData((void **)mSound);
        if (result != FMOD_OK)
        {
            return result;
        }

        mDSPCodec->mSubSoundListCurrent = 0;
        mDSPCodec->mSubSoundListNum     = mSound->mSubSoundListNum;
    }
#endif


#endif

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::alloc()
{
    FMOD_RESULT result;

    result = ChannelRealManual3D::alloc();
    if (result != FMOD_OK)
    {
        return result;
    }

    /*
        Standard PCM *wavetable* playback.
    */
    if (!(mMode & FMOD_CREATECOMPRESSEDSAMPLE)
#if defined(PLATFORM_PS3) || defined(PLATFORM_WINDOWS_PS3MODE)
         && (mParent->mFlags & CHANNELI_FLAG_MUSICOWNED)
#endif          
        )
    {
        DSPWaveTable *dspwave = SAFE_CAST(DSPWaveTable, mDSPWaveTable);
        if (!dspwave)
        {
            return FMOD_ERR_INTERNAL;
        }
        
        mDSPCodec = 0;

        /*
            Disconnect existing nodes first.
        */
        result = mDSPHead->disconnectFrom(0);
        if (result != FMOD_OK)
        {
            return result;
        }
        if (mDSPLowPass)
        {
            result = mDSPLowPass->disconnectFrom(0);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        result = mDSPWaveTable->disconnectFrom(0);
        if (result != FMOD_OK)
        {
            return result;
        }

        /*
            Add to parent's channelgroup along with any lowpass.
        */
        if (mDSPLowPass)
        {
            result = mParent->mChannelGroup->mDSPMixTarget->addInputQueued(mDSPHead, false, 0, &mDSPConnection);
            if (result != FMOD_OK)
            {
                return result;
            }
            result = mDSPHead->addInputQueued(mDSPLowPass, false, 0, 0);
            if (result != FMOD_OK)
            {
                return result;
            }
            result = mDSPLowPass->addInputQueued(mDSPWaveTable, false, 0, 0);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        else
        {
            result = mParent->mChannelGroup->mDSPMixTarget->addInputQueued(mDSPHead, false, 0, &mDSPConnection);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mDSPHead->addInputQueued(mDSPWaveTable, false, 0, 0);
            if (result != FMOD_OK)
            {
                return result;
            }
        }

        if (!(mFlags & CHANNELREAL_FLAG_NOREVERB))
        {
            mDSPReverb = mDSPWaveTable;

            result = addToReverbs(mDSPReverb);
            if (result != FMOD_OK)
            {
                return result;
            }
        }

        result = setLoopPoints(mSound->mLoopStart, mSound->mLoopLength);
        if (result != FMOD_OK)
        {
            return result;
        }

        mMinFrequency = -mMaxFrequency;

        dspwave->mSpeed.mHi         = 0;
        dspwave->mSpeed.mLo         = 0;
        dspwave->mPosition.mHi      = 0;
        dspwave->mPosition.mLo      = 0;
        dspwave->mDSPClockStart.mHi = 0;
        dspwave->mDSPClockStart.mLo = 0;
        dspwave->mDSPClockEnd.mHi   = 0;
        dspwave->mDSPClockEnd.mLo   = 0;
        dspwave->mDSPClockPause.mHi = 0;
        dspwave->mDSPClockPause.mLo = 0;

        dspwave->mChannel           = this;
        dspwave->mSound             = mSound;

        dspwave->mDirection         = DSPWAVETABLE_SPEEDDIR_FORWARDS;

        mDSPHead->setActive(false);
        if (mDSPLowPass)
        {
            mDSPLowPass->setActive(false);
        }
        mDSPWaveTable->setFinished(false);
        mDSPWaveTable->setActive(false);
    }
    else
    /*
        Advanced - Compressed codec playback with external resampler unit.
    */
    {
#ifdef FMOD_SUPPORT_DSPCODEC
        DSPCodec       *dsp;

        /*
            Disconnect existing nodes first.
        */
        result = mDSPHead->disconnectFrom(0);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (mDSPLowPass)
        {
            result = mDSPLowPass->disconnectFrom(0);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        if (mDSPWaveTable)
        {
            result = mDSPWaveTable->disconnectFrom(0);
            if (result != FMOD_OK)
            {
                return result;
            }
        }

        /*
            Grab a codec from the pool based on format.
        */
        result = mSystem->allocateDSPCodec(mSound->mFormat, &dsp);
        if (result != FMOD_OK)
        {
            return result;
        }      

#ifdef FMOD_SUPPORT_XMA
        /*
            If its XMA, set whether there is a seektable or not
        */
        if (mSound->mFormat == FMOD_SOUND_FORMAT_XMA)
        {
            DSPCodec *dspcodec   = SAFE_CAST(DSPCodec, dsp);
            CodecXMA *xma        = (CodecXMA *)dspcodec->mCodec;

			xma->mXMASeekable = mSound->mCodec ? mSound->mCodec->mXMASeekable : mSound->mSubSoundParent->mCodec->mXMASeekable;
        }
#endif
        result = setupDSPCodec(dsp);
        if (result != FMOD_OK)
        {
            return result;
        }

        mDSPHead->setActive(false);

        dsp->setFinished(false);
        dsp->setActive(false);

        if (mDSPLowPass)
        {
            result = mParent->mChannelGroup->mDSPMixTarget->addInputQueued(mDSPHead, false, 0, &mDSPConnection);
            if (result != FMOD_OK)
            {
                return result;
            }
            result = mDSPHead->addInputQueued(mDSPLowPass, false, 0, 0);
            if (result != FMOD_OK)
            {
                return result;
            }
            result = mDSPLowPass->addInputQueued(dsp, false, 0, 0);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        else
        {
            result = mParent->mChannelGroup->mDSPMixTarget->addInputQueued(mDSPHead, false, 0, &mDSPConnection);
            if (result != FMOD_OK)
            {
                return result;
            }
            result = mDSPHead->addInputQueued(dsp, false, 0, 0);
            if (result != FMOD_OK)
            {
                return result;
            }
        }

        if (!(mFlags & CHANNELREAL_FLAG_NOREVERB))
        {
            mDSPReverb = dsp;

            result = addToReverbs(mDSPReverb);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
#else
        return FMOD_ERR_INTERNAL;
#endif
    }
   
	return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::alloc(DSPI *dsp)
{
    FMOD_RESULT          result;
    FMOD_SOUND_FORMAT    format;
    int                  channels;

    result = ChannelReal::alloc();
    if (result != FMOD_OK)
    {
        return result;
    }

    mDSPCodec = 0;

    format   = dsp->mDescription.mFormat;
    channels = dsp->mDescription.channels;
          
    {
        FMOD_DSP_DESCRIPTION_EX descriptionex;

        FMOD_memset(&descriptionex, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));
        FMOD_strcpy(descriptionex.name, "FMOD Resampler Unit");
        descriptionex.version       = 0x00010100;
        descriptionex.channels      = 0; 
        descriptionex.mCategory     = FMOD_DSP_CATEGORY_RESAMPLER;

        result = mSystem->createDSP(&descriptionex, (DSPI **)&mDSPResampler);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mDSPResampler->setUserData((void **)this);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mDSPResampler->setTargetFrequency((int)mParent->mChannelGroup->mDSPMixTarget->mDefaultFrequency);
        if (result != FMOD_OK)
        {
            return result;
        }

        mMinFrequency = 0;
    }


    /*
        Connect the mixer to the resampler and the resampler to the reader
    */
    result = mDSPHead->disconnectFrom(0);
    if (result != FMOD_OK)
    {
        return result;
    }
    if (mDSPLowPass)
    {
        result = mDSPLowPass->disconnectFrom(0);
        if (result != FMOD_OK)
        {
            return result;
        }
    }
    if (mDSPWaveTable)
    {
        result = mDSPWaveTable->disconnectFrom(0);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    result = mDSPHead->addInputQueued(mDSPResampler, false, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }
    result = mDSPResampler->addInputQueued(dsp, false, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }
    result = mParent->mChannelGroup->mDSPMixTarget->addInputQueued(mDSPHead, false, 0, &mDSPConnection);
    if (result != FMOD_OK)
    {
        return result;
    }

    mDSPReverb = mDSPResampler;

    result = addToReverbs(mDSPReverb);
    if (result != FMOD_OK)
    {
        return result;
    }

    {
        DSPResampler *resampler = SAFE_CAST(DSPResampler, mDSPResampler);
     
        resampler->mLength                    = mLength;
        resampler->mLoopCount                 = mLoopCount;
        resampler->mNoDMA->mLoopStart         = mLoopStart;
        resampler->mNoDMA->mLoopLength        = mLoopLength;
        resampler->mNoDMA->mMode              = mMode;
        resampler->mNoDMA->mDSPClockStart.mHi = 0;
        resampler->mNoDMA->mDSPClockStart.mLo = 0;
        resampler->mNoDMA->mDSPClockEnd.mHi   = 0;
        resampler->mNoDMA->mDSPClockEnd.mLo   = 0;
        resampler->mNoDMA->mDSPClockPause.mHi = 0;
        resampler->mNoDMA->mDSPClockPause.mLo = 0;
        resampler->mNoDMA->mNewLoopCount      = -2;

        DSPWaveTable *dspwave = SAFE_CAST(DSPWaveTable, mDSPWaveTable);
        if (dspwave)
        {
            dspwave->mSound = 0;
        }
    }

    mDSPHead->setActive(false);

    mDSPResampler->setFinished(false);
    mDSPResampler->setActive(false);

    dsp->setActive(false);
    
	return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::start()
{
    if (!(mFlags & CHANNELREAL_FLAG_PAUSED))
    {
        mDSPHead->setActive(true);

        if (mSound && mDSPWaveTable)
        {
            mDSPWaveTable->setActive(true);
        }
        if (mDSPResampler)
        {
            mDSPResampler->setActive(true);
        }
        if (mDSPLowPass)
        {
            mDSPLowPass->setActive(true);
        }
    #ifdef FMOD_SUPPORT_DSPCODEC
        if (mDSPCodec)
        {
            mDSPCodec->setActive(true);
        }
    #endif
        if (mDSP)
        {
            mDSP->setActive(true);
        }       
    }

 	return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::stop()
{
    FMOD_RESULT result;
    int count;
   
    if (mDSPHead)
    {
        mDSPHead->setActive(false);
        mDSPHead->disconnectAll(false, true);
        mDSPHead->stopBuffering();
    }

#ifdef FMOD_SUPPORT_DSPCODEC
    if (mDSPCodec)
    {       
        mDSPCodec->setFinished(true, false);
        mDSPCodec->setActive(false);
        mDSPCodec->disconnectAll(false, true);      /* Just outputs, there are no inputs. */
        
        mDSPCodec->freeFromPool();
        mDSPCodec = 0;
    }
#endif

#ifndef FMOD_STATICFORPLUGINS
    /*
        Clear up reverb pointers.
    */
    if (mParent && !(mParent->mFlags & CHANNELI_FLAG_MUSICOWNED) && mDSPReverb)
    {
        int instance;  

        for (instance = 0; instance < FMOD_REVERB_MAXINSTANCES; instance++)
        {
            if (mSystem->mReverbGlobal.mInstance[instance].mDSP)
            {
                DSPConnectionI *connection = 0;

                result = mSystem->mReverbGlobal.getChanProperties(instance, mParent->mIndex, 0, &connection);

                result = mSystem->mReverbGlobal.mInstance[instance].mDSP->disconnectFrom(mDSPReverb, connection);
            }

            mSystem->mReverbGlobal.resetConnectionPointer(instance, mParent->mIndex);
        }

#ifdef FMOD_SUPPORT_MULTIREVERB
        if (mSystem->mReverb3D.mInstance[0].mDSP)
        {
            mSystem->mReverb3D.mInstance[0].mDSP->disconnectFrom(mDSPReverb);
            mSystem->mReverb3D.resetConnectionPointer(0, mParent->mIndex);
        }
        
        ReverbI *reverb_c = SAFE_CAST(ReverbI, mSystem->mReverb3DHead.getNext());
        while (reverb_c != &(mSystem->mReverb3DHead))
        {      
            if (reverb_c->mInstance[0].mDSP)
            {
                reverb_c->mInstance[0].mDSP->disconnectFrom(mDSPReverb);
                reverb_c->resetConnectionPointer(0, mParent->mIndex);
            }
            reverb_c = SAFE_CAST(ReverbI, reverb_c->getNext());
        }
#endif

        mDSPReverb = 0;
    }
#endif

    if (mDSPResampler)
    {
        mDSPResampler->setFinished(true, false);
        mDSPResampler->setActive(false);
        mDSPResampler->release();
        mDSPResampler = 0;
    }

    if (mDSPWaveTable)
    {
        mDSPWaveTable->reset();
        mDSPWaveTable->setFinished(true, false);
        mDSPWaveTable->setActive(false);
        mDSPWaveTable->disconnectAll(false, true);      /* Just outputs, there are no inputs. */
    }

    if (mDSP)
    {
        DSPI *prev;
        int numoutputs;

        result = mDSP->getNumOutputs(&numoutputs);
        if (result != FMOD_OK)
        {
            return result;
        }

        for(count = 0; count < numoutputs; count++)
        {
            result = mDSP->getOutput(count, &prev);
            if (result == FMOD_OK)
            {
                result = prev->disconnectFrom(mDSP);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }
        }        
    }

    ChannelReal::stop();

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::setPaused(bool paused)
{   
    FMOD_RESULT result;

    result = mDSPHead->setActive(!paused);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (mSound && mDSPWaveTable)
    {
        mDSPWaveTable->setActive(!paused);
    }
    if (mDSPResampler)
    {
        mDSPResampler->setActive(!paused);
    }
    if (mDSPLowPass)
    {
        mDSPLowPass->setActive(!paused);
    }
#ifdef FMOD_SUPPORT_DSPCODEC
    if (mDSPCodec)
    {
        mDSPCodec->setActive(!paused);
    }
#endif
    if (mDSP)
    {
        mDSP->setActive(!paused);
    }

	return ChannelReal::setPaused(paused);	
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::getPaused(bool *paused)
{   
    FMOD_RESULT result;
    bool active;

    result = mDSPHead->getActive(&active);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (!active)
    {
        *paused = true;
        return FMOD_OK;
    }

    if (mSound && mDSPWaveTable)
    {
        mDSPWaveTable->getActive(&active);

        if (!active)
        {
            *paused = true;
            return FMOD_OK;
        }

    }
    if (mDSPResampler)
    {
        mDSPResampler->getActive(&active);

        if (!active)
        {
            *paused = true;
            return FMOD_OK;
        }
    }

#ifdef FMOD_SUPPORT_DSPCODEC
    if (mDSPCodec)
    {
        mDSPCodec->getActive(&active);
        if (!active)
        {
            *paused = true;
            return FMOD_OK;
        }
    }
#endif

    if (mDSP)
    {
        mDSP->getActive(&active);
        if (!active)
        {
            *paused = true;
            return FMOD_OK;
        }
    }

	return ChannelReal::getPaused(paused);	
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::setReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES *prop)
{
#ifdef FMOD_SUPPORT_REVERB
    FMOD_RESULT result;
    DSPConnectionI *connection;
    float lindirect;
    int instance;

    if (!prop)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (prop->ConnectionPoint && mDSPReverb != (DSPI *)prop->ConnectionPoint)
    {
        FMOD_RESULT result;
        int          instance;

        /*
            Add an input to standard reverb
        */
        for (instance = 0; instance < FMOD_REVERB_MAXINSTANCES; instance++)
        {
            if (mSystem->mReverbGlobal.mInstance[instance].mDSP)
            {
                DSPConnectionI *connection = 0;

                result = mSystem->mReverbGlobal.getChanProperties(instance, mParent->mIndex, 0, &connection);

                result = mSystem->mReverbGlobal.mInstance[instance].mDSP->disconnectFrom(mDSPReverb, connection);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }
        }

#ifdef FMOD_SUPPORT_MULTIREVERB        
        /*
            Add an input to 3d reverb
        */
        if (mSystem->mReverb3D.mInstance[0].mDSP)
        {
            result = mSystem->mReverb3D.mInstance[0].mDSP->disconnectFrom(mDSPReverb);
            if (result != FMOD_OK)
            {
                return result;
            }
        }

        /*
            Traverse reverb list and add an input to each one that has a DSP
        */
        ReverbI *reverb_c = SAFE_CAST(ReverbI, mSystem->mReverb3DHead.getNext());
        while (reverb_c != &(mSystem->mReverb3DHead))
        {      
            if (reverb_c->mInstance[0].mDSP)
            {
                result = reverb_c->mInstance[0].mDSP->disconnectFrom(mDSPReverb);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }
            reverb_c = SAFE_CAST(ReverbI, reverb_c->getNext());
        }
#endif
        mDSPReverb = (DSPI *)prop->ConnectionPoint;

        result = addToReverbs(mDSPReverb);
        if (result != FMOD_OK)
        {
            return result;
        }
    }


    lindirect = FMOD_POW(10.0f, prop->Direct / 2000.0f); // Convert to linear gain

    if (lindirect != mParent->mReverbDryVolume)
    {
        mParent->mReverbDryVolume = lindirect;
        updateDirectMix(mParent->mVolume);
    }

    /*
        How many instances is the user trying to set.  Just use this for error checking later.
        Setting just one instance will return an error if it doesnt exist.  If the user sets 
        something like 0xFFFFFFFF dont bail out as they probably want to set all instances.
    */
    int numinstances = 0;
    for (instance = 0; instance < FMOD_REVERB_MAXINSTANCES; instance++)
    {
        if (prop->Flags & (FMOD_REVERB_CHANNELFLAGS_INSTANCE0 << instance))
        {
            numinstances++;
        }
    }

    for (instance = 0; instance < FMOD_REVERB_MAXINSTANCES; instance++)
    {
        if (prop->Flags & (FMOD_REVERB_CHANNELFLAGS_INSTANCE0 << instance) || (!numinstances && !instance))
        {
            /*
                Set main reverb's channel properties and update the reverb mix
            */
            result = mSystem->mReverbGlobal.setChanProperties(instance, mParent->mIndex, prop);
            if (numinstances <= 1 && result != FMOD_OK)
            {
                return result;
            }

            if (mSystem->mReverbGlobal.mInstance[instance].mDSP)
            {
                mSystem->mReverbGlobal.getChanProperties(instance, mParent->mIndex, 0, &connection);
   	            if (!connection)
                {  
                    if (!mDSPReverb)
                    {
    			        if      (mDSPCodec)     mDSPReverb = mDSPCodec;
                        else if (mDSPResampler) mDSPReverb = mDSPResampler;
                        else                    mDSPReverb = mDSPWaveTable;
                    }

                    addToReverbs(mDSPReverb);
                }
                result = updateReverbMix(&mSystem->mReverbGlobal, mParent->mVolume);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }
        }
        else
        {
            // update direct mix on other instances (ignoring errors)
            FMOD_REVERB_CHANNELPROPERTIES cprop;
            result = mSystem->mReverbGlobal.getChanProperties(instance, mParent->mIndex, &cprop);
            cprop.Direct = prop->Direct;
            result = mSystem->mReverbGlobal.setChanProperties(instance, mParent->mIndex, &cprop);
        }
    }

#ifdef FMOD_SUPPORT_MULTIREVERB
    /*
        Also for 3D master reverb
    */
    
    /*
		Only apply 3D reverb property to instance #0.  If user specified nothing (default to instance 0), or instance 0 flag.
    */
	if (prop->Flags & FMOD_REVERB_CHANNELFLAGS_INSTANCE0 || !numinstances)
    {
        result = mSystem->mReverb3D.setChanProperties(0, mParent->mIndex, prop);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    if (mSystem->mReverb3D.mInstance[0].mDSP)
    {
        mSystem->mReverb3D.getChanProperties(0, mParent->mIndex, 0, &connection);
   	    if (!connection)
        {  
            if (!mDSPReverb)
            {
    			if      (mDSPCodec)     mDSPReverb = mDSPCodec;
                else if (mDSPResampler) mDSPReverb = mDSPResampler;
                else                    mDSPReverb = mDSPWaveTable;
            }

            addToReverbs(mDSPReverb);
        }
        result = updateReverbMix(&mSystem->mReverb3D, mParent->mVolume);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    /*
        Also for physicals in 3D reverb list
    */
    ReverbI *reverb_c;
    for (reverb_c = SAFE_CAST(ReverbI, mSystem->mReverb3DHead.getNext()); reverb_c != &mSystem->mReverb3DHead; reverb_c = SAFE_CAST(ReverbI, reverb_c->getNext()))
    {
        if(reverb_c->getMode() == FMOD_REVERB_PHYSICAL)
        {
            reverb_c->setChanProperties(0, mParent->mIndex, prop);

            if( reverb_c->mInstance[0].mDSP)
            {
                reverb_c->getChanProperties(0, mParent->mIndex, 0, &connection);
   	            if (!connection)
                {  
                    if (!mDSPReverb)
                    {
    			        if      (mDSPCodec)     mDSPReverb = mDSPCodec;
                        else if (mDSPResampler) mDSPReverb = mDSPResampler;
                        else                    mDSPReverb = mDSPWaveTable;
                    }

                    addToReverbs(mDSPReverb);
                }
                result = updateReverbMix(reverb_c, mParent->mVolume);
                if (result != FMOD_OK)
                {
                    return result;
                }
            } 
        }
    }
#endif

    return FMOD_OK;
#else
    return FMOD_ERR_UNSUPPORTED;
#endif
}



/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::updateDirectMix(float volume)
{
    FMOD_RESULT    result;
    float          gainlin;

    if (mParent->mFlags & CHANNELI_FLAG_REALMUTE)
    {
        volume = 0;
    }

    /*
       Calculate level based on current state
    */
    gainlin  = volume;
    gainlin *= mParent->mFadeVolume;
    gainlin *= mParent->mReverbDryVolume;
    gainlin *= ((1.0f - mParent->m3DPanLevel) + (mParent->mVolume3D * mParent->m3DPanLevel));
    gainlin *= mParent->mChannelGroup->mRealVolume;
    gainlin *= ((1.0f - mParent->m3DPanLevel) + (mParent->mConeVolume3D * mParent->m3DPanLevel));
    if (mSound && mSound->mSoundGroup)
    {
        gainlin *= mSound->mSoundGroup->mVolume;
    }

    float transmission = (1.0f - mParent->mDirectOcclusion) *
			             (1.0f - mParent->mUserDirectOcclusion) *
                         mParent->mChannelGroup->mRealDirectOcclusionVolume;

    if (mDSPLowPass)
    {
        float hrtf_cutoff = 22050.0f;

        transmission *= mParent->mLowPassGain;

        if (mSystem->mFlags & FMOD_INIT_SOFTWARE_HRTF)
        {
            float hrtf_min_angle = mSystem->mAdvancedSettings.HRTFMinAngle / 2.0f;
            float hrtf_max_angle = mSystem->mAdvancedSettings.HRTFMaxAngle / 2.0f;
            float abs_angle = (mAngleToListener <= 180.0f) ? mAngleToListener : 360.0f-mAngleToListener;

            if (abs_angle <= hrtf_min_angle)
            {                
                hrtf_cutoff = 22050.0f;        /* Max cutoff frequency - i.e. no filter. */
            }
            else if (abs_angle >= hrtf_max_angle)
            {               
                hrtf_cutoff = (float)mSystem->mAdvancedSettings.HRTFFreq;        /* Min cutoff frequency - i.e. maximum filter */
            }
            else
            {
                /*
                    Linear interpolation between min and max frequency with linear angle
                */
                float filter_strength = (abs_angle - hrtf_min_angle) / (hrtf_max_angle - hrtf_min_angle);
                float range = 22050.0f - mSystem->mAdvancedSettings.HRTFFreq;
                
                hrtf_cutoff = mSystem->mAdvancedSettings.HRTFFreq + range * (1.0f - filter_strength);
            }
        }

        hrtf_cutoff = (((1.0f - mParent->m3DPanLevel) * 22050.0f) + (hrtf_cutoff * mParent->m3DPanLevel));

        if (transmission < 1.0f || hrtf_cutoff < 22050.0f)
        {
            float cutoff;

            mDSPLowPass->setBypass(false);

            cutoff = 22050.0f * transmission * transmission;

            /*
                Try to override the occlusion 
            */
            if (hrtf_cutoff < cutoff)
            {
                cutoff = hrtf_cutoff;
            }
            
            mDSPLowPass->setParameter(FMOD_DSP_LOWPASS_SIMPLE_CUTOFF, cutoff);
        }
        else
        {
            mDSPLowPass->setBypass(true);
        }
    }
    else
    {
        gainlin *= transmission;
    }

    result = mDSPConnection->setMix(gainlin);
    if (result != FMOD_OK)
    {
        return result;
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::updateReverbMix(ReverbI* reverb, float volume)
{
#ifdef FMOD_SUPPORT_REVERB
    int instance;

    if (!reverb)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (mParent->mFlags & CHANNELI_FLAG_REALMUTE)
    {
        volume = 0;
    }

    for (instance = 0; instance < FMOD_REVERB_MAXINSTANCES; instance++)
    {
        if (reverb->mInstance[instance].mDSP)
        {
            FMOD_RESULT result;
            FMOD_REVERB_CHANNELPROPERTIES props;
            DSPConnectionI *connection;
            float gainlin = 0.0f;
            bool morphing = false;

            /*
                Get properties
            */
            result = reverb->getChanProperties(instance, mParent->mIndex, &props, &connection);
            if (result != FMOD_OK)
            {
                return result;
            }

            if (!connection)
            {
               return FMOD_OK;
            }

#ifdef FMOD_SUPPORT_MULTIREVERB
            result = mSystem->get3DReverbActive(&morphing);
            if (result != FMOD_OK)
            {
                return result;
            }
#endif

            float transmission = (1.0f - mParent->mReverbOcclusion) *
			                     (1.0f - mParent->mUserReverbOcclusion) *
                                 mParent->mChannelGroup->mRealReverbOcclusionVolume;

            /*
                Main reverb
            */
            if (reverb == &mSystem->mReverbGlobal)
            {
                if (mMode & FMOD_3D)
                {
                    if (morphing)
                    {
                        gainlin = 0.0f;
                    }
                    else
                    {
                        gainlin = (float)FMOD_POW(10.0f, props.Room / 2000.0f); // Convert to linear gain

                        if (!props.ConnectionPoint)
                        {
                            gainlin *= transmission;
                            gainlin *= mParent->mFadeVolume;
                            gainlin *= mParent->mVolume3D;
                            gainlin *= mParent->mChannelGroup->mRealVolume;
                            if (mSound && mSound->mSoundGroup)
                            {
                                gainlin *= mSound->mSoundGroup->mVolume;
                            }
                            gainlin *= volume;
                        }
                    }
                }
                else
                {
                    gainlin = (float)FMOD_POW(10.0f, props.Room / 2000.0f); 

                    if (!props.ConnectionPoint)
                    {
                        gainlin *= mParent->mChannelGroup->mRealVolume;
                        gainlin *= volume;
                    }
                }
            }

#ifdef FMOD_SUPPORT_MULTIREVERB
            /*
                Main 3D reverb
            */
            else if (reverb == &mSystem->mReverb3D )
            {
                if (mMode & FMOD_3D) // 3D channel
                {
                    gainlin = (float)FMOD_POW(10.0f, props.Room / 2000.0f); // Convert to linear gain

                    if (!props.ConnectionPoint)
                    {
                        gainlin *= transmission;
                        gainlin *= mParent->mFadeVolume;
                        gainlin *= mParent->mVolume3D;
                        gainlin *= mParent->mChannelGroup->mRealVolume;
                        if (mSound && mSound->mSoundGroup)
                        {
                            gainlin *= mSound->mSoundGroup->mVolume;
                        }

                        gainlin *= volume;
                    }
                }
                else    // 2D channels contribute nothing
                {
                    gainlin = 0.0f;
                }
            }
            /*
                Physical 3D reverbs
            */
            else
            {
                // 2D channel : no contribution
                //
                if (mMode & FMOD_2D)
                {
                    gainlin = 0.0f;
                }
                else
                {
                    /*
                        Positioned 3D reverbs, including 3D main module if present
                    */

                    /*
                        Find listener and source components
                    */
                    float source_gain;
                    reverb->getPresenceGain(0, mParent->mIndex, &source_gain);
            
                    /*
                        Take the maximum of source presence and listener presence components
                    */
                    float listener_gain = reverb->getGain();
                    gainlin = (listener_gain > source_gain) ? listener_gain : source_gain;

                    /*
                        Apply other gains
                    */
                    gainlin *= (float)FMOD_POW(10.0f, props.Room / 2000.0f); /* Convert to linear gain  */

                    if (!props.ConnectionPoint)
                    {
                        gainlin *= transmission;
                        gainlin *= mParent->mFadeVolume;
                        gainlin *= mParent->mVolume3D;
                        gainlin *= mParent->mChannelGroup->mRealVolume;
                        if (mSound && mSound->mSoundGroup)
                        {
                            gainlin *= mSound->mSoundGroup->mVolume;
                        }

                        gainlin *= volume;
                    }
                }
            }
#endif

            /*
                Find connection and set channel's reverb input level as appropriate
            */
            result = connection->setMix(gainlin);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
    }

#endif
    
    return FMOD_OK;
}

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::getReverbProperties(FMOD_REVERB_CHANNELPROPERTIES *prop)
{
#ifdef FMOD_SUPPORT_REVERB
    FMOD_RESULT result;
    int instance = 0;

    if(!prop)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    /*
        The first instance set is always returned (so 0|1|2|3 will return 0, 2|3 will return 2 etc).
    */
    instance = (prop->Flags & FMOD_REVERB_CHANNELFLAGS_INSTANCE0) ? 0 :
               (prop->Flags & FMOD_REVERB_CHANNELFLAGS_INSTANCE1) ? 1 : 
               (prop->Flags & FMOD_REVERB_CHANNELFLAGS_INSTANCE2) ? 2 : 
               (prop->Flags & FMOD_REVERB_CHANNELFLAGS_INSTANCE3) ? 3 : 
               0;

    result = mSystem->mReverbGlobal.getChanProperties(instance, mParent->mIndex, prop);
    if(result != FMOD_OK)
    {
        return result;
    }
#endif

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::setLowPassGain(float gain)
{
    /*
        Need to flush the mLowPassGain value set in parent (via updateDirectMix)
    */
    return setVolume(mParent->mVolume);
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::set3DOcclusion(float directOcclusion, float reverbOcclusion)
{
    FMOD_RESULT    result;
    
    if (mSubChannelIndex > 0)
    {
        return FMOD_OK;
    }

    /*
        Modify Room property to be reverbVolume in mB
    */
    mParent->mReverbOcclusion = reverbOcclusion;

    /*
        Set direct occlusion factors 
    */
    mParent->mDirectOcclusion = directOcclusion;

    /*
        Update direct and listener reverb mix levels
    */
    result = updateDirectMix(mParent->mVolume);
    if (result != FMOD_OK)
    {
        return result;
    }

#ifdef FMOD_SUPPORT_MULTIREVERB
    ReverbI *reverb_c;
    for (reverb_c = SAFE_CAST(ReverbI, mSystem->mReverb3DHead.getNext()); reverb_c != &mSystem->mReverb3DHead; reverb_c = SAFE_CAST(ReverbI, reverb_c->getNext()))
    {
        if (reverb_c->getMode() == FMOD_REVERB_PHYSICAL)
        {
            result = updateReverbMix(reverb_c, mParent->mVolume);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
    }
#endif

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::setVolume(float volume)
{   
    FMOD_RESULT    result;

    if (mSubChannelIndex > 0)
    {
        return FMOD_OK;
    }

    /*
        Update direct and listener reverb mix levels
    */
    result = updateDirectMix(volume);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (!(mFlags & CHANNELREAL_FLAG_NOREVERB))
    {
        result = updateReverbMix(&mSystem->mReverbGlobal, volume);
        if (result != FMOD_OK)
        {
            return result;
    
        }

        #ifdef FMOD_SUPPORT_MULTIREVERB
        ReverbI *reverb_c;

        result = updateReverbMix(&mSystem->mReverb3D, volume);
        if (result != FMOD_OK)
        {
            return result;
        }

        for (reverb_c = SAFE_CAST(ReverbI, mSystem->mReverb3DHead.getNext());
             reverb_c != &mSystem->mReverb3DHead;
             reverb_c = SAFE_CAST(ReverbI, reverb_c->getNext()))
        {
            if(reverb_c->getMode() == FMOD_REVERB_PHYSICAL)
            {
                result = updateReverbMix(reverb_c, volume);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }
        }
        #endif
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::setFrequency(float frequency)
{
    if (mDSPResampler || mDSPCodec)
    {
        DSPResampler *dspresampler;

#ifdef FMOD_SUPPORT_DSPCODEC
        if (mDSPCodec)
        {           
            dspresampler = SAFE_CAST(DSPResampler, mDSPCodec);
        }
        else
#endif
        {
            dspresampler = SAFE_CAST(DSPResampler, mDSPResampler);
        }
        if (!dspresampler)
        {
            return FMOD_ERR_INVALID_PARAM;
        }
   
        frequency *= ((1.0f - mParent->m3DPanLevel) + (mParent->mPitch3D * mParent->m3DPanLevel));
        frequency *= mParent->mChannelGroup->mRealPitch;

        if (frequency > mMaxFrequency)
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "ChannelSoftware::setFrequency", "Warning!!! Extreme frequency being set (%.02f hz).  Possibly because of bad velocity in set3DAttributes call.\n", frequency));
            frequency = mMaxFrequency;
        }
        if (frequency < mMinFrequency)
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "ChannelSoftware::setFrequency", "Warning!!! Extreme frequency being set (%.02f hz).  Possibly because of bad velocity in set3DAttributes call.\n", frequency));
            frequency = mMinFrequency;
        }

        return dspresampler->setFrequency(frequency);
    }
    else if (mDSPWaveTable)
    {
        DSPWaveTable *dspwave;
    
        dspwave = SAFE_CAST(DSPWaveTable, mDSPWaveTable);
        if (!dspwave)
        {
            return FMOD_ERR_INTERNAL;
        }

        frequency *= ((1.0f - mParent->m3DPanLevel) + (mParent->mPitch3D * mParent->m3DPanLevel));
        frequency *= mParent->mChannelGroup->mRealPitch;

        if (frequency > mMaxFrequency)
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "ChannelSoftware::setFrequency", "Warning!!! Extreme frequency being set (%.02f hz).  Possibly because of bad velocity in set3DAttributes call.\n", frequency));
            frequency = mMaxFrequency;
        }
        if (frequency < mMinFrequency)
        {
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "ChannelSoftware::setFrequency", "Warning!!! Extreme frequency being set (%.02f hz).  Possibly because of bad velocity in set3DAttributes call.\n", frequency));
            frequency = mMinFrequency;
        }
    
        return dspwave->setFrequency(frequency);
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::setPan(float lrpan, float fbpan)
{
    int numchannels;
    float pan = (lrpan + 1.0f) / 2.0f;
    float l,r;
    unsigned int channelmask = 0;

    if (mSound)
    {
        numchannels = mSound->mChannels;
        channelmask = mSound->mDefaultChannelMask;
    }
    else if (mDSP)
    {
        numchannels = mDSP->mDescription.channels;
    }
    else
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (numchannels == 1)
    {
        if (mParent->mSpeakerMode == FMOD_SPEAKERMODE_STEREO_LINEAR)
        {
            l = 1.0f - pan;
            r = pan;
        }
        else
        {
            l = FMOD_SQRT(1.0f - pan);
            r = FMOD_SQRT(pan);
        }
        
        return setSpeakerMix(l, r, 0, 0, 0, 0, 0, 0);
    }
    else 
    {
        /*
            Stereo source panning is 0 = 1.0:0.0.  0.5 = 1.0:1.0.  1.0 = 0:1.0.
        */
        if (pan <= 0.5f)
        {
            l = 1.0f;
            r = pan * 2.0f;
        }
        else
        {
            l = (1.0f - pan) * 2.0f;
            r = 1.0f;
        }
        
        if (numchannels == 2 && !(channelmask & SPEAKER_ALLMONO))
        {
            return setSpeakerMix(l, r, 0, 0, 0, 0, 0, 0);
        }
        else
        {
           return setSpeakerMix(l, r, 1.0f, 1.0f, l, r, l, r);
        }
    }
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    
	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::setDSPClockDelay()
{
#ifdef FMOD_STATICFORPLUGINS
    return FMOD_ERR_UNSUPPORTED;
#else
    if (mDSPWaveTable)
    {
        DSPWaveTable *dspwave = SAFE_CAST(DSPWaveTable, mDSPWaveTable);

        dspwave->mDSPClockStart.mHi = mParent->mDSPClockDelay.mHi;
        dspwave->mDSPClockStart.mLo = mParent->mDSPClockDelay.mLo;
        dspwave->mDSPClockEnd.mHi = mParent->mDSPClockEnd.mHi;
        dspwave->mDSPClockEnd.mLo = mParent->mDSPClockEnd.mLo;
        dspwave->mDSPClockPause.mHi = mParent->mDSPClockPause.mHi;
        dspwave->mDSPClockPause.mLo = mParent->mDSPClockPause.mLo;
    }
    if (mDSPCodec || mDSPResampler)
    {
        DSPResampler *resampler = SAFE_CAST(DSPResampler, mDSPCodec ? mDSPCodec : mDSPResampler);

        resampler->mNoDMA->mDSPClockStart.mHi = mParent->mDSPClockDelay.mHi;
        resampler->mNoDMA->mDSPClockStart.mLo = mParent->mDSPClockDelay.mLo;
        resampler->mNoDMA->mDSPClockEnd.mHi   = mParent->mDSPClockEnd.mHi;
        resampler->mNoDMA->mDSPClockEnd.mLo   = mParent->mDSPClockEnd.mLo;
        resampler->mNoDMA->mDSPClockPause.mHi = mParent->mDSPClockPause.mHi;
        resampler->mNoDMA->mDSPClockPause.mLo = mParent->mDSPClockPause.mLo;
    }

    return FMOD_OK;
#endif
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::setSpeakerMix(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright)
{
    FMOD_RESULT         result;
    int                 numinputlevels;
    float               levels[DSP_MAXLEVELS_OUT * DSP_MAXLEVELS_IN];
    int                 numchannels;
    int                 count, count2;
    FMOD_SPEAKERMAPTYPE speakermap = FMOD_SPEAKERMAPTYPE_DEFAULT;
       
    if (mSubChannelIndex > 0)
    {
        return FMOD_OK;
    }

    if (mSound)
    {
        numchannels = mSound->mChannels;

        if (mSound->mDefaultChannelMask & SPEAKER_ALLMONO)
        {
            speakermap = FMOD_SPEAKERMAPTYPE_ALLMONO;
        }
        else if (mSound->mDefaultChannelMask & SPEAKER_ALLSTEREO)
        {
            speakermap = FMOD_SPEAKERMAPTYPE_ALLSTEREO;
        }
        else if (mSound->mDefaultChannelMask & SPEAKER_PROTOOLS)
        {
            speakermap = FMOD_SPEAKERMAPTYPE_51_PROTOOLS;
        }
        /*
            Special case for quad sounds inside a 6 or 8 channel stream so that 
            DSPI::calculateSpeakerLevels() maps the channels in the correct order.
        */
        else if (mSound->mDefaultChannelMask == SPEAKER_QUAD)
        {
            numchannels = 4;
        }
    }
    else if (mDSP)
    {
        numchannels = mDSP->mDescription.channels;
    }
    else
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if ((mSound && mSound->mMode & FMOD_3D) && (mParent->mSpeakerMode == FMOD_SPEAKERMODE_PROLOGIC))
    {
        result = DSPI::calculateSpeakerLevels(frontleft, frontright, center, lfe, backleft, backright, sideleft, sideright, FMOD_SPEAKERMODE_STEREO, numchannels, speakermap, &levels[0], &numinputlevels);
    }
    else
    {
        result = DSPI::calculateSpeakerLevels(frontleft, frontright, center, lfe, backleft, backright, sideleft, sideright, mParent->mSpeakerMode, numchannels, speakermap, &levels[0], &numinputlevels);
    }
    if (result != FMOD_OK)
	{
        return result;
	}

    if (mParent->mFlags & CHANNELI_FLAG_USEDINPUTMIX)
    {
        for (count = 0; count < mSystem->mMaxOutputChannels; count++)
        {
            for (count2 = 0; count2 < numinputlevels; count2++)
            {
                levels[(count * numinputlevels) + count2] *= mParent->mInputMix[count2];
            }
        }
    }

    result = mDSPConnection->setLevels(&levels[0], numinputlevels);
    if (result != FMOD_OK)
    {
        return result;
    }

#ifdef FMOD_SUPPORT_REVERB
    int instance;
    FMOD_REVERB_CHANNELPROPERTIES props;

    for (instance = 0; instance < FMOD_REVERB_MAXINSTANCES; instance++)
    {
        if (mSystem->mReverbGlobal.mInstance[instance].mDSP)
        {
            FMOD_RESULT result;
            DSPConnectionI *connection;

            /*
                Get connection
            */
            mSystem->mReverbGlobal.getChanProperties(instance, mParent->mIndex, &props, &connection);
            if (connection && (connection->mInputUnit == mDSPCodec || connection->mInputUnit == mDSPWaveTable || connection->mInputUnit == mDSPResampler) && !props.ConnectionPoint)
            {
                result = connection->setLevels(&levels[0], numinputlevels);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }
        }
    }

#ifdef FMOD_SUPPORT_MULTIREVERB
    if (mSystem->mReverb3D.mInstance[0].mDSP)
    {
        DSPConnectionI *connection;

        mSystem->mReverb3D.getChanProperties(0, mParent->mIndex, &props, &connection);
        if (connection && (connection->mInputUnit == mDSPCodec || connection->mInputUnit == mDSPWaveTable || connection->mInputUnit == mDSPResampler) && !props.ConnectionPoint)
        {
            result = connection->setLevels(&levels[0], numinputlevels);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
    }

    /*
        Also for physicals in 3D reverb list
    */
    for (ReverbI* reverb_c  = SAFE_CAST(ReverbI, mSystem->mReverb3DHead.getNext());
                  reverb_c != &mSystem->mReverb3DHead;
                  reverb_c  = SAFE_CAST(ReverbI, reverb_c->getNext()))
    {
        if (reverb_c->getMode() == FMOD_REVERB_PHYSICAL)
        {
            if (reverb_c->mInstance[0].mDSP)
            {
                DSPConnectionI *connection;

                reverb_c->getChanProperties(0, mParent->mIndex, &props, &connection);
                if (connection && (connection->mInputUnit == mDSPCodec || connection->mInputUnit == mDSPWaveTable || connection->mInputUnit == mDSPResampler) && !props.ConnectionPoint)
                {
                    result = connection->setLevels(&levels[0], numinputlevels);
                    if (result != FMOD_OK)
                    {
                        return result;
                    }
                }
            } 
        }
    }
#endif

#endif

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::setSpeakerLevels(int speaker, float *levels, int numlevels)
{
    FMOD_RESULT     result;
    float           clevels[DSP_MAXLEVELS_OUT][DSP_MAXLEVELS_IN];
    int             count;
       
    if (mSubChannelIndex > 0)
    {
        return FMOD_OK;
    }

    result = mDSPConnection->getLevels(&clevels[0][0], DSP_MAXLEVELS_IN);
    if (result != FMOD_OK)
    {
        return result;
    }

    for (count = 0; count < numlevels; count++)
    {
        clevels[speaker][count] = levels[count] * mParent->mInputMix[count];
    }

    result = mDSPConnection->setLevels(&clevels[0][0], DSP_MAXLEVELS_IN);
    if (result != FMOD_OK)
    {
        return result;
    }

#ifdef FMOD_SUPPORT_REVERB
    int instance;
    FMOD_REVERB_CHANNELPROPERTIES props;


    for (instance = 0; instance < FMOD_REVERB_MAXINSTANCES; instance++)
    {
        if (mSystem->mReverbGlobal.mInstance[instance].mDSP)
        {
            FMOD_RESULT result;
            DSPConnectionI *connection;

            /*
                Get connection
            */
            result = mSystem->mReverbGlobal.getChanProperties(instance, mParent->mIndex, &props, &connection);
            if (connection && (connection->mInputUnit == mDSPCodec || connection->mInputUnit == mDSPWaveTable || connection->mInputUnit == mDSPResampler) && !props.ConnectionPoint)
            {
                result = connection->setLevels(&clevels[0][0], DSP_MAXLEVELS_IN);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }
        }
    }

#ifdef FMOD_SUPPORT_MULTIREVERB
    if (mSystem->mReverb3D.mInstance[0].mDSP)
    {
        DSPConnectionI *connection;

        mSystem->mReverb3D.getChanProperties(0, mParent->mIndex, &props, &connection);
        if (connection && (connection->mInputUnit == mDSPCodec || connection->mInputUnit == mDSPWaveTable || connection->mInputUnit == mDSPResampler) && !props.ConnectionPoint)
        {
            result = connection->setLevels(&clevels[0][0], DSP_MAXLEVELS_IN);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
    }

    /*
        Also for physicals in 3D reverb list
    */
    ReverbI *reverb_c;
    for (reverb_c = SAFE_CAST(ReverbI, mSystem->mReverb3DHead.getNext()); reverb_c != &mSystem->mReverb3DHead; reverb_c = SAFE_CAST(ReverbI, reverb_c->getNext()))
    {
        if (reverb_c->getMode() == FMOD_REVERB_PHYSICAL)
        {
            if (reverb_c->mInstance[0].mDSP)
            {
                DSPConnectionI *connection;

                reverb_c->getChanProperties(0, mParent->mIndex, &props, &connection);
                if (connection && (connection->mInputUnit == mDSPCodec || connection->mInputUnit == mDSPWaveTable || connection->mInputUnit == mDSPResampler) && !props.ConnectionPoint)
                {
                    result = connection->setLevels(&clevels[0][0], DSP_MAXLEVELS_IN);
                    if (result != FMOD_OK)
                    {
                        return result;
                    }
                }
            } 
        }
    }
#endif

#endif

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::setPosition(unsigned int position, FMOD_TIMEUNIT postype)
{ 
    unsigned int      pcm = 0, endpoint;
    int               channels;
    FMOD_SOUND_FORMAT format;
    float             frequency;

    if (mSubChannelIndex > 0)
    {
        return FMOD_OK;
    }

    if (postype != FMOD_TIMEUNIT_MS && postype != FMOD_TIMEUNIT_PCM && postype != FMOD_TIMEUNIT_PCMBYTES)
    {
        return FMOD_ERR_FORMAT;
    }
#ifdef FMOD_SUPPORT_DSPCODEC
    else if (mDSPCodec)
    {
        channels    = mDSPCodec->mDescription.channels;
        format      = mDSPCodec->mDescription.mFormat;
        frequency   = mDSPCodec->mDefaultFrequency;
    }
#endif
    else if (mSound)
    {
        channels    = mSound->mChannels;
        format      = mSound->mFormat;
        frequency   = mSound->mDefaultFrequency;
    }
    else if (mDSPResampler)
    {
        channels    = mDSPResampler->mDescription.channels;
        format      = FMOD_SOUND_FORMAT_PCMFLOAT;
        frequency   = mDSPResampler->mDefaultFrequency;
    }
    else 
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (postype == FMOD_TIMEUNIT_PCM)
    {
        pcm = position;
    }
    else if (postype == FMOD_TIMEUNIT_PCMBYTES)
    {
        SoundI::getSamplesFromBytes(position, &pcm, channels, format);
    }
    else if (postype == FMOD_TIMEUNIT_MS)
    {
        pcm = (unsigned int)((float)position / 1000.0f * frequency);
    }
   
    if (mSound)
    {
        if (mMode & FMOD_LOOP_OFF)
        {
            endpoint = mSound->mLength - 1;
        }
        else
        {
            endpoint = mLoopStart + mLoopLength - 1;
        }
    }
    else
    {
        endpoint = (unsigned int)-1;
    }

    if (pcm > endpoint)
    {
        return FMOD_ERR_INVALID_POSITION;
    }
        
    /*
        Recurse through channel head downwards calling setposition to all dsp units.
    */
    if (0)
    {
    }
#ifdef FMOD_SUPPORT_DSPCODEC
    else if (mDSPCodec)
    {
        return mDSPCodec->setPosition(pcm, false);
    }
#endif
    else if (mDSP)
    {
        return mDSP->setPosition(pcm, true);
    }
    else if (mDSPWaveTable)
    {
        return mDSPWaveTable->setPosition(pcm, false);
    }
    else if (mDSPResampler)
    {
        return mDSPResampler->setPosition(pcm, true);
    }
    else 
    {
        return mDSPHead->setPosition(pcm, true);  /* Channel based sound is always wavetable, dspcodec, or resampler.  If this is called they must have done a dsp connection and it will flush. */
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::getPosition(unsigned int *position, FMOD_TIMEUNIT postype)
{
    int channels;
    FMOD_SOUND_FORMAT format;
    float frequency;
    bool getsubsoundtime = false;
    unsigned int pcmcurrent;
    int subsoundlistcurrent = mSubSoundListCurrent;

    if (!position)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    postype &= ~FMOD_TIMEUNIT_BUFFERED; /* Don't need that crap. */

    if (postype == FMOD_TIMEUNIT_SENTENCE_MS)
    {
        postype = FMOD_TIMEUNIT_MS;
        getsubsoundtime = true;
    }
    else if (postype == FMOD_TIMEUNIT_SENTENCE_PCM)
    {
        postype = FMOD_TIMEUNIT_PCM;
        getsubsoundtime = true;
    }
    else if (postype == FMOD_TIMEUNIT_SENTENCE_PCMBYTES)
    {
        postype = FMOD_TIMEUNIT_PCMBYTES;
        getsubsoundtime = true;
    }
    else if (postype == FMOD_TIMEUNIT_SENTENCE || postype == FMOD_TIMEUNIT_SENTENCE_SUBSOUND)
    {
        getsubsoundtime = true;
    }
    else if (postype != FMOD_TIMEUNIT_MS && postype != FMOD_TIMEUNIT_PCM && postype != FMOD_TIMEUNIT_PCMBYTES)
    {
        return FMOD_ERR_FORMAT;
    }
    
    if (getsubsoundtime 
#ifdef FMOD_SUPPORT_SENTENCING
        && !mSound->mSubSoundList
#endif
        )
    {
        return FMOD_ERR_INVALID_PARAM;
    }

#ifdef FMOD_SUPPORT_DSPCODEC
    if (mDSPCodec)
    {
        DSPResampler *dspcodec = SAFE_CAST(DSPResampler, mDSPCodec);
        if (!dspcodec)
        {
            return FMOD_ERR_INVALID_PARAM;
        }

        channels    = mDSPCodec->mDescription.channels;
        format      = mDSPCodec->mDescription.mFormat;
        frequency   = mSound->mDefaultFrequency;

        mPosition = dspcodec->mPosition.mHi;
        subsoundlistcurrent = mDSPCodec->mSubSoundListCurrent;
    }    
    else
#endif
    if (mSound && mDSPWaveTable)
    {
        DSPWaveTable *dspwave = SAFE_CAST(DSPWaveTable, mDSPWaveTable);
        if (!dspwave)
        {
            return FMOD_ERR_INTERNAL;
        }

        channels    = mSound->mChannels;
        format      = mSound->mFormat;
        frequency   = mSound->mDefaultFrequency;
    
        if (dspwave->mNewPosition != 0xFFFFFFFF)
        {
            mPosition = dspwave->mNewPosition;
        }
        else
        {
            mPosition = dspwave->mPosition.mHi;
        }
    }
    else if (mDSPResampler)
    {
        channels    = mDSPResampler->mDescription.channels;
        format      = FMOD_SOUND_FORMAT_PCMFLOAT;
        frequency   = mDSPResampler->mDefaultFrequency;
    }    
    else
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    pcmcurrent = mPosition;

#ifdef FMOD_SUPPORT_SENTENCING
    if (getsubsoundtime)
    {
        int count = 0;

        for (count = 0; count < mSound->mSubSoundListNum; count++)
        {
            unsigned int length = mSound->mSubSoundList[count].mLength;

            if (pcmcurrent >= length)
            {
                pcmcurrent -= length;
            }
            else
            {
                break;
            }
        }
    }
#endif

    if (postype == FMOD_TIMEUNIT_PCM)
    {
        *position = pcmcurrent;
    }
    else if (postype == FMOD_TIMEUNIT_PCMBYTES)
    {
        SoundI::getBytesFromSamples(pcmcurrent, position, channels, format);
    }
    else if (postype == FMOD_TIMEUNIT_MS)
    {
        *position = (unsigned int)((float)pcmcurrent / frequency * 1000.0f);
    }
    else if (postype == FMOD_TIMEUNIT_SENTENCE)
    {
        *position = mSubSoundListCurrent;
    }
#ifdef FMOD_SUPPORT_SENTENCING
    else if (postype == FMOD_TIMEUNIT_SENTENCE_SUBSOUND)
    {
        *position = mSound->mSubSoundList[subsoundlistcurrent].mIndex;
    }
#endif

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::setLoopPoints(unsigned int loopstart, unsigned int looplength)
{
    FMOD_RESULT result;
    
    result = ChannelReal::setLoopPoints(loopstart, looplength);
    if (result != FMOD_OK)
    {
        return result;
    }
    
#ifdef FMOD_SUPPORT_DSPCODEC

    if (mDSPCodec)
    {    
        mDSPCodec->mNoDMA->mLoopStart  = mLoopStart;
        mDSPCodec->mNoDMA->mLoopLength = mLoopLength;
    }   

#endif
    
    return FMOD_OK;
}

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::setLoopCount(int loopcount)
{
    FMOD_RESULT result;
    
    result = ChannelReal::setLoopCount(loopcount);
    if (result != FMOD_OK)
    {
        return result;
    }

#ifdef FMOD_SUPPORT_DSPCODEC

    if (mDSPCodec)
    {    
        mDSPCodec->mNoDMA->mNewLoopCount = mLoopCount;
        mDSPCodec->mNoDMA->mLoopCountIncrement++;
    }   

#endif
    
    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::setMode(FMOD_MODE mode)
{
    FMOD_RESULT result;
    
    result = ChannelReal::setMode(mode);
    if (result != FMOD_OK)
    {
        return result;
    }

#ifdef FMOD_SUPPORT_DSPCODEC

    if (mDSPCodec)
    {    
        mDSPCodec->mNoDMA->mMode = mMode;
    }   

#endif
 
    return FMOD_OK;
}

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::isPlaying(bool *isplaying, bool includethreadlatency)
{
    if (!isplaying)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (mFlags & CHANNELREAL_FLAG_ALLOCATED)
    {
        *isplaying = true;
    }
    else
    {
#ifdef FMOD_SUPPORT_DSPCODEC
        if (mDSPCodec)
        {
            mDSPCodec->getFinished(isplaying);
            *isplaying = !*isplaying;
        }
        else
#endif
        if (mDSPResampler)
        {
            mDSPResampler->getFinished(isplaying);
            *isplaying = !*isplaying;
        }
        else if (mDSPWaveTable)
        {
            if (!mSound)
            {
                *isplaying = false;
            }
            else
            {
                mDSPWaveTable->getFinished(isplaying);
                *isplaying = !*isplaying;
            }
        }
        else
        {
            *isplaying = false;
        }
    }

    if (!*isplaying)
    {
        mFlags &= ~(CHANNELREAL_FLAG_ALLOCATED | CHANNELREAL_FLAG_PLAYING);
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]
    Retrieves the spectrum from the currently playing output signal.

	[PARAMETERS]
    'spectrumarray'     Pointer to an array of floats to receive spectrum data.  Data range is 0-1.  Decibels = 10.0f * (float)log10(val) * 2.0f;
    'windowsize'        Number of PCM samples to analyze.  The resulting spectrum placed in the spectrumarray parameter will be HALF the size of this value (ie 1024 will place 512 floats in spectrumarray).  Must be a power of 2. (ie 128/256/512 etc).  Min = 128.  Max = 16384.
    'channeloffset'     Channel to analyze.  If the signal is multichannel (such as a stereo output), then this value represents which channel to analyze.  On a stereo signal 0 = left, 1 = right.
    'windowtype'        Pre-FFT window method.  This filters the PCM data before entering the spectrum analyzer to reduce transient frequency error for more accurate results.  See FMOD_DSP_FFT_WINDOW for different types of fft window techniques possible and for a more detailed explanation.
     
	[RETURN_VALUE]

	[REMARKS]
    The larger the windowsize, the more CPU the FFT will take.  Choose the right value to trade off between accuracy / speed.<br>
    The larger the windowsize, the more 'lag' the spectrum will seem to inherit.  This is because the window size stretches the analysis back in time to what was already played.  For example if the window size happened to be 44100 and the output rate was 44100 it would be analyzing the past second of data, and giving you the average spectrum over that time period.<br>
    If you are not displaying the result in dB, then the data may seem smaller than it should be.  To display it you may want to normalize the data - that is, find the maximum value in the resulting spectrum, and scale all values in the array by 1 / max.  (ie if the max was 0.5f, then it would become 1).<br>
    To get the spectrum for both channels of a stereo signal, call this function twice, once with channeloffset = 0, and again with channeloffset = 1.  Then add the spectrums together and divide by 2 to get the average spectrum for both channels.<br>

    [PLATFORMS]
    
	[SEE_ALSO]
    FMOD_DSP_FFT_WINDOW
]
*/
FMOD_RESULT ChannelSoftware::getSpectrum(float *spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype)
{
#ifdef FMOD_SUPPORT_GETSPECTRUM
    FMOD_RESULT result;

#ifdef FMOD_STATICFORPLUGINS

    result = FMOD_ERR_UNSUPPORTED;

#else

    DSPFilter      *dsphead;
    float          *buffer;
    unsigned int    position, length, bufferlength;
    int             numchannels, windowsize;
    static DSPFFT   fft;

    dsphead = (DSPFilter *)mDSPHead;
    if (!dsphead)
    {
        return FMOD_ERR_INITIALIZATION;
    }

    windowsize = numvalues * 2;

    if (windowsize != (1 <<  7) &&
        windowsize != (1 <<  8) &&
        windowsize != (1 <<  9) &&
        windowsize != (1 << 10) &&
        windowsize != (1 << 11) &&
        windowsize != (1 << 12) &&
        windowsize != (1 << 13) &&
        windowsize != (1 << 14))
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (mSound)
    {
        numchannels = mSound->mChannels;
    }
    else if (mDSP)
    {
        numchannels = mDSP->mDescription.channels;
    }
    else
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (channeloffset >= numchannels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    result = dsphead->startBuffering();
    if (result != FMOD_OK)
    {
        return result;
    }

    result = dsphead->getHistoryBuffer(&buffer, &position, &length);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (windowsize > (int)length)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    result = mSystem->getDSPBufferSize(&bufferlength, 0);
    
    position -= windowsize;
    if ((int)position < 0)
    {
        position += length;
    }

    result = fft.getSpectrum(buffer, position, length, spectrumarray, windowsize, channeloffset, numchannels, windowtype);

#endif
    return result;
#else
    return FMOD_ERR_UNSUPPORTED;
#endif // FMOD_SUPPORT_GETSPECTRUM
}


/*
[
	[DESCRIPTION]
    Retrieves the spectrum from the currently playing output signal.

	[PARAMETERS]
    'spectrumarray'     Pointer to an array of floats to receive spectrum data.  Data range is 0-1.  Decibels = 10.0f * (float)log10(val) * 2.0f;
    'windowsize'        Number of PCM samples to analyze.  The resulting spectrum placed in the spectrumarray parameter will be HALF the size of this value (ie 1024 will place 512 floats in spectrumarray).  Must be a power of 2. (ie 128/256/512 etc).  Min = 128.  Max = 16384.
    'channeloffset'     Channel to analyze.  If the signal is multichannel (such as a stereo output), then this value represents which channel to analyze.  On a stereo signal 0 = left, 1 = right.
    'windowtype'        Pre-FFT window method.  This filters the PCM data before entering the spectrum analyzer to reduce transient frequency error for more accurate results.  See FMOD_DSP_FFT_WINDOW for different types of fft window techniques possible and for a more detailed explanation.
     
	[RETURN_VALUE]

	[REMARKS]
    The larger the windowsize, the more CPU the FFT will take.  Choose the right value to trade off between accuracy / speed.<br>
    The larger the windowsize, the more 'lag' the spectrum will seem to inherit.  This is because the window size stretches the analysis back in time to what was already played.  For example if the window size happened to be 44100 and the output rate was 44100 it would be analyzing the past second of data, and giving you the average spectrum over that time period.<br>
    If you are not displaying the result in dB, then the data may seem smaller than it should be.  To display it you may want to normalize the data - that is, find the maximum value in the resulting spectrum, and scale all values in the array by 1 / max.  (ie if the max was 0.5f, then it would become 1).<br>
    To get the spectrum for both channels of a stereo signal, call this function twice, once with channeloffset = 0, and again with channeloffset = 1.  Then add the spectrums together and divide by 2 to get the average spectrum for both channels.<br>

    [PLATFORMS]
    
	[SEE_ALSO]
    FMOD_DSP_FFT_WINDOW
]
*/
FMOD_RESULT ChannelSoftware::getWaveData(float *wavearray, int numvalues, int channeloffset)
{
    FMOD_RESULT result;

#ifdef FMOD_STATICFORPLUGINS

    result = FMOD_ERR_UNSUPPORTED;

#else

    DSPFilter      *dsphead;
    float          *buffer;
    unsigned int    position, length;
    int             numchannels, count;

    dsphead = (DSPFilter *)mDSPHead;
    if (!dsphead)
    {
        return FMOD_ERR_INITIALIZATION;
    }

    if (mSound)
    {
        numchannels = mSound->mChannels;
    }
    else if (mDSP)
    {
        numchannels = mDSP->mDescription.channels;
    }
    else
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (channeloffset >= numchannels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    result = dsphead->startBuffering();
    if (result != FMOD_OK)
    {
        return result;
    }

    result = dsphead->getHistoryBuffer(&buffer, &position, &length);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (numvalues > (int)length)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
   
    position -= numvalues;
    if ((int)position < 0)
    {
        position += length;
    }

    for (count = 0; count < numvalues; count++)
    {
        wavearray[count] = buffer[position*numchannels+channeloffset];
        position++;
        if (position >= length)
        {
            position = 0;
        }
    }

#endif
    return result;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::getDSPHead(DSPI **dsp)
{   
    *dsp = mDSPHead;

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::addToReverbs(DSPI *dsptarget)
{
#ifdef FMOD_SUPPORT_REVERB
    FMOD_RESULT result;
    int          instance;

    if (!dsptarget)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    /*
        Add an input to standard reverb
    */
    for (instance = 0; instance < FMOD_REVERB_MAXINSTANCES; instance++)
    {
        if (mSystem->mReverbGlobal.mInstance[instance].mDSP)
        {
            DSPConnectionI *connection;

            result = mSystem->mReverbGlobal.mInstance[instance].mDSP->addInputQueued(dsptarget, false, 0, &connection);
            if (result != FMOD_OK)
            {
                return result;
            }

            mSystem->mReverbGlobal.setChanProperties(instance, mParent->mIndex, 0, connection);
            mSystem->mReverbGlobal.setPresenceGain(instance, mParent->mIndex, 1.0f);
        }
    }

#ifdef FMOD_SUPPORT_MULTIREVERB
    /*
        Add an input to 3d reverb
    */
    if (mSystem->mReverb3D.mInstance[0].mDSP)
    {
        DSPConnectionI *connection;

        result = mSystem->mReverb3D.mInstance[0].mDSP->addInputQueued(dsptarget, false, 0, &connection);
        if (result != FMOD_OK)
        {
            return result;
        }

        mSystem->mReverb3D.setChanProperties(0, mParent->mIndex, 0, connection);
        mSystem->mReverb3D.setPresenceGain(0, mParent->mIndex, 1.0f);
    }

    /*
        Traverse reverb list and add an input to each one that has a DSP
    */
    ReverbI *reverb_c = SAFE_CAST(ReverbI, mSystem->mReverb3DHead.getNext());
    while (reverb_c != &(mSystem->mReverb3DHead))
    {      
        if (reverb_c->mInstance[0].mDSP)
        {
            DSPConnectionI *connection;

            result = reverb_c->mInstance[0].mDSP->addInputQueued(dsptarget, false, 0, &connection);
            if (result != FMOD_OK)
            {
                return result;
            }

            reverb_c->setChanProperties(0, mParent->mIndex, 0, connection);
            reverb_c->setPresenceGain(0, mParent->mIndex, 1.0f);
        }
        reverb_c = SAFE_CAST(ReverbI, reverb_c->getNext());
    }
#endif

#endif    
    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
FMOD_RESULT ChannelSoftware::moveChannelGroup(ChannelGroupI *oldchannelgroup, ChannelGroupI *newchannelgroup, bool forcedspreconnect)
{
    FMOD_RESULT result;

    if (oldchannelgroup == newchannelgroup && !forcedspreconnect)
    {
        return FMOD_OK;
    }

    /*
        1. disconnect from previous channel group's head.
    */
    if (oldchannelgroup && oldchannelgroup->mDSPMixTarget)
    {
        result = oldchannelgroup->mDSPMixTarget->disconnectFrom(mDSPHead); 
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    /*
        2. reconnect to DSP group's head.
    */
    result = newchannelgroup->mDSPMixTarget->addInputQueued(mDSPHead, false, mDSPConnection, &mDSPConnection);
    if (result != FMOD_OK)
    {
        return result;
    }

    return FMOD_OK;
}


}

#endif
