#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_DSPCODEC

#include "fmod.h"
#include "fmod_channel_software.h"
#include "fmod_codeci.h"
#include "fmod_dspi.h"
#include "fmod_dsp_codec.h"
#include "fmod_sample_software.h"
#include "fmod_soundi.h"

#ifdef PLATFORM_PS3_SPU
    #include "fmod_systemi_spu.h"
    #include "fmod_spu_printf.h"
    #include <cell/dma.h>
#else
    #include "fmod_systemi.h"
#endif

#ifdef FMOD_SUPPORT_SENTENCING
    #include "fmod_codec_fsb.h"
#endif

namespace FMOD
{

FMOD_DSP_DESCRIPTION_EX dspcodec;

#ifdef PLUGIN_EXPORTS

#ifdef __cplusplus
extern "C" {
#endif

    /*
        FMODGetDSPDescription is mandantory for every fmod plugin.  This is the symbol the registerplugin function searches for.
        Must be declared with F_API to make it export as stdcall.
    */
    F_DECLSPEC F_DLLEXPORT FMOD_DSP_DESCRIPTION_EX * F_API FMODGetDSPDescriptionEx()
    {
        return DSPCodec::getDescriptionEx();
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
    
	[SEE_ALSO]
]
*/
FMOD_DSP_DESCRIPTION_EX *DSPCodec::getDescriptionEx()
{
    FMOD_memset(&dspcodec, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));

    FMOD_strcpy(dspcodec.name, "FMOD DSP Codec");
    dspcodec.version       = 0x00010100;
    dspcodec.create        = DSPCodec::createCallback;
    dspcodec.release       = DSPCodec::releaseCallback;
    dspcodec.reset         = DSPCodec::resetCallback;
    dspcodec.read          = DSPCodec::readCallback;
    dspcodec.setposition   = DSPCodec::setPositionCallback;

    dspcodec.numparameters = 0;
    dspcodec.paramdesc     = 0;
    dspcodec.setparameter  = DSPCodec::setParameterCallback;
    dspcodec.getparameter  = DSPCodec::getParameterCallback;

    dspcodec.mType         = (FMOD_DSP_TYPE)FMOD_DSP_TYPE_CODECREADER;
    dspcodec.mSize         = sizeof(DSPCodec);

    return &dspcodec;
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
FMOD_RESULT DSPCodec::createInternal()
{
    init();
    
    mNoDMA->mNewPosition = mNewPosition = 0xffffffff;
    mNoDMA->mSetPosIncrement = mSetPosIncrementPrev = 0;
    mNoDMA->mLoopCountIncrement = mLoopCountIncrementPrev = 0;
    
    return FMOD_OK;
}


#ifndef PLATFORM_PS3_SPU
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
FMOD_RESULT DSPCodec::release(bool freethis)
{
    FMOD_RESULT result;

    result = mCodec->mDescription.close(mCodec);
    if (result != FMOD_OK)
    {
        return result;
    }
    
#ifdef PLATFORM_PS3
    result = DSPResampler::release(freethis);
#else
    result = DSPResampler::release(false);

    if (freethis)
    {
        FMOD_Memory_FreeType(this, mWaveFormat.format == FMOD_SOUND_FORMAT_XMA ? FMOD_MEMORY_XBOX360_PHYSICAL : FMOD_MEMORY_NORMAL);
    }
#endif

    return result;
}
#endif


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
FMOD_RESULT DSPCodec::releaseInternal()
{
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
    
	[SEE_ALSO]
]
*/
FMOD_RESULT DSPCodec::resetInternal()
{
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
#ifdef FMOD_SUPPORT_SENTENCING
FMOD_RESULT DSPCodec::updateDSPCodec(SoundI *sound, int subsoundindex)
{
    Codec          *chancodec, *soundcodec;
    SampleSoftware *sample;
    
    #ifdef PLATFORM_PS3_SPU
    
    char  samplesoftwaredma  [sizeof(SampleSoftware)         + 32] __attribute__ ((aligned (16)));  // 336 + 32 bytes
    char  codecdma           [sizeof(CodecFSB)               + 32] __attribute__ ((aligned (16)));  // 220 + 32 bytes
    char  waveformatdma      [sizeof(FMOD_CODEC_WAVEFORMAT)  + 32] __attribute__ ((aligned (16)));  // 296 + 32 bytes

    unsigned int subsoundpointer = cellDmaGetUint32((uint64_t)&sound->mSubSound[subsoundindex], TAG1, TID, RID);

    FMOD_PS3_SPU_AlignedDMA((void **)&samplesoftwaredma, subsoundpointer, sizeof(SampleSoftware));

    sample      = (SampleSoftware *)(samplesoftwaredma);
    chancodec   = (Codec *)mCodec;

    FMOD_PS3_SPU_AlignedDMA((void **)&codecdma, (unsigned int)(sample->mCodec ? sample->mCodec : sample->mSubSoundParent->mCodec), sizeof(CodecFSB));

    soundcodec  = (Codec *)codecdma;


    /*
        Now, need to point the codec pointers to the right stuff
    */

    if (soundcodec->mType == FMOD_SOUND_TYPE_FSB)
    {
        soundcodec->mDescription.getwaveformat  = CodecFSB::getWaveFormatCallback;
    }
    else
    {
        FMOD_PS3_SPU_AlignedDMA((void **)&waveformatdma, (unsigned int)soundcodec->waveformat, sizeof(FMOD_CODEC_WAVEFORMAT));

        soundcodec->waveformat                  = (FMOD_CODEC_WAVEFORMAT *)waveformatdma;
        soundcodec->mDescription.getwaveformat  = Codec::defaultGetWaveFormat;
    }

    #else

    sample     = SAFE_CAST(SampleSoftware, sound->mSubSound[subsoundindex]);        /* PS3 - DMA THIS IN FROM PPU ADDRESS */
    chancodec  = SAFE_CAST(Codec, mCodec);                                          /* PS3 - DMA THIS IN FROM PPU ADDRESS */
    soundcodec = sample->mCodec ? sample->mCodec : sample->mSubSoundParent->mCodec; /* PS3 - DMA THIS IN FROM PPU ADDRESS */

    #endif

    if (!soundcodec || !chancodec->waveformat)
    {
        return FMOD_ERR_INTERNAL;
    }

    #if !defined(PLATFORM_PS3) && !defined(PLATFORM_WINDOWS_PS3MODE)
    if (!(sample->mMode & FMOD_CREATECOMPRESSEDSAMPLE))
    {
        return FMOD_ERR_FORMAT;
    }
    #endif

    soundcodec->mDescription.getwaveformat(soundcodec, sample->mSubSoundIndex, chancodec->waveformat);  /* Update the channelcodec using the sound codec wavedata */

    /*
        Reset the 'file' pointer to point to the new sound's buffer, and reset pos/length etc.
    */
    #ifdef PLATFORM_PS3
    mMemoryFile.init(mSystem, sample->mLengthBytes, 2048);
    mMemoryFile.mBuffer = mMemoryFile.mBufferMemoryPS3;
    #else
    mMemoryFile.init(mSystem, sample->mLengthBytes, 0);
    #endif

    mMemoryFile.mPosition = 0;    
    mMemoryFile.mMem = sample->mBuffer;

    /*
        Copy codec specific things from the source sound to the dspcodec used in this channel.
    */
    #if defined(FMOD_SUPPORT_FSB) && defined(FMOD_SUPPORT_XMA)
    if (sample->mType == FMOD_SOUND_TYPE_FSB && sample->mFormat == FMOD_SOUND_FORMAT_XMA)
    {
        CodecFSB  *fsb = (CodecFSB *)soundcodec;
        CodecXMA *destxma = (CodecXMA *)chancodec;
        int **seektable = (int **)fsb->plugindata;

        if (destxma->mXMASeekable)
        {
            destxma->mSeekTable = seektable[sample->mSubSoundIndex];
        }
    
        fsb->mDescription.getwaveformat(fsb, sample->mSubSoundIndex, chancodec->waveformat);
    }
    #endif
    
    return FMOD_OK;
}
#endif

#if defined(PLATFORM_PS3_SPU) && defined(FMOD_SUPPORT_SENTENCING)
/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
void DSPCodec_dmasubsoundlist(SoundI **soundi, void *soundimem, void *soundlistmem, int subsoundlistnum)
{
    void *lsaddress = soundimem;

    FMOD_PS3_SPU_AlignedDMA(&lsaddress, (unsigned int)*soundi, sizeof(SampleSoftware));

    *soundi = (SoundI *)lsaddress;

    lsaddress = soundlistmem;

    FMOD_PS3_SPU_AlignedDMA(&lsaddress, (unsigned int)(*soundi)->mSubSoundList, subsoundlistnum * sizeof(SoundSentenceEntry));

    (*soundi)->mSubSoundList = (SoundSentenceEntry *)lsaddress;
}

#endif

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT DSPCodec::readInternal(signed short *inbuffer, signed short *outbuffer, unsigned int length, int inchannels, int outchannels)
{   
    FMOD_RESULT result;
    unsigned int size = length;
    unsigned int bytespersample = 0;   
    int retries = 0;
    SoundI *sound = (SoundI *)mDescription.userdata;

    SoundI::getBytesFromSamples(1, &bytespersample, mDescription.channels, mDescription.mFormat);

    if (mFlags & FMOD_DSP_FLAG_QUEUEDFORDISCONNECT)
    {
        FMOD_memset(outbuffer, 0, length * bytespersample);
        return FMOD_OK;
    }
    
    /*
        mNewPositon for if setPosition is called on SPU (this happens with negative frequency playback on PS3)
    */
    if ((mNoDMA->mSetPosIncrement > mSetPosIncrementPrev && mNoDMA->mNewPosition != 0xFFFFFFFF) || mNewPosition != 0xFFFFFFFF)
    {
        unsigned int newpos = (mNewPosition != 0xFFFFFFFF) ? mNewPosition : mNoDMA->mNewPosition;

#ifdef FMOD_SUPPORT_SENTENCING
        if (sound)
        {
            unsigned int soundoffset;
            SoundSentenceEntry *entry;

            soundoffset          = 0;
            mSubSoundListCurrent = 0;

            #ifdef PLATFORM_PS3_SPU
            
            char  soundidmamem[sizeof(SampleSoftware) + 32];    // 336 + 32 bytes
            char *soundidma = (char *)FMOD_ALIGNPOINTER(soundidmamem, 16);

            char  subsoundlistdmamem[FMOD_PS3_SUBSOUNDLISTMAXITEMS * sizeof(SoundSentenceEntry) + 32];  // 4096 + 32 bytes
            char *subsoundlistdma = (char *)FMOD_ALIGNPOINTER(subsoundlistdmamem, 16);

            DSPCodec_dmasubsoundlist(&sound, soundidma, subsoundlistdma, mSubSoundListNum);

            #endif

            entry = &sound->mSubSoundList[0];   

            while (soundoffset + entry[mSubSoundListCurrent].mLength < newpos)
            {
                soundoffset += entry[mSubSoundListCurrent].mLength;
                mSubSoundListCurrent++;
            }

            newpos -= soundoffset;
        
            result = updateDSPCodec(sound, entry[mSubSoundListCurrent].mIndex);   /* Reset the codec pointer with the new subsound's info. */
            if (result != FMOD_OK)
            {
                return result;
            }

        }
#endif
        mPosition = (mNewPosition != 0xFFFFFFFF) ? mNewPosition : mNoDMA->mNewPosition;

        mCodec->reset();
        mCodec->setPosition(0, newpos, FMOD_TIMEUNIT_PCM);

        mSetPosIncrementPrev = mNoDMA->mSetPosIncrement;
        mNewPosition = 0xFFFFFFFF;
    }

    if (mNoDMA->mNewLoopCount >= -1 && mNoDMA->mLoopCountIncrement > mLoopCountIncrementPrev)
    {
        mLoopCount = mNoDMA->mNewLoopCount;
        mNoDMA->mNewLoopCount = -2;

        mLoopCountIncrementPrev = mNoDMA->mLoopCountIncrement;
    }
                
    while (size)
    {
        unsigned int endpoint, toread;
        unsigned int read;

        if (mNoDMA->mMode & FMOD_LOOP_NORMAL && mLoopCount)
        {
            endpoint = mNoDMA->mLoopStart + mNoDMA->mLoopLength - 1;
        }
        else
        {
            if (mCodec->mFlags & FMOD_CODEC_ACCURATELENGTH)
            {
                endpoint = mLength - 1;
            }
            else
            {
                endpoint = (unsigned int)-1;
            }
        }

        toread = size;

        if (mPosition > endpoint)
        {
            toread = 0;
        }
        else if (mPosition + toread > endpoint)
        {
            toread = (endpoint - mPosition) + 1;
        }

        result = mCodec->read(outbuffer, toread * bytespersample, &read);
        if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
        {
            return result;
        }

        read /= bytespersample;

        outbuffer += (read * inchannels);
        mPosition += read;

        if (read <= size)
        {
            size -= read;
        }
        else
        {
            size = 0;
        }

#if 0
        if (!read)
        {
            if (retries > 255)    /* Sometimes the XMA codec returns 0 if it is not ready yet.  20 is the worst i've seen playing 32 xmas so 255 should be plenty. */
            {
                result = FMOD_ERR_FILE_EOF;
                #ifdef PLATFORM_XENON
                OutputDebugString("FMOD ************* ERROR! XMA hardware timeout.\n");
                #endif
            }
            retries++;
        }
#endif        
 
        if ((mDirection == DSPRESAMPLER_SPEEDDIR_BACKWARDS && mPosition == read) ||
            (result == FMOD_ERR_FILE_EOF || mPosition > endpoint))
        {
#ifdef FMOD_SUPPORT_SENTENCING
            if (mDirection == DSPRESAMPLER_SPEEDDIR_FORWARDS && sound && mPosition < endpoint && mSubSoundListCurrent < mSubSoundListNum - 1)
            {
                SoundSentenceEntry *entry;
                int firstcurrent = 0;

                mSubSoundListCurrent++;
                
                /* PS3 - DMA SOUNDI, then DMA SUBSOUNDLIST (make a function to keep it neat) */

                #ifdef PLATFORM_PS3_SPU
                
                char  soundidmamem[sizeof(SampleSoftware) + 32];    // 336 + 32 bytes
                char *soundidma = (char *)FMOD_ALIGNPOINTER(soundidmamem, 16);

                char  subsoundlistdmamem[FMOD_PS3_SUBSOUNDLISTMAXITEMS * sizeof(SoundSentenceEntry) + 32];  // 4096 + 32 bytes
                char *subsoundlistdma = (char *)FMOD_ALIGNPOINTER(subsoundlistdmamem, 16);

                DSPCodec_dmasubsoundlist(&sound, soundidma, subsoundlistdma, mSubSoundListNum);

                #endif

                entry = &sound->mSubSoundList[0];   


                firstcurrent = mSubSoundListCurrent; 
                while (!entry[mSubSoundListCurrent].mLength)
                {
                    mSubSoundListCurrent++;
            
                    if (mSubSoundListCurrent >= mSubSoundListNum)
                    {
                        mSubSoundListCurrent = 0;
                    }

                    if (mSubSoundListCurrent == firstcurrent)   /* Went right around and found no entries. EOF */
                    {
                        return FMOD_ERR_FILE_EOF;
                    }
                }

                result = updateDSPCodec(sound, sound->mSubSoundList[mSubSoundListCurrent].mIndex);   /* Reset the codec pointer with the new subsound's info. */
                if (result != FMOD_OK)
                {
                    return result;
                }
            }
            else
#endif

            if (mNoDMA->mMode & FMOD_LOOP_NORMAL && mLoopCount && retries <= 255)
            {
                unsigned int newpos = mNoDMA->mLoopStart;

#ifdef FMOD_SUPPORT_SENTENCING
                if (sound)
                {
                    unsigned int soundoffset;
                    SoundSentenceEntry *entry;

                    soundoffset    = 0;
                    mSubSoundListCurrent = 0;

                    /* PS3 - DMA SOUNDI, then DMA SUBSOUNDLIST (make a function to keep it neat) */

                    #ifdef PLATFORM_PS3_SPU
                    
                    char  soundidmamem[sizeof(SampleSoftware) + 32];    // 336 + 32 bytes
                    char *soundidma = (char *)FMOD_ALIGNPOINTER(soundidmamem, 16);

                    char  subsoundlistdmamem[FMOD_PS3_SUBSOUNDLISTMAXITEMS * sizeof(SoundSentenceEntry) + 32];  // 4096 + 32 bytes
                    char *subsoundlistdma = (char *)FMOD_ALIGNPOINTER(subsoundlistdmamem, 16);

                    DSPCodec_dmasubsoundlist(&sound, soundidma, subsoundlistdma, mSubSoundListNum);

                    #endif

                    entry = &sound->mSubSoundList[0];   

                    while (soundoffset + entry[mSubSoundListCurrent].mLength < newpos || !entry[mSubSoundListCurrent].mLength)
                    {
                        soundoffset += entry[mSubSoundListCurrent].mLength;
                        mSubSoundListCurrent++;
                  
                        if (mSubSoundListCurrent >= mSubSoundListNum)
                        {
                            return FMOD_ERR_FILE_EOF;
                        }
                    }

                    newpos -= soundoffset;
    
                    result = updateDSPCodec(sound, entry[mSubSoundListCurrent].mIndex);   /* Reset the codec pointer with the new subsound's info. */
                    if (result != FMOD_OK)
                    {
                        return result;
                    }

                }
#endif
                mPosition = mNoDMA->mLoopStart;
                mCodec->setPosition(0, newpos, FMOD_TIMEUNIT_PCM);

                if (mLoopCount > 0)
                {
                    mLoopCount--;
                }
            }
            else
            {
                if (size)
                {
                    FMOD_memset(outbuffer, 0, size * bytespersample);
                }
                return FMOD_ERR_FILE_EOF;
            }
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
    
	[SEE_ALSO]
]
*/
FMOD_RESULT DSPCodec::setPositionInternal(unsigned int position, bool fromspu)
{      
#ifdef PLATFORM_PS3
    if (fromspu)
    {
        mNewPosition = position;
    }
    else
#endif
    {
        mNoDMA->mNewPosition = position;

        mNoDMA->mSetPosIncrement++;
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
    
	[SEE_ALSO]
]
*/
FMOD_RESULT DSPCodec::getPositionInternal(unsigned int *position)
{
    if (mNoDMA->mSetPosIncrement > mSetPosIncrementPrev)
    {
        *position = mNoDMA->mNewPosition;
    }
    else
    {
        *position = mPosition;
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
    
	[SEE_ALSO]
]
*/
FMOD_RESULT DSPCodec::setParameterInternal(int index, float value)
{
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
FMOD_RESULT DSPCodec::getParameterInternal(int index, float *value, char *valuestr)
{
    return FMOD_OK;
}


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

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK DSPCodec::createCallback(FMOD_DSP_STATE *dsp)
{
    DSPCodec *dspcodec = (DSPCodec *)dsp;

    return dspcodec->createInternal();
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
FMOD_RESULT F_CALLBACK DSPCodec::releaseCallback(FMOD_DSP_STATE *dsp)
{
    DSPCodec *dspcodec = (DSPCodec *)dsp;

    return dspcodec->releaseInternal();
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
FMOD_RESULT F_CALLBACK DSPCodec::resetCallback(FMOD_DSP_STATE *dsp)
{
    DSPCodec *dspcodec = (DSPCodec *)dsp;

    return dspcodec->resetInternal();
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
FMOD_RESULT F_CALLBACK DSPCodec::readCallback(FMOD_DSP_STATE *dsp, float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int outchannels)
{
    DSPCodec *dspcodec = (DSPCodec *)dsp;

    return dspcodec->readInternal((signed short *)inbuffer, (signed short *)outbuffer, length, inchannels, outchannels);
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
FMOD_RESULT F_CALLBACK DSPCodec::setPositionCallback(FMOD_DSP_STATE *dsp, unsigned int pos)
{
    DSPCodec *dspcodec = (DSPCodec *)dsp;

    return dspcodec->setPositionInternal(pos);
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
FMOD_RESULT F_CALLBACK DSPCodec::setParameterCallback(FMOD_DSP_STATE *dsp, int index, float value)
{
    DSPCodec *dspcodec = (DSPCodec *)dsp;

    return dspcodec->setParameterInternal(index, value);
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
FMOD_RESULT F_CALLBACK DSPCodec::getParameterCallback(FMOD_DSP_STATE *dsp, int index, float *value, char *valuestr)
{
    DSPCodec *dspcodec = (DSPCodec *)dsp;

    return dspcodec->getParameterInternal(index, value, valuestr);
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

#ifdef FMOD_SUPPORT_MEMORYTRACKER

FMOD_RESULT DSPCodec::getMemoryUsedImpl(MemoryTracker *tracker)
{
        #ifdef PLATFORM_PS3
//        DMAFile         mMemoryFile;
        #else
//        MemoryFile      mMemoryFile;
        #endif   
        
//        Codec          *mCodec;

    return FMOD_OK;
}

#endif


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

#ifdef FMOD_SUPPORT_MEMORYTRACKER
#ifdef FMOD_SUPPORT_MPEG

FMOD_RESULT DSPCodecMPEG::getMemoryUsedImpl(MemoryTracker *tracker)
{
    tracker->add(false, FMOD_MEMBITS_DSPCODEC, sizeof(*this));

//        CodecMPEG             mCodecMemory;
//    CHECK_RESULT(mCodecMemory.getMemoryUsed(tracker));

//??? already counted up here   CHECK_RESULT(DSPCodec::getMemoryUsed(tracker));

    return FMOD_OK;
}

#endif
#endif

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

#ifdef FMOD_SUPPORT_MEMORYTRACKER
#ifdef FMOD_SUPPORT_IMAADPCM

FMOD_RESULT DSPCodecADPCM::getMemoryUsedImpl(MemoryTracker *tracker)
{
    tracker->add(false, FMOD_MEMBITS_DSPCODEC, sizeof(*this));

//        char                 mResampleBufferMemory[((64 + (FMOD_DSP_RESAMPLER_OVERFLOWLENGTH * 4)) * sizeof(short) * 2 * 2) + 16];    // *2 = stereo max.  *2 = double buffer.
//        CodecWav             mCodecMemory;

//    CHECK_RESULT(DSPCodec::getMemoryUsed(tracker));

    return FMOD_OK;
}

#endif
#endif

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

#ifdef FMOD_SUPPORT_MEMORYTRACKER
#ifdef FMOD_SUPPORT_XMA

FMOD_RESULT DSPCodecXMA::getMemoryUsedImpl(MemoryTracker *tracker)
{
    tracker->add(false, FMOD_MEMBITS_DSPCODEC, sizeof(*this));

//        char               mResampleBufferMemory[((512 + (FMOD_DSP_RESAMPLER_OVERFLOWLENGTH * 4)) * sizeof(short) * 2 * 2) + 16];      // *2 = stereo max.  *2 = double buffer.
//        CodecXMA           mCodecMemory;
//        CodecXMA_DecoderHW mCodecMemoryBlock;

//    CHECK_RESULT(DSPCodec::getMemoryUsed(tracker));

    return FMOD_OK;
}

#endif
#endif

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

#ifdef FMOD_SUPPORT_MEMORYTRACKER
#ifdef FMOD_SUPPORT_CELT

FMOD_RESULT DSPCodecCELT::getMemoryUsedImpl(MemoryTracker *tracker)
{
    tracker->add(false, FMOD_MEMBITS_DSPCODEC, sizeof(*this));

//        char               mResampleBufferMemory[((512 + (FMOD_DSP_RESAMPLER_OVERFLOWLENGTH * 4)) * sizeof(short) * 2 * 2) + 16];      // *2 = stereo max.  *2 = double buffer.
//        CodecXMA           mCodecMemory;
//        CodecXMA_DecoderHW mCodecMemoryBlock;

//    CHECK_RESULT(DSPCodec::getMemoryUsed(tracker));

    return FMOD_OK;
}

#endif
#endif

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

#ifdef FMOD_SUPPORT_MEMORYTRACKER
#ifdef FMOD_SUPPORT_RAW

FMOD_RESULT DSPCodecRaw::getMemoryUsedImpl(MemoryTracker *tracker)
{
    tracker->add(false, FMOD_MEMBITS_DSPCODEC, sizeof(*this));

//        char      mResampleBufferMemory[((256 + (FMOD_DSP_RESAMPLER_OVERFLOWLENGTH * 4)) * sizeof(short) * 16 * 2) + 16];      // *16 = 16 channel max.  *2 = double buffer.
//        CodecRaw  mCodecMemory;

//    CHECK_RESULT(DSPCodec::getMemoryUsed(tracker));

    return FMOD_OK;
}

#endif
#endif

}

#endif
