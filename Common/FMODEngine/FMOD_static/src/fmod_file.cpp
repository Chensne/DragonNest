#include "fmod_settings.h"

#include "fmod_debug.h"
#include "fmod_file.h"
#include "fmod_string.h"
#include "fmod_stringw.h"
#include "fmod_thread.h"
#include "fmod_time.h"
#include "fmod_types.h"

#ifdef PLATFORM_PS3_SPU
    #include "fmod_systemi_spu.h"
    #include "fmod_spu_printf.h"
    #include "fmod_file_dma.h"
#else
    #include "fmod_systemi.h"
#endif

#ifdef PLATFORM_PS2_EE
    #include "fmod_cmd.h"
#endif

#ifdef FMOD_SUPPORT_NET
  #include "fmod_file_net.h"
#endif

#ifdef FMOD_SUPPORT_CDDA
  #include "fmod_file_cdda.h"
  #include "fmod_os_cdda.h"
#endif

#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifndef PLATFORM_PS3_SPU
extern "C"
{

/*
[API]
[
	[DESCRIPTION]
	Mutex function to synchronize user file reads with FMOD's file reads.  This function tells fmod that you are using the disk so that it will 
    block until you are finished with it.<br>
    This function also blocks if FMOD is already using the disk, so that you cannot do a read at the same time FMOD is reading.

	[PARAMETERS]
    'busy'  1 = you are about to perform a disk access.  0 = you are finished with the disk.
 
	[RETURN_VALUE]

	[REMARKS]
    Use this function as a wrapper around your own file reading functions if you want to do simulatenous file reading while FMOD is also reading.
    ie
    <PRE>
    FMOD_File_SetDiskBusy(1);
    myfread(...);
    FMOD_File_SetDiskBusy(0);
    </PRE>
    Warning! This is a critical section internally.  If you do not match your busy = true with a busy = false your program may hang!<br>
    If you forget to set diskbusy to false it will stop FMOD from reading from the disk.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox360, PlayStation 2, PlayStation Portable, PlayStation 3, Wii, Solaris, iPhone

	[SEE_ALSO]
    File_GetDiskBusy
]
*/
FMOD_RESULT F_API FMOD_File_SetDiskBusy(int busy)
{
    #ifdef PLATFORM_PS2_EE
    {
        FMOD_RESULT result;
        struct
        {
            int busy;
        } cmd = { busy };

        result = FMOD_Command_Send(2, FMOD::FMOD_CMD_SETDISKBUSY, &cmd, sizeof(cmd), 0, 0, 0, true);
        if (result != FMOD_OK)
        {
            return result;
        }
    }
    #endif

    if (busy)
    {
        FMOD_OS_CriticalSection_Enter(FMOD::gGlobal->gFileCrit);
        FMOD::gGlobal->gFileBusy = true;
    }
    else
    {
        FMOD::gGlobal->gFileBusy = false;
        FMOD_OS_CriticalSection_Leave(FMOD::gGlobal->gFileCrit);
    }

    return FMOD_OK;
}


/*
[API]
[
	[DESCRIPTION]
	Information function to retreive the state of fmod's disk access.

	[PARAMETERS]
    'busy'      Address of an integer to receive the busy state of the disk at the current time.
 
	[RETURN_VALUE]

	[REMARKS]
    Do not use this function to syncrhonize your own reads with, as due to timing, you might call this function and it says false = it is not busy,
    but the split second after call this function, internally FMOD might set it to busy.  Use File_SetDiskBusy for proper mutual exclusion as it uses semaphores.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox360, PlayStation 2, PlayStation Portable, PlayStation 3, Wii, Solaris, iPhone

	[SEE_ALSO]
    File_SetDiskBusy
]
*/
FMOD_RESULT F_API FMOD_File_GetDiskBusy(int *busy)
{
    if (!busy)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *busy = FMOD::gGlobal->gFileBusy;

    return FMOD_OK;
}

}
#endif

namespace FMOD
{

#ifndef PLATFORM_PS3_SPU

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
void fileThreadFunc(void *data)
{
    FileThread *fileThread = (FileThread *)data;

    fileThread->threadFunc();
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
FMOD_RESULT FileThread::threadFunc()
{
    if (mThreadActive)
    {
        File           *file;

        FMOD_OS_CriticalSection_Enter(mFileListCrit);

        mFileListCurrent = mFileListHead.getNext();

        while (mFileListCurrent != &mFileListHead)
        {            
            mFileListNext = mFileListCurrent->getNext();

            file = SAFE_CAST(File, mFileListCurrent);
            if (file->mFlags & FMOD_FILE_FLIPPING)
            {
                FMOD_OS_CriticalSection_Leave(mFileListCrit);
        
                file->flip(false);
        
                FMOD_OS_CriticalSection_Enter(mFileListCrit);
            }

            mFileListCurrent = mFileListNext;
        }

        FMOD_OS_CriticalSection_Leave(mFileListCrit);
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
FileThread::FileThread()
{
    mFileListCrit    = 0;
    mFileListCurrent = 0;
    mFileListNext    = 0;
    mThreadActive    = false;
    mDeviceType      = 0;
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
FMOD_RESULT FileThread::init(int devicetype, bool owned, SystemI *system)
{
    FMOD_RESULT result;

    FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "FileThread::init", "created thread for %p\n", this));
    FLOG_INDENT(4);

    mDeviceType = devicetype;
    mOwned      = owned;

    result = FMOD_OS_CriticalSection_Create(&mFileListCrit);
    if (result != FMOD_OK)
    {
        FLOG_INDENT(-4);
        return result;
    }

    result = mThread.initThread("FMOD file thread", fileThreadFunc, this, FILE_THREADPRIORITY, 0, FILE_STACKSIZE, true, 0, system);
    if (result != FMOD_OK)
    {
        FLOG_INDENT(-4);
        FMOD_OS_CriticalSection_Free(mFileListCrit);
        return result;
    }

    mThreadActive = true;

    addAfter(&FMOD::gGlobal->gFileThreadHead);

    FLOG_INDENT(-4);
    FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "FileThread::init", "done\n", this));

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
FMOD_RESULT FileThread::release()
{
    removeNode();

    mThreadActive = false;

    mThread.closeThread();

    if (mFileListCrit)
    {
        FMOD_OS_CriticalSection_Free(mFileListCrit);
    }

    FMOD_Memory_Free(this);

    FLOG((FMOD_DEBUG_USER_ANDREW, __FILE__, __LINE__, "FileThread::release", "released thread for %p\n", this));

    return FMOD_OK;
}

#endif  //!PLATFORM_PS3_SPU

#ifndef PLATFORM_PS3_SPU

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
FMOD_RESULT File::getFileThread()
{
    FMOD_RESULT     result;
    LinkedListNode *current;
    int             devicetype;
    bool            found, owned;
    FileThread     *filethread = 0;
   
    FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "File::getFileThread", "creating file thread\n"));

