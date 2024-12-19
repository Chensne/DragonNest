#ifndef _FMOD_FILE_H
#define _FMOD_FILE_H

#include "fmod_settings.h"

#include "fmod_linkedlist.h"
#include "fmod_memory.h"
#include "fmod_metadata.h"
#include "fmod_os_misc.h"
#include "fmod_time.h"

//#ifndef PLATFORM_PS3_SPU
    #include "fmod_thread.h"
//#endif

#define FMOD_FILE_QUEUEREAD   0
#define FMOD_FILE_QUEUESEEK   1
#define FMOD_FILE_QUEUEMAX    64            /* Must be a power of 2 */

#ifdef PLATFORM_PS2
    #ifndef SEEK_SET
    #define SEEK_SET	(0)
    #endif
    #ifndef SEEK_CUR
    #define SEEK_CUR	(1)
    #endif
    #ifndef SEEK_END
    #define SEEK_END	(2)
    #endif
#endif

#define PS3_DMAFILE_BLOCKSIZE   256

namespace FMOD
{
    class File;

    const unsigned int FILE_SECTORSIZE  = (2*1024);  /* default.  CD sector size */

    typedef struct
    {
        int                 mType;
        File               *mHandle;
        void               *mReadBuff;
        int                 mReadSize;
        int                 mReadCount;
        int                 mSeekPos;
        signed char         mSeekMode;
    } FileQueue;

    enum
    {
        DEVICE_USER = 0,
        DEVICE_MEMORY,
        DEVICE_RECORD,
        DEVICE_NET,
        DEVICE_CDDA,
        DEVICE_DISK
    };

    #define FMOD_FILE_SEEKABLE        0x00000001
    #define FMOD_FILE_BUFFERISSTRING  0x00000002
    #define FMOD_FILE_UNICODE         0x00000004
    #define FMOD_FILE_BIGENDIAN       0x00000008
    #define FMOD_FILE_BUSY            0x00000010
    #define FMOD_FILE_STARVING        0x00000020
    #define FMOD_FILE_EXIT            0x00000040
    #define FMOD_FILE_FLIPPING        0x00000080
    #define FMOD_FILE_FRONTBUFFERFULL 0x00000100
    #define FMOD_FILE_BACKBUFFERFULL  0x00000200
    #define FMOD_FILE_USEOLDDECRYPT   0x00000400
    #define FMOD_FILE_STREAMING       0x00000800  /* PS3 - If using FIOS, we can set streaming file reads to higher priority */

    #if defined(PLATFORM_WII) || defined(PLATFORM_GC)
        #define FMOD_FILE_ALIGN 32
    #else
        #define FMOD_FILE_ALIGN 0
    #endif

    class FileThread : public LinkedListNode  /* This linked list node entry is for File::gFileThreadHead */
    {
#ifndef PLATFORM_PS3_SPU
      public :

        Thread                   mThread;
        bool                     mThreadActive;
        LinkedListNode           mFileListHead;
        LinkedListNode          *mFileListCurrent;
        LinkedListNode          *mFileListNext;
        FMOD_OS_CRITICALSECTION *mFileListCrit;
        int                      mDeviceType;
        bool                     mOwned;

        FMOD_RESULT              threadFunc();

        FileThread();

        FMOD_RESULT     init(int devicetype, bool owned, SystemI *system);
        FMOD_RESULT     release();
#endif
    };

    class File : public LinkedListNode      /* This linked list node entry is for FileThread::mHead */
    {
        DECLARE_MEMORYTRACKER

        friend class FileThread;

      protected:
                                
	    unsigned int             mLength;
        unsigned int             mLengthOriginal;
        unsigned int             mFileSize;
        void                    *mRiderUserData;
        void                    *mRiderHandle;
        int                      mDeviceType;
                                
      private:                  
                                
		FMOD_PPCALIGN16(char               mName[FMOD_STRING_MAXNAMELEN]);
        FMOD_PPCALIGN16(char               mEncryptionKey[32]);
        FMOD_PPCALIGN16(int                mEncryptionKeyLength);
        FMOD_PPCALIGN16(int                mEncryptionKeyIndex);
        FMOD_PPCALIGN16(unsigned int       mBufferPos);
        FMOD_PPCALIGN16(unsigned int       mBufferSize);
        FMOD_PPCALIGN16(unsigned int       mBufferSkip);
        FMOD_PPCALIGN16(unsigned int       mBlockSize);
        FMOD_PPCALIGN16(unsigned int       mBlockOffset);
        FMOD_PPCALIGN16(unsigned int       mCurrentPosition);
        FMOD_PPCALIGN16(unsigned int       mNextPosition);
        FMOD_PPCALIGN16(unsigned int       mNextPositionDisplay);
        FMOD_PPCALIGN16(unsigned int       mStartOffset);
        FMOD_PPCALIGN16(int                mPercentBuffered);
        FMOD_PPCALIGN16(FMOD_RESULT        mAsyncError);
        FMOD_PPCALIGN16(FileThread        *mFileThread);
        FMOD_PPCALIGN16(FMOD_OS_SEMAPHORE *mSema);
                                       
