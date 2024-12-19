#ifndef _FMOD_CODECI_H
#define _FMOD_CODECI_H

#include "fmod_settings.h"

#include "fmod.h"
#include "fmod_debug.h"
#include "fmod_file.h"
#include "fmod_file_memory.h"
#include "fmod_memory.h"
#include "fmod_os_misc.h"
#include "fmod_plugin.h"

#include <stdio.h>

namespace FMOD
{
    class Metadata;
    class ChannelReal;
    class MemoryTracker;

    typedef FMOD_RESULT (F_CALLBACK *FMOD_CODEC_INITCALLBACK) (FMOD_CODEC_STATE *codec, int numstreams);
    typedef FMOD_RESULT (F_CALLBACK *FMOD_CODEC_RESETCALLBACK)(FMOD_CODEC_STATE *codec);
    typedef FMOD_RESULT (F_CALLBACK *FMOD_CODEC_CANPOINTCALLBACK)(FMOD_CODEC_STATE *codec);
    typedef FMOD_RESULT (F_CALLBACK *FMOD_CODEC_GETMUSICNUMCHANNELS)(FMOD_CODEC_STATE *codec, int *numchannels);
    typedef FMOD_RESULT (F_CALLBACK *FMOD_CODEC_SETMUSICCHANNELVOLUME)(FMOD_CODEC_STATE *codec, int channel, float volume);
    typedef FMOD_RESULT (F_CALLBACK *FMOD_CODEC_GETMUSICCHANNELVOLUME)(FMOD_CODEC_STATE *codec, int channel, float *volume);
    typedef FMOD_RESULT (F_CALLBACK *FMOD_CODEC_GETHARDWAREMUSICHANNEL)(FMOD_CODEC_STATE *codec, ChannelReal **realchannel);
    typedef FMOD_RESULT (F_CALLBACK *FMOD_CODEC_UPDATE)(FMOD_CODEC_STATE *codec);
    typedef FMOD_RESULT (F_CALLBACK *FMOD_CODEC_GETMEMORYUSED)(FMOD_CODEC_STATE *codec, MemoryTracker *tracker);

    struct FMOD_CODEC_DESCRIPTION_EX : public FMOD_CODEC_DESCRIPTION, public SortedLinkedListNode
    {
        FMOD_SOUND_TYPE         mType;        
        int                     mSize;
        FMOD_OS_LIBRARY        *mModule;
        unsigned int            mHandle;

        FMOD_CODEC_INITCALLBACK             init;
        FMOD_CODEC_RESETCALLBACK            reset;
        FMOD_CODEC_CANPOINTCALLBACK         canpoint;
        FMOD_CODEC_GETMUSICNUMCHANNELS      getmusicnumchannels;
        FMOD_CODEC_SETMUSICCHANNELVOLUME    setmusicchannelvolume;
        FMOD_CODEC_GETMUSICCHANNELVOLUME    getmusicchannelvolume;
        FMOD_CODEC_GETHARDWAREMUSICHANNEL   gethardwaremusicchannel;
        FMOD_CODEC_UPDATE                   update;
        FMOD_CODEC_GETMEMORYUSED            getmemoryused;
    };


    typedef unsigned int FMOD_CODEC_FLAG;

    #define FMOD_CODEC_ACCURATELENGTH       0x00000001
    #define FMOD_CODEC_FROMFSB              0x00000002
    #define FMOD_CODEC_SEEKING              0x00000004
    #define FMOD_CODEC_PADDED               0x00000008
    #define FMOD_CODEC_USERLENGTH           0x00000010
    #define FMOD_CODEC_HARDWAREMUSICVOICES  0x00000020
    #define FMOD_CODEC_FSBXMAMIXEDCHANNELS  0x00000040
    #define FMOD_CODEC_PADDED4              0x00000080

    typedef struct
    {
        unsigned int    offset;
        char            name[256];
    } SYNCDATA;

    typedef struct
    {
        unsigned int    offset;
    } SYNCDATA_NONAME;

    class ChannelReal;

    class Codec : public Plugin, public FMOD_CODEC_STATE
    {
        DECLARE_MEMORYTRACKER_NONVIRTUAL                        /* Codecs are plugins and go through mDescription callback interface. */

      public:

        FMOD_CODEC_WAVEFORMAT      *mWaveFormatMemory;          /* NULL for FMOD_CREATECOMPRESSED codecs (they point to the sound's wave format), but alloced for each sound codec. */

        FMOD_SOUND_TYPE             mType;
        FMOD_CODEC_DESCRIPTION_EX   mDescription;
        unsigned int                mSrcDataOffset;
        unsigned int                mLoopPoints[2];
        int                         mSubSoundIndex;
        FMOD_CODEC_FLAG             mFlags;
        unsigned int                mBlockAlign;

