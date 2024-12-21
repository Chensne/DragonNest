#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_FSB

#include "fmod.h"
#include "fmod_async.h"
#include "fmod_codec_fsb.h"
#include "fmod_debug.h"
#include "fmod_dsp_codec.h"
#include "fmod_file.h"
#include "fmod_metadata.h"
#include "fmod_outputi.h"
#include "fmod_soundi.h"
#include "fmod_speakermap.h"
#include "fmod_string.h"

#ifdef PLATFORM_PS3_SPU
    #include "fmod_common_spu.h"
    #include "fmod_spu_printf.h"
    #include "fmod_systemi_spu.h"
    #include <cell/dma.h>
#else
    #include "fmod_systemi.h"
#endif

#ifdef FMOD_SUPPORT_IMAADPCM
    #include "fmod_codec_wav_imaadpcm.h"
#endif

#ifdef FMOD_SUPPORT_GCADPCM
    #include "fmod_codec_gcadpcm.h"
#endif

#ifdef FMOD_SUPPORT_XMA
    #include "fmod_codec_xma.h"
#endif

#ifdef FMOD_SUPPORT_MPEG
    #include "fmod_codec_mpeg.h"
#endif

#ifdef FMOD_SUPPORT_VAG
    #include "fmod_codec_swvag.h"
#endif

#ifdef FMOD_SUPPORT_CELT
    #include "fmod_codec_celt.h"
#endif

namespace FMOD
{

#ifndef PLATFORM_PS3_SPU

FMOD_CODEC_DESCRIPTION_EX fsbcodec;


#ifdef PLUGIN_EXPORTS

#ifdef __cplusplus
extern "C" {
#endif

    /*
        FMODGetCodecDescription is mandatory for every fmod plugin.  This is the symbol the registerplugin function searches for.
        Must be declared with F_API to make it export as stdcall.
    */
    F_DECLSPEC F_DLLEXPORT FMOD_CODEC_DESCRIPTION_EX * F_API FMODGetCodecDescriptionEx()
    {
        return CodecFSB::getDescriptionEx();
    }

#ifdef __cplusplus
}
#endif

#endif  /* PLUGIN_EXPORTS */


#ifdef FMOD_FSB_USEHEADERCACHE
CodecFSBCache CodecFSB::gCacheHead;
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
FMOD_CODEC_DESCRIPTION_EX *CodecFSB::getDescriptionEx()
{
    FMOD_memset(&fsbcodec, 0, sizeof(FMOD_CODEC_DESCRIPTION_EX));

    fsbcodec.name          = "FMOD FSB Codec";
    fsbcodec.version       = 0x00010100;
    fsbcodec.timeunits     = FMOD_TIMEUNIT_PCM | FMOD_TIMEUNIT_RAWBYTES;
    fsbcodec.open          = &CodecFSB::openCallback;
    fsbcodec.close         = &CodecFSB::closeCallback;
    fsbcodec.read          = &CodecFSB::readCallback;
    fsbcodec.setposition   = &CodecFSB::setPositionCallback;
    fsbcodec.getposition   = &CodecFSB::getPositionCallback;
    fsbcodec.soundcreate   = &CodecFSB::soundcreateCallback;
    fsbcodec.getwaveformat = &CodecFSB::getWaveFormatCallback;
    fsbcodec.reset         = &CodecFSB::resetCallback;
    fsbcodec.canpoint      = &CodecFSB::canPointCallback;
#ifdef FMOD_SUPPORT_MEMORYTRACKER
    fsbcodec.getmemoryused = &CodecFSB::getMemoryUsedCallback;
#endif

    fsbcodec.mType         = FMOD_SOUND_TYPE_FSB;
    fsbcodec.mSize         = sizeof(CodecFSB);

    return &fsbcodec;
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
FMOD_RESULT CodecFSB::openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO *userexinfo)
{
    unsigned int    rd;
    int             count, numsamples, subsoundindex;
    char           *shdrblock = 0;
    FMOD_RESULT     result = FMOD_OK;
    unsigned int    offset, soffset;
    bool            mixedchannels = false;

    init(FMOD_SOUND_TYPE_FSB);

#ifdef FMOD_FSB_USEHEADERCACHE
    mCacheEntry = 0;
#endif


    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "CodecFSB::openInternal", "attempting to open as FSB..\n"));

	result = mFile->seek(0, SEEK_SET);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->read(&mHeader, 1, sizeof(FMOD_FSB_HEADER), &rd);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (rd != sizeof(FMOD_FSB_HEADER))
    {
        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "CodecFSB::openInternal", "rd != sizeof(FMOD_FSB_HEADER)\n"));
        return FMOD_ERR_FILE_BAD;
    }

    /*
        If it failed, use old method of decryption to check if it is an older FSB.
    */
    if (userexinfo && userexinfo->encryptionkey && (FMOD_strncmp(mHeader.id, "FSB3", 4) && FMOD_strncmp(mHeader.id, "FSB4", 4)))
    {
        int offset = 0, keylen = FMOD_strlen(userexinfo->encryptionkey);
        unsigned char *ptr = (unsigned char *)&mHeader;

        for (count = 0; count < (int)sizeof(FMOD_FSB_HEADER); count++)
        {
            FMOD_ENCRYPT(ptr[count], userexinfo->encryptionkey[offset]);        /* put it back the way it was. */
            FMOD_DECRYPT_OLD(ptr[count], userexinfo->encryptionkey[offset]);    /* Now redecrypt using the old method. */
            offset++;
            if (offset >= keylen)
            {
                offset = 0;
            }
        }

        mFile->mFlags |= FMOD_FILE_USEOLDDECRYPT;
    }

    if (!FMOD_strncmp(mHeader.id, "FSB4", 4))
    {
        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "CodecFSB::openInternal", "found FSB4\n"));
    }
    else if (!FMOD_strncmp(mHeader.id, "FSB2", 4) || !FMOD_strncmp(mHeader.id, "FSB3", 4))
    {
        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "CodecFSB::openInternal", "found %s\n", mHeader.id));
        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "CodecFSB::openInternal", "Sorry %s not supported any more.  Please recompile using a newer FSBankEx\n", mHeader.id));
        return FMOD_ERR_FORMAT;
    }
    else
    {
        FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "CodecFSB::openInternal", "Header check failed.  Not an FSB\n"));

        return FMOD_ERR_FORMAT;
    }

#ifdef PLATFORM_ENDIAN_BIG
    mHeader.numsamples = FMOD_SWAPENDIAN_DWORD(mHeader.numsamples);
    mHeader.datasize   = FMOD_SWAPENDIAN_DWORD(mHeader.datasize);
    mHeader.shdrsize   = FMOD_SWAPENDIAN_DWORD(mHeader.shdrsize);
    mHeader.version    = FMOD_SWAPENDIAN_DWORD(mHeader.version);
    mHeader.mode       = FMOD_SWAPENDIAN_DWORD(mHeader.mode);
#endif

#ifndef FMOD_FSB_FORCE_3_0
    if (mHeader.version < FMOD_FSB_VERSION_3_1)
    {
        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "CodecFSB::openInternal", "Error - FSB 3.0 not supported any more, mustb e 3.1\n"));
        return FMOD_ERR_VERSION;
    }
#endif

	mSrcDataOffset = 0;

    mFlags |= FMOD_CODEC_FROMFSB;
    if (mHeader.mode & FMOD_FSB_SOURCE_MPEG_PADDED)
    {
        mFlags |= FMOD_CODEC_PADDED;
    }
    else if (mHeader.mode & FMOD_FSB_SOURCE_MPEG_PADDED4)
    {
        mFlags |= FMOD_CODEC_PADDED4;
    }

    if (mHeader.numsamples < 1)
    {
        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "CodecFSB::openInternal", "Error - mHeader.numsamples < 1\n"));
        return FMOD_ERR_FILE_BAD;
    }

    /*
        If there is an inclusion list and the number of subsounds has been set by the user, go into 'sorted mode'.
    */
    numsamples = mHeader.numsamples;
    if (userexinfo && userexinfo->inclusionlist && userexinfo->inclusionlistnum && userexinfo->numsubsounds == userexinfo->inclusionlistnum)
    {
        numsamples = userexinfo->inclusionlistnum;
    }

    if (mHeader.mode & FMOD_FSB_SOURCE_FORMAT)
    {
        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "CodecFSB::openInternal", "Error - FMOD Ex does no longer support 'use source format' FSB files.\n"));
        return FMOD_ERR_FORMAT;
    }

    mNonInterleaved = (mHeader.mode & FMOD_FSB_SOURCE_NOTINTERLEAVED) ? true : false;
    if (mNonInterleaved)
    {
        if (usermode & FMOD_CREATESTREAM)
        {
            FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "CodecFSB::openInternal", "Error - Cannot open a non interleaved FSB as a stream.\n"));
            return FMOD_ERR_FORMAT;
        }

        if (usermode & FMOD_SOFTWARE)
        {
            FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "CodecFSB::openInternal", "Error - Cannot open a non interleaved FSB with FMOD_SOFTWARE mode.\n"));
            return FMOD_ERR_FORMAT;
        }

        if (mSystem->mOutput->mDescription.getsamplemaxchannels)
        {
            if (mSystem->mOutput->mDescription.getsamplemaxchannels(mSystem->mOutput, usermode, FMOD_SOUND_FORMAT_NONE) > 1)
            {
                FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "CodecFSB::openInternal", "Error - Cannot open a non interleaved FSB with current output mode.\n"));
                return FMOD_ERR_FORMAT;
            }
        }
    }

#ifdef FMOD_FSB_USEHEADERCACHE
    bool found = false;
    CodecFSBCache *current = 0;

	if (mHeader.version >= FMOD_FSB_VERSION_4_0)
	{
	    FMOD_FSB_HEADER header;
	    
        FMOD_OS_CriticalSection_Enter(FMOD::gGlobal->gAsyncCrit);

	    memcpy(&header, &mHeader, sizeof(FMOD_FSB_HEADER));
	    header.numsamples = numsamples;
	    
		for (current = (CodecFSBCache *)gCacheHead.getNext(); current != &gCacheHead; current = (CodecFSBCache *)current->getNext())
		{
			if (!memcmp(&header, &current->mHeader, sizeof(FMOD_FSB_HEADER)))
			{
				found = true;
				break;
			}
		}
	}

    if (found)
    {
        FMOD_OS_CriticalSection_Leave(FMOD::gGlobal->gAsyncCrit);

        /*
            Busy loop here until other thread has finished loading cache data, we don't want to use the crit because
            we are sharing gAsyncCrit which would mean we would block multiple non-blocking loads from main thread.
        */
        while (current->mStillLoading)
        {
            FMOD_OS_Time_Sleep(5);
        }

        result = mFile->seek(mHeader.shdrsize, SEEK_CUR);
        if (result != FMOD_OK)
        {
            return result;
        }

        shdrblock   = current->mShdrData;
        mShdr       = current->mShdr;
        mShdrb      = current->mShdrb;
        mDataOffset = current->mDataOffset;

        current->mShareCount++;

        mCacheEntry = current;
    }
    else
    {
        shdrblock = 0;
        
		if (mHeader.version >= FMOD_FSB_VERSION_4_0)
		{
			mCacheEntry = FMOD_Object_Calloc(CodecFSBCache);
			if (!mCacheEntry)
			{
				return FMOD_ERR_MEMORY;
			}

            mCacheEntry->mShdr  = 0;
            mCacheEntry->mShdrb = 0;
            mCacheEntry->mShdrData = 0;
            mCacheEntry->mDataOffset = 0;

			mCacheEntry->addBefore(&gCacheHead);
			mCacheEntry->mShareCount = 0;
			FMOD_memcpy(&mCacheEntry->mHeader, &mHeader, sizeof(FMOD_FSB_HEADER));
			
			if (numsamples != mHeader.numsamples)
			{
			    mCacheEntry->mHeader.numsamples = numsamples;      /* Dont cache this stuff. */
			    mCacheEntry->mShdr = 0;
			    mCacheEntry->mShdrb = 0;
			    mCacheEntry->mDataOffset = 0;
			}

            mCacheEntry->mStillLoading = true;
            FMOD_OS_CriticalSection_Leave(FMOD::gGlobal->gAsyncCrit);
		}
	}