        FMOD_RESULT              flip(bool frommainthread);
        FMOD_RESULT              checkBufferedStatus();
        FMOD_RESULT              seekAndReset();
#ifndef PLATFORM_PS3_SPU         
        FMOD_RESULT              getFileThread();
#endif
                               
      public:                  
                               
        unsigned int             mFlags;
        SystemI                 *mSystem;

        #ifdef PLATFORM_PS3
        char                     mBufferMemoryPS3[PS3_DMAFILE_BLOCKSIZE]  __attribute__((aligned(16)));
        #endif

        void                    *mBuffer;
        void                    *mBufferMemory;

        File();
        
        FMOD_RESULT             init(SystemI *system, unsigned int size, int blocksize);

        FMOD_RESULT             getByte(unsigned char  *byte = 0);
        FMOD_RESULT             getByte(unsigned short *byte);
        FMOD_RESULT             getByte(unsigned int   *byte);
        FMOD_RESULT             getByte(signed char    *byte);
        FMOD_RESULT             getByte(signed short   *byte);
        FMOD_RESULT             getByte(signed int     *byte);
        FMOD_RESULT             getWord(unsigned short *word = 0);
        FMOD_RESULT             getWord(unsigned int   *word);
        FMOD_RESULT             getWord(signed short   *word);
        FMOD_RESULT             getWord(signed int     *word);
        FMOD_RESULT             getDword(unsigned int  *dword = 0);
        FMOD_RESULT             getDword(signed int    *dword);
                                
        FMOD_RESULT             setStartOffset(unsigned int offset);
        FMOD_RESULT             getStartOffset(unsigned int *offset);
        FMOD_RESULT             setBuffer(void *buffer) { mBuffer = buffer; return FMOD_OK; }
        FMOD_RESULT             enableDoubleBuffer(unsigned int sizebytes, void *oldbuffer = 0);
                                
        FMOD_RESULT             getName(char **name);
        FMOD_RESULT             setName(char *name);
        FMOD_RESULT             setBigEndian(bool bigendian) { bigendian ? mFlags |= FMOD_FILE_BIGENDIAN : mFlags &= ~FMOD_FILE_BIGENDIAN; return FMOD_OK; }
        FMOD_RESULT             isBusy(bool *isbusy, unsigned int *percent) { if (isbusy) *isbusy = (mFlags & FMOD_FILE_BUSY ? true : false); if (percent) *percent = mPercentBuffered; return FMOD_OK; }
        FMOD_RESULT             isStarving(bool *starving) { if (!starving) return FMOD_ERR_INVALID_PARAM; *starving = (mFlags & FMOD_FILE_STARVING ? true : false); return FMOD_OK; }
                                
        virtual FMOD_RESULT     getMetadata(Metadata **metadata) { return FMOD_ERR_TAGNOTFOUND; }
        virtual FMOD_RESULT     getSize(unsigned int *size) { *size = mLength; return FMOD_OK; }                               
        virtual FMOD_RESULT     reallyOpen(const char *name_or_data, unsigned int *filesize) = 0;
        virtual FMOD_RESULT     reallyClose() = 0;
        virtual FMOD_RESULT     reallyRead(void *buffer, unsigned int size, unsigned int *read) = 0;
        virtual FMOD_RESULT     reallySeek(unsigned int pos) = 0;
        virtual FMOD_RESULT     reallyCancel() { return FMOD_OK; }

        FMOD_RESULT             open(const char *name_or_data, unsigned int length, bool unicode = false, const char *encryptionkey = 0);
        FMOD_RESULT             close();
        FMOD_RESULT             cancel();
        FMOD_RESULT             read(void *buffer, unsigned int size, unsigned int count, unsigned int *read = 0);
        FMOD_RESULT             seek(int pos, int mode);
        FMOD_RESULT             tell(unsigned int *pos);
        
      protected:

        friend class SystemI;

        #ifndef PLATFORM_PS3_SPU
        static FMOD_RESULT shutDown();
        #endif
   };
}

#endif


