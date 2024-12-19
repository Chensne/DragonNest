#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_DSPCODEC

#include "fmod_channel_software.h"
#ifdef FMOD_SUPPORT_MPEG
#include "fmod_codec_mpeg.h"
#endif
#ifdef FMOD_SUPPORT_XMA
#include "fmod_codec_xma.h"
#endif
#ifdef FMOD_SUPPORT_IMAADPCM
#include "fmod_codec_wav_imaadpcm.h"
#endif
#include "fmod_autocleanup.h"
#include "fmod_codeci.h"
#include "fmod_dspi.h"
#include "fmod_dsp_codec.h"
#include "fmod_dsp_codecpool.h"
#include "fmod_pluginfactory.h"
#include "fmod_systemi.h"
#include "fmod_localcriticalsection.h"

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

class DSPCodecPoolInitCleanup
{
public:
    DSPCodecPoolInitCleanup()
        : mPool(0), mPoolSize(0)
    { }

    void setPool(DSPCodec **pool, int poolsize)
    {
        mPool = pool;
        mPoolSize = poolsize;
    }

    void cleanup(bool)
    {
        if (mPool)
        {
            for (int i = 0; i < mPoolSize; ++i)
            {
                if (mPool[i])
                {
                    mPool[i]->release();
                }
            }
        }
    }

private:
    DSPCodec **mPool;
    int mPoolSize;
};