    devicetype = DEVICE_DISK;
    owned      = false;

#ifdef FMOD_SUPPORT_NET
    if (!FMOD_strnicmp("http://", mName, 7))
    {
        devicetype = DEVICE_NET;
        owned      = true;
    }
#endif
#ifdef FMOD_SUPPORT_CDDA
    if (FMOD_OS_CDDA_IsDeviceName(mName))
    {
        devicetype = DEVICE_CDDA;
        owned      = true;
    }
#endif

    found = false;
    if (devicetype == DEVICE_DISK)
    {
        current = FMOD::gGlobal->gFileThreadHead.getNext();
        while (current != &FMOD::gGlobal->gFileThreadHead)
        {
            filethread = SAFE_CAST(FileThread, current);

            if (filethread->mDeviceType == DEVICE_DISK)
            {
                found = true;
                break;
            }

            current = current->getNext();
        }
    }

    if (!found)
    {
        filethread = FMOD_Object_Alloc(FileThread);
        if (!filethread)
        {
            return FMOD_ERR_MEMORY;
        }

        result = filethread->init(devicetype, owned, mSystem);
        if (result != FMOD_OK)
        {
            FMOD_Memory_Free(filethread);
            return result;
        }
    }

    mFileThread = filethread;

    FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "File::getFileThread", "done\n"));

    return FMOD_OK;
}

#endif // !PLATFORM_PS3_SPU

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
File::File()
{
    mFlags               = FMOD_FILE_BUFFERISSTRING | FMOD_FILE_SEEKABLE;
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
FMOD_RESULT File::init(SystemI *system, unsigned int filesize, int blocksize)
{
    mBlockSize           = blocksize;
    mLength              = 0;
    mFileThread          = 0;
    mRiderHandle         = 0;
    mRiderUserData       = 0;
    mSystem              = system;
    mStartOffset         = 0;
    mBuffer              = 0;
    mBufferMemory        = 0;
#ifdef PLATFORM_PS3
    mBufferSize          = mBlockSize;
#else
    mBufferSize          = 0;
#endif
    mCurrentPosition     = 0;
    mNextPosition        = 0;
    mNextPositionDisplay = 0;
    mBlockOffset         = 0;
    mBufferPos           = 0;
    mBufferSkip          = 0;
    mAsyncError          = FMOD_OK;
    mEncryptionKeyLength = 0;
    mEncryptionKeyIndex  = 0;	
    mSema                = 0;
    mPercentBuffered     = 0;
    
    mLengthOriginal      = filesize;
    mLength              = filesize;
    mFileSize            = filesize;

    FMOD_memset(mName, 0, FMOD_STRING_MAXNAMELEN);

    return FMOD_OK;
}

#ifndef PLATFORM_PS3_SPU

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
FMOD_RESULT File::shutDown()
{
    LinkedListNode *current, *next;

    FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "File::shutDown","\n"));

    current = FMOD::gGlobal->gFileThreadHead.getNext();
    while (current != &FMOD::gGlobal->gFileThreadHead)
    {
        FileThread *filethread;

        next = current->getNext();

        filethread = SAFE_CAST(FileThread, current);
        filethread->release();

        current = next;
    }

#ifdef FMOD_SUPPORT_NET
    NetFile::shutDown();
#endif

#ifdef FMOD_SUPPORT_CDDA
    CddaFile::shutDown();