#endif

    if (!mShdrb && !mShdr)
    {
        /*
            Allocate and read FSB sample header pointers
        */   
        if (mHeader.mode & FMOD_FSB_SOURCE_BASICHEADERS)
        {
            mShdrb = (FMOD_FSB_SAMPLE_HEADER_BASIC **)FMOD_Memory_Calloc(sizeof(void *) * numsamples);
            if (!mShdrb)
            {
                return FMOD_ERR_MEMORY;
            }
        }
        else
        {
            mShdr = (FMOD_FSB_SAMPLE_HEADER **)FMOD_Memory_Calloc(sizeof(void *) * numsamples);
            if (!mShdr)
            {
                return FMOD_ERR_MEMORY;
            }
        }
    }

    if (!shdrblock)
    {
        /*
            Allocate and read FSB sample header data
        */   
        shdrblock = (char *)FMOD_Memory_Calloc(mHeader.shdrsize);
        if (!shdrblock)
        {
            return FMOD_ERR_MEMORY;
        }
        
        result = mFile->read(shdrblock, 1, mHeader.shdrsize, &rd);
        if (result != FMOD_OK)
        {
            FMOD_Memory_Free(shdrblock);
            return result;
        }
        if (rd != mHeader.shdrsize)
        {
            FMOD_Memory_Free(shdrblock);
            FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "CodecFSB::openInternal", "rd != mHeader.shdrsize\n"));
            return FMOD_ERR_FILE_BAD;
        }        
    }

    if (!mDataOffset)
    {
        /*
            Calculate file offsets for each sound, and if little endian, do some byte swapping.
        */
        mDataOffset = (unsigned int *)FMOD_Memory_Calloc(sizeof(unsigned int) * numsamples);
        if (!mDataOffset)
        {
            FMOD_Memory_Free(shdrblock);
            return FMOD_ERR_MEMORY;
        }
    }
    
    #ifdef FMOD_FSB_USEHEADERCACHE
    if (!found && mCacheEntry)
    {
        mCacheEntry->mShdrData = shdrblock;       /* pointer to sample header block */

        if (numsamples == mHeader.numsamples)
        {
	        mCacheEntry->mShdr = mShdr;               /* array of sample header pointers */
            mCacheEntry->mShdrb = mShdrb;             /* array of sample header pointers */
            mCacheEntry->mDataOffset = mDataOffset;   /* array of file sample data offsets. */
        }
    }
    #endif

    mSrcDataOffset = (mHeader.version >= FMOD_FSB_VERSION_4_0 ? sizeof(FMOD_FSB_HEADER) : sizeof(FMOD_FSB3_HEADER)) + mHeader.shdrsize;
   
    waveformat   = 0;
    offset       = mSrcDataOffset;
    mFirstSample = 0;

    if ((usermode & (FMOD_SOFTWARE | FMOD_HARDWARE)) && !(mOriginalMode & (FMOD_SOFTWARE | FMOD_HARDWARE)))
    {
        usermode &= ~(FMOD_SOFTWARE | FMOD_HARDWARE);
    }
    if ((mMode & (FMOD_SOFTWARE | FMOD_HARDWARE)) && !(mOriginalMode & (FMOD_SOFTWARE | FMOD_HARDWARE)))
    {
        mMode &= ~(FMOD_SOFTWARE | FMOD_HARDWARE);
    }

    mUserMode = usermode;

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "CodecFSB::openInternal", "FSB contains %d sounds\n", mHeader.numsamples));

    /*
        Fill out base class members, also pointing to or allocating storage for them.
    */
    for (soffset = 0, subsoundindex = 0, count = 0; count < mHeader.numsamples; count++)
    {
        bool found = true;

        if (userexinfo && userexinfo->inclusionlist && userexinfo->inclusionlistnum && userexinfo->numsubsounds == userexinfo->inclusionlistnum)
        {
            int count2;

            for (count2 = 0; count2 < userexinfo->inclusionlistnum; count2++)
            {
                if (userexinfo->inclusionlist[count2] >= mHeader.numsamples)
                {
                    return FMOD_ERR_INVALID_PARAM;
                }

                if (userexinfo->inclusionlist[count2] == count)
                {
                    break;
                }
            }
            if (count2 == userexinfo->inclusionlistnum)
            {
                found = false;
            }
        }

        if ((mHeader.mode & FMOD_FSB_SOURCE_BASICHEADERS) && count)    /* exclude first sample */
        {
            FMOD_FSB_SAMPLE_HEADER_BASIC *s = (FMOD_FSB_SAMPLE_HEADER_BASIC *)((char *)shdrblock + soffset);

            if (found)
            {
                mShdrb[subsoundindex] = s;
                mDataOffset[subsoundindex] = offset;
            }

            #ifdef PLATFORM_ENDIAN_BIG
            #ifdef FMOD_FSB_USEHEADERCACHE
            if (!current || !current->mShareCount)	/* Don't swap more than once! */
            #endif
            {
                s->lengthsamples         = FMOD_SWAPENDIAN_DWORD(s->lengthsamples);
                s->lengthcompressedbytes = FMOD_SWAPENDIAN_DWORD(s->lengthcompressedbytes);
            }
            #endif

            offset  += (s->lengthcompressedbytes + FSB_SAMPLE_DATA_ALIGN - 1) & ~(FSB_SAMPLE_DATA_ALIGN - 1);
            soffset += sizeof(FMOD_FSB_SAMPLE_HEADER_BASIC);

            mChannels = mFirstSample->numchannels;
            if (mMode & FMOD_SOFTWARE && mChannels > mSystem->mMaxInputChannels)
            {
                return FMOD_ERR_TOOMANYCHANNELS;
            }

            /*
                Add offset for ADPCM coefficients - assume they're the same size as the ones with firstsample
                i.e. all GCADPCM samples in an FSB must have the same number of channels
            */
            if (mFirstSample->mode & FSOUND_GCADPCM)
            {
                #ifdef FMOD_SUPPORT_GCADPCM
                int sizeofheader = sizeof(FMOD_FSB_SAMPLE_HEADER);
                soffset += (mFirstSample->size - sizeofheader);

                /*
                    Make sure GCADPCM samples are aligned
                */
                s->lengthsamples /= 14;
                s->lengthsamples *= 14;
                #else
                return FMOD_ERR_FORMAT;
                #endif
            }
            else if (mFirstSample->mode & FSOUND_XMA)
            {
                #ifdef FMOD_SUPPORT_XMA
                if (*((int *)((char *)mFirstSample + sizeof(FMOD_FSB_SAMPLE_HEADER) + sizeof(FMOD_XMA_LOOPINFO))))
                {
                    if (mFirstSample->size_32bits) /* We didnt flag the block size change, so this is a dodgy way to work out if the FSB is the old FSB format. */
                    {
                        soffset += ((((s->lengthcompressedbytes+32767) / 32768 * sizeof(unsigned int)) + 12) + 16); /* +8 = numentries and something else, +16 = loopinfo */
                    }
                    else
                    {
                        soffset += ((((s->lengthcompressedbytes+2047) / 2048 * sizeof(unsigned int)) + 8) + 16); /* +8 = numentries and something else, +16 = loopinfo */
                    }
                }
                else
                {
                    soffset += 16; /* +16 = loopinfo */              
                }
                #else
                return FMOD_ERR_FORMAT;
                #endif               
            }
            else if (mFirstSample->mode & FSOUND_IMAADPCM)
            {
                #ifdef FMOD_SUPPORT_IMAADPCM
                #ifdef PLATFORM_XBOX
                if (usermode & FMOD_HARDWARE)
                {
                }
                else
                #endif
                {
                    mReadBufferLength     = 36 * (mFirstSample->numchannels < 2 ? 2 : mFirstSample->numchannels);
                    mPCMBufferLength      = 64;
                    mPCMBufferLengthBytes = mPCMBufferLength * sizeof(signed short) * (mFirstSample->numchannels < 2 ? 2 : mFirstSample->numchannels);
    	        }
                #else
                return FMOD_ERR_FORMAT;
                #endif
            }           
            else if (mFirstSample->mode & FSOUND_MPEG)
            {
                #ifdef FMOD_SUPPORT_MPEG_SONYDECODER
                if (mFirstSample->mode & FSOUND_MPEG_LAYER2)
                {
                    FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "CodecFSB::openInternal", "Error!  Only MP3 format is supported when using the Sony MP3 decoder.\n"));
                    return FMOD_ERR_FORMAT;
                }
                #endif

                if (mFirstSample->numchannels > 2 &&  !(usermode & FMOD_CREATESTREAM))
                {
                    FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "CodecFSB::openInternal", "Error!  Tried to load %d channel MP2/MP3.  Static codecs can only support stereo maximum.  Multichannel MP2/MP3 can only be streamed.\n", mFirstSample->numchannels));
                    return FMOD_ERR_TOOMANYCHANNELS;
                }
            }
            else if (mFirstSample->mode & FSOUND_CELT)
            {
                if (mFirstSample->numchannels > 2 &&  !(usermode & FMOD_CREATESTREAM))
                {
                    FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "CodecFSB::openInternal", "Error!  Tried to load %d channel CELT.  Static codecs can only support stereo maximum.  Multichannel CELT can only be streamed.\n", mFirstSample->numchannels));
                    return FMOD_ERR_TOOMANYCHANNELS;
                }
            }

            if (userexinfo && userexinfo->speakermap)
            {
                if (userexinfo->speakermap == FMOD_SPEAKERMAPTYPE_ALLMONO)
                {
                    mFirstSample->mode |= FSOUND_CHANNELMODE_ALLMONO;
                }
                else if (userexinfo->speakermap == FMOD_SPEAKERMAPTYPE_ALLSTEREO)
                {
                    mFirstSample->mode |= FSOUND_CHANNELMODE_ALLSTEREO;
                }
                else if (userexinfo->speakermap == FMOD_SPEAKERMAPTYPE_51_PROTOOLS)
                {
                    mFirstSample->mode |= FSOUND_CHANNELMODE_PROTOOLS;
                }
            }
        }
        else
        {
            FMOD_FSB_SAMPLE_HEADER *s = (FMOD_FSB_SAMPLE_HEADER *)((char *)shdrblock + soffset);
            FMOD_FSB_SAMPLE_HEADER tmpsample;

            if (!count)
            {
                mFirstSample = s;
            }
            
            if (found)
            {           
                if (!(mHeader.mode & FMOD_FSB_SOURCE_BASICHEADERS))
                {
                    mShdr[subsoundindex] = s;
                }

                if (s->name[0] != 0)
                {
                    mDataOffset[subsoundindex] = offset;
                }
                else
                {
                    if (!count)
                    {
                        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "CodecFSB::openInternal", "!count\n"));
                        return FMOD_ERR_FILE_BAD;
                    }
                    mDataOffset[subsoundindex] = mDataOffset[subsoundindex - 1];
                }
            }

            FMOD_memcpy(&tmpsample, s, sizeof(FMOD_FSB_SAMPLE_HEADER));    /* FMOD_memcpy'd instead of pointed to for alignment reasons */

            #ifdef PLATFORM_ENDIAN_BIG
            #ifdef FMOD_FSB_USEHEADERCACHE
            if (!current || !current->mShareCount)	/* Don't swap more than once! */
            #endif
            {
                tmpsample.size                  = FMOD_SWAPENDIAN_WORD(tmpsample.size);
                
                tmpsample.lengthsamples         = FMOD_SWAPENDIAN_DWORD(tmpsample.lengthsamples);
                tmpsample.lengthcompressedbytes = FMOD_SWAPENDIAN_DWORD(tmpsample.lengthcompressedbytes);
                tmpsample.loopstart             = FMOD_SWAPENDIAN_DWORD(tmpsample.loopstart);
                tmpsample.loopend               = FMOD_SWAPENDIAN_DWORD(tmpsample.loopend);
                tmpsample.mode                  = FMOD_SWAPENDIAN_DWORD(tmpsample.mode);
                tmpsample.deffreq               = FMOD_SWAPENDIAN_DWORD(tmpsample.deffreq);
                
                tmpsample.defvol                = FMOD_SWAPENDIAN_WORD(tmpsample.defvol);
                tmpsample.defpan                = FMOD_SWAPENDIAN_WORD(tmpsample.defpan);
                tmpsample.defpri                = FMOD_SWAPENDIAN_WORD(tmpsample.defpri);
                tmpsample.numchannels           = FMOD_SWAPENDIAN_WORD(tmpsample.numchannels);

#ifndef FMOD_FSB_FORCE_3_0
                tmpsample.size_32bits           = FMOD_SWAPENDIAN_DWORD(tmpsample.size_32bits);
                tmpsample.varvol                = FMOD_SWAPENDIAN_WORD(tmpsample.varvol);
                tmpsample.varpan                = FMOD_SWAPENDIAN_WORD(tmpsample.varpan);

                FMOD_SWAPENDIAN_FLOAT(tmpsample.mindistance);
                FMOD_SWAPENDIAN_FLOAT(tmpsample.maxdistance);
#endif
            }
            #endif
    
            offset  += (tmpsample.name[0] ? (tmpsample.lengthcompressedbytes + FSB_SAMPLE_DATA_ALIGN - 1) & ~(FSB_SAMPLE_DATA_ALIGN - 1) : 0);

            if (tmpsample.size == 65535)   /* Stupid backwards compatibility thing. 0xffff signifies we are to re-use a new 32bit var. */
            {
                if ((int)tmpsample.size_32bits < tmpsample.size)
                {
                    return FMOD_ERR_UNSUPPORTED;    /* This will barf on an old FSB that just happens to have size = 64k, because the memory used to be 'varfreq' and it is always going to be lower than 64k */
                }

                soffset += tmpsample.size_32bits;
            }
            else
            {
                soffset += tmpsample.size;
            }

#ifndef FMOD_FSB_FORCE_3_0
            tmpsample.varvol <<= 2;
            tmpsample.varpan <<= 2;
            if (tmpsample.mindistance == tmpsample.maxdistance)
            {
                tmpsample.mindistance = 1.0f;
                tmpsample.maxdistance = 10000.0f;
            }