FMOD_RESULT DSPCodecPool::init(FMOD_DSP_CATEGORY category, int resamplerpcmblocksize, int numdspcodecs)
{
    int count;
    FMOD_DSP_DESCRIPTION_EX descriptionex;
    AutoCleanup<bool, DSPCodecPoolInitCleanup> initCleanup(true);

    if (!mSystem->mSoftware)
    {
        return FMOD_ERR_NEEDSSOFTWARE;
    }

    if (numdspcodecs >= FMOD_DSP_CODECPOOL_MAXCODECS)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    LocalCriticalSection crit(mSystem->mDSPCodecPoolInitCrit, true);

    if (mNumDSPCodecs > 0)
    {
        // already initialised (probably by another thread)
        if(mNumDSPCodecs != numdspcodecs)
        {
            return FMOD_ERR_INTERNAL;
        }
        else
        {
            return FMOD_OK;
        }
    }

    mPool = (DSPCodec **)FMOD_Memory_CallocType(sizeof(DSPCodec *) * numdspcodecs, FMOD_MEMORY_PERSISTENT);
    if (!mPool)
    {
        return FMOD_ERR_MEMORY;
    }
    initCleanup.setPool(mPool, numdspcodecs);

    #if defined(FMOD_SUPPORT_XMA) && defined(FMOD_SUPPORT_XMA_NEWHAL)
    if (category == FMOD_DSP_CATEGORY_DSPCODECXMA)
    {
        mFileBufferPool = (unsigned char *)FMOD_Memory_CallocType(((2048 * 2) * numdspcodecs) + 2048, FMOD_MEMORY_PERSISTENT | FMOD_MEMORY_XBOX360_PHYSICAL);   /* + 2048 for alignment. */
        if (!mFileBufferPool)
        {
            return FMOD_ERR_MEMORY;
        }
    }
    #endif
    
    descriptionex = *DSPCodec::getDescriptionEx();
    
    for (count = 0; count < numdspcodecs; count++)
    {
        FMOD_RESULT           result;
        DSPI                  *dsp;
        DSPCodec              *dspcodec;

        descriptionex.channels = 2;                       // Maximum possible stereo? This is for memory allocation.
        descriptionex.mFormat = FMOD_SOUND_FORMAT_PCM16;
        descriptionex.mCategory = category;
        descriptionex.mResamplerBlockLength = resamplerpcmblocksize;

        if (0)
        {
        }
        #ifdef FMOD_SUPPORT_MPEG
        if (category == FMOD_DSP_CATEGORY_DSPCODECMPEG)
        {   
            descriptionex.mSize = sizeof(DSPCodecMPEG);
        }
        #endif
        #ifdef FMOD_SUPPORT_XMA
        else if (category == FMOD_DSP_CATEGORY_DSPCODECXMA)
        {
            descriptionex.mSize = sizeof(DSPCodecXMA);
        }
        #endif
        #ifdef FMOD_SUPPORT_IMAADPCM
        else if (category == FMOD_DSP_CATEGORY_DSPCODECADPCM)
        {
            descriptionex.mSize = sizeof(DSPCodecADPCM);
        }
        #endif
        #ifdef FMOD_SUPPORT_CELT
        else if (category == FMOD_DSP_CATEGORY_DSPCODECCELT)
        {
            descriptionex.mSize = sizeof(DSPCodecCELT);
        }
        #endif
        #ifdef FMOD_SUPPORT_RAW
        else if (category == FMOD_DSP_CATEGORY_DSPCODECRAW)
        {
            descriptionex.mResamplerBlockLength = 256;
            descriptionex.mSize = sizeof(DSPCodecRaw);
        }
        #endif
        else
        {
            return FMOD_ERR_FORMAT;
        }

        result = mSystem->createDSP(&descriptionex, &dsp);
        if (result != FMOD_OK)
        {
            return result;
        }
        AutoRelease<DSPI> dsp_cleanup(dsp); 

        dspcodec = SAFE_CAST(DSPCodec, dsp);
        
        dspcodec->mFlags = 0;

        if (0)
        {
        }
        #ifdef FMOD_SUPPORT_MPEG
        if (category == FMOD_DSP_CATEGORY_DSPCODECMPEG)
        {   
            DSPCodecMPEG *dspcodecmpeg = SAFE_CAST(DSPCodecMPEG, dspcodec);
            CodecMPEG    *mpeg = &dspcodecmpeg->mCodecMemory;
            FMOD_CODEC_DESCRIPTION_EX *desc;
            
            result = mSystem->mPluginFactory->getCodec(mSystem->mMPEGPluginHandle, &desc);
            if (result != FMOD_OK)
            {
                return result;
            }
            FMOD_memcpy(&mpeg->mDescription, desc, sizeof(FMOD_CODEC_DESCRIPTION_EX));
            mpeg->mDescription.getwaveformat = &Codec::defaultGetWaveFormat;

            dspcodec->mCodec = mpeg;
            dspcodec->mWaveFormat.format = FMOD_SOUND_FORMAT_MPEG;

            mpeg->mMemoryBlock = &dspcodecmpeg->mCodecMemoryBlock;
        }
        #endif
        #ifdef FMOD_SUPPORT_XMA
        else if (category == FMOD_DSP_CATEGORY_DSPCODECXMA)
        {
            DSPCodecXMA *dspcodecxma = SAFE_CAST(DSPCodecXMA, dspcodec);
            CodecXMA    *xma = &dspcodecxma->mCodecMemory;
            
            FMOD_memcpy(&xma->mDescription, CodecXMA::getDescriptionEx(), sizeof(FMOD_CODEC_DESCRIPTION_EX));
            xma->mDescription.getwaveformat = &Codec::defaultGetWaveFormat;
            dspcodec->mCodec = xma;
            dspcodec->mWaveFormat.format = FMOD_SOUND_FORMAT_XMA;

#ifdef FMOD_SUPPORT_XMA_NEWHAL
            xma->mDecoder = &dspcodecxma->mCodecMemoryBlock;
            xma->mDecoder->mFileBuffer = (unsigned char *)(FMOD_ALIGNPOINTER(mFileBufferPool, 2048) + (2048 * 2 * count));
#else
            xma->mDecoder = xma->mDecoderHW = &dspcodecxma->mCodecMemoryBlock;
#endif                    
        }
        #endif
        #ifdef FMOD_SUPPORT_IMAADPCM
        else if (category == FMOD_DSP_CATEGORY_DSPCODECADPCM)
        {
            DSPCodecADPCM *dspcodecadpcm = SAFE_CAST(DSPCodecADPCM, dspcodec);
            CodecWav      *wav = &dspcodecadpcm->mCodecMemory;
            FMOD_CODEC_DESCRIPTION_EX *desc;
            
            result = mSystem->mPluginFactory->getCodec(mSystem->mWAVPluginHandle, &desc);
            if (result != FMOD_OK)
            {
                return result;
            }
            FMOD_memcpy(&wav->mDescription, desc, sizeof(FMOD_CODEC_DESCRIPTION_EX));
            wav->mDescription.getwaveformat = &Codec::defaultGetWaveFormat;

            dspcodec->mCodec = wav;
            dspcodec->mWaveFormat.format = FMOD_SOUND_FORMAT_IMAADPCM;
        }
        #endif
        #ifdef FMOD_SUPPORT_CELT
        else if (category == FMOD_DSP_CATEGORY_DSPCODECCELT)
        {
            DSPCodecCELT *dspcodeccelt = SAFE_CAST(DSPCodecCELT, dspcodec);
            CodecCELT    *celt         = &dspcodeccelt->mCodecMemory;
            FMOD_CODEC_DESCRIPTION_EX *desc;
            
            result = mSystem->mPluginFactory->getCodec(mSystem->mCELTPluginHandle, &desc);
            if (result != FMOD_OK)
            {
                return result;
            }

            FMOD_memcpy(&celt->mDescription, desc, sizeof(FMOD_CODEC_DESCRIPTION_EX));
            celt->mDescription.getwaveformat = &Codec::defaultGetWaveFormat;

            dspcodec->mCodec = celt;
            dspcodec->mWaveFormat.format = FMOD_SOUND_FORMAT_CELT;
        }
        #endif
        #ifdef FMOD_SUPPORT_RAW
        else if (category == FMOD_DSP_CATEGORY_DSPCODECRAW)
        {
            DSPCodecRaw *dspcodecraw = SAFE_CAST(DSPCodecRaw, dspcodec);
            CodecRaw    *raw = &dspcodecraw->mCodecMemory;

            FMOD_memcpy(&raw->mDescription, CodecRaw::getDescriptionEx(), sizeof(FMOD_CODEC_DESCRIPTION_EX));
            raw->mDescription.getwaveformat = &Codec::defaultGetWaveFormat;
            
            dspcodec->mCodec = raw;
            dspcodec->mWaveFormat.format = FMOD_SOUND_FORMAT_PCM16;
        }
        #endif
        else
        {
            return FMOD_ERR_FORMAT;
        }

        dspcodec->mCodec->mFile           = &dspcodec->mMemoryFile;
        dspcodec->mCodec->waveformat      = &dspcodec->mWaveFormat;
        dspcodec->mCodec->mSrcDataOffset  = 0;                          /* Raw data is going to be placed right at the start. */
        dspcodec->mCodec->mFlags         |= FMOD_CODEC_ACCURATELENGTH;
        dspcodec->mPool                   = this;
        dspcodec->mPoolIndex              = count;
        dspcodec->setFinished(true, true);                              /* Start off finished so that it can be allocated. */
        mAllocated[count] = false;

        dsp_cleanup.releasePtr();
        mPool[count] = dspcodec;
    }

    // set this last so places that access it don't jump in too early
    mNumDSPCodecs = numdspcodecs;
    
    initCleanup.releasePtr();
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
FMOD_RESULT DSPCodecPool::close()
{
    if (mPool)
    {
        int count;
        
        for (count = 0; count < mNumDSPCodecs; count++)
        {
            DSPCodec *dspcodec = SAFE_CAST(DSPCodec, mPool[count]);

            if (dspcodec)
            {          
                dspcodec->mCodec->mFile = 0;             /* Stop it trying to free our static memory file. */
                dspcodec->mCodec->mReadBuffer = 0;       /* Stop it trying to free the global read buffer multiple times if it happens to be pointing to it. */
                dspcodec->mCodec->mWaveFormatMemory = 0; /* Stop it trying to free the sound's wave format if it happens to be pointing to it. */

                mPool[count]->release();
            }
        }

        FMOD_Memory_Free(mPool);
        mPool = 0;        
        mNumDSPCodecs = 0;
    }

    if (mReadBuffer)
    {
        FMOD_Memory_Free(mReadBuffer);
        mReadBuffer = 0;
    }

    #if defined(FMOD_SUPPORT_XMA) && defined(FMOD_SUPPORT_XMA_NEWHAL)
    if (mFileBufferPool)
    {
        FMOD_Memory_FreeType(mFileBufferPool, FMOD_MEMORY_PERSISTENT | FMOD_MEMORY_XBOX360_PHYSICAL);
        mFileBufferPool = 0;
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
FMOD_RESULT DSPCodecPool::alloc(DSPCodec **dspcodec)
{
    int count;

    for (count = 0; count < mNumDSPCodecs; count++)
    {
        bool finished;
        
        mPool[count]->getFinished(&finished);

        if (!mAllocated[count] && finished)
        {
            mAllocated[count] = true;    
            *dspcodec = mPool[count];
            return FMOD_OK;
        }
    }

    return FMOD_ERR_CHANNEL_ALLOC;
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
FMOD_RESULT DSPCodecPool::areAnyFree()
{
    int count;

    for (count = 0; count < mNumDSPCodecs; count++)
    {
        bool finished;

        mPool[count]->getFinished(&finished);

        if (!mAllocated[count] && finished)
        {
            return FMOD_OK;
        }
    }

    return FMOD_ERR_CHANNEL_ALLOC;
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

FMOD_RESULT DSPCodecPool::getMemoryUsedImpl(MemoryTracker *tracker)
{
    if (mPool)
    {
        tracker->add(false, FMOD_MEMBITS_DSPCODEC, sizeof(DSPCodec *) * mNumDSPCodecs);

        for (int i=0; i < mNumDSPCodecs; i++)
        {
            if (mPool[i])
            {
                switch (mPool[i]->mDescription.mCategory)
                {
#ifdef FMOD_SUPPORT_MPEG
                    case FMOD_DSP_CATEGORY_DSPCODECMPEG :
                    {
                        CHECK_RESULT(((DSPCodecMPEG *)mPool[i])->getMemoryUsed(tracker));
                        break;
                    }
#endif

#ifdef FMOD_SUPPORT_IMAADPCM
                    case FMOD_DSP_CATEGORY_DSPCODECADPCM :
                    {
                        CHECK_RESULT(((DSPCodecADPCM *)mPool[i])->getMemoryUsed(tracker));
                        break;
                    }
#endif

#ifdef FMOD_SUPPORT_XMA
                    case FMOD_DSP_CATEGORY_DSPCODECXMA :
                    {
                        CHECK_RESULT(((DSPCodecXMA *)mPool[i])->getMemoryUsed(tracker));
                        break;
                    }
#endif

#ifdef FMOD_SUPPORT_CELT
                    case FMOD_DSP_CATEGORY_DSPCODECCELT :
                    {
                        CHECK_RESULT(((DSPCodecCELT *)mPool[i])->getMemoryUsed(tracker));
                        break;
                    }
#endif

#ifdef FMOD_SUPPORT_RAW
                    case FMOD_DSP_CATEGORY_DSPCODECRAW :
                    {
                        CHECK_RESULT(((DSPCodecRaw *)mPool[i])->getMemoryUsed(tracker));
                        break;
                    }
#endif

                    default :
                        break;
                }
            }
        }
    }

    return FMOD_OK;
}

#endif

}

#endif