#endif

    if (FMOD::gGlobal->gFileCrit)
    {
        FMOD_OS_CriticalSection_Free(FMOD::gGlobal->gFileCrit);
        FMOD::gGlobal->gFileCrit = 0;
    }

    FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "File::shutDown","done\n"));

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
FMOD_RESULT File::open(const char *name_or_data, unsigned int length, bool unicode, const char *encryptionkey)
{
    FMOD_RESULT result;

    FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "File::open", "name %s : length %d : unicode %s\n", name_or_data, length, unicode ? "YES" : "NO"));

    mFlags              &= ~FMOD_FILE_BIGENDIAN;
    mStartOffset         = 0;
    mCurrentPosition     = 0;
    mNextPosition        = 0;
    mNextPositionDisplay = 0;
    mBlockOffset         = 0;
    mBufferPos           = 0;
    mBufferSkip          = 0;
    mAsyncError          = FMOD_OK;
    mFlags              &= ~FMOD_FILE_EXIT;
    mFlags              &= ~FMOD_FILE_STARVING;
    mEncryptionKeyLength = 0;
    mEncryptionKeyIndex  = 0;	

    mLength              = length;
    mFileSize            = length;    

    unicode ? mFlags |= FMOD_FILE_UNICODE : mFlags &= ~FMOD_FILE_UNICODE;

    if (encryptionkey)
    {
        mEncryptionKeyLength = FMOD_strlen(encryptionkey);
        if (mEncryptionKeyLength > 32)
        {
            mEncryptionKeyLength = 32;
        }

        FMOD_memset(mEncryptionKey, 0, 32);
        FMOD_strncpy(mEncryptionKey, encryptionkey, mEncryptionKeyLength);
    }
    
    mBufferSize = mBlockSize;           /* Blocking single buffer */    
    if (mBufferSize && !mBuffer)
    {
        mBufferMemory = FMOD_Memory_Calloc(mBufferSize + FMOD_FILE_ALIGN);
        if (!mBufferMemory)
        {
            return FMOD_ERR_MEMORY;
        }
        
        #if (FMOD_FILE_ALIGN > 0)
        mBuffer = (void *)FMOD_ALIGNPOINTER(mBufferMemory, FMOD_FILE_ALIGN);
        #else
        mBuffer = mBufferMemory;
        #endif
    }

    result = reallyOpen(name_or_data, &mFileSize);
    if (result != FMOD_OK)
    {
        if (mBufferMemory)
        {
            FMOD_Memory_Free(mBufferMemory);
            mBuffer = mBufferMemory = 0;
        }      
        return result;
    }

    if (name_or_data && mFlags & FMOD_FILE_BUFFERISSTRING)
    {
        if (mFlags & FMOD_FILE_UNICODE)
        {
            FMOD_strncpyW((short*)mName, (short*)name_or_data, FMOD_STRING_MAXNAMELEN / 2);
            FMOD_wtoa((short*)mName);
        }
        else
        {
            FMOD_strncpy(mName, name_or_data, FMOD_STRING_MAXNAMELEN);
        }
        mName[FMOD_STRING_MAXNAMELEN - 1] = 0; //null terminator
    }

    #ifndef PLATFORM_PS3_SPU
    if (mSystem && mSystem->mOpenRiderCallback)
    {
        mSystem->mOpenRiderCallback(name_or_data, unicode, &mFileSize, &mRiderHandle, &mRiderUserData);
    }
    #endif

    if (!mLength)
    {
        mLength = mFileSize;
    }
    mLengthOriginal = mLength;

    FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "File::open", "done.  handle %p.  File size = %d\n", this, mLength));

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
FMOD_RESULT File::close()
{
    FMOD_RESULT result;

    FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "File::close", "handle %p\n", this));
    
    /*
        Cancel any pending file requests.
        This is needed for Wii / GC and to cancel net reads.
    */
    cancel();

    #ifndef PLATFORM_PS3_SPU
    if (mSema && mFlags & FMOD_FILE_BUSY)
    {
        FMOD_OS_Semaphore_Wait(mSema);
        FMOD_OS_Semaphore_Signal(mSema);
    }
    #endif

    if (mFileThread)
    {
        FMOD_OS_CriticalSection_Enter(mFileThread->mFileListCrit);
        if (this == mFileThread->mFileListNext)
        {
            mFileThread->mFileListNext = getNext();
        }
        removeNode();
        FMOD_OS_CriticalSection_Leave(mFileThread->mFileListCrit);

        if (mFileThread->mOwned)
        {
            mFileThread->release();
        }

        mFileThread = 0;
    }

    result = reallyClose();

    if (mSema)
    {
        FMOD_OS_Semaphore_Free(mSema);
    }

    #ifndef PLATFORM_PS3_SPU
    if (mSystem && mSystem->mCloseRiderCallback)
    {
        FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "File::close", "calling rider callback\n"));

        mSystem->mCloseRiderCallback(mRiderHandle, mRiderUserData);
    }
    #endif

    if (mBufferMemory)
    {
        FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "File::close", "free mBuffer (mBuffer = %p, mBufferMemory = %p)\n", mBuffer, mBufferMemory));
        FMOD_Memory_Free(mBufferMemory);
        mBuffer = mBufferMemory = 0;
    }

    FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "File::close", "handle %p done\n", this));

    return result;
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
FMOD_RESULT File::cancel()
{
    mFlags |= FMOD_FILE_EXIT;

    return reallyCancel();
}