#endif

            if (usermode & FMOD_CREATECOMPRESSEDSAMPLE)
            {
                if (s->mode & FSOUND_MPEG)
                {
                    tmpsample.loopstart /= 576;
                    tmpsample.loopstart *= 576;
                }
            }

            FMOD_memcpy(s, &tmpsample, sizeof(FMOD_FSB_SAMPLE_HEADER));    /* FMOD_memcpy'd back */

            if (!s->numchannels)
            {
                return FMOD_ERR_FILE_BAD;
            }

            if (found)
            {
                if (s->numchannels != mFirstSample->numchannels)
                {
                    mixedchannels = true;
                }

                if (s->numchannels > mChannels)
                {
                    mChannels = s->numchannels;
                }
            }

            if (mMode & FMOD_SOFTWARE && mChannels > mSystem->mMaxInputChannels)
            {
                return FMOD_ERR_TOOMANYCHANNELS;
            }

            /*
                Make sure GCADPCM samples are aligned
            */
            if (s->mode & FSOUND_GCADPCM)
            {
                #ifdef FMOD_SUPPORT_GCADPCM
                s->lengthsamples /= 14;
                s->lengthsamples *= 14;
                #else
                return FMOD_ERR_FORMAT;
                #endif
            }

            /*
                If any of the subsounds has software mode and the
                FSB is XMA format, we need to set mDecodeXMA
            */
            if (s->mode & FSOUND_XMA && !(usermode & FMOD_HARDWARE))
            {
                #ifdef FMOD_SUPPORT_XMA
                mDecodeXMA = true;
                #else
                return FMOD_ERR_FORMAT;
                #endif            
            }

            if (s->mode & FSOUND_IMAADPCM)
            {
                #ifdef FMOD_SUPPORT_IMAADPCM
                #ifdef PLATFORM_XBOX
                if (usermode & FMOD_HARDWARE)
                {
                }
                else
                #endif
                {
                    mReadBufferLength     = 36 * (mChannels < 2 ? 2 : mChannels);
                    mPCMBufferLength      = 64;
        	        mPCMBufferLengthBytes = mPCMBufferLength * sizeof(signed short) * (mChannels < 2 ? 2 : mChannels);
    	        }
                #else
                return FMOD_ERR_FORMAT;
                #endif
            }
            else if (s->mode & FSOUND_MPEG)
            {
                #ifdef FMOD_SUPPORT_MPEG_SONYDECODER
                if (s->mode & FSOUND_MPEG_LAYER2)
                {
                    FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "CodecFSB::openInternal", "Error!  Only MP3 format is supported when using the Sony MP3 decoder.\n"));
                    return FMOD_ERR_FORMAT;
                }
                #endif

                if (s->numchannels > 2 &&  !(usermode & FMOD_CREATESTREAM))
                {
                    FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "CodecFSB::openInternal", "Error!  Tried to load %d channel MP2/MP3.  Static codecs can only support stereo maximum.  Multichannel MP2/MP3 can only be streamed.\n", mFirstSample->numchannels));
                    return FMOD_ERR_TOOMANYCHANNELS;
                }
            }
            else if (s->mode & FSOUND_CELT)
            {
                if (s->numchannels > 2 &&  !(usermode & FMOD_CREATESTREAM))
                {
                    FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "CodecFSB::openInternal", "Error!  Tried to load %d channel CELT.  Static codecs can only support stereo maximum.  Multichannel CELT can only be streamed.\n", mFirstSample->numchannels));
                    return FMOD_ERR_TOOMANYCHANNELS;
                }
            }

            if (usermode & FMOD_2D)
            {
                if (s->mode & FSOUND_HW3D)
                {
                    s->mode &= ~FSOUND_HW3D;
                    s->mode |=  FSOUND_HW2D;
                }
                else if (s->mode & FSOUND_3D)
                {
                    s->mode &= ~FSOUND_3D;
                    s->mode |=  FSOUND_2D;
                }
            }

            if (userexinfo && userexinfo->speakermap)
            {
                if (userexinfo->speakermap == FMOD_SPEAKERMAPTYPE_ALLMONO)
                {
                    s->mode |= FSOUND_CHANNELMODE_ALLMONO;
                }
                else if (userexinfo->speakermap == FMOD_SPEAKERMAPTYPE_ALLSTEREO)
                {
                    s->mode |= FSOUND_CHANNELMODE_ALLSTEREO;
                }
                else if (userexinfo->speakermap == FMOD_SPEAKERMAPTYPE_51_PROTOOLS)
                {
                    s->mode |= FSOUND_CHANNELMODE_PROTOOLS;
                }
            }
        }

        if (found)
        {
            subsoundindex++;
        }
    }


    /*
        ======================================================

        SET UP CODEC SPECIFIC STUFF

        ======================================================
    */


    // ========================================================================================================
    // GC ADPCM
    // ========================================================================================================
    if (mFirstSample->mode & FSOUND_GCADPCM)
    {
#ifdef FMOD_SUPPORT_GCADPCM
        FMOD_GCADPCMINFO **info;

        info = (FMOD_GCADPCMINFO **)FMOD_Memory_Calloc(sizeof(void *) * numsamples);
        if (!info)
        {
            return FMOD_ERR_MEMORY;
        }

        for (soffset = 0, subsoundindex = 0, count = 0; count < mHeader.numsamples; count++)
        {
            bool found = true;

            if (userexinfo && userexinfo->inclusionlist && userexinfo->inclusionlistnum && userexinfo->numsubsounds == userexinfo->inclusionlistnum)
            {
                int count2;

                for (count2 = 0; count2 < userexinfo->inclusionlistnum; count2++)
                {
                    if (userexinfo->inclusionlist[count2] == count)
                    {
                        break;
                    }
                }
                if (count2 == userexinfo->inclusionlistnum)
                {
                    found = false;
                }
            }

            if ((mHeader.mode & FMOD_FSB_SOURCE_BASICHEADERS) && count)    /* exclude first sample */
            {
                FMOD_FSB_SAMPLE_HEADER_BASIC *s = (FMOD_FSB_SAMPLE_HEADER_BASIC *)((char *)shdrblock + soffset);
   
                if (found)
                {
                    info[subsoundindex] = (FMOD_GCADPCMINFO *)FMOD_Memory_Calloc(sizeof(FMOD_GCADPCMINFO) * mFirstSample->numchannels);
                    if (!info[subsoundindex])
                    {
                        return FMOD_ERR_MEMORY;
                    }

                    FMOD_memcpy(info[subsoundindex], (char *)s + sizeof(FMOD_FSB_SAMPLE_HEADER_BASIC), sizeof(FMOD_GCADPCMINFO) * mFirstSample->numchannels);
                }

                soffset += sizeof(FMOD_FSB_SAMPLE_HEADER_BASIC);

                /*
                    Add offset for ADPCM coefficients - assume they're the same size as the ones with firstsample
                    i.e. all GCADPCM samples in an FSB must have the same number of channels
                */
                soffset += (mFirstSample->size - sizeof(FMOD_FSB_SAMPLE_HEADER));
            }
            else
            {
                FMOD_FSB_SAMPLE_HEADER *s = (FMOD_FSB_SAMPLE_HEADER *)((char *)shdrblock + soffset);

                if (found)
                {
                    info[subsoundindex] = (FMOD_GCADPCMINFO *)FMOD_Memory_Calloc(sizeof(FMOD_GCADPCMINFO) * s->numchannels);
                    if (!info[subsoundindex])
                    {
                        return FMOD_ERR_MEMORY;
                    }

                    FMOD_memcpy(info[subsoundindex], ((char *)s) + sizeof(FMOD_FSB_SAMPLE_HEADER), sizeof(FMOD_GCADPCMINFO) * s->numchannels);
                }

                if (s->size == 65535)   /* Stupid backwards compatibility thing. 0xffff signifies we are to re-use a new 32bit var. */
                {
                    if ((int)s->size_32bits < s->size)
                    {
                        return FMOD_ERR_UNSUPPORTED;    /* This will barf on an old FSB that just happens to have size = 64k, because the memory used to be 'varfreq' and it is always going to be lower than 64k */
                    }

                    soffset += s->size_32bits;
                }
                else
                {
                    soffset += s->size;
                }
            }

            if (found)
            {
                subsoundindex++;
            }
        }

        plugindata = info;
#else
        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "CodecFSB::openInternal", "Error!  Tried to open an GCADPCM bank on a platform that doesn't support it!\n"));
        return FMOD_ERR_FORMAT;
#endif
    }

    // ========================================================================================================
    // XMA
    // ========================================================================================================
    else if (mFirstSample->mode & FSOUND_XMA)
    {
#ifdef FMOD_SUPPORT_XMA
        int **seektable;
        int headersize;
        int dataoffset = 0;
        int seektablesize;

        if (usermode & FMOD_CREATESTREAM || usermode & FMOD_SOFTWARE || usermode & FMOD_CREATECOMPRESSEDSAMPLE)
        {
            mDecodeXMA = true;
        }

        if (mDecodeXMA)
        {
            mXMA = FMOD_Object_Calloc(CodecXMA);
            if (!mXMA)
            {
                return FMOD_ERR_MEMORY;
            }            
            
            FMOD_memcpy(&mXMA->mDescription, CodecXMA::getDescriptionEx(), sizeof(FMOD_CODEC_DESCRIPTION_EX));
            mXMA->mDescription.getwaveformat = &Codec::defaultGetWaveFormat;          

            mXMA->mFile = mFile;
            mXMA->mWaveFormatMemory = (FMOD_CODEC_WAVEFORMAT *)FMOD_Memory_Calloc(sizeof(FMOD_CODEC_WAVEFORMAT));
            if (!mXMA->mWaveFormatMemory)
            {
                return FMOD_ERR_MEMORY;
            }
            mXMA->waveformat            = mXMA->mWaveFormatMemory;
            mXMA->waveformat->frequency = mFirstSample->deffreq;
#ifdef FMOD_SUPPORT_XMA_NEWHAL
            if (mFirstSample->size_32bits || mChannels > 2)
            {
                mXMA->mBlockSize = 32*1024;
            }
            else
            {
                mXMA->mBlockSize = 2*1024;
            }
#endif            
            if (mixedchannels)
            {
                mXMA->mFlags |= FMOD_CODEC_FSBXMAMIXEDCHANNELS;
            }

            if (!(usermode & FMOD_CREATECOMPRESSEDSAMPLE))
            {               
                result = mXMA->XMAInit((mChannels + 1) / 2);    /* stream for every stereo pair. (and if odd, 1 for last mono channel as well) */
                if (result != FMOD_OK)
                {
                    return result;
                }               
            }           
        }

        /*
             Check if there is a seektable.
        */
        headersize = sizeof(FMOD_FSB_SAMPLE_HEADER);

        FMOD_memcpy(&seektablesize, (char *)mFirstSample + headersize + sizeof(FMOD_XMA_LOOPINFO), 4);

        #ifdef PLATFORM_ENDIAN_BIG
        seektablesize = FMOD_SWAPENDIAN_DWORD(seektablesize);
        #endif

        if (seektablesize)
        {
            mXMASeekable = mXMA->mXMASeekable = true;

            seektable = (int **)FMOD_Memory_Calloc(sizeof(int *) * numsamples);
            if (!seektable)
            {
                return FMOD_ERR_MEMORY;
            }

            for (soffset = 0, subsoundindex = 0, count = 0; count < mHeader.numsamples; count++)
            {
                if ((mHeader.mode & FMOD_FSB_SOURCE_BASICHEADERS) && count)    /* exclude first sample */
                {
                    FMOD_FSB_SAMPLE_HEADER_BASIC *s = (FMOD_FSB_SAMPLE_HEADER_BASIC *)((char *)shdrblock + soffset);
                    int *stab;

                    /*
                        Now get seek table length
                    */
                    FMOD_memcpy(&seektablesize, (char *)s + sizeof(FMOD_FSB_SAMPLE_HEADER_BASIC) + sizeof(FMOD_XMA_LOOPINFO), 4);
                    
                    #ifdef PLATFORM_ENDIAN_BIG
                    seektablesize = FMOD_SWAPENDIAN_DWORD(seektablesize);
                    #endif

                    /*
                        Copy in the seektable
                    */
                    stab = (int *)((char *)s + sizeof(FMOD_FSB_SAMPLE_HEADER_BASIC) + sizeof(FMOD_XMA_LOOPINFO) + 4);
                    #ifdef PLATFORM_ENDIAN_BIG
                    #ifdef FMOD_FSB_USEHEADERCACHE
                    if (!current || (current && !current->mShareCount))
                    #endif
                    {
                        /*
                            Flip the endian of the seektable
                        */  
                        for (int i = 0; i < seektablesize / 4; i++)
                        {
                            stab[i] = FMOD_SWAPENDIAN_DWORD(stab[i]);
                        }
                    }
                    #endif

                    if (subsoundindex < numsamples)
                    {
                        seektable[subsoundindex] = stab;
                    }

                    soffset += sizeof(FMOD_FSB_SAMPLE_HEADER_BASIC);
                    
                    if (*((int *)((char *)mFirstSample + sizeof(FMOD_FSB_SAMPLE_HEADER) + sizeof(FMOD_XMA_LOOPINFO))))
                    {
                        if (mFirstSample->size_32bits) /* We didnt flag the block size change, so this is a dodgy way to work out if the FSB is the old FSB format. */
                        {
                            soffset += ((((s->lengthcompressedbytes+32767) / 32768 * sizeof(unsigned int)) + 12) + 16); /* +8 = numentries and something else, +16 = loopinfo */
                        }
                        else
                        {
                            soffset += ((((s->lengthcompressedbytes+2047) / 2048 * sizeof(unsigned int)) + 8) + 16); /* +8 = numentries and something else, +16 = loopinfo */
                        }
                    }
                    else
                    {
                        soffset += 16; /* +16 = loopinfo */              
                    }
                }
                else
                {
                    FMOD_FSB_SAMPLE_HEADER *s = (FMOD_FSB_SAMPLE_HEADER *)((char *)shdrblock + soffset);
                    int *stab;
                    
                    headersize = sizeof(FMOD_FSB_SAMPLE_HEADER);

                    /*
                        Now get seek table length
                    */
                    FMOD_memcpy(&seektablesize, (char *)s + headersize + sizeof(FMOD_XMA_LOOPINFO), 4);
                    
                    #ifdef PLATFORM_ENDIAN_BIG
                    seektablesize = FMOD_SWAPENDIAN_DWORD(seektablesize);
                    #endif

                    /*
                        Copy in the seektable
                    */
                    stab = (int *)((char *)s + headersize + sizeof(FMOD_XMA_LOOPINFO) + 4);

                    #ifdef PLATFORM_ENDIAN_BIG
                    #ifdef FMOD_FSB_USEHEADERCACHE
                    if (!current || (current && !current->mShareCount))
                    #endif
                    {
                        /*
                            Flip the endian of the seektable
                        */  
                        for (int i = 0; i < seektablesize / 4; i++)
                        {
                            stab[i] = FMOD_SWAPENDIAN_DWORD(stab[i]);
                        }
                    }
                    #endif

                    if (subsoundindex < numsamples)
                    {
                        seektable[subsoundindex] = stab;
                    }

                    if (s->size == 65535)   /* Stupid backwards compatibility thing. 0xffff signifies we are to re-use a new 32bit var. */
                    {
                        if ((int)s->size_32bits < s->size)
                        {
                            return FMOD_ERR_UNSUPPORTED;    /* This will barf on an old FSB that just happens to have size = 64k, because the memory used to be 'varfreq' and it is always going to be lower than 64k */
                        }

                        soffset += s->size_32bits;
                    }
                    else
                    {
                        soffset += s->size;
                    }
                }

                if (userexinfo && userexinfo->inclusionlist && userexinfo->inclusionlistnum && userexinfo->numsubsounds == userexinfo->inclusionlistnum)
                {
                    int count2;

                    for (count2 = 0; count2 < userexinfo->inclusionlistnum; count2++)
                    {
                        if (userexinfo->inclusionlist[count2] == count)
                        {
                            subsoundindex++;
                            break;
                        }
                    }
                }
                else
                {
                    subsoundindex++;
                }
            }

            plugindata = seektable;
        }
        else
        {
            mXMASeekable = mXMA->mXMASeekable = false;

            FLOG((FMOD_DEBUG_LEVEL_WARNING, __FILE__, __LINE__, "CodecFSB::openInternal", "FSB was built *without* XMA seek table. Seeking will not be allowed.\n"));
        
            plugindata = 0;
        }
#else
        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "CodecFSB::openInternal", "Error!  Tried to open an XMA bank on a platform that doesn't support it!\n"));
        return FMOD_ERR_FORMAT;