        /*
            Temporary buffer for reading compressed data into.  Not used on all codecs.
        */
        unsigned char              *mReadBuffer;        /* For reading one block worth of compressed data into */
        unsigned int                mReadBufferLength;

        /*
            Buffer Logic for decompressing chunks of data into a temporary PCM buffer;
        */
        unsigned char              *mPCMBuffer;             /* For decompressing that one block of data into */
        unsigned char              *mPCMBufferMemory;
        unsigned int                mPCMBufferLength;
        unsigned int                mPCMBufferLengthBytes;
        unsigned int                mPCMBufferOffsetBytes;
        unsigned int                mPCMBufferFilledBytes;

        /*
            Stuff that is passed into the driver for internal use
        */
        FMOD_MODE                   mMode;                  /* This mode may have been changed by createSound */
        FMOD_MODE                   mOriginalMode;          /* Original mode as passed in to createSound by user */
        Metadata                   *mMetadata;
        File                       *mFile;
        bool                        mNonInterleaved;

        #ifdef FMOD_SUPPORT_XMA
        bool                        mXMASeekable;
        #endif

        static FMOD_INLINE FMOD_RESULT F_CALLBACK defaultFileRead     (void *handle, void *buffer, unsigned int size, unsigned int *read, void *userdata) { return ((File *)handle)->read(buffer, 1, size, read); }
        static FMOD_INLINE FMOD_RESULT F_CALLBACK defaultFileSeek     (void *handle, unsigned int pos, void *userdata) { return ((File *)handle)->seek(pos, SEEK_SET); }
#ifndef PLATFORM_PS3_SPU
        static FMOD_INLINE FMOD_RESULT F_CALLBACK defaultMetaData     (FMOD_CODEC_STATE *codec, FMOD_TAGTYPE type, char *name, void *data, unsigned int datalen, FMOD_TAGDATATYPE datatype, int unique) { return ((Codec *)codec)->metaData(type, name, data, datalen, datatype, unique == 1 ? true : false); }
#endif
        static FMOD_INLINE FMOD_RESULT F_CALLBACK defaultGetWaveFormat(FMOD_CODEC_STATE *codec, int index, FMOD_CODEC_WAVEFORMAT *waveformat_out)
        {
            if (!codec->waveformat)
            {
                return FMOD_ERR_INTERNAL;
            }

            if (index < 0 || (codec->numsubsounds == 0 && index > 0) || (codec->numsubsounds > 0 && index >= codec->numsubsounds))
            {
                return FMOD_ERR_INVALID_PARAM;
            }
            
            FMOD_memcpy(waveformat_out, &codec->waveformat[index], sizeof(FMOD_CODEC_WAVEFORMAT));

            return FMOD_OK;
        }

      public:

        Codec()
        {
            mType         = FMOD_SOUND_TYPE_UNKNOWN;
            mMetadata     = 0;
            fileread      = &defaultFileRead;
            fileseek      = &defaultFileSeek;
#ifndef PLATFORM_PS3_SPU
            metadata      = &defaultMetaData;            
#endif
        }

#ifndef PLATFORM_PS3_SPU
        FMOD_RESULT init(FMOD_SOUND_TYPE type)
        { 
            Plugin::init();

            mType        = type;
            mMetadata    = 0;           
            numsubsounds = 0;
            waveformat   = 0;
            
            return FMOD_OK;
        }

        FMOD_RESULT release();
        FMOD_RESULT getLength(unsigned int *length, FMOD_TIMEUNIT lengthtype);
        FMOD_RESULT getPosition(unsigned int *position, FMOD_TIMEUNIT postype);
        FMOD_RESULT metaData(FMOD_TAGTYPE type, const char *name, void *data, unsigned int datalen, FMOD_TAGDATATYPE datatype, bool unique);
        FMOD_RESULT getMetadataFromFile();
        FMOD_RESULT canPointTo() 
        {
            if (mDescription.canpoint)
            {
                return mDescription.canpoint(this);
            }
            else
            {
                return FMOD_ERR_MEMORY_CANTPOINT;
            }
        }
#endif
        FMOD_RESULT reset() 
        { 
            mPCMBufferOffsetBytes = 0; 
            if (mPCMBuffer) 
            {
                FMOD_memset(mPCMBuffer, 0, mPCMBufferLengthBytes); 
            }
            if (mDescription.reset) 
            {
                return mDescription.reset(this); 
            }
            return FMOD_OK;
        }
        FMOD_RESULT read(void *buffer, unsigned int size, unsigned int *bytesread);
        FMOD_RESULT setPosition(int subsound, unsigned int position, FMOD_TIMEUNIT postype);
        FMOD_RESULT getHardwareMusicChannel(ChannelReal **realchannel) 
        {
            if (mDescription.gethardwaremusicchannel)
            {
                return mDescription.gethardwaremusicchannel(this, realchannel);
            }
            else
            {
                return FMOD_ERR_INVALID_HANDLE; 
            }
        }
    };
}

#endif