#endif // !PLATFORM_PS3_SPU


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
FMOD_RESULT File::flip(bool frommainthread)
{
    FMOD_RESULT   result = FMOD_OK;

    if (frommainthread && mSema)
    {
        FMOD_OS_Semaphore_Wait(mSema);
    }
    {
        unsigned int  bytestoread = mBlockSize - mBufferSkip;
        unsigned int  bytestoreadorig;
        unsigned int  bytesread   = 0;
        char         *destptr     = (char *)mBuffer + mBlockOffset + mBufferSkip;

        mFlags |= FMOD_FILE_BUSY;

        FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "File::flip", "%p    **** fill   to %d with %d bytes.  mBlockOffset %d mBufferSkip %d\n", this, mBlockOffset + mBufferSkip, bytestoread, mBlockOffset, mBufferSkip));

        bytestoreadorig = bytestoread;

        while (bytestoread)
        {
            unsigned int  read;
            unsigned int  r = 0;

            read = bytestoread;

            #ifdef PLATFORM_PS3_SPU
            result = FMOD_DmaFile_ReadCallback(this, destptr, read, &r);
            #else
            result = reallyRead(destptr, read, &r);
            #endif
            if (result != FMOD_OK)
            {
                bytestoread = r;

                if ((int)mLength == -1 && result == FMOD_ERR_FILE_EOF)
                {
                    if (mNextPositionDisplay)
                    {
                        mLength = mFileSize = mLengthOriginal = mNextPositionDisplay;
                    }
                }
            }

            #ifndef PLATFORM_PS3_SPU
            if (mSystem && mSystem->mReadRiderCallback)
            {
                mSystem->mReadRiderCallback(mRiderHandle, destptr, r, 0, mRiderUserData);
            }
            #endif

            if (!r || r > bytestoread)
            {
                if (r > bytestoread)
                {
                    FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "File::flip", "    BAD LENGTH RETURNED FROM FILE READ FUNCTION, TERMINATING.\n"));
                }
                break;
            }

            bytestoread          -= r;
            destptr              += r;
            bytesread            += r;
            mNextPositionDisplay += r;

            mPercentBuffered = (int)(((float)mNextPositionDisplay - (float)mCurrentPosition) / (float)mBufferSize * 100.0f);
            if (mPercentBuffered < 0)
            {
                mPercentBuffered = 0;
            }

            if (mFlags & FMOD_FILE_EXIT)
            {
                mFlags &= ~FMOD_FILE_EXIT;
                break;
            }
        }

        FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "File::flip", "%p    **** filled to %8d got  %8d bytes\n", this, mBlockOffset + mBufferSkip, bytesread));

        if (!mBlockOffset)
        {
            mFlags |= FMOD_FILE_FRONTBUFFERFULL;
        }
        else
        {
            mFlags |= FMOD_FILE_BACKBUFFERFULL;
        }
        
        mBlockOffset += mBlockSize;
        if (mBlockOffset >= mBufferSize)
        {
            mBlockOffset = 0;
        }

        mBufferSkip = 0;
        mAsyncError = result;        
        mFlags &= ~FMOD_FILE_FLIPPING;
    }

    mFlags &= ~FMOD_FILE_BUSY;

    if (mSema)
    {
        FMOD_OS_Semaphore_Signal(mSema);
    }

    return result;
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
FMOD_RESULT File::seekAndReset()
{
    FMOD_RESULT result;
    unsigned int alignedpos;

    #ifndef PLATFORM_PS3_SPU
    if (mSema && mFlags & FMOD_FILE_BUSY)
    {
        FMOD_OS_Semaphore_Wait(mSema);
        FMOD_OS_Semaphore_Signal(mSema);
    }
    #endif

    alignedpos = mCurrentPosition;
    alignedpos /= mBufferSize;      /* We are seeking resetting to the start of the WHOLE buffer */
    alignedpos *= mBufferSize;       

    mBufferPos           = mCurrentPosition - alignedpos;
    mBlockOffset         = 0;
    mNextPosition        = alignedpos; /* This will immediately be incremeneted by the flip */
    mNextPositionDisplay = mNextPosition;
    mBufferSkip          = 0;

    mFlags &= ~FMOD_FILE_FRONTBUFFERFULL;   /* Reset state of buffers. */
    mFlags &= ~FMOD_FILE_BACKBUFFERFULL;


    FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "File::seekAndReset", "%p    seek to %d\n", this, alignedpos));
    FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "File::seekAndReset", "%p    reset mBufferPos to %d\n", this, mBufferPos));

    #ifdef PLATFORM_PS3_SPU

    result = FMOD_DmaFile_SeekCallback(this, alignedpos);

    #else

    result = reallySeek(alignedpos);

    #endif

    #ifndef PLATFORM_PS3_SPU
    if (mSystem && mSystem->mSeekRiderCallback)
    {
        mSystem->mSeekRiderCallback(mRiderHandle, alignedpos, mRiderUserData);
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

	[SEE_ALSO]
]
*/
FMOD_RESULT File::checkBufferedStatus()
{
    FMOD_RESULT result;
    int diff;

    #ifndef PLATFORM_PS3_SPU

    if (mAsyncError != FMOD_OK && mAsyncError != FMOD_ERR_FILE_EOF && mAsyncError != FMOD_ERR_FILE_DISKEJECTED)
    {
        return mAsyncError;
    }
    
    #endif

    if (mNextPosition < mCurrentPosition ||     /* If the position after the buffer */
        (mBufferSize > mBlockSize && !(mFlags & (FMOD_FILE_BACKBUFFERFULL | FMOD_FILE_FLIPPING)) && mCurrentPosition < mNextPosition - mBlockSize))    /* or behind the buffer. */
    {
        diff = -1;       
    }
    else
    {
        mPercentBuffered = (int)(((float)mNextPositionDisplay - (float)mCurrentPosition) / (float)mBufferSize * 100.0f);
        if (mPercentBuffered < 0 || mBufferSkip)
        {
            mPercentBuffered = 0;
        }

        diff = mNextPosition - mCurrentPosition;
        diff += (mBlockSize - 1);
        diff /= mBlockSize;
    }	

    if (mBufferSkip)
    {
        if (diff > 2)
        {
            mBufferSkip = 0;
        }
        else
        {
            diff = -1;                          /* We want to do a blocking read here.  Not a background flip. -1 stops it from going into first 2 if statements. */
            mNextPositionDisplay = mBufferSkip;
            mNextPosition        = mCurrentPosition - mBufferPos;
        }
    }

    FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "File::checkBufferedStatus", "%p    mCurrentPosition %d mNextPosition %d nextpos diffbytes %d diff %d\n", this, mCurrentPosition, mNextPosition, mNextPosition - mCurrentPosition, diff));

    if (diff != 2)
    {
        #ifndef PLATFORM_PS3_SPU
        if (mFlags & FMOD_FILE_BUSY)
        {
            mFlags |= FMOD_FILE_STARVING;
        
            FMOD_OS_Semaphore_Wait(mSema);
            FMOD_OS_Semaphore_Signal(mSema);

            mFlags &= ~FMOD_FILE_STARVING;
        }
        #endif
    }

    /* 
        If diff = 1, then we are reading from the front buffer, so non blocking fill the back buffer.
    */
    if(0)
    {
    }
#ifndef PLATFORM_PS3_SPU
    else if (diff == 1 && mBufferSize > mBlockSize)  /* This case is only for double buffered (buffsize > blocksize) */
    {
        FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "File::checkBufferedStatus", "%p    issue non blocking flip\n", this));
    
        mFlags |= FMOD_FILE_BUSY;
        mFlags |= FMOD_FILE_FLIPPING;

        FMOD_OS_Semaphore_Wait(mSema);      /* ENTER */

        mFileThread->mThread.wakeupThread();

        mNextPositionDisplay = mNextPosition;
        mNextPosition += mBlockSize;

        return FMOD_OK;
    }