#endif
    }

    // ========================================================================================================
    // IMA ADPCM
    // ========================================================================================================
    else if (mFirstSample->mode & FSOUND_IMAADPCM)
    {
#ifdef FMOD_SUPPORT_IMAADPCM

        if (usermode & FMOD_CREATECOMPRESSEDSAMPLE
            #ifdef PLATFORM_XBOX
            && (usermode & FMOD_SOFTWARE)   /* Has to be software for xbox as well, because hardware can decode this stuff itself.  All other platforms decompress regardless. */
            #endif        
        )
        {
            mDecodeADPCM = true;
        }

        if (mDecodeADPCM)
        {
            mADPCM = FMOD_Object_Calloc(CodecWav);
            if (!mADPCM)
            {
                return FMOD_ERR_MEMORY;
            }            
            
            FMOD_memcpy(&mADPCM->mDescription, CodecWav::getDescriptionEx(), sizeof(FMOD_CODEC_DESCRIPTION_EX));
            mADPCM->mDescription.getwaveformat = &Codec::defaultGetWaveFormat;

            mADPCM->mFile = mFile;
            mADPCM->mWaveFormatMemory = (FMOD_CODEC_WAVEFORMAT *)FMOD_Memory_Calloc(sizeof(FMOD_CODEC_WAVEFORMAT));
            if (!mADPCM->mWaveFormatMemory)
            {
                return FMOD_ERR_MEMORY;
            }
            mADPCM->waveformat = mADPCM->mWaveFormatMemory;
            mADPCM->mPCMBufferLengthBytes = mPCMBufferLengthBytes;
        }
        
#else
        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "CodecFSB::openInternal", "Error!  Tried to open an ADPCM bank on a platform that doesn't support it!\n"));
        return FMOD_ERR_FORMAT;
#endif
    }


    // ========================================================================================================
    // MPEG
    // ========================================================================================================
    else if (mFirstSample->mode & FSOUND_MPEG)
    {
#ifdef FMOD_SUPPORT_MPEG

        mMPEG = FMOD_Object_Calloc(CodecMPEG);
        if (!mMPEG)
        {
            return FMOD_ERR_MEMORY;
        }            
        
        if (!CodecMPEG::gInitialized)
        {
            CodecMPEG::initAll();
            CodecMPEG::gInitialized = true;
        }
        
        FMOD_memcpy(&mMPEG->mDescription, CodecMPEG::getDescriptionEx(), sizeof(FMOD_CODEC_DESCRIPTION_EX));
        mMPEG->mDescription.getwaveformat = &Codec::defaultGetWaveFormat;          

        mMPEG->mFile = mFile;
        mMPEG->mWaveFormatMemory = (FMOD_CODEC_WAVEFORMAT *)FMOD_Memory_Calloc(sizeof(FMOD_CODEC_WAVEFORMAT));
        if (!mMPEG->mWaveFormatMemory)
        {
            return FMOD_ERR_MEMORY;
        }
        mMPEG->waveformat        = mMPEG->mWaveFormatMemory;
        mMPEG->mSystem           = mSystem;
        
        mPCMBufferLength             = 1152;
        mMPEG->mPCMBufferLength      = 1152;
        mMPEG->mPCMBufferLengthBytes = 1152 * sizeof(signed short) * (mChannels < 2 ? 2 : mChannels);
        mMPEG->mPCMFrameLengthBytes  = 1152 * sizeof(signed short) * (mChannels < 2 ? 2 : mChannels);
        mMPEG->mChannels             = 0;
        mMPEG->mFlags               |= FMOD_CODEC_FROMFSB;
        if (mHeader.mode & FMOD_FSB_SOURCE_MPEG_PADDED)
        {
            mMPEG->mFlags |= FMOD_CODEC_PADDED;
        }
        else if (mHeader.mode & FMOD_FSB_SOURCE_MPEG_PADDED4)
        {
            mMPEG->mFlags |= FMOD_CODEC_PADDED4;
        }

        if (!(usermode & FMOD_CREATECOMPRESSEDSAMPLE))
        {
            mMPEG->mMemoryBlockMemory = (CodecMPEG_MemoryBlock *)FMOD_Memory_Calloc(sizeof(CodecMPEG_MemoryBlock) * (mChannels > 2 ? mChannels : 1) + 16);
            if (!mMPEG->mMemoryBlockMemory)
            {
                return FMOD_ERR_MEMORY;
            }
            mMPEG->mMemoryBlock = (CodecMPEG_MemoryBlock *)FMOD_ALIGNPOINTER(mMPEG->mMemoryBlockMemory, 16);

            mMPEG->mPCMBufferMemory = (unsigned char *)FMOD_Memory_Calloc(mMPEG->mPCMBufferLengthBytes + 16);
            if (!mMPEG->mPCMBufferMemory)
            {
                return FMOD_ERR_MEMORY;
            }
            mMPEG->mPCMBuffer = (unsigned char *)FMOD_ALIGNPOINTER(mMPEG->mPCMBufferMemory, 16);

            if (mChannels > 2)
            {
                mMPEG->mChannels = mChannels;
            }

            mMPEG->resetFrame();
            
            for (count = 0; count < (mChannels > 2 ? mChannels : 1); count++)
            {
                mMPEG->mMemoryBlock[count].mHasXingNumFrames  = false;
                mMPEG->mMemoryBlock[count].mHasXingToc        = false;

                #ifndef FMOD_SUPPORT_MPEG_SONYDECODER

                mMPEG->mMemoryBlock[count].mFrameSizeOld      = -1;
                mMPEG->mMemoryBlock[count].mSynthBo           = 1;
                mMPEG->mMemoryBlock[count].mLayer             = 0;
                mMPEG->mMemoryBlock[count].mSynthBuffs        = (float *)FMOD_ALIGNPOINTER(mMPEG->mMemoryBlock[count].mSynthBuffsMem, 16);

                #endif
            }
        }
        
#else
        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "CodecFSB::openInternal", "Error!  Tried to open an MPEG bank on a platform that doesn't support it!\n"));

        return FMOD_ERR_FORMAT;
#endif
    }


    // ========================================================================================================
    // VAG
    // ========================================================================================================
    else if (mFirstSample->mode & FSOUND_VAG)
    {
#ifdef FMOD_SUPPORT_VAG
        bool decodevag = true;
 
        #if defined(PLATFORM_PS2) || defined (PLATFORM_PSP)
        if (!(usermode & FMOD_SOFTWARE))   
        {
            decodevag = false;
        }
        #endif        
        
        if (decodevag)
        {
            mVAG = FMOD_Object_Calloc(CodecVAG);
            if (!mVAG)
            {
                return FMOD_ERR_MEMORY;
            }            
            
            mPCMBufferLength = 28 * mChannels;
            mPCMBufferLengthBytes = 56 * mChannels;

            FMOD_memcpy(&mVAG->mDescription, CodecVAG::getDescriptionEx(), sizeof(FMOD_CODEC_DESCRIPTION_EX));
            mVAG->mDescription.getwaveformat = &Codec::defaultGetWaveFormat;

            mVAG->mFile = mFile;
            mVAG->mWaveFormatMemory = (FMOD_CODEC_WAVEFORMAT *)FMOD_Memory_Calloc(sizeof(FMOD_CODEC_WAVEFORMAT));
            if (!mVAG->mWaveFormatMemory)
            {
                return FMOD_ERR_MEMORY;
            }
            mVAG->waveformat = mVAG->mWaveFormatMemory;
            mVAG->mPCMBufferLengthBytes = mPCMBufferLengthBytes;           
        }              
#else
        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "CodecFSB::openInternal", "Error!  Tried to open an VAG bank on a platform that doesn't support it!\n"));

        return FMOD_ERR_FORMAT;
#endif
    }

    // ========================================================================================================
    // CELT
    // ========================================================================================================
    else if(mFirstSample->mode & FSOUND_CELT)
    {
#ifdef FMOD_SUPPORT_CELT
        mCELT = FMOD_Object_Calloc(CodecCELT);
        if (!mCELT)
        {
            return FMOD_ERR_MEMORY;
        }

        /*
            Generate CELT mode(s)

            1 for mono, and one for stereo.
        */
        if (mixedchannels)
        {
            /*
                Allocate both stereo and mono modes
            */
            result = CodecCELT::createCELTModeMono();
            if (result != FMOD_OK)
            {
                return result;
            }
            result = CodecCELT::createCELTModeStereo();
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        else
        {
            if (mChannels >= 2)
            {
                /*
                    Stereo or multichannel
                */
                result = CodecCELT::createCELTModeStereo();
                if (result != FMOD_OK)
                {
                    return result;
                }
            }
            else
            {
                /*
                    Mono only
                */
                result = CodecCELT::createCELTModeMono();
                if (result != FMOD_OK)
                {
                    return result;
                }
            }
        }

        /*
            Check bitstream version.
        */
        int bitstreamversion;
        
        FMOD_memcpy(&bitstreamversion, (char *)mFirstSample + sizeof(FMOD_FSB_SAMPLE_HEADER), 4);

        #ifdef PLATFORM_ENDIAN_BIG
        bitstreamversion = FMOD_SWAPENDIAN_DWORD(bitstreamversion);
        #endif

        if (bitstreamversion != CodecCELT::getBitstreamVersion())
        {
            FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "CodecFSB::openInternal", "Bitstream version of FSB %08x doesn't match %08x.", bitstreamversion, CodecCELT::getBitstreamVersion()));
            return FMOD_ERR_FORMAT;
        }

        FMOD_memcpy(&mCELT->mDescription, CodecCELT::getDescriptionEx(), sizeof(FMOD_CODEC_DESCRIPTION_EX));
        mCELT->mDescription.getwaveformat = &Codec::defaultGetWaveFormat;

        mCELT->mFile = mFile;
        mCELT->mWaveFormatMemory = (FMOD_CODEC_WAVEFORMAT *)FMOD_Memory_Calloc(sizeof(FMOD_CODEC_WAVEFORMAT));
        if (!mCELT->mWaveFormatMemory)
        {
            return FMOD_ERR_MEMORY;
        }
        mCELT->waveformat = mCELT->mWaveFormatMemory;

        mPCMBufferLength             = FMOD_CELT_FRAMESIZESAMPLES;
        mCELT->mPCMBufferLength      = FMOD_CELT_FRAMESIZESAMPLES;
        mCELT->mPCMBufferLengthBytes = FMOD_CELT_FRAMESIZESAMPLES * sizeof(signed short) * (mChannels < 2 ? 2 : mChannels);
        mCELT->mPCMFrameLengthBytes  = FMOD_CELT_FRAMESIZESAMPLES * sizeof(signed short) * mChannels;

        if (!(usermode & FMOD_CREATECOMPRESSEDSAMPLE))
        {
            mCELT->mPCMBufferMemory = (unsigned char *)FMOD_Memory_Calloc(mCELT->mPCMBufferLengthBytes + 16);
            if (!mCELT->mPCMBufferMemory)
            {
                return FMOD_ERR_MEMORY;
            }
            mCELT->mPCMBuffer = (unsigned char *)FMOD_ALIGNPOINTER(mCELT->mPCMBufferMemory, 16);

            result = mCELT->CELTinit((mChannels + 1) / 2);     /* stream for every stereo pair. */
            if (result != FMOD_OK)
            {
                return result;
            }
        }
#else
        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "CodecFSB::openInternal", "Error!  Tried to open a CELT bank on a platform that doesn't support it!.\n"));
        return FMOD_ERR_FORMAT;
#endif
    }

#ifdef FMOD_FSB_USEHEADERCACHE
    if (mCacheEntry)
    {
        mCacheEntry->mStillLoading = false;
    }
#endif

    /*
        ======================================================

        Load syncpoints / seektables.

        ======================================================
    */
    if (!(mHeader.mode & FMOD_FSB_SOURCE_BASICHEADERS))
    {
        int fsbheadersize = sizeof(FMOD_FSB_SAMPLE_HEADER);
        int headersize;

        headersize = fsbheadersize;

        for (soffset = 0, subsoundindex = 0, count = 0; count < mHeader.numsamples && subsoundindex < numsamples; count++)
        {
            FMOD_FSB_SAMPLE_HEADER *s;
            bool found = true;

            if (userexinfo && userexinfo->inclusionlist && userexinfo->inclusionlistnum && userexinfo->numsubsounds == userexinfo->inclusionlistnum)
            {
                int count2;

                for (count2 = 0; count2 < userexinfo->inclusionlistnum; count2++)
                {
                    if (userexinfo->inclusionlist[count2] == count)
                    {
                        break;
                    }
                }
                if (count2 == userexinfo->inclusionlistnum)
                {
                    found = false;
                }
            }

            s = (FMOD_FSB_SAMPLE_HEADER *)((char *)shdrblock + soffset);

            #ifdef FMOD_SUPPORT_XMA
            if (mFirstSample->mode & FSOUND_XMA)
            {
                int seektablelength = 0;

                headersize = fsbheadersize;

                /*
                    Get seek table length
                */
                FMOD_memcpy(&seektablelength, (char *)s + headersize + sizeof(FMOD_XMA_LOOPINFO), 4);
                #ifdef PLATFORM_ENDIAN_BIG
                seektablelength = FMOD_SWAPENDIAN_DWORD(seektablelength);
                #endif

                headersize += sizeof(FMOD_XMA_LOOPINFO);
                headersize += sizeof(int);
                headersize += seektablelength;
            }
            #endif

#ifdef FMOD_SUPPORT_GCADPCM
            if (mFirstSample->mode & FSOUND_GCADPCM)
            {
                headersize = fsbheadersize + (sizeof(FMOD_GCADPCMINFO) * s->numchannels);
            }
#endif

            if (s->mode & FSOUND_SYNCPOINTS && found)
            {
                char *syncpointdata = (char *)s + headersize;

                if (!FMOD_strncmp(syncpointdata, "SYNC", 4))
                {
                    if (!mSyncPointData)
                    {
                        mSyncPointData = (char **)FMOD_Memory_Calloc(numsamples * sizeof(char *));
                        if (!mSyncPointData)
                        {
                            return FMOD_ERR_MEMORY;
                        }
                    }

                    mSyncPointData[subsoundindex] = syncpointdata;
                }
            }

            if (s->size == 65535)   /* Stupid backwards compatibility thing. 0xffff signifies we are to re-use a new 32bit var. */
            {
                if ((int)s->size_32bits < s->size)
                {
                    return FMOD_ERR_UNSUPPORTED;    /* This will barf on an old FSB that just happens to have size = 64k, because the memory used to be 'varfreq' and it is always going to be lower than 64k */
                }

                soffset += s->size_32bits;
            }
            else
            {
                soffset += s->size;
            }

            if (found)
            {
                subsoundindex++;
            }
        }
    }


    /*
        ======================================================

        ALLOCATE BUFFERS

        ======================================================
    */
    if (!(usermode & FMOD_CREATECOMPRESSEDSAMPLE))  /* There's going to be a global readbuff and unique pcm buffs for every dspcodec, dont alloc them here. */
    {
        if (mReadBufferLength)
        {
            mReadBuffer = (unsigned char *)FMOD_Memory_Calloc(mReadBufferLength);
            if (!mReadBuffer)
            {
                return FMOD_ERR_MEMORY;
            }
        }

        if (mPCMBufferLengthBytes)
        {
            mPCMBufferMemory = (unsigned char *)FMOD_Memory_Calloc(mPCMBufferLengthBytes + 16);
            if (!mPCMBufferMemory)
            {
                return FMOD_ERR_MEMORY;
            }
            mPCMBuffer = (unsigned char *)FMOD_ALIGNPOINTER(mPCMBufferMemory, 16);
        }
    }
    
    numsubsounds = numsamples;
    
    if (userexinfo && userexinfo->inclusionlist && userexinfo->inclusionlistnum && userexinfo->numsubsounds == userexinfo->inclusionlistnum)
    {
        mHeader.numsamples = numsamples;
        userexinfo->inclusionlist = 0;
        userexinfo->inclusionlistnum = 0;   /* Get rid of the inclusionlist. */
    }

    /*
        ======================================================
        
        SET UP CODEC POOLS

        ======================================================
    */
    if (usermode & FMOD_CREATECOMPRESSEDSAMPLE)
    {
        if (0)
        {
        }
        #ifdef FMOD_SUPPORT_XMA
        /*
            Create a pool of XMA codecs if using realtime software mixed xma playback.
        */
        else if (mXMA)
        {
            int count;

            if (mChannels > 2)                           
            {
                return FMOD_ERR_TOOMANYCHANNELS;    /* Sorry we're only allocating memory for a pool of maximum stereo voices. */
            }
        
            getWaveFormatInternal(0, &mXMA->waveformat[0]);

            #ifdef FMOD_SUPPORT_DSPCODEC
            if (!mSystem->mDSPCodecPool_XMA.mNumDSPCodecs)
            {
                result = mSystem->mDSPCodecPool_XMA.init(FMOD_DSP_CATEGORY_DSPCODECXMA, 512, mSystem->mAdvancedSettings.maxXMAcodecs ? mSystem->mAdvancedSettings.maxXMAcodecs : FMOD_ADVANCEDSETTINGS_MAXXMACODECS);
                if (result != FMOD_OK)
                {
                    return result;
                }
        
                for (count = 0; count < mSystem->mDSPCodecPool_XMA.mNumDSPCodecs; count++)
                {
                    DSPCodec *dspcodec   = SAFE_CAST(DSPCodec, mSystem->mDSPCodecPool_XMA.mPool[count]);
                    CodecXMA *xma        = (CodecXMA *)dspcodec->mCodec;

                    xma->mSrcDataOffset = 0;    /* Raw data will start at 0. */
                    xma->mWaveFormatMemory = 0;    /* This will be set up upon play. */
                    
                    result = xma->XMAInit(1);           
                    if (result != FMOD_OK)
                    {
                        return result;
                    }
                }       
            }
            #endif
        }  
        #endif    

        #ifdef FMOD_SUPPORT_IMAADPCM
        /*
            Create a pool of ADPCM codecs if using realtime software mixed adpcm playback.
        */
        else if (mADPCM)
        {      
            if (mChannels > 2)                           
            {
                return FMOD_ERR_TOOMANYCHANNELS;    /* Sorry we're only allocating memory for a pool of maximum stereo voices. */
            }

            mADPCM->mReadBufferLength     = mReadBufferLength;
            mADPCM->mReadBuffer           = 0;
            mADPCM->mSamplesPerADPCMBlock = 64; 

            getWaveFormatInternal(0, &mADPCM->waveformat[0]);

            #ifdef FMOD_SUPPORT_DSPCODEC            
            if (!mSystem->mDSPCodecPool_ADPCM.mNumDSPCodecs)
            {
                int count;

                result = mSystem->mDSPCodecPool_ADPCM.init(FMOD_DSP_CATEGORY_DSPCODECADPCM, 64, mSystem->mAdvancedSettings.maxADPCMcodecs ? mSystem->mAdvancedSettings.maxADPCMcodecs : FMOD_ADVANCEDSETTINGS_MAXADPCMCODECS);
                if (result != FMOD_OK)
                {
                    return result;
                }
   
                for (count = 0; count < mSystem->mDSPCodecPool_ADPCM.mNumDSPCodecs; count++)
                {
                    DSPCodec *dspcodec   = SAFE_CAST(DSPCodec, mSystem->mDSPCodecPool_ADPCM.mPool[count]);
                    CodecWav *wav        = (CodecWav *)dspcodec->mCodec;

                    wav->mSrcDataOffset  = 0;   /* Raw data will start at 0. */
                    wav->mSrcFormat      = &wav->mSrcFormatMemory;
                    wav->mSrcFormat->Format.wFormatTag = WAVE_FORMAT_IMA_ADPCM;
                }       
            }
            #endif
        }  
        #endif
        #ifdef FMOD_SUPPORT_MPEG
        /*
            Create a pool of MPEG codecs if using realtime software mixed MPEG playback.
        */
        else if (mMPEG)
        {      
            if (mChannels > 2)                           
            {
                return FMOD_ERR_TOOMANYCHANNELS;    /* Sorry we're only allocating memory for a pool of maximum stereo voices. */
            }

            mMPEG->mReadBufferLength     = mReadBufferLength;
            mMPEG->mReadBuffer           = mReadBuffer;

            getWaveFormatInternal(0, &mMPEG->waveformat[0]);
           
            #ifdef FMOD_SUPPORT_DSPCODEC
            if (!mSystem->mDSPCodecPool_MPEG.mNumDSPCodecs)
            {
                int count;

                result = mSystem->mDSPCodecPool_MPEG.init(FMOD_DSP_CATEGORY_DSPCODECMPEG, 1152, mSystem->mAdvancedSettings.maxMPEGcodecs ? mSystem->mAdvancedSettings.maxMPEGcodecs : FMOD_ADVANCEDSETTINGS_MAXMPEGCODECS );
                if (result != FMOD_OK)
                {
                    return result;
                }
    
                for (count = 0; count < mSystem->mDSPCodecPool_MPEG.mNumDSPCodecs; count++)
                {
                    DSPCodec *dspcodec   = SAFE_CAST(DSPCodec, mSystem->mDSPCodecPool_MPEG.mPool[count]);
                    CodecMPEG *mpeg       = (CodecMPEG *)dspcodec->mCodec;

                    mpeg->mSrcDataOffset    = 0;    /* Raw data will start at 0. */
                    mpeg->mWaveFormatMemory = 0;    /* This will be set up upon play. */                       
                    mpeg->resetFrame();            
                }       
            }
            #endif
        }  
        #endif
        #ifdef FMOD_SUPPORT_CELT
        else if (mCELT)
        {
            if (!mSystem->mDSPCodecPool_CELT.mNumDSPCodecs)
            {
                int count;

                result = mSystem->mDSPCodecPool_CELT.init(FMOD_DSP_CATEGORY_DSPCODECCELT, FMOD_CELT_FRAMESIZESAMPLES, mSystem->mAdvancedSettings.maxCELTcodecs ? mSystem->mAdvancedSettings.maxCELTcodecs : FMOD_ADVANCEDSETTINGS_MAXCELTCODECS);
                if (result != FMOD_OK)
                {
                    return result;
                }

                for (count = 0; count < mSystem->mDSPCodecPool_CELT.mNumDSPCodecs; count++)
                {
                    DSPCodec  *dspcodec = SAFE_CAST(DSPCodec, mSystem->mDSPCodecPool_CELT.mPool[count]);
                    CodecCELT *celt     = (CodecCELT *)dspcodec->mCodec;

                    celt->mSrcDataOffset = 0;       /* Raw data will start at 0. */
                    celt->mWaveFormatMemory = 0;    /* This will be set up upon play. */

                    result = celt->CELTinit(1);
                    if (result != FMOD_OK)
                    {
                        return result;
                    }
                }
            }
        }
        #endif
    }


    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "CodecFSB::openInternal", "done.\n"));

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
FMOD_RESULT CodecFSB::closeInternal()
{
    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "CodecFSB::closeInternal", "\n"));

#ifdef FMOD_FSB_USEHEADERCACHE
    if (mCacheEntry)
    {
        if (mCacheEntry->mShdrData)
        {
            mFirstSample = 0;
        }
        if (mCacheEntry->mShdr)
        {
            mShdr = 0;
        }
        if (mCacheEntry->mShdrb)
        {
            mShdrb = 0;
        }
        if (mCacheEntry->mDataOffset)
        {
            mDataOffset = 0;
        }
    
        FMOD_OS_CriticalSection_Enter(FMOD::gGlobal->gAsyncCrit);

        if (!mCacheEntry->mShareCount)
        {
            if (mCacheEntry->mShdrData)
            {
                FMOD_Memory_Free(mCacheEntry->mShdrData);
            }
            if (mCacheEntry->mShdr)
            {
                FMOD_Memory_Free(mCacheEntry->mShdr);
            }
            if (mCacheEntry->mShdrb)
            {
                FMOD_Memory_Free(mCacheEntry->mShdrb);
            }
            if (mCacheEntry->mDataOffset)
            {
                FMOD_Memory_Free(mCacheEntry->mDataOffset);
            }
            mCacheEntry->removeNode();

            FMOD_Memory_Free(mCacheEntry);
            mCacheEntry = 0;
        }
        else
        {
            mCacheEntry->mShareCount--;
        }

        FMOD_OS_CriticalSection_Leave(FMOD::gGlobal->gAsyncCrit);
    }
#endif

    if (mFirstSample)	/* The start of the sample header block. */
    {
        FMOD_Memory_Free(mFirstSample);
        mFirstSample = 0;
    }
    if (mShdr)
    {
        FMOD_Memory_Free(mShdr);
        mShdr = 0;
    }
    if (mShdrb)
    {
        FMOD_Memory_Free(mShdrb);
        mShdrb = 0;
    }
    if (mDataOffset)
    {
        FMOD_Memory_Free(mDataOffset);
        mDataOffset = 0;
    }
    if (mSyncPointData)
    {
        FMOD_Memory_Free(mSyncPointData);
        mSyncPointData = 0;
    }
    if (mPCMBufferMemory)
    {
        FMOD_Memory_Free(mPCMBufferMemory);
        mPCMBuffer = mPCMBufferMemory = 0;
    }
    mPCMBufferLengthBytes = 0;

    if (mReadBuffer)
    {
        FMOD_Memory_Free(mReadBuffer);
        mReadBuffer = 0;
    }
    mReadBufferLength = 0;

#ifdef FMOD_SUPPORT_GCADPCM
    {
        FMOD_GCADPCMINFO **info = (FMOD_GCADPCMINFO **)plugindata;

        if (info)
        {
            for (int count = 0; count < mHeader.numsamples; count++)
            {
                if (info[count])
                {
                    FMOD_Memory_Free(info[count]);
                    info[count] = 0;
                }
            }
        }
    }

    if (plugindata)
    {
        FMOD_Memory_Free(plugindata);
        plugindata = 0;
    }
#endif

#ifdef FMOD_SUPPORT_XMA
    if (plugindata)
    {
        FMOD_Memory_Free(plugindata);
        plugindata = 0;
    }
    
    if (mXMA)
    {
        mXMA->mReadBuffer = 0;
        mXMA->mFile = 0;
        mXMA->mSeekTable = 0;   /* This is part of FSB memory, not the XMA codec. */
        mXMA->release();
    }
#endif

#ifdef FMOD_SUPPORT_IMAADPCM
    if (mADPCM)
    {
        mADPCM->mReadBuffer = 0;
        mADPCM->mFile = 0;
        mADPCM->release();
    }
#endif

#ifdef FMOD_SUPPORT_MPEG
    if (mMPEG)
    {
        mMPEG->mReadBuffer = 0;
        mMPEG->mFile = 0;
        mMPEG->release();
    }
#endif

#ifdef FMOD_SUPPORT_VAG
    if (mVAG)
    {
        mVAG->mReadBuffer = 0;
        mVAG->mFile = 0;
        mVAG->release();
    }
#endif

#ifdef FMOD_SUPPORT_CELT
    if (mCELT)
    {
        mCELT->mReadBuffer = 0;
        mCELT->mFile = 0;
        mCELT->release();
    }