#endif
    /* 
        If diff = 2, then we are reading from the front buffer and the fill is more than 1 ahead, so there is no current danger of undderun.
        This means we can just exit.
    */
    else if ((mBufferSize > mBlockSize && diff == 2) || (mBufferSize == mBlockSize && diff == 1)) /* This is the idle case.  If it is doublebuffered the diff must be 2, or if it is single buffered the diff must be 1 */
    {
        return FMOD_OK;
    }
    /*
        If diff = 0, or diff = something large (ie there was a seek), do a blocking read.
    */
    else
    {
        if (diff && mFlags & FMOD_FILE_SEEKABLE)  /* was there a seek */
        {
            result = seekAndReset();
            if (result != FMOD_OK)
            {
                return result;
            }
        }

        /*
            Fill the front buffer.
        */
        FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "File::checkBufferedStatus", "%p    FORCIBLY FILL FRONTBUFFER\n", this));
        
        result = flip(true);
        if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
        {
            if (result == FMOD_ERR_FILE_DISKEJECTED)
            {
                mFlags |= FMOD_FILE_STARVING;
            }
            return result;
        }
        mFlags &= ~FMOD_FILE_STARVING;

        if (mBufferSize == mBlockSize && result == FMOD_ERR_FILE_EOF && (int)mLength == -1)
        {
            return result;
        }

        mNextPositionDisplay = mNextPosition;
        mNextPosition += mBlockSize;

        /*
            Fill the back buffer.
        */
        if (mBufferPos >= mBlockSize)
        {
            FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "File::checkBufferedStatus", "%p    FORCIBLY FILL BACKBUFFER\n", this));
            
            result = flip(true);
            if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
            {
                if (result == FMOD_ERR_FILE_DISKEJECTED)
                {
                    mFlags |= FMOD_FILE_STARVING;
                }
                return result;
            }
            mFlags &= ~FMOD_FILE_STARVING;

            mNextPositionDisplay = mNextPosition;
            mNextPosition += mBlockSize;
        }
        
        return result;
    }

    return FMOD_ERR_INTERNAL;
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
FMOD_RESULT File::read(void *buffer, unsigned int size, unsigned int count, unsigned int *rd)
{
    FMOD_RESULT  result = FMOD_OK;
    unsigned int oldsize = size;
    unsigned int bytesleft;
    unsigned int r = 0;
    bool         eof = false;

    if (!buffer)
    {
        return FMOD_ERR_INVALID_PARAM; 
    }

    if ((int)(size * count) < 0)
    {
        FLOG(((FMOD_DEBUGLEVEL)(FMOD_DEBUG_TYPE_FILE | FMOD_DEBUG_LEVEL_ERROR), __FILE__, __LINE__, "File::read", "Tried to read %d bytes\n", size * count));
        return FMOD_ERR_INVALID_PARAM;
    }

    mFlags &= ~FMOD_FILE_EXIT;

    size *= count;

    if ((mCurrentPosition + size) > mStartOffset + mLength)
    {
        if (mCurrentPosition <= mStartOffset + mLength)
        {
            size = (mStartOffset + mLength) - mCurrentPosition;
        }
        else
        {
            size = 0;
            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "File::read", "(mCurrentPosition + size) > mStartOffset + mLength\n"));
            return FMOD_ERR_FILE_BAD;
        }
        eof = true;
    }

    FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "File::read", "%p----> want to read %d\n", this, size));

    bytesleft = size;

    while (bytesleft)
    {
        size = bytesleft;

        #ifndef PLATFORM_PS3_SPU
        /*
            Direct read - single buffered only.
        */
        if ((mBlockSize == mBufferSize && size > mBlockSize && !mBufferPos && mFlags & FMOD_FILE_SEEKABLE)
            #if defined(PLATFORM_GC) || defined(PLATFORM_WII)
            &&
            ((mDeviceType == DEVICE_DISK || mDeviceType == DEVICE_USER) ? (((FMOD_UINT_NATIVE)((char *)buffer + r) & (FMOD_FILE_ALIGN - 1)) ? false : true) : true)  /* Dest must be 32 byte aligned (if it is a disk file) */
            #endif
        )
        {
            FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "File::read", "%p    mCurrentPosition %d mNextPosition %d nextpos diffbytes %d\n", this, mCurrentPosition, mNextPosition, mNextPosition - mCurrentPosition));
            FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "File::read", "%p    DIRECT READ want %d bytes\n", this, size));

            if (mBlockSize)
            {
                if (mCurrentPosition != mNextPosition)
                {
                    result = seekAndReset();
                    if (result != FMOD_OK)
                    {
                        return result;
                    }
                }

                size /= mBlockSize;
                size *= mBlockSize;
            }

            #ifdef PLATFORM_PS3_SPU
            result = FMOD_DmaFile_ReadCallback(this, (char *)buffer + r, size, &size);
            #else
            result = reallyRead((char *)buffer + r, size, &size);
            #endif

            mFlags &= ~FMOD_FILE_BUSY;

            #ifndef PLATFORM_PS3_SPU
            if (mSystem && mSystem->mReadRiderCallback)
            {
                mSystem->mReadRiderCallback(mRiderHandle, (char *)buffer + r, size, 0, mRiderUserData);
            }
            #endif

            if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
            {
                return result;
            }

            FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "File::read", "%p    DIRECT READ got  %d bytes\n", this, size));

            mNextPositionDisplay = mNextPosition;
            mNextPosition += size;
            
            if (!size)
            {
                result = FMOD_ERR_FILE_EOF;
            }
            
            if (result == FMOD_ERR_FILE_EOF)
            {
                break;
            }
        }

        /*
            Buffered read - single or double buffered.
        */
        else
        #endif
        {
            result = checkBufferedStatus();             

            if (result == FMOD_ERR_FILE_EOF && !(mBlockSize == mBufferSize && (int)mLength == -1))
            {
                result = FMOD_OK;           /* We dont care about the EOF for the flip status, we determine EOF ourselves. */
            }
            if (result != FMOD_OK)
            {
                break;
            }

            if (size > (mBlockSize - (mBufferPos % mBlockSize)))
            {
                size = mBlockSize - (mBufferPos % mBlockSize);
            }

            FMOD_memcpy((char *)buffer + r, (char *)mBuffer + mBufferPos, size);

            FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "File::read", "%p    copied %d bytes from mBufferPos = %d\n", this, size, mBufferPos));

            mBufferPos += size;
            if (mBufferPos >= mBufferSize)
            {
                FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "File::read", "%p    buffer wrap\n", this));           
                mBufferPos = 0;
            }
        }

        mCurrentPosition += size;
        bytesleft -= size;
        r += size;
    }

    FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "File::read", "%p<---- done\n", this));

    if (oldsize == 2)
    {    
        unsigned short *wptr = (unsigned short *)buffer;
        unsigned int i;

        #ifdef PLATFORM_ENDIAN_LITTLE
        if (mFlags & FMOD_FILE_BIGENDIAN)
        #else
        if (!(mFlags & FMOD_FILE_BIGENDIAN))
        #endif
        {
            for (i=0; i < (r / oldsize); i++)
            {
                wptr[i] = FMOD_SWAPENDIAN_WORD(wptr[i]);
            }
        }
    }
    else if (oldsize == 4)
    {    
        unsigned int *dptr = (unsigned int *)buffer;
        unsigned int i;

        #ifdef PLATFORM_ENDIAN_LITTLE
        if (mFlags & FMOD_FILE_BIGENDIAN)
        #else
        if (!(mFlags & FMOD_FILE_BIGENDIAN))
        #endif
        {
            for (i=0; i < (r / oldsize); i++)
            {
                dptr[i] = FMOD_SWAPENDIAN_DWORD(dptr[i]);
            }
        }
    }
    
    r /= oldsize;

    if (mEncryptionKeyLength)
    {
        unsigned char *val = (unsigned char *)buffer;
        unsigned int count;

        if (mFlags & FMOD_FILE_USEOLDDECRYPT)
        {
            for (count = 0; count < r; count++)
            {
                FMOD_DECRYPT_OLD(val[count], mEncryptionKey[mEncryptionKeyIndex]);

                mEncryptionKeyIndex++;
                if (mEncryptionKeyIndex >= mEncryptionKeyLength)
                {
                    mEncryptionKeyIndex = 0;
                }
            }
        }
        else
        {
            for (count = 0; count < r; count++)
            {
                FMOD_DECRYPT(val[count], mEncryptionKey[mEncryptionKeyIndex]);

                mEncryptionKeyIndex++;
                if (mEncryptionKeyIndex >= mEncryptionKeyLength)
                {
                    mEncryptionKeyIndex = 0;
                }
            }
        }
    }

    if (rd)
    {
        *rd = r;
    }

    if (result == FMOD_OK && eof)
    {
        result = FMOD_ERR_FILE_EOF;
    }

    return result;
}