#endif

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "CodecFSB::closeInternal", "done\n"));

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
FMOD_RESULT CodecFSB::readInternal(void *buffer, unsigned int sizebytes, unsigned int *bytesread)
{
    FMOD_RESULT result = FMOD_OK;
    unsigned int bytesreadinternal;
    unsigned int srcmode = mHeader.mode & FMOD_FSB_SOURCE_BASICHEADERS ? mFirstSample->mode : mShdr[mCurrentIndex]->mode;
    FMOD_CODEC_WAVEFORMAT waveformat;

    getWaveFormatInternal(mCurrentIndex, &waveformat);

    if (0)
    {
    }
#ifdef FMOD_SUPPORT_XMA
    else if (mXMA)  /* This means it is a codec wanting to be decompressed to PCM */
    {
        result = mXMA->read(buffer, sizebytes * waveformat.channels / mChannels, &bytesreadinternal);
        if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
        {
            return result;
        }
    }
#endif
#ifdef FMOD_SUPPORT_MPEG
    else if (mMPEG)  /* This means it is a codec wanting to be decompressed to PCM */
    {
        result = mMPEG->read(buffer, sizebytes * waveformat.channels / mChannels, &bytesreadinternal);
        if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
        {
            return result;
        }
    }
#endif
#ifdef FMOD_SUPPORT_IMAADPCM
    else if (srcmode & FSOUND_IMAADPCM && waveformat.format == FMOD_SOUND_FORMAT_PCM16)
    {
        unsigned char readbuff[36 * 16];
        int readbufflength = 36 * waveformat.channels;
        int blockalign = 36 * waveformat.channels;

        result = mFile->read(readbuff, 1, blockalign, 0);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (waveformat.channels == 1)
        {
            result = IMAAdpcm_DecodeM16(readbuff, (signed short *)mPCMBuffer, 1, blockalign, 64, 1);
        }
        else if (srcmode & FSOUND_IMAADPCMSTEREO && waveformat.channels == 2)
        {
            result = IMAAdpcm_DecodeS16(readbuff, (signed short *)mPCMBuffer, 1, blockalign, 64);
        }
        else
        {
            int count = 0;
    
            blockalign /= waveformat.channels;

            for (count = 0; count < waveformat.channels; count++)
            {                   
                signed short tempin[4096];
                int count2;

                for (count2 = 0; count2 < (int)readbufflength / waveformat.channels; count2++)
                {
                    tempin[count2] = ((signed short *)readbuff)[(count2 * waveformat.channels) + count];
                }

                result = IMAAdpcm_DecodeM16((unsigned char *)tempin, (signed short *)mPCMBuffer + count, 1, blockalign, 64, waveformat.channels);
            }
        }

        bytesreadinternal = 64 * sizeof(signed short) * waveformat.channels;
    }
#endif
#ifdef FMOD_SUPPORT_VAG
    else if (mVAG)  /* This means it is a codec wanting to be decompressed to PCM */
    {
        result = mVAG->read(buffer, sizebytes * waveformat.channels / mChannels, &bytesreadinternal);
        if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
        {
            return result;
        }
    }
#endif
#ifdef FMOD_SUPPORT_CELT
    else if (mCELT)
    {
        result = mCELT->read(buffer, sizebytes * waveformat.channels / mChannels, &bytesreadinternal);
    }
#endif
    else
    {
        result = mFile->read(buffer, 1, sizebytes / mChannels * waveformat.channels, &bytesreadinternal);
	    if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
        {
            return result;
        }

        /*
            Convert data from unsigned to signed that fmod requires.
        */
        if (waveformat.format == FMOD_SOUND_FORMAT_PCM8)
        {
            unsigned int len;
            unsigned char *src = (unsigned char *)buffer;

            len = bytesreadinternal >> 3;
            while (len)
            {
                src[0] ^= 128;
                src[1] ^= 128;
                src[2] ^= 128;
                src[3] ^= 128;
                src[4] ^= 128;
                src[5] ^= 128;
                src[6] ^= 128;
                src[7] ^= 128;
                src+=8;
                len--;
            }
            len = bytesreadinternal & 7;
            while (len)
            {
                src[0] ^= 128;
                src++;
                len--;
            }
        }

        if (!(mHeader.mode & FMOD_FSB_SOURCE_BIGENDIANPCM))
        {
            #ifdef PLATFORM_ENDIAN_BIG
            if (waveformat.format == FMOD_SOUND_FORMAT_PCM16)
            {
                unsigned int count;
                signed short *wptr = (signed short *)buffer;
            
                for (count=0; count < bytesreadinternal >> 1; count++)
                {
                    wptr[count] = FMOD_SWAPENDIAN_WORD(wptr[count]);
                }
        
            }
            else if (waveformat.format == FMOD_SOUND_FORMAT_PCMFLOAT)
            {
                unsigned int count;
                unsigned int *wptr = (unsigned int *)buffer;
            
                for (count=0; count < bytesreadinternal >> 1; count++)
                {
                    wptr[count] = FMOD_SWAPENDIAN_DWORD(wptr[count]);
                }
            }
            #endif
        }
        else
        {
            #ifdef PLATFORM_ENDIAN_LITTLE
            if (waveformat.format == FMOD_SOUND_FORMAT_PCM16)
            {
                unsigned int count;
                signed short *wptr = (signed short *)buffer;
            
                for (count=0; count < bytesreadinternal >> 1; count++)
                {
                    wptr[count] = FMOD_SWAPENDIAN_WORD(wptr[count]);
                }
        
            }
            else if (waveformat.format == FMOD_SOUND_FORMAT_PCMFLOAT)
            {
                unsigned int count;
                unsigned int *wptr = (unsigned int *)buffer;
            
                for (count=0; count < bytesreadinternal >> 1; count++)
                {
                    wptr[count] = FMOD_SWAPENDIAN_DWORD(wptr[count]);
                }
            }
            #endif
        }
    }

    /*
        Upscale to the stream channel format if the incoming data is lower. (just duplicate channel data)
    */
    if (waveformat.channels < mChannels)
    {
        int count, block;
        int blocksize, numblocks;
        char *srcptr, *destptr;
       
        /* 
            Stereo XADPCM is a special case where it is interleaved every 4 bytes. 
        */
        if (srcmode & FSOUND_IMAADPCM && waveformat.format == FMOD_SOUND_FORMAT_IMAADPCM && mChannels == 2)
        {
            blocksize = 4;
        }
        /*
            GCADPCM is interleaved like PCM16
        */
        else if (waveformat.format == FMOD_SOUND_FORMAT_GCADPCM)
        {
            blocksize = 2;
        }
        /*
            VAG is being decoded to software, so we want to deinterleave it from a block of 28 16bit words into the pcm16 dest.
        */
        else if (srcmode & FSOUND_VAG && waveformat.format == FMOD_SOUND_FORMAT_PCM16)
        {
            blocksize = 2;
        }
        else
        {
            blocksize = waveformat.blockalign / waveformat.channels;
        }
        
        srcptr  = (char *)buffer + (bytesreadinternal                                  ) - (blocksize * waveformat.channels);
        destptr = (char *)buffer + (bytesreadinternal / waveformat.channels * mChannels) - (blocksize * mChannels);
        numblocks = bytesreadinternal / (blocksize * waveformat.channels);

        /*
            8bit data
        */
        if (blocksize == 1)
        {
            if (waveformat.channels == 1)
            {
                for (block = 0; block < numblocks; block++)
                {
                    for (count = mChannels - 1; count >= 0; count--)
                    {
                        destptr[count] = srcptr[0];
                    }

                    srcptr  --;
                    destptr -= mChannels;
                }
            }
            else
            {
                for (block = 0; block < numblocks; block++)
                {
                    int srcchannel = waveformat.channels - 1;

                    for (count = mChannels - 1; count >= 0; count--)
                    {
                        if (count > srcchannel)
                        {
                            destptr[count] = 0;
                        }
                        else
                        {
                            destptr[count] = srcptr[srcchannel];
                            srcchannel--;
                        }              
                    }

                    srcptr  -= waveformat.channels;
                    destptr -= mChannels;
                }
            }
        }
        /*
            16bit data
        */
        else if (blocksize == 2)
        {
            signed short *srcptrw  = (signed short *)srcptr;
            signed short *destptrw = (signed short *)destptr;

            if (waveformat.channels == 1)
            {
                for (block = 0; block < numblocks; block++)
                {
                    for (count = mChannels - 1; count >= 0; count--)
                    {
                        destptrw[count] = srcptrw[0];               
                    }

                    srcptrw  --;
                    destptrw -= mChannels;
                }
            }
            else
            {
                for (block = 0; block < numblocks; block++)
                {
                    int srcchannel = waveformat.channels - 1;

                    for (count = mChannels - 1; count >= 0; count--)
                    {
                        if (count > srcchannel)
                        {
                            destptrw[count] = 0;
                        }
                        else
                        {
                            destptrw[count] = srcptrw[srcchannel];
                            srcchannel--;
                        }
                    }

                    srcptrw  -= waveformat.channels;
                    destptrw -= mChannels;
                }
            }
        }
        /*
            32bit data
        */
        else if (blocksize == 4)
        {
            int *srcptrd  = (int *)srcptr;
            int *destptrd = (int *)destptr;

            if (waveformat.channels == 1)
            {
                for (block = 0; block < numblocks; block++)
                {
                    for (count = mChannels - 1; count >= 0; count--)
                    {
                        destptrd[count] = srcptrd[0];               
                    }

                    srcptrd  --;
                    destptrd -= mChannels;
                }
            }
            else
            {
                for (block = 0; block < numblocks; block++)
                {
                    int srcchannel = waveformat.channels - 1;

                    for (count = mChannels - 1; count >= 0; count--)
                    {
                        if (count > srcchannel)
                        {
                            destptrd[count] = 0;
                        }
                        else
                        {
                            destptrd[count] = srcptrd[srcchannel];
                            srcchannel--;
                        }               
                    }

                    srcptrd  -= waveformat.channels;
                    destptrd -= mChannels;
                }
            }
        }
        /*
            Other types of sample data (native compressed data?)
        */
        else
        {
            if (waveformat.channels == 1)
            {
                for (block = 0; block < numblocks; block++)
                {
                    for (count = mChannels - 1; count >= 0; count--)
                    {
                        FMOD_memcpy(destptr + (count * blocksize), srcptr, blocksize);               
                    }

                    srcptr  -= blocksize;
                    destptr -= (blocksize * mChannels);
                }
            }
            else
            {
                for (block = 0; block < numblocks; block++)
                {
                    int srcchannel = waveformat.channels - 1;

                    for (count = mChannels - 1; count >= 0; count--)
                    {
                        if (count > srcchannel)
                        {
                            FMOD_memset(destptr + (count * blocksize), 0, blocksize);
                        }
                        else
                        {
                            FMOD_memcpy(destptr + (count * blocksize), srcptr + (srcchannel * blocksize), blocksize);
                            srcchannel--;
                        }                
                    }

                    srcptr  -= (blocksize * waveformat.channels);
                    destptr -= (blocksize * mChannels);
                }
            }
        }

        *bytesread = bytesreadinternal * mChannels / waveformat.channels;
    }
    else
    {
        *bytesread = bytesreadinternal;
    }

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
FMOD_RESULT CodecFSB::setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    FMOD_RESULT result = FMOD_OK;

    if (subsound < 0 || (numsubsounds && subsound >= numsubsounds))
    {
        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "CodecFSB::setPositionInternal", "ERROR - Invalid subsound.  subsound %d position %d postype %d\n", subsound, position, postype));
        return FMOD_ERR_INVALID_POSITION;
    }

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "CodecFSB::setPositionInternal", "subsound %d position %d postype %d\n", subsound, position, postype));

    if (mFile->mFlags & FMOD_FILE_SEEKABLE)
    {
        unsigned int pos = 0, srcmode;
        FMOD_CODEC_WAVEFORMAT waveformat;

        if (subsound != mCurrentIndex)
        {
            mCurrentIndex = subsound;
        }

        getWaveFormatInternal(mCurrentIndex, &waveformat);

        srcmode = mHeader.mode & FMOD_FSB_SOURCE_BASICHEADERS ? mFirstSample->mode : mShdr[mCurrentIndex]->mode;

        if (postype == FMOD_TIMEUNIT_RAWBYTES)
        {
            pos = mDataOffset[mCurrentIndex] + position;

            result = mFile->seek(pos, SEEK_SET);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        else
#ifdef FMOD_SUPPORT_XMA
        if (srcmode & FSOUND_XMA && waveformat.format == FMOD_SOUND_FORMAT_PCM16)
        {
            int **seektable = (int **)plugindata;

            mXMA->mSrcDataOffset = mDataOffset[mCurrentIndex];
            if (mXMA->mXMASeekable)
            {
                mXMA->mSeekTable = seektable[mCurrentIndex];
            }
            mXMA->mFile          = mFile;
            mXMA->mNumStreams    = (waveformat.channels + 1) / 2;

            FMOD_memcpy(mXMA->mWaveFormatMemory, &waveformat, sizeof(FMOD_CODEC_WAVEFORMAT));

            result = mXMA->setPositionInternal(subsound, position, postype);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        else
#endif
#ifdef FMOD_SUPPORT_MPEG
        if (srcmode & FSOUND_MPEG && waveformat.format == FMOD_SOUND_FORMAT_PCM16)
        {
            mMPEG->mSrcDataOffset       = mDataOffset[mCurrentIndex];
            mMPEG->mFile                = mFile;
            mMPEG->mPCMFrameLengthBytes = 1152 * sizeof(signed short) * waveformat.channels;

            FMOD_memcpy(mMPEG->mWaveFormatMemory, &waveformat, sizeof(FMOD_CODEC_WAVEFORMAT));

            result = mMPEG->setPositionInternal(subsound, position, postype);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        else
#endif
#ifdef FMOD_SUPPORT_IMAADPCM
        if (srcmode & FSOUND_IMAADPCM && waveformat.format == FMOD_SOUND_FORMAT_PCM16)
        {
            unsigned int pcm = position;
            unsigned int pcmaligned = pcm;
            unsigned int excessbytes = 0;

            pcmaligned /= 64;
            pcmaligned *= 64;

            SoundI::getBytesFromSamples(pcmaligned, &pos, waveformat.channels, FMOD_SOUND_FORMAT_IMAADPCM);
           
            pos += mDataOffset[mCurrentIndex];

            result = mFile->seek(pos, SEEK_SET);
            if (result != FMOD_OK)
            {
                return result;
            }

            SoundI::getBytesFromSamples(pcm - pcmaligned, &excessbytes, waveformat.channels, waveformat.format);

            while (excessbytes)
            {
                static char buff[1000];
                unsigned int read = 0, toread = 1000;

                if (toread > excessbytes)
                {
                    toread = excessbytes;
                }

                result = Codec::read(buff, toread, &read);
                if (result != FMOD_OK)
                {
                    break;
                }

                excessbytes -= read;
            }
        }
        else
#endif
#ifdef FMOD_SUPPORT_VAG
        if (srcmode & FSOUND_VAG && waveformat.format == FMOD_SOUND_FORMAT_PCM16)
        {
            mVAG->mSrcDataOffset = mDataOffset[mCurrentIndex];
            mVAG->mFile          = mFile;

            FMOD_memcpy(mVAG->mWaveFormatMemory, &waveformat, sizeof(FMOD_CODEC_WAVEFORMAT));

            result = mVAG->setPositionInternal(subsound, position, postype);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        else
#endif
#ifdef FMOD_SUPPORT_CELT
        if (srcmode & FSOUND_CELT && waveformat.format == FMOD_SOUND_FORMAT_PCM16)
        {
            mCELT->mSrcDataOffset = mDataOffset[mCurrentIndex];
            mCELT->mFile          = mFile;

            FMOD_memcpy(mCELT->mWaveFormatMemory, &waveformat, sizeof(FMOD_CODEC_WAVEFORMAT));

            result = mCELT->setPositionInternal(subsound, position, postype);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        else
#endif
        {
            result = SoundI::getBytesFromSamples(position, &pos, waveformat.channels, waveformat.format);
            if (result != FMOD_OK)
            {
                return result;
            }

            pos += mDataOffset[mCurrentIndex];

            result = mFile->seek(pos, SEEK_SET);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
    }

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "CodecFSB::setPositionInternal", "done\n"));

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
FMOD_RESULT CodecFSB::getPositionInternal(unsigned int *position, FMOD_TIMEUNIT postype)
{
    FMOD_RESULT result;

    unsigned int pos;
    FMOD_CODEC_WAVEFORMAT waveformat;

    getWaveFormatInternal(mCurrentIndex, &waveformat);

    result = mFile->tell(&pos);
    if (result != FMOD_OK)
    {
        return result;
    }

    pos -= mDataOffset[mCurrentIndex];

    if (postype == FMOD_TIMEUNIT_PCM && waveformat.format == FMOD_SOUND_FORMAT_GCADPCM)
    {
        SoundI::getSamplesFromBytes(pos, position, waveformat.channels, waveformat.format);
    }
    else
    {
        return FMOD_ERR_INVALID_PARAM;
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
FMOD_RESULT CodecFSB::getNumSyncPoints(int subsound, int *numsyncpoints)
{
    char *syncpointdata = mSyncPointData[subsound];

    if (!syncpointdata)
    {
        *numsyncpoints = 0;
        return FMOD_OK;
    }

    syncpointdata += 4;     /* Skip 'SYNC' label. */
    
    *numsyncpoints = *((int *)syncpointdata);

    #ifdef PLATFORM_ENDIAN_BIG
    *numsyncpoints = FMOD_SWAPENDIAN_DWORD(*numsyncpoints);
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
FMOD_RESULT CodecFSB::getSyncPointData(int subsound, int index, char **name, int *offset)
{
    char *syncpointdata = mSyncPointData[subsound];

    syncpointdata += 8;     /* Skip 'SYNC' label, Skip 'numsyncpoints' dword. */

    if (mShdr[subsound]->mode & FSOUND_SYNCPOINTS_NONAMES)
    {
        SYNCDATA_NONAME *syncdata = (SYNCDATA_NONAME *)syncpointdata;
   
        *name = 0;
        *offset = syncdata[index].offset;

        #ifdef PLATFORM_ENDIAN_BIG
        *offset = FMOD_SWAPENDIAN_DWORD(*offset);
        #endif
    }
    else
    {
        SYNCDATA *syncdata = (SYNCDATA *)syncpointdata;

		*name = syncdata[index].name;
		*offset = syncdata[index].offset;

        #ifdef PLATFORM_ENDIAN_BIG
        *offset = FMOD_SWAPENDIAN_DWORD(*offset);
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
FMOD_RESULT CodecFSB::soundcreateInternal(int subsound, FMOD_SOUND *sound)
{
    FMOD_RESULT            result;
    SoundI                *s = (SoundI *)sound;
    FMOD_CODEC_WAVEFORMAT  waveformat;

    getWaveFormatInternal(subsound, &waveformat);

    if (!(mMode & FMOD_CREATESTREAM))
    {
        mChannels = waveformat.channels;
    }

    {
        FMOD_FSB_SAMPLE_HEADER *shdr = (FMOD_FSB_SAMPLE_HEADER *)(mHeader.mode & FMOD_FSB_SOURCE_BASICHEADERS ? mFirstSample : mShdr[subsound]);
        FMOD_FSB_SAMPLE_HEADER tmp;

        FMOD_memcpy(&tmp, shdr, sizeof(FMOD_FSB_SAMPLE_HEADER));   /* FMOD_memcpy'd instead of pointed to for alignment reasons */

        result = s->setDefaults((float)tmp.deffreq, (float)tmp.defvol / 255.0f, tmp.defpan == 0 ? -1.0f : tmp.defpan == 255 ? 1.0f : tmp.defpan == 128 ? 0.0f : ((float)tmp.defpan / 255.0f * 2.0f) - 1.0f, tmp.defpri);
        if (result != FMOD_OK)
        {
            return result;
        }
#ifndef FMOD_FSB_FORCE_3_0
        result = s->set3DMinMaxDistance(tmp.mindistance, tmp.maxdistance);
        if ((result != FMOD_OK) && (result != FMOD_ERR_NEEDS3D))
        {
            return result;
        }
#endif
    }

    if (mSyncPointData)
    {
        int count, numsyncpoints;

        if (getNumSyncPoints(subsound, &numsyncpoints) == FMOD_OK)
        {
            char *name;
            int offset;

			for (count = 0; count < numsyncpoints; count++)
			{
                getSyncPointData(subsound, count, &name, &offset);

				s->addSyncPointInternal(offset, FMOD_TIMEUNIT_PCM, name, 0, subsound, false);
			}

            s->syncPointFixIndicies();
        }
    }

    return FMOD_OK;
}

#endif //!PLATFORM_PS3_SPU


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
FMOD_RESULT CodecFSB::getWaveFormatInternal(int index, FMOD_CODEC_WAVEFORMAT *waveformat_out)
{
    FMOD_memset(waveformat_out, 0, sizeof(FMOD_CODEC_WAVEFORMAT));

    if (mHeader.mode & FMOD_FSB_SOURCE_BASICHEADERS)
    {
        unsigned int mode = 0;

        #ifdef PLATFORM_PS3_SPU

        char  firstsamplemem[sizeof(FMOD_FSB_SAMPLE_HEADER) + 32];      // 80 + 32 bytes
        char *firstsample = (char *)FMOD_ALIGNPOINTER(firstsamplemem, 16);

        FMOD_PS3_SPU_AlignedDMA((void **)&firstsample, (unsigned int)mFirstSample, sizeof(FMOD_FSB_SAMPLE_HEADER));

        mFirstSample = (FMOD_FSB_SAMPLE_HEADER *)firstsample;
        
        #endif

        #ifndef PLATFORM_PS3_SPU
        if (mFirstSample->mode & FSOUND_HW3D || mFirstSample->mode & FSOUND_HW2D)
        {
            mode |= FMOD_HARDWARE;
            if (mFirstSample->mode & FSOUND_HW2D)
            {
                mode |= FMOD_2D;
            }
            if (mFirstSample->mode & FSOUND_HW3D)
            {
                mode |= FMOD_3D;
            }
        }
        else
        #endif
        {
            mode |= FMOD_SOFTWARE;

            if (mFirstSample->mode & FSOUND_2D)
            {
                mode |= FMOD_2D;
            }
            if (mFirstSample->mode & FSOUND_3D)
            {
                mode |= FMOD_3D;
            }
        }

        waveformat_out->mode = (FMOD_MODE)mode;
        #ifndef PLATFORM_PS3_SPU
        FMOD_strncpy(waveformat_out->name, mFirstSample->name, FMOD_STRING_MAXNAMELEN);
        #endif
        waveformat_out->channels      = mFirstSample->numchannels;
        waveformat_out->frequency     = mFirstSample->deffreq;

        /*
            Set FMOD Ex format based on fmod3 flags.
        */
        if (mFirstSample->mode & FSOUND_8BITS)
        {
            waveformat_out->format = FMOD_SOUND_FORMAT_PCM8;
        }
        else if (mFirstSample->mode & FSOUND_16BITS)
        {
            waveformat_out->format = FMOD_SOUND_FORMAT_PCM16;
        }
        else if (mFirstSample->mode & FSOUND_32BITS)
        {
            waveformat_out->format = FMOD_SOUND_FORMAT_PCMFLOAT;
        }
        else if (mFirstSample->mode & FSOUND_IMAADPCM)
        {
            #ifdef FMOD_SUPPORT_IMAADPCM
            #ifdef PLATFORM_XBOX
            if (mUserMode & FMOD_HARDWARE)
            {
                waveformat_out->format = FMOD_SOUND_FORMAT_IMAADPCM;
            }
            else
            #endif
            {
                if (mADPCM)
                {
                    waveformat_out->format  = FMOD_SOUND_FORMAT_IMAADPCM;
                }
                else
                {
                    waveformat_out->format  = FMOD_SOUND_FORMAT_PCM16;
                }
    	    }
            #else
            return FMOD_ERR_FORMAT;
            #endif
        }
        else if (mFirstSample->mode & FSOUND_VAG)
        {
            #ifdef FMOD_SUPPORT_VAG
            if (mVAG)
            {
                waveformat_out->format = FMOD_SOUND_FORMAT_PCM16;
                waveformat_out->blockalign = 28 * sizeof(short) * waveformat_out->channels;
            }
            else
            {
                waveformat_out->format = FMOD_SOUND_FORMAT_VAG;
            }
            #else
            return FMOD_ERR_FORMAT;
            #endif
        }
        #ifdef FMOD_SUPPORT_GCADPCM
        else if (mFirstSample->mode & FSOUND_GCADPCM)
        {
            waveformat_out->format = FMOD_SOUND_FORMAT_GCADPCM;
        }
        #endif
        #ifdef FMOD_SUPPORT_XMA
        else if (mFirstSample->mode & FSOUND_XMA)
        {
            if ((waveformat_out->mode & FMOD_SOFTWARE || mUserMode & FMOD_SOFTWARE || mUserMode & FMOD_CREATESTREAM) && !(mUserMode & (FMOD_CREATECOMPRESSEDSAMPLE | FMOD_HARDWARE)))
            {
                waveformat_out->format = FMOD_SOUND_FORMAT_PCM16;
            }
            else
            {
                waveformat_out->format = FMOD_SOUND_FORMAT_XMA;
                waveformat_out->mode |= FMOD_CREATECOMPRESSEDSAMPLE;
            }
        }
        #endif                
        #ifdef FMOD_SUPPORT_MPEG
        else if (mFirstSample->mode & FSOUND_MPEG)
        {
            #ifdef FMOD_SUPPORT_STREAMING
            if (mUserMode & FMOD_CREATECOMPRESSEDSAMPLE && (waveformat_out->mode & FMOD_SOFTWARE || mUserMode & FMOD_SOFTWARE) && !(mUserMode & FMOD_CREATESTREAM))
            #else
            if (mUserMode & FMOD_CREATECOMPRESSEDSAMPLE && (waveformat_out->mode & FMOD_SOFTWARE || mUserMode & FMOD_SOFTWARE))
            #endif
            {
                waveformat_out->format = FMOD_SOUND_FORMAT_MPEG;
                waveformat_out->mode |= FMOD_CREATECOMPRESSEDSAMPLE;
            }
            else
            {
                waveformat_out->format = FMOD_SOUND_FORMAT_PCM16;
            }
        }
        #endif                
        
        /*
            Block size override based on machine specific block sizes.
        */
        if (!waveformat_out->blockalign)
        {
            if (0) {}
#ifdef FMOD_SUPPORT_VAG
            else if (waveformat_out->format == FMOD_SOUND_FORMAT_VAG)
            {
                waveformat_out->blockalign = 0;
                while (waveformat_out->blockalign <= SOUND_READCHUNKSIZE)
                {
                    waveformat_out->blockalign += 16 * waveformat_out->channels;   /* Make the block size bigger to do bigger dma chunks. */
                }
                waveformat_out->blockalign -= 16 * waveformat_out->channels;       /* Rewind one to get it under. */
            }
#endif
#ifdef FMOD_SUPPORT_GCADPCM
            else if (waveformat_out->format == FMOD_SOUND_FORMAT_GCADPCM)
            {
                waveformat_out->blockalign = 32 * waveformat_out->channels;
            }
#endif
#ifdef FMOD_SUPPORT_XMA
            else if (waveformat_out->format == FMOD_SOUND_FORMAT_XMA)
            {
                waveformat_out->blockalign = 2048;
            }
#endif
#ifdef FMOD_SUPPORT_MPEG
            else if (waveformat_out->format == FMOD_SOUND_FORMAT_MPEG)
            {
                waveformat_out->blockalign = 1152 * sizeof(signed short) * waveformat_out->channels;
            }
#endif
            else
            {
                SoundI::getBytesFromSamples(1, (unsigned int *)&waveformat_out->blockalign, waveformat_out->channels, waveformat_out->format);               
            }
        }

        if (index)
        {
            waveformat_out->lengthbytes = mShdrb[index]->lengthcompressedbytes;
            waveformat_out->lengthpcm   = mShdrb[index]->lengthsamples;
        }
        else
        {
            if (mShdrb[index])  /* This being non 0 is a special case where inclusionlist + numsubsounds is set in exinfo. */
            {
                waveformat_out->lengthbytes = mShdrb[index]->lengthcompressedbytes;
                waveformat_out->lengthpcm   = mShdrb[index]->lengthsamples;
            }
            else
            {
                waveformat_out->lengthpcm   = mFirstSample->lengthsamples;
                waveformat_out->lengthbytes = mFirstSample->lengthcompressedbytes;
            }
        }

        mode = mFirstSample->mode & FSOUND_CHANNELMODE_MASK;
        if (mode == FSOUND_CHANNELMODE_ALLMONO)
        {
            waveformat_out->channelmask = SPEAKER_ALLMONO;
        }
        else if (mode == FSOUND_CHANNELMODE_ALLSTEREO)
        {
            waveformat_out->channelmask = SPEAKER_ALLSTEREO;
        }
        else if (mode == FSOUND_CHANNELMODE_PROTOOLS)
        {
            waveformat_out->channelmask = SPEAKER_PROTOOLS;
        }
    }
    else
    {
        unsigned int mode = 0;

        #ifdef PLATFORM_PS3_SPU

        char  shdrmem[sizeof(FMOD_FSB_SAMPLE_HEADER) + 32];     // 80 + 32 bytes
        char *shdr = (char *)FMOD_ALIGNPOINTER(shdrmem, 16);

        unsigned int shdrmram = cellDmaGetUint32((uint64_t)&mShdr[index], TAG1, TID, RID);

        FMOD_PS3_SPU_AlignedDMA((void **)&shdr, shdrmram, sizeof(FMOD_FSB_SAMPLE_HEADER));

        mShdr[0] = (FMOD_FSB_SAMPLE_HEADER *)shdr;

        index = 0;

        #endif

        #ifndef PLATFORM_PS3_SPU
        if (mShdr[index]->mode & FSOUND_HW3D || mShdr[index]->mode & FSOUND_HW2D)
        {
            mode |= FMOD_HARDWARE;
            if (mShdr[index]->mode & FSOUND_HW2D)
            {
                mode |= FMOD_2D;
            }
            if (mShdr[index]->mode & FSOUND_HW3D)
            {
                mode |= FMOD_3D;
            }
        }
        else
        #endif
        {
            mode |= FMOD_SOFTWARE;

            if (mShdr[index]->mode & FSOUND_2D)
            {
                mode |= FMOD_2D;
            }
            if (mShdr[index]->mode & FSOUND_3D)
            {
                mode |= FMOD_3D;
            }
        }

        if (mShdr[index]->mode & FSOUND_LOOP_NORMAL)
        {
            mode |= FMOD_LOOP_NORMAL;
        }
        else if (mShdr[index]->mode & FSOUND_LOOP_BIDI)
        {
            mode |= FMOD_LOOP_BIDI;
        }

        waveformat_out->mode = (FMOD_MODE)mode;
        #ifndef PLATFORM_PS3_SPU
        int namelength = FMOD_MIN( sizeof(FMOD_CODEC_WAVEFORMAT().name), FMOD_STRING_MAXNAMELEN ); //hack - codec name length is hardcoded
        FMOD_strncpy(waveformat_out->name, mShdr[index]->name, namelength);
        FMOD_memset(waveformat_out->name + FMOD_FSB_NAMELEN, 0, namelength - FMOD_FSB_NAMELEN);
        #endif
        waveformat_out->channels    = mShdr[index]->numchannels;
        waveformat_out->frequency   = mShdr[index]->deffreq;
        waveformat_out->lengthpcm   = mShdr[index]->lengthsamples;
        waveformat_out->loopstart   = mShdr[index]->loopstart;
        waveformat_out->loopend     = mShdr[index]->loopend;
        waveformat_out->lengthbytes = mShdr[index]->lengthcompressedbytes;

        if (mShdr[index]->mode & FSOUND_8BITS)
        {
            waveformat_out->format = FMOD_SOUND_FORMAT_PCM8;
        }
        else if (mShdr[index]->mode & FSOUND_16BITS)
        {
            waveformat_out->format = FMOD_SOUND_FORMAT_PCM16;
        }
        else if (mShdr[index]->mode & FSOUND_32BITS)
        {
            waveformat_out->format = FMOD_SOUND_FORMAT_PCMFLOAT;
        }
        else if (mShdr[index]->mode & FSOUND_IMAADPCM)
        {
            #ifdef FMOD_SUPPORT_IMAADPCM
            #ifdef PLATFORM_XBOX
            if (mUserMode & FMOD_HARDWARE)
            {
                waveformat_out->format = FMOD_SOUND_FORMAT_IMAADPCM;
            }
            else
            #endif
            {
                if (mDecodeADPCM)
                {
                    waveformat_out->format = FMOD_SOUND_FORMAT_IMAADPCM;
                }
                else
                {
                    waveformat_out->format = FMOD_SOUND_FORMAT_PCM16;
                }
    	    }
            #else
            return FMOD_ERR_FORMAT;
            #endif
        }
        else if (mShdr[index]->mode & FSOUND_VAG)
        {
            #ifdef FMOD_SUPPORT_VAG
            if (mVAG)
            {
                waveformat_out->format = FMOD_SOUND_FORMAT_PCM16;
                waveformat_out->blockalign = 28 * sizeof(short) * waveformat_out->channels;
            }
            else
            {
                waveformat_out->format = FMOD_SOUND_FORMAT_VAG;
            }
            #else
            return FMOD_ERR_FORMAT;
            #endif
        }
#ifdef FMOD_SUPPORT_GCADPCM
        else if (mShdr[index]->mode & FSOUND_GCADPCM)
        {
            waveformat_out->format = FMOD_SOUND_FORMAT_GCADPCM;
        }
#endif
#ifdef FMOD_SUPPORT_XMA
        else if (mShdr[index]->mode & FSOUND_XMA)
        {               
            if (mUserMode & FMOD_CREATESTREAM || ((waveformat_out->mode & FMOD_SOFTWARE || mUserMode & FMOD_SOFTWARE) && !(mUserMode & FMOD_CREATECOMPRESSEDSAMPLE)))
            {
                waveformat_out->format = FMOD_SOUND_FORMAT_PCM16;
            }
            else
            {
                waveformat_out->format = FMOD_SOUND_FORMAT_XMA;
                waveformat_out->mode |= FMOD_CREATECOMPRESSEDSAMPLE;
            }
        } 
#endif            
#ifdef FMOD_SUPPORT_MPEG
        else if (mShdr[index]->mode & FSOUND_MPEG)
        {               
            #ifdef FMOD_SUPPORT_STREAMING
            if (mUserMode & FMOD_CREATECOMPRESSEDSAMPLE && !(mUserMode & FMOD_CREATESTREAM))
            #else
            if (mUserMode & FMOD_CREATECOMPRESSEDSAMPLE)
            #endif
            {
                waveformat_out->format = FMOD_SOUND_FORMAT_MPEG;
                waveformat_out->mode |= FMOD_CREATECOMPRESSEDSAMPLE;
            }
            else
            {
                waveformat_out->format = FMOD_SOUND_FORMAT_PCM16;
            }
        }
#endif
#ifdef FMOD_SUPPORT_CELT
        else if (mShdr[index]->mode & FSOUND_CELT)
        {               
            #ifdef FMOD_SUPPORT_STREAMING
            if (mUserMode & FMOD_CREATECOMPRESSEDSAMPLE && !(mUserMode & FMOD_CREATESTREAM))
            #else
            if (mUserMode & FMOD_CREATECOMPRESSEDSAMPLE)
            #endif
            {
                waveformat_out->format = FMOD_SOUND_FORMAT_CELT;
                waveformat_out->mode |= FMOD_CREATECOMPRESSEDSAMPLE;
            }
            else
            {
                waveformat_out->format = FMOD_SOUND_FORMAT_PCM16;
            }
        }
#endif
        
        /*
            Block size override based on machine specific block sizes.
        */
        if (!waveformat_out->blockalign)
        {
            if (0) {}
#ifdef FMOD_SUPPORT_VAG
            else if (waveformat_out->format == FMOD_SOUND_FORMAT_VAG)
            {
                waveformat_out->blockalign = 0;
                while (waveformat_out->blockalign <= SOUND_READCHUNKSIZE)
                {
                    waveformat_out->blockalign += 16 * waveformat_out->channels;   /* Make the block size bigger to do bigger dma chunks. */
                }
                waveformat_out->blockalign -= 16 * waveformat_out->channels;       /* Rewind one to get it under. */
            }
#endif
#ifdef FMOD_SUPPORT_GCADPCM
            else if (waveformat_out->format == FMOD_SOUND_FORMAT_GCADPCM)
            {
                waveformat_out->blockalign = 32 * waveformat_out->channels;
            }
#endif
#ifdef FMOD_SUPPORT_XMA
            else if (waveformat_out->format == FMOD_SOUND_FORMAT_XMA)
            {
                waveformat_out->blockalign = 2048;               
            }
#endif
#ifdef FMOD_SUPPORT_MPEG
            else if (waveformat_out->format == FMOD_SOUND_FORMAT_MPEG)
            {
                waveformat_out->blockalign = 1152 * sizeof(signed short) * waveformat_out->channels;
            }
#endif
            else
            {
                SoundI::getBytesFromSamples(1, (unsigned int *)&waveformat_out->blockalign, waveformat_out->channels, waveformat_out->format);               
            }
        }

        mode = mShdr[index]->mode & FSOUND_CHANNELMODE_MASK;
        if (mode == FSOUND_CHANNELMODE_ALLMONO)
        {
            waveformat_out->channelmask = SPEAKER_ALLMONO;
        }
        else if (mode == FSOUND_CHANNELMODE_ALLSTEREO)
        {
            waveformat_out->channelmask = SPEAKER_ALLSTEREO;
        }
        else if (mode == FSOUND_CHANNELMODE_PROTOOLS)
        {
            waveformat_out->channelmask = SPEAKER_PROTOOLS;
        }
    }

    if (!waveformat_out->channelmask)
    {
        if (waveformat_out->channels == 4)
        {
            waveformat_out->channelmask = SPEAKER_QUAD;
        }
        else if (waveformat_out->channels == 6)
        {
            waveformat_out->channelmask = SPEAKER_5POINT1;
        }
    }

    return FMOD_OK;
}


#ifdef FMOD_SUPPORT_MEMORYTRACKER

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
FMOD_RESULT CodecFSB::getMemoryUsedImpl(MemoryTracker *tracker)
{
    if (mShdrb)
    {
        tracker->add(false, FMOD_MEMBITS_CODEC, sizeof(void *) * mHeader.numsamples);
    }
    else if (mShdr)
    {
        tracker->add(false, FMOD_MEMBITS_CODEC, sizeof(void *) * mHeader.numsamples);
    }

    tracker->add(false, FMOD_MEMBITS_CODEC, mHeader.shdrsize);

    if (mDataOffset)
    {
        tracker->add(false, FMOD_MEMBITS_CODEC, sizeof(unsigned int) * mHeader.numsamples);
    }

    #ifdef FMOD_SUPPORT_IMAADPCM
    if (mADPCM)
    {
        tracker->add(false, FMOD_MEMBITS_CODEC, sizeof(CodecWav));
        
        if (mADPCM->mWaveFormatMemory)
        {
            tracker->add(false, FMOD_MEMBITS_CODEC, sizeof(FMOD_CODEC_WAVEFORMAT));
        }

        if (mADPCM->mPCMBufferMemory)
        {
            tracker->add(false, FMOD_MEMBITS_CODEC, mADPCM->mPCMBufferLengthBytes + 16);
        }
    }
    #endif
    #ifdef FMOD_SUPPORT_MPEG
    if (mMPEG)
    {    
        tracker->add(false, FMOD_MEMBITS_CODEC, sizeof(CodecMPEG));

        if (mMPEG->mWaveFormatMemory)
        {
             tracker->add(false, FMOD_MEMBITS_CODEC, sizeof(FMOD_CODEC_WAVEFORMAT));
        }

        if (mMPEG->mMemoryBlockMemory)
        {
             tracker->add(false, FMOD_MEMBITS_CODEC, sizeof(CodecMPEG_MemoryBlock) * (mChannels > 2 ? mChannels : 1) + 16);
        }

        if (mMPEG->mPCMBufferMemory)
        {
            tracker->add(false, FMOD_MEMBITS_CODEC, mMPEG->mPCMBufferLengthBytes + 16);
        }
    }
    #endif
    #ifdef FMOD_SUPPORT_VAG
    if (mVAG)
    {
        tracker->add(false, FMOD_MEMBITS_CODEC, sizeof(CodecVAG));

        if (mVAG->mWaveFormatMemory)
        {
             tracker->add(false, FMOD_MEMBITS_CODEC, sizeof(FMOD_CODEC_WAVEFORMAT));
        }
    }
    #endif

    if (mReadBuffer)
    {
        tracker->add(false, FMOD_MEMBITS_CODEC, mReadBufferLength);
    }

    if (mPCMBufferMemory)
    {
        tracker->add(false, FMOD_MEMBITS_CODEC, mPCMBufferLengthBytes + 16);
    }

    if (mSyncPointData)
    {
        tracker->add(false, FMOD_MEMBITS_CODEC, mHeader.numsamples * sizeof(char *));
    }


#ifdef FMOD_FSB_USEHEADERCACHE
    /*
        NEED TO WORK OUT HOW TO MAKE THIS ONLY HAPPEN ONCE.  WE CANT PUT IT IN SYSTEMI::GETMEMORYUSED - THEN THE PLUGIN VERSION COMPLAINS.
    */
    {
        CodecFSBCache *current = 0;

        FMOD_OS_CriticalSection_Enter(FMOD::gGlobal->gAsyncCrit);

        for (current = (CodecFSBCache *)CodecFSB::gCacheHead.getNext(); current != &CodecFSB::gCacheHead; current = (CodecFSBCache *)current->getNext())
        {
            tracker->add(false, FMOD_MEMBITS_CODEC, sizeof(CodecFSBCache));
        }

        FMOD_OS_CriticalSection_Leave(FMOD::gGlobal->gAsyncCrit);
    }
#endif  
    return FMOD_OK;
}

#endif


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
FMOD_RESULT CodecFSB::resetInternal() 
{
    #ifdef FMOD_SUPPORT_XMA
    if (mXMA)
    {
        mXMA->reset();
    }
    #endif
    #ifdef FMOD_SUPPORT_IMAADPCM
    if (mADPCM)
    {
        mADPCM->reset();
    }
    #endif
    #ifdef FMOD_SUPPORT_MPEG
    if (mMPEG)
    {
        mMPEG->reset();
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
FMOD_RESULT CodecFSB::canPointInternal() 
{
    #ifdef FMOD_SUPPORT_XMA
    if (mXMA)
    {
        return FMOD_ERR_MEMORY_CANTPOINT;
    }
    #endif
    #ifdef FMOD_SUPPORT_IMAADPCM
    if (mADPCM)
    {
        return FMOD_ERR_MEMORY_CANTPOINT;
    }
    #endif
    #ifdef FMOD_SUPPORT_MPEG
    if (mMPEG)
    {
        return FMOD_ERR_MEMORY_CANTPOINT;
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
FMOD_RESULT F_CALLBACK CodecFSB::openCallback(FMOD_CODEC_STATE *codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO *userexinfo)
{
    CodecFSB *fsb = (CodecFSB *)codec;    

    return fsb->openInternal(usermode, userexinfo);
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
FMOD_RESULT F_CALLBACK CodecFSB::closeCallback(FMOD_CODEC_STATE *codec)
{
    CodecFSB *fsb = (CodecFSB *)codec;    

    return fsb->closeInternal();
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
FMOD_RESULT F_CALLBACK CodecFSB::readCallback(FMOD_CODEC_STATE *codec, void *buffer, unsigned int sizebytes, unsigned int *bytesread)
{
    CodecFSB *fsb = (CodecFSB *)codec;    

    return fsb->readInternal(buffer, sizebytes, bytesread);
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
FMOD_RESULT F_CALLBACK CodecFSB::setPositionCallback(FMOD_CODEC_STATE *codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    CodecFSB *fsb = (CodecFSB *)codec;    

    return fsb->setPositionInternal(subsound, position, postype);
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
FMOD_RESULT F_CALLBACK CodecFSB::getPositionCallback(FMOD_CODEC_STATE *codec, unsigned int *position, FMOD_TIMEUNIT postype)
{
    CodecFSB *fsb = (CodecFSB *)codec;    

    return fsb->getPositionInternal(position, postype);
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
FMOD_RESULT F_CALLBACK CodecFSB::soundcreateCallback(FMOD_CODEC_STATE *codec, int subsound, FMOD_SOUND *sound)
{
    CodecFSB *fsb = (CodecFSB *)codec;    

    return fsb->soundcreateInternal(subsound, sound);
}

#endif //!PLATFORM_PS3_SPU


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
FMOD_RESULT F_CALLBACK CodecFSB::getWaveFormatCallback(FMOD_CODEC_STATE *codec, int subsound, FMOD_CODEC_WAVEFORMAT *waveformat)
{
    CodecFSB *fsb = (CodecFSB *)codec;    

    return fsb->getWaveFormatInternal(subsound, waveformat);
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
FMOD_RESULT F_CALLBACK CodecFSB::resetCallback(FMOD_CODEC_STATE *codec)
{
    CodecFSB *fsb = (CodecFSB *)codec;    

    return fsb->resetInternal();
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
FMOD_RESULT F_CALLBACK CodecFSB::canPointCallback(FMOD_CODEC_STATE *codec)
{
    CodecFSB *fsb = (CodecFSB *)codec;    

    return fsb->canPointInternal();
}


#ifdef FMOD_SUPPORT_MEMORYTRACKER
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
FMOD_RESULT F_CALLBACK CodecFSB::getMemoryUsedCallback(FMOD_CODEC_STATE *codec, MemoryTracker *tracker)
{
    CodecFSB *fsb = (CodecFSB *)codec;    

    return fsb->getMemoryUsed(tracker);
}
#endif


#endif //!PLATFORM_PS3_SPU

}

#endif