#ifndef PLATFORM_PS3_SPU

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
FMOD_RESULT File::getByte(unsigned char *val)
{
    FMOD_RESULT result;
    unsigned int rd;
    unsigned char temp;

	result = read(&temp, 1, 1, &rd);

    if (val)
    {
        *val = temp;
    }

    return result;
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
FMOD_RESULT File::getByte(unsigned short *val)
{
    FMOD_RESULT result;
    unsigned int rd;
    unsigned char temp;

	result = read(&temp, 1, 1, &rd);

    if (val)
    {
        *val = temp;
    }

    return result;
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
FMOD_RESULT File::getByte(unsigned int *val)
{
    FMOD_RESULT result;
    unsigned int rd;
    unsigned char temp;

	result = read(&temp, 1, 1, &rd);
    
    if (val)
    {
        *val = temp;
    }

    return result;
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
FMOD_RESULT File::getByte(signed char *val)
{
    FMOD_RESULT result;
    unsigned int rd;
    signed char temp;

	result = read(&temp, 1, 1, &rd);

    if (val)
    {
        *val = temp;
    }

    return result;
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
FMOD_RESULT File::getByte(signed short *val)
{
    FMOD_RESULT result;
    unsigned int rd;
    signed char temp;

	result = read(&temp, 1, 1, &rd);

    if (val)
    {
        *val = temp;
    }

    return result;
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
FMOD_RESULT File::getByte(signed int *val)
{
    FMOD_RESULT result;
    unsigned int rd;
    signed char temp;

	result = read(&temp, 1, 1, &rd);
    
    if (val)
    {
        *val = temp;
    }

    return result;
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
FMOD_RESULT File::getWord(unsigned short *val)
{
    FMOD_RESULT result;
    unsigned int rd;
    unsigned short temp;

	result = read(&temp, 2, 1, &rd);

    if (val)
    {
        *val = temp;
    }

    return result;
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
FMOD_RESULT File::getWord(unsigned int *val)
{
    FMOD_RESULT result;
    unsigned int rd;
    unsigned short temp;

	result = read(&temp, 2, 1, &rd);

    if (val)
    {
        *val = temp;
    }

    return result;
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
FMOD_RESULT File::getWord(signed short *val)
{
    FMOD_RESULT result;
    unsigned int rd;
    signed short temp;

	result = read(&temp, 2, 1, &rd);

    if (val)
    {
        *val = temp;
    }

    return result;
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
FMOD_RESULT File::getWord(signed int *val)
{
    FMOD_RESULT result;
    unsigned int rd;
    signed short temp;

	result = read(&temp, 2, 1, &rd);

    if (val)
    {
        *val = temp;
    }

    return result;
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
FMOD_RESULT File::getDword(unsigned int *val)
{    
    FMOD_RESULT result;
    unsigned int rd;
    unsigned int temp;

	result = read(&temp, 4, 1, &rd);

    if (val)
    {
        *val = temp;
    }

    return result;
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
FMOD_RESULT File::getDword(signed int *val)
{    
    FMOD_RESULT result;
    unsigned int rd;
    signed int temp;

	result = read(&temp, 4, 1, &rd);

    if (val)
    {
        *val = temp;
    }

    return result;
}

#endif // !PLATFORM_PS3_SPU

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
FMOD_RESULT File::seek(int pos, int mode)
{
    FMOD_RESULT  result = FMOD_OK;
    unsigned int newpos = 0;
    int          delta;

	if (mode != SEEK_SET && mode != SEEK_CUR && mode != SEEK_END)
    {
		return FMOD_ERR_INVALID_PARAM;
    }

    mFlags &= ~FMOD_FILE_EXIT;

	if (mode == SEEK_SET) 
    {
        newpos = mStartOffset + pos;
    }
	else if (mode == SEEK_CUR) 
    {
        newpos = mCurrentPosition + pos;
    }
	else if (mode == SEEK_END)
    {
        newpos = mStartOffset + mLength + pos;
    }
    
    if (newpos > mStartOffset + mLength)
    {
        if (pos >= 0)
        {
            newpos = mStartOffset + mLength;
        }
        else
        {
            newpos = 0;
        }
    }

    /*
        If no read has been done and not seekable and we are trying to seek beyond the first (future) read size, return an error.
    */
    if (!mNextPosition && !(mFlags & FMOD_FILE_SEEKABLE) && newpos >= mBlockSize)
    {
        return FMOD_ERR_FILE_COULDNOTSEEK;
    }

    /* 
        Single buffered, not seekable, dont allow seeks outside the current buffer 
    */
    if ((mBlockSize == mBufferSize) && !(mFlags & FMOD_FILE_SEEKABLE) && (mNextPosition >= mBlockSize))   
    {
        if (mNextPosition && newpos < mNextPosition - mBlockSize)
        {
            return FMOD_ERR_FILE_COULDNOTSEEK;
        }
        if (newpos >= mNextPosition + mBlockSize)
        {
            return FMOD_ERR_FILE_COULDNOTSEEK;
        }
    }

    delta = newpos - mCurrentPosition;

    mCurrentPosition = newpos;

    if (mEncryptionKeyLength)
    {
        mEncryptionKeyIndex = mCurrentPosition % mEncryptionKeyLength;
    }

    FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "File::seek", "%p seek %d bytes to %d\n",this, delta, newpos));

    if (mBufferSize)
    {
        mBufferPos = mCurrentPosition % mBufferSize;    /* We are seeking resetting to the start of the WHOLE buffer */
    }
    else
    {
        #ifdef PLATFORM_PS3_SPU

        result = FMOD_DmaFile_SeekCallback(this, newpos);

        #else

        result = reallySeek(newpos);

        if (mSystem && mSystem->mSeekRiderCallback)
        {
            mSystem->mSeekRiderCallback(mRiderHandle, newpos, mRiderUserData);
        }

        #endif
    }
    
    return result;
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
FMOD_RESULT File::tell(unsigned int *pos)
{
    FMOD_RESULT result = FMOD_OK;

    if (!pos)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mFlags &= ~FMOD_FILE_EXIT;

    *pos = mCurrentPosition;
    *pos -= mStartOffset;

	return result;
}

#ifndef PLATFORM_PS3_SPU

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
FMOD_RESULT File::setStartOffset(unsigned int offset)
{
    mStartOffset = offset;

    mLength = mLengthOriginal;

    if (mStartOffset + mLength > mFileSize)
    {
        mLength = mFileSize - mStartOffset;
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
FMOD_RESULT File::getStartOffset(unsigned int *offset)
{
    if (!offset)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *offset = mStartOffset;

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
FMOD_RESULT File::enableDoubleBuffer(unsigned int sizebytes, void *oldbuffer)
{
    FMOD_RESULT result;
    unsigned int oldblocksize, oldbuffersize;

    FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "File::enableDoubleBuffer", "%p buffersize = %d bytes\n", this, sizebytes));

    if (!mBlockSize)
    {
        return FMOD_OK;
    }

    #ifndef PLATFORM_PS3_SPU
    result = FMOD_OS_Semaphore_Create(&mSema);
    if (result != FMOD_OK)
    {
        return result;
    }
    FMOD_OS_Semaphore_Signal(mSema);    /* Put it up to 1 */
    #endif

    if (sizebytes < FILE_SECTORSIZE)
    {
        sizebytes = FILE_SECTORSIZE;
    }
    if (sizebytes < mBlockSize)
    {
        sizebytes = mBlockSize;     /* Can't go smaller, we can only go bigger */
    }

    oldblocksize = mBlockSize;
    oldbuffersize = mBufferSize;

    mBlockSize = sizebytes;
    mBlockSize /= oldblocksize;
    mBlockSize *= oldblocksize;

    mBufferSkip = oldblocksize;     /* This is for the next file flip, it will skip what we already have in the older buffer */
    mBlockOffset = 0;
    mNextPosition = mBlockSize;
    mNextPositionDisplay = mNextPosition;

    mBufferSize = mBlockSize;
    mBufferSize *= 2;              /* Make it a double buffer */

    if (oldbuffer)
    {
        mBufferMemory = FMOD_Memory_CallocType(mBufferSize + FMOD_FILE_ALIGN, FMOD_MEMORY_STREAM_FILE);
        if (!mBufferMemory)
        {
            return FMOD_ERR_MEMORY;
        }
        FMOD_memcpy(mBufferMemory, oldbuffer, oldbuffersize);
    }
    else
    {
        mBufferMemory = FMOD_Memory_ReAllocType(mBufferMemory, mBufferSize + FMOD_FILE_ALIGN, FMOD_MEMORY_STREAM_FILE);
        if (!mBufferMemory)
        {
            return FMOD_ERR_MEMORY;
        }
    }

    #if (FMOD_FILE_ALIGN > 0)
    mBuffer = (void *)FMOD_ALIGNPOINTER(mBufferMemory, FMOD_FILE_ALIGN);
    #else
    mBuffer = mBufferMemory;
    #endif


    result = getFileThread();
    if (result != FMOD_OK)
    {
        return result;
    }

    FMOD_OS_CriticalSection_Enter(mFileThread->mFileListCrit);
    addAfter(&mFileThread->mFileListHead);
    FMOD_OS_CriticalSection_Leave(mFileThread->mFileListCrit);

    #if 0
    {
        FILE *fp;
        unsigned int offset = 0;

        printf("-------------------------------------------------------\n");
   
        fp = fopen(mName, "rb");

        srand(1);

        do
        {
            unsigned char buff1[4096], buff2[4096];
            unsigned int size;
            
            offset = 0; //FMOD_RAND() % mLength;
            size = 1; //FMOD_RAND() % 4096;

            fseek(fp, offset, SEEK_SET);
            fread(buff1, size, 1, fp);

            seek(offset, SEEK_SET);
            read(buff2, size, 1, 0);
            
            if (memcmp(buff1, buff2, size))
            {
                size = size;
            }

            offset++;
        } while (offset < mStartOffset + mLength);       
    }
    #endif

    result = checkBufferedStatus();
    if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
    {
        return result;
    }

    FLOG((FMOD_DEBUG_TYPE_FILE, __FILE__, __LINE__, "File::enableDoubleBuffer", "%p done\n", this));

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
FMOD_RESULT File::getName(char **name)
{
    if (name)
    {
        *name = mName;
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
FMOD_RESULT File::setName(char *name)
{
    if (!name)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    FMOD_strncpy(mName, name, FMOD_STRING_MAXNAMELEN);
    mName[FMOD_STRING_MAXNAMELEN - 1] = 0; //null terminator

    return FMOD_OK;
}

#endif // !PLATFORM_PS3_SPU



#ifdef FMOD_SUPPORT_MEMORYTRACKER
FMOD_RESULT File::getMemoryUsedImpl(MemoryTracker *tracker)
{
    #ifndef PLATFORM_PS3_SPU
    if (mSema)
    {
        tracker->add(false, FMOD_MEMBITS_FILE, gSizeofSemaphore);
    }

    tracker->add(false, FMOD_MEMBITS_FILE, mBufferSize + FMOD_FILE_ALIGN);    /* mSyncPointHead & mSyncPointTail */
    #endif

    return FMOD_OK;
}
#endif


/*
[API]
[
	[DESCRIPTION]
	Callback for opening a file.  

	[PARAMETERS]
	'name'	    This is the filename passed in by the user.  You may treat this as you like.
    'unicode'   Tells the callback if the string being passed in is a double byte unicode string or not.  You may have to support this unless you know the target application will not support unicode.
    'filesize'  The size of the file to be passed back to fmod, in bytes.
    'handle'    This is to store a handle generated by the user.  This will be the handle that gets passed into the other callbacks.  Optional but may be needed.
    'userdata'  This is to store userdata to be passed into the other callbacks.  Optional.
 
	[RETURN_VALUE]

	[REMARKS]
    Return the appropriate error code such as FMOD_ERR_FILE_NOTFOUND if the file fails to open.
    If the callback is from System::attachFileSystem, then the return value is ignored.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox360, PlayStation 2, PlayStation Portable, PlayStation 3, Wii, Solaris, iPhone

	[SEE_ALSO]
    System::setFileSystem
    System::attachFileSystem
    FMOD_FILE_CLOSECALLBACK
    FMOD_FILE_READCALLBACK
    FMOD_FILE_SEEKCALLBACK
]
*/
/*
FMOD_RESULT F_CALLBACK FMOD_FILE_OPENCALLBACK(const char *name, int unicode, unsigned int *filesize, void **handle, void **userdata);
{
#if 0
	 // here purely for documentation purposes.
#endif
}
*/

/*
[API]
[
	[DESCRIPTION]
    Calback for closing a file.

	[PARAMETERS]
	'handle'    This is the handle returned from the open callback to use for your own file routines.
    'userdata'  Userdata initialized in the FMOD_FILE_OPENCALLBACK.

	[RETURN_VALUE]

	[REMARKS]
	Close any user created file handle and perform any cleanup nescessary for the file here.
    If the callback is from System::attachFileSystem, then the return value is ignored.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox360, PlayStation 2, PlayStation Portable, PlayStation 3, Wii, Solaris, iPhone

	[SEE_ALSO]
    System::setFileSystem
    System::attachFileSystem
    FMOD_FILE_OPENCALLBACK
    FMOD_FILE_READCALLBACK
    FMOD_FILE_SEEKCALLBACK
]
*/
/*
FMOD_RESULT F_CALLBACK FMOD_FILE_CLOSECALLBACK(void *handle, void *userdata);
{
#if 0
	 // here purely for documentation purposes.
#endif
}
*/

/*
[API]
[
	[DESCRIPTION]
    Callback for reading from a file.

	[PARAMETERS]
	'handle'    This is the handle you returned from the open callback to use for your own file routines.
	'buffer'	The buffer to read your data into.
	'sizebytes' The number of bytes to read.
    'bytesread' The number of bytes successfully read.
    'userdata'  Userdata initialized in the FMOD_FILE_OPENCALLBACK.

	[RETURN_VALUE]

	[REMARKS]
    If the callback is from System::attachFileSystem, then the return value is ignored.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox360, PlayStation 2, PlayStation Portable, PlayStation 3, Wii, Solaris, iPhone

	[SEE_ALSO]
    System::setFileSystem
    System::attachFileSystem
    FMOD_FILE_OPENCALLBACK
    FMOD_FILE_CLOSECALLBACK
    FMOD_FILE_SEEKCALLBACK
]
*/
/*
FMOD_RESULT F_CALLBACK FMOD_FILE_READCALLBACK(void *handle, void *buffer, unsigned int sizebytes, unsigned int *bytesread, void *userdata);
{
#if 0
	 // here purely for documentation purposes.
#endif
}
*/

/*
[API]
[
	[DESCRIPTION]
    Callback for seeking within a file.

	[PARAMETERS]
	'handle'    This is the handle returned from the open callback to use for your own file routines.
	'pos'       This is the position or offset to seek to in the file in bytes.
    'userdata'  Data initialized in the FMOD_FILE_OPENCALLBACK.

	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox360, PlayStation 2, PlayStation Portable, PlayStation 3, Wii, Solaris, iPhone

	[SEE_ALSO]
    System::setFileSystem
    FMOD_FILE_OPENCALLBACK
    FMOD_FILE_CLOSECALLBACK
    FMOD_FILE_READCALLBACK
]
*/
/*
FMOD_RESULT F_CALLBACK FMOD_FILE_SEEKCALLBACK(void *handle, unsigned int pos, void *userdata);
{
#if 0
	 // here purely for documentation purposes.
#endif
}
*/

}
