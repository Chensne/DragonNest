#include "fmod_settings.h"

#include "fmod.h"
#include "fmod_debug.h"
#include "fmod_memory.h"
#include "fmod_os_misc.h"
#include "fmod_string.h"
#include "fmod_types.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef DEBUG
    #include <assert.h>
#endif

//#define FMOD_MEMORY_ALLOC_FAIL_RANDOM 30
//#define FMOD_MEMORY_ALLOC_FAIL_AFTER  822
#ifdef FMOD_MEMORY_ALLOC_FAIL_AFTER
   static int FMOD_Memory_FailCount = 0;
#endif

#ifdef FMOD_MEMORY_ALLOC_FAIL_RANDOM
    #include <time.h>
#endif

#define FMOD_MEMORY_BYTESTOBLOCKS(_pool, _bytes) (((_bytes) + (mBlockSize - 1)) / mBlockSize)


extern "C"
{
    void *FMOD_Memory_allocC(int len, const char *file, const int line)
    {
        return FMOD::gGlobal->gSystemPool->alloc(len, file, line);
    }
    void *FMOD_Memory_callocC(int len, const char *file, const int line)
    {
        return FMOD::gGlobal->gSystemPool->calloc(len, file, line);
    }

    void *FMOD_Memory_reallocC(void *ptr, int len, const char *file, const int line)
    {
        return FMOD::gGlobal->gSystemPool->realloc(ptr, len, file, line);
    }

    void FMOD_Memory_freeC(void *ptr, const char *file, const int line)
    {
        FMOD::gGlobal->gSystemPool->free(ptr, file, line);
    }
}


namespace FMOD
{


void *MemSingleton::alloc(int len, const char *file, const int line)
{
    if (!mRefCount)
    {
        mBuffer = FMOD::gGlobal->gSystemPool->alloc(len, file, line);
        FLOG((FMOD_DEBUG_TYPE_MEMORY, __FILE__, __LINE__, "MemSingleton::alloc", "Allocated a singleton memory buffer %d bytes\n", len));
    }
    mRefCount++;

    return mBuffer;
}

void MemSingleton::free(const char *file, const int line)
{
    if (mRefCount)
    {
        mRefCount--;
    }
    if (!mRefCount)
    {
        if (mBuffer)
        {
            FMOD::gGlobal->gSystemPool->free(mBuffer, file, line);
            mBuffer = 0;
            FLOG((FMOD_DEBUG_TYPE_MEMORY, __FILE__, __LINE__, "MemSingleton::free", "Freed singleton memory buffer\n"));
        }
    }
}


/*
    The wrapper is needed to handle stdcall callbacks when malloc/free is normally cdecl
*/
void * F_CALLBACK Memory_DefaultMalloc(unsigned int size, FMOD_MEMORY_TYPE type)
{
    return FMOD_OS_Memory_Alloc(size, type);
}
void * F_CALLBACK Memory_DefaultRealloc(void *data, unsigned int size, FMOD_MEMORY_TYPE type)
{
    return FMOD_OS_Memory_Realloc(data, size, type);
}
void F_CALLBACK   Memory_DefaultFree(void *ptr, FMOD_MEMORY_TYPE type)
{
    FMOD_OS_Memory_Free(ptr, type);
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]

 	[RETURN_VALUE]
	void

	[REMARKS]
]
*/
MemPool::MemPool()
{
    int count;

    mAlloc      = Memory_DefaultMalloc;
    mRealloc    = Memory_DefaultRealloc;
    mFree       = Memory_DefaultFree;
#ifdef FMOD_MEMORY_THREADSAFE
    mCrit       = 0;
#endif
    for (count = 0; count < FMOD_MEMORY_MAXTHREADS; count++)
    {
        mCurrentAllocated[count] = 0;
        mCurrentAllocatedThreadID[count] = 0;
    }

    mCurrentAllocatedSecondary = 0;
    mMaxAllocatedSecondary     = 0;

#ifdef FMOD_SUPPORT_DLMALLOC
    mDLMallocSpace = 0;
#endif
#ifdef FMOD_DEBUG
    mAllocCount = 0;
#endif
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]

 	[RETURN_VALUE]
	void

	[REMARKS]
]
*/
MemPool::~MemPool()
{
    close();
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]

 	[RETURN_VALUE]
	void

	[REMARKS]
]
*/
FMOD_RESULT MemPool::init(void *poolmem, int poolsize, int blocksize)
{
    FMOD_RESULT result;
    void *oldpoolmem = poolmem;
    int   count;

    if (!poolmem || !poolsize)
    {
        return FMOD_ERR_MEMORY;
    }

    close();

    #define ALIGNMENT   256

    /*
        Align the pool memory and reduce the pool size by the number of bytes we had to align by.
    */
    poolmem = (unsigned char *)FMOD_ALIGNPOINTER(poolmem, ALIGNMENT);
    poolsize -= (int)((FMOD_SINT_NATIVE)poolmem - (FMOD_SINT_NATIVE)oldpoolmem);
    poolsize &= ~((unsigned int)blocksize-1);

#ifdef FMOD_SUPPORT_DLMALLOC
    mDLMallocSpace = create_mspace_with_base(poolmem, poolsize, 0);
    if (!mDLMallocSpace)
    {
        return FMOD_ERR_MEMORY;
    }
#else
    int bitmapsize;

    mBlockSize  = blocksize;
    #ifdef FMOD_MEMORY_USEBITS
    bitmapsize       = FMOD_MEMORY_BYTESTOBLOCKS(pool, (poolsize + 7) / 8);
    #else
    bitmapsize       = FMOD_MEMORY_BYTESTOBLOCKS(pool, poolsize);
    #endif
    bitmapsize      +=  (blocksize-1);
    bitmapsize      &= ~(blocksize-1);

    mSizeBlocks = FMOD_MEMORY_BYTESTOBLOCKS(pool, poolsize - bitmapsize);
    mSizeBytes  = mSizeBlocks * mBlockSize;
    mBitmap     = (unsigned char *)poolmem;
    mData       = mBitmap + bitmapsize;

    set(0, 0, mSizeBlocks);

    FMOD_memset(mData, 0, mSizeBytes);
#endif

    mNumBlocks                 = 0;
    mMaxBlocks                 = 0;
    mMaxAllocated              = 0;
    mMaxAllocatedSecondary     = 0;
    mActualMaxBytes            = 0;
    mWastage                   = 0;
    mFirstFreeBlock            = 0;
    mCurrentAllocatedSecondary = 0;
    for (count = 0; count < FMOD_MEMORY_MAXTHREADS; count++)
    {
        mCurrentAllocated[count] = 0;
        mCurrentAllocatedThreadID[count] = 0;
    }

#ifdef FMOD_MEMORY_THREADSAFE
    result = FMOD_OS_CriticalSection_Create(&mCrit, true);
    if (result != FMOD_OK)
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
	void

	[REMARKS]
]
*/
FMOD_RESULT MemPool::initCustom(void *poolmem, int poolsize, int blocksize)
{
    FMOD_RESULT result;
    int bitmapsize, count;

    if (!poolsize)
    {
        return FMOD_ERR_MEMORY;
    }

    close();

    mBlockSize  = blocksize;
    mSizeBlocks = FMOD_MEMORY_BYTESTOBLOCKS(pool, poolsize);
    mSizeBlocks = mSizeBlocks & 0xFFFFFFFC;
    mSizeBytes  = mSizeBlocks * mBlockSize;

#ifdef FMOD_MEMORY_USEBITS
    bitmapsize = (mSizeBlocks + 7) / 8;
#else
    bitmapsize = mSizeBlocks;
#endif

    mBitmap = (unsigned char *)FMOD_Memory_Alloc(bitmapsize);
    if (!mBitmap)
    {
        return FMOD_ERR_MEMORY;
    }
    mData = (unsigned char *)poolmem;

    set(0, 0, mSizeBlocks);

    mNumBlocks           = 0;
    mMaxBlocks           = 0;
    mMaxAllocated        = 0;
    mActualMaxBytes      = 0;
    mWastage             = 0;
    mCustomPool          = true;
    mFirstFreeBlock      = 0;

    for (count = 0; count < FMOD_MEMORY_MAXTHREADS; count++)
    {
        mCurrentAllocated[count] = 0;
        mCurrentAllocatedThreadID[count] = 0;
    }

    mAlloc            = 0;
    mRealloc          = 0;
    mFree             = 0;
    
#ifdef FMOD_MEMORY_THREADSAFE
    result = FMOD_OS_CriticalSection_Create(&mCrit);
    if (result != FMOD_OK)
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
	void

	[REMARKS]
]
*/
void MemPool::close()
{
    int count;

    if (mCustomPool && mBitmap)
    {
        FMOD_Memory_Free(mBitmap);
    }

    mBitmap       = 0;
    mData         = 0;
    mSizeBytes    = 0;
    mSizeBlocks   = 0;
    mNumBlocks    = 0;
    mMaxBlocks    = 0;
    mMaxAllocated = 0;
    
    for (count = 0; count < FMOD_MEMORY_MAXTHREADS; count++)
    {
        mCurrentAllocated[count] = 0;
        mCurrentAllocatedThreadID[count] = 0;
    }

    mAlloc      = Memory_DefaultMalloc;
    mRealloc    = Memory_DefaultRealloc;
    mFree       = Memory_DefaultFree;
    mCustomPool = false;

#ifdef FMOD_MEMORY_THREADSAFE
    if (mCrit)
    {
        FMOD_OS_CriticalSection_Free(mCrit, true);
        mCrit = 0;
    }
#endif
}



/*
[
	[DESCRIPTION]

	[PARAMETERS]

 	[RETURN_VALUE]

	[REMARKS]
]
*/
void MemPool::set(int blockoffset, int value, int numblocks)
{
    #ifdef FMOD_MEMORY_USEBITS
    int byteoffset;
    int bitoffset;
    int count;
    int blocksleft;

    blocksleft = numblocks;    
    byteoffset = blockoffset / 8;
    bitoffset  = blockoffset & 7;
    
    /*
        First align the blocks to a 32 block boundary, so we can then do a dword at a time.
    */
    count = 0;
    if (blockoffset & 31)
    {
        count = 32 - (blockoffset & 31);
        if (count > numblocks)
        {
            count = numblocks;
        }
    }

    while (count)
    {    
        if (value)
        {
            mBitmap[byteoffset] |= (1<<bitoffset);
        }
        else
        {
            mBitmap[byteoffset] &= ~(1<<bitoffset);
        }

        bitoffset++;
        if (bitoffset >= 8)
        {
            bitoffset = 0;
            byteoffset++;
        }
        count--;
        blocksleft--;
    }

    count = blocksleft / 8;
    if (count)
    {
        FMOD_memset(&mBitmap[byteoffset], value ? 0xFF : 0, count);
        
        byteoffset += count;
        blocksleft -= count * 8;
    }

    count = blocksleft & 31;
    while (count)
    {    
        if (value)
        {
            mBitmap[byteoffset] |= (1<<bitoffset);
        }
        else
        {
            mBitmap[byteoffset] &= ~(1<<bitoffset);
        }

        bitoffset++;
        if (bitoffset >= 8)
        {
            bitoffset = 0;
            byteoffset++;
        }
        count--;
    }
    #else
    FMOD_memset(&mBitmap[blockoffset], value, numblocks);
    #endif

    if (value)
    {
        if (blockoffset == mFirstFreeBlock)     /* Only move firstfree forward if it is the firstfree block that is being used. */
        {
            mFirstFreeBlock = blockoffset + numblocks;
        }

        /* 
            If mFirstFreeBlock is now pointing to a used block because we just filled in a hole.  Scan it forward. 
        */
        #ifdef FMOD_MEMORY_USEBITS
        byteoffset = mFirstFreeBlock / 8;
        bitoffset  = mFirstFreeBlock & 7;
        if (mBitmap[byteoffset] & (1 << bitoffset))
        #else
        if (mBitmap[mFirstFreeBlock])
        #endif
        {
            mFirstFreeBlock = findFreeBlocks(mFirstFreeBlock, mSizeBlocks, 1);
        }
    }
    else
    {
        if (blockoffset < mFirstFreeBlock)
        {
            mFirstFreeBlock = blockoffset;
        }
    }
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]

 	[RETURN_VALUE]

	[REMARKS]
]
*/
int MemPool::findFreeBlocks(int offset, int to, int numblocks)
{
    int found = 0;
    #ifdef FMOD_MEMORY_USEBITS
    int one_ls_bitoffset, byteoffset;
    
    byteoffset       = offset >> 3;
    one_ls_bitoffset = (1 << (offset & 7));

    while (found < numblocks && offset < to && offset < mSizeBlocks)
    {
        if (mBitmap[byteoffset] & one_ls_bitoffset || (!(offset & 31) && *((unsigned int *)&mBitmap[byteoffset]) == 0xFFFFFFFF))
        {
            found = 0;
        }
        else
        {
            found++;
        }

        if (!(offset & 31) && *((unsigned int *)&mBitmap[byteoffset]) == 0xFFFFFFFF)
        {
            byteoffset += 4;
            offset     += 32;
        }
        else
        {
            offset++;

            one_ls_bitoffset *= 2;
            if (!(offset & 7))
            {
                one_ls_bitoffset = 1;
                byteoffset++;
            }
        }
    }

    #else

    while (found < numblocks && offset < to && offset < mSizeBlocks)
    {
        if (mBitmap[offset])
        {
            found = 0;
        }
        else
        {
            found++;
        }

        offset++;
    }

    #endif

    if (found == numblocks)
    {
        offset -= numblocks;
    }
    else
    {
        offset = -1;
    }

    return offset;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]

 	[RETURN_VALUE]

	[REMARKS]
]
*/
void *MemPool::alloc(int len, const char *file, const int line, FMOD_MEMORY_TYPE type, bool clear)
{
#ifdef FMOD_DEBUG
    ++mAllocCount;
#endif
	MemBlockHeader *block = 0;
    int numblocks = 0, reallen = len;

    {          
        #ifdef FMOD_MEMORY_ALLOC_FAIL_RANDOM
        srand(clock());
        if (
        #ifdef FMOD_MEMORY_ALLOC_FAIL_AFTER
             FMOD_Memory_FailCount >= FMOD_MEMORY_ALLOC_FAIL_AFTER && 
        #endif
             !(FMOD_RAND() % FMOD_MEMORY_ALLOC_FAIL_RANDOM))
        {
            FLOG(((FMOD_DEBUGLEVEL)(FMOD_DEBUG_TYPE_MEMORY | FMOD_DEBUG_LEVEL_ERROR), __FILE__, __LINE__, "MemPool::alloc", "Failed allocating %-45s line %5d.  Wanted %d bytes, current %d/%d (rounding wastage = %d bytes)\n", file, line, len, mCurrentAllocated[0], mSizeBytes, mWastage));

            if (FMOD::gGlobal->gSystemCallback)
            {
                char fileLine[256];
                sprintf(fileLine, "%s (%d)", file, line);

                FMOD::gGlobal->gSystemCallback(NULL, FMOD_SYSTEM_CALLBACKTYPE_MEMORYALLOCATIONFAILED, (void*)fileLine, (void*)len);
            }

            return 0;
        }
        #elif defined(FMOD_MEMORY_ALLOC_FAIL_AFTER) 
        if (FMOD_Memory_FailCount >= FMOD_MEMORY_ALLOC_FAIL_AFTER)
        {
            FLOG(((FMOD_DEBUGLEVEL)(FMOD_DEBUG_TYPE_MEMORY | FMOD_DEBUG_LEVEL_ERROR), __FILE__, __LINE__, "MemPool::alloc", "Failed allocating %-45s line %5d.  Wanted %d bytes, current %d/%d (rounding wastage = %d bytes)\n", file, line, len, mCurrentAllocated[0], mSizeBytes, mWastage));

            if (FMOD::gGlobal->gSystemCallback)
            {
                char fileLine[256];
                sprintf(fileLine, "%s (%d)", file, line);

                FMOD::gGlobal->gSystemCallback(NULL, FMOD_SYSTEM_CALLBACKTYPE_MEMORYALLOCATIONFAILED, (void*)fileLine, (void*)len);
            }

            FMOD_Memory_FailCount = 0;
            return 0;
        }
        #endif
        #ifdef FMOD_MEMORY_ALLOC_FAIL_AFTER
        FMOD_Memory_FailCount++;
        #endif
    }
    
#ifdef FMOD_MEMORY_THREADSAFE
    if (!mCrit)
    {
        FMOD_RESULT result = FMOD_OS_CriticalSection_Create(&mCrit, true);
        if (result != FMOD_OK)
        {
            FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "MemPool::alloc", "Error creating critical section!\n"));

            return 0;
        }
    }
    FMOD_OS_CriticalSection_Enter(mCrit);
#endif

    if (!mCustomPool)
    {
        reallen += sizeof(MemBlockHeader);
    }
   	
    if (mAlloc)
    {
        /*
            Only want the flags that have been specified.
        */
        type &= FMOD::gGlobal->gMemoryTypeFlags;

        block = (MemBlockHeader *)mAlloc(reallen, type);
    }
    else
    {
#ifdef FMOD_SUPPORT_DLMALLOC
        if (!mCustomPool)
        {
            block = (MemBlockHeader *)FMOD::mspace_malloc(mDLMallocSpace, reallen);
        }
        else
#endif
        {
            numblocks = FMOD_MEMORY_BYTESTOBLOCKS(pool, reallen);
        
            /* 
                Simple search for a free block in our memory pool.  First fit.
            */    
            {
                int offset;
                
                offset = findFreeBlocks(mFirstFreeBlock, mSizeBlocks, numblocks);
                if (offset >= 0)
                {
                    set(offset, 1, numblocks);

                    if (mCustomPool)
                    {
                        block = (MemBlockHeader *)FMOD_Memory_Alloc(sizeof(MemBlockHeader));
                    }
                    else
                    {
                        block = (MemBlockHeader *)(mData + (offset * mBlockSize));
                    }
                    block->mBlockOffset = offset;
                }
            }
        }
    }
    
	if (!block)
    {
        FLOG(((FMOD_DEBUGLEVEL)(FMOD_DEBUG_TYPE_MEMORY | FMOD_DEBUG_LEVEL_ERROR), __FILE__, __LINE__, "MemPool::alloc", "Failed allocating %-45s line %5d.  Wanted %d bytes, current %d/%d (rounding wastage = %d bytes)\n", file, line, len, mCurrentAllocated[0], mSizeBytes, mWastage));
        
#ifdef FMOD_MEMORY_THREADSAFE
        FMOD_OS_CriticalSection_Leave(mCrit);
#endif
        
        if (FMOD::gGlobal->gSystemCallback)
        {
            char fileLine[256];
            sprintf(fileLine, "%s (%d)", file, line);

            FMOD::gGlobal->gSystemCallback(NULL, FMOD_SYSTEM_CALLBACKTYPE_MEMORYALLOCATIONFAILED, (void*)fileLine, (void*)len);
        }
        
        return 0;
    }
			
	block->mSize      = len;
    block->mNumBlocks = numblocks;  
    block->mThreadID  = getCurrentThreadID();

    if (type & FMOD_MEMORY_SECONDARY)
    {
        mCurrentAllocatedSecondary += block->mSize;

        if (mCurrentAllocatedSecondary > mMaxAllocatedSecondary)
        {
            mMaxAllocatedSecondary = mCurrentAllocatedSecondary;
        }
    }
    else
    {
	    mCurrentAllocated[0]                += block->mSize; /* All threads. */
	    mCurrentAllocated[block->mThreadID] += block->mSize;

	    if (mCurrentAllocated[0] > mMaxAllocated)
        {
		    mMaxAllocated = mCurrentAllocated[0];
        }
    }

	mNumBlocks += block->mNumBlocks;
	if (mNumBlocks > mMaxBlocks)
    {
		mMaxBlocks      = mNumBlocks;
        mActualMaxBytes = mMaxBlocks      * mBlockSize;
        mWastage        = mActualMaxBytes - mMaxAllocated;
    }

    if (!mCustomPool)
    {
        block++;    /* return pointer to actual data, not our memory block header */

        if (clear && block)
        {
            FMOD_memset(block, 0, len);
        }
    }

#ifdef FMOD_MEMORY_THREADSAFE
    FMOD_OS_CriticalSection_Leave(mCrit);
#endif

    FLOG((FMOD_DEBUG_TYPE_MEMORY, file, line, "MemPool::alloc", "%6d bytes (%p) (alloc %d)\n", len, block, mAllocCount));

    return block;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]

 	[RETURN_VALUE]

	[REMARKS]
]
*/
void MemPool::free(void *ptr, const char *file, const int line, FMOD_MEMORY_TYPE type)
{
	MemBlockHeader *block = (MemBlockHeader *)ptr;

#ifdef FMOD_MEMORY_THREADSAFE
    if (!mCrit)
    {
        FMOD_RESULT result = FMOD_OS_CriticalSection_Create(&mCrit, true);
        if (result != FMOD_OK)
        {
            FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "MemPool::free", "Error creating critical section!\n"));

            return;
        }
    }
    FMOD_OS_CriticalSection_Enter(mCrit);
#endif

    if (!mCustomPool)
    {
        block--;     /* Find the actual block header */
    }

    FLOG((FMOD_DEBUG_TYPE_MEMORY, file, line, "MemPool::free", "%6d bytes (%p)\n", block->mSize, ptr));

    #ifdef DEBUG
    assert(block->mThreadID);   /* If this is 0, it must have already been freed? */
    #endif

    if (type & FMOD_MEMORY_SECONDARY)
    {
        mCurrentAllocatedSecondary -= block->mSize;
    }
    else
    {
        mCurrentAllocated[0]                -= block->mSize;
        mCurrentAllocated[block->mThreadID] -= block->mSize;
    }

    mNumBlocks -= block->mNumBlocks;    
    block->mThreadID = 0;

    if (mFree)
    {
        /*
            Only want the flags that have been specified.
        */
        type &= FMOD::gGlobal->gMemoryTypeFlags;

        mFree(block, type);
    }
    else
    {
#ifdef FMOD_SUPPORT_DLMALLOC
        if (!mCustomPool)
        {
            mspace_free(mDLMallocSpace, block);
        }
        else
#endif
        {
            set(block->mBlockOffset, 0, block->mNumBlocks);
        }
    }

#ifdef FMOD_MEMORY_THREADSAFE
    FMOD_OS_CriticalSection_Leave(mCrit);
#endif

    if (mCustomPool)
    {
        FMOD_Memory_Free(block);
    }
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]

 	[RETURN_VALUE]

	[REMARKS]
]
*/
void *MemPool::calloc(int len, const char *file, const int line, FMOD_MEMORY_TYPE type)
{
	void *ptr;
	
    ptr = alloc(len, file, line, type, true);

	return ptr;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]

 	[RETURN_VALUE]

	[REMARKS]
]
*/
void *MemPool::realloc(void *ptr, int len, const char *file, const int line, FMOD_MEMORY_TYPE type)
{
	MemBlockHeader *block = (MemBlockHeader *)ptr;
    int reallen = len, numblocks = 0;
    
    if (!ptr)
    {
        return MemPool::alloc(len, file, line);
    }

    {          
        #ifdef FMOD_MEMORY_ALLOC_FAIL_RANDOM
        srand(clock());
        if (
        #ifdef FMOD_MEMORY_ALLOC_FAIL_AFTER
             FMOD_Memory_FailCount >= FMOD_MEMORY_ALLOC_FAIL_AFTER && 
        #endif
             !(FMOD_RAND() % FMOD_MEMORY_ALLOC_FAIL_RANDOM))
        {
            FLOG(((FMOD_DEBUGLEVEL)(FMOD_DEBUG_TYPE_MEMORY | FMOD_DEBUG_LEVEL_ERROR), __FILE__, __LINE__, "MemPool::realloc", "Failed allocating %-45s line %5d.  Wanted %d bytes, current %d/%d (rounding wastage = %d bytes)\n", file, line, len, mCurrentAllocated[0], mSizeBytes, mWastage));

            if (FMOD::gGlobal->gSystemCallback)
            {
                char fileLine[256];
                sprintf(fileLine, "%s (%d)", file, line);

                FMOD::gGlobal->gSystemCallback(NULL, FMOD_SYSTEM_CALLBACKTYPE_MEMORYALLOCATIONFAILED, (void*)fileLine, (void*)len);
            }

            return 0;
        }
        #elif defined(FMOD_MEMORY_ALLOC_FAIL_AFTER) 
        if (FMOD_Memory_FailCount >= FMOD_MEMORY_ALLOC_FAIL_AFTER)
        {
            FLOG(((FMOD_DEBUGLEVEL)(FMOD_DEBUG_TYPE_MEMORY | FMOD_DEBUG_LEVEL_ERROR), __FILE__, __LINE__, "MemPool::realloc", "Failed allocating %-45s line %5d.  Wanted %d bytes, current %d/%d (rounding wastage = %d bytes)\n", file, line, len, mCurrentAllocated[0], mSizeBytes, mWastage));

            if (FMOD::gGlobal->gSystemCallback)
            {
                char fileLine[256];
                sprintf(fileLine, "%s (%d)", file, line);

                FMOD::gGlobal->gSystemCallback(NULL, FMOD_SYSTEM_CALLBACKTYPE_MEMORYALLOCATIONFAILED, (void*)fileLine, (void*)len);
            }

            FMOD_Memory_FailCount = 0;
            return 0;
        }
        #endif
        #ifdef FMOD_MEMORY_ALLOC_FAIL_AFTER
        FMOD_Memory_FailCount++;
        #endif
    }

#ifdef FMOD_MEMORY_THREADSAFE
    if (!mCrit)
    {
        FMOD_RESULT result = FMOD_OS_CriticalSection_Create(&mCrit, true);
        if (result != FMOD_OK)
        {
            FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "MemPool::realloc", "Error creating critical section!\n"));

            return 0;
        }
    }
    FMOD_OS_CriticalSection_Enter(mCrit);
#endif

    if (!mCustomPool)
    {
        reallen += sizeof(MemBlockHeader);
    	block--;
    }

    FLOG((FMOD_DEBUG_TYPE_MEMORY, file, line, "MemPool::realloc", "from %6d to %6d bytes\n", block->mSize, len));

    mCurrentAllocated[0]                -= block->mSize;    /* All threads. */
    mCurrentAllocated[block->mThreadID] -= block->mSize;
    mNumBlocks                          -= block->mNumBlocks;

    if (mRealloc)
    {
        /*
            Only want the flags that have been specified.
        */
        type &= FMOD::gGlobal->gMemoryTypeFlags;

    	block = (MemBlockHeader *)mRealloc(block, reallen, type);
    }
    else
    {
#ifdef FMOD_SUPPORT_DLMALLOC
        if (!mCustomPool)
        {
            block = (MemBlockHeader *)mspace_realloc(mDLMallocSpace, block, reallen);
        }
        else
#endif
        {
            int offset;

            numblocks = FMOD_MEMORY_BYTESTOBLOCKS(pool, reallen);

            set(block->mBlockOffset, 0, block->mNumBlocks);

            offset = findFreeBlocks(block->mBlockOffset, block->mBlockOffset + numblocks, numblocks);

            if (offset >= 0)
            {
                /*
                    We are able to simply extend the block
                */
                set(offset, 1, numblocks);
                block = (MemBlockHeader *)(mData + (offset * mBlockSize));
                block->mBlockOffset = offset;
            }
            else
            {
                /*
                    We have to find a new place and move the data
                */
                offset = findFreeBlocks(mFirstFreeBlock, mSizeBlocks, numblocks);
                if (offset >= 0)
                {
            	    MemBlockHeader *newblock;

                    set(offset, 1, numblocks);

                    if (mCustomPool)
                    {
                        newblock = block;
                    }
                    else
                    {
                        newblock = (MemBlockHeader *)(mData + (offset * mBlockSize));
                    }

                    newblock->mBlockOffset = offset;
                    
                    if (!mCustomPool)
                    {
                        FMOD_memmove(newblock + 1, block + 1, block->mSize);
                    }

                    block = newblock;

                }
                else
                {
                    block = 0;
                }
            }
        }
    }

	if (!block)
    {
        FLOG(((FMOD_DEBUGLEVEL)(FMOD_DEBUG_TYPE_MEMORY | FMOD_DEBUG_LEVEL_ERROR), __FILE__, __LINE__, "MemPool::realloc", "Failed allocating %-45s line %5d.  Wanted %d bytes, current %d/%d (rounding wastage = %d bytes)\n", file, line, len, mCurrentAllocated[0], mSizeBytes, mWastage));

#ifdef FMOD_MEMORY_THREADSAFE
        FMOD_OS_CriticalSection_Leave(mCrit);
#endif

        if (FMOD::gGlobal->gSystemCallback)
        {
            char fileLine[256];
            sprintf(fileLine, "%s (%d)", file, line);

            FMOD::gGlobal->gSystemCallback(NULL, FMOD_SYSTEM_CALLBACKTYPE_MEMORYALLOCATIONFAILED, (void*)fileLine, (void*)len);
        }

		return 0;
    }

	block->mSize      = len;
    block->mNumBlocks = numblocks;
    block->mThreadID  = getCurrentThreadID();

    mCurrentAllocated[0]                += block->mSize; /* All threads. */
    mCurrentAllocated[block->mThreadID] += block->mSize;

	if (mCurrentAllocated[0] > mMaxAllocated)
    {
		mMaxAllocated = mCurrentAllocated[0];
    }

	mNumBlocks += block->mNumBlocks;
	if (mNumBlocks > mMaxBlocks)
    {
		mMaxBlocks      = mNumBlocks;
        mActualMaxBytes = mMaxBlocks      * mBlockSize;
        mWastage        = mActualMaxBytes - mMaxAllocated;
    }

    if (!mCustomPool)
    {
        block++;    /* return pointer to actual data, not our memory block header */
    }

#ifdef FMOD_MEMORY_THREADSAFE
    FMOD_OS_CriticalSection_Leave(mCrit);
#endif

	return block;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]

 	[RETURN_VALUE]

	[REMARKS]
]
*/
int MemPool::getSize(void *ptr, const char *file, const int line)
{
	MemBlockHeader *block = (MemBlockHeader *)ptr;

    if (!mCustomPool)
    {
        block--;     /* Find the actual block header */
    }
    
    return block->mSize;
}

}

/*
[API]
[
	[DESCRIPTION]
    Callback to allocate a block of memory.

	[PARAMETERS]
    'size'          Size in bytes of the memory block to be allocated and returned.
    'type'          Type of memory allocation.

	[RETURN_VALUE]
    On success, a pointer to the newly allocated block of memory is returned.
    On failure, NULL is returned.

	[REMARKS]
    Returning an aligned pointer, of 16 byte alignment is recommended for speed purposes.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox360, PlayStation 2, PlayStation Portable, PlayStation 3, Wii, Solaris, iPhone

	[SEE_ALSO]
    Memory_Initialize
    Memory_GetStats
    FMOD_MEMORY_REALLOCCALLBACK
    FMOD_MEMORY_FREECALLBACK
    FMOD_MEMORY_TYPE
]
*/
/*
void * F_CALLBACK FMOD_MEMORY_ALLOCCALLBACK(unsigned int size, FMOD_MEMORY_TYPE type)
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
    Callback to re-allocate a block of memory to a different size.

	[PARAMETERS]
    'ptr'   Pointer to a block of memory to be resized.  If this is NULL then a new block of memory is simply allocated.
    'size'  Size of the memory to be reallocated.  The original memory must be preserved.
    'type'  Type of memory allocation.

	[RETURN_VALUE]
    On success, a pointer to the newly re-allocated block of memory is returned.
    On failure, NULL is returned.

	[REMARKS]
    Returning an aligned pointer, of 16 byte alignment is recommended for speed purposes.

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox360, PlayStation 2, PlayStation Portable, PlayStation 3, Wii, Solaris, iPhone

	[SEE_ALSO]
    Memory_Initialize
    Memory_GetStats
    FMOD_MEMORY_ALLOCCALLBACK
    FMOD_MEMORY_FREECALLBACK
    FMOD_MEMORY_TYPE
]
*/
/*
void * F_CALLBACK FMOD_MEMORY_REALLOCCALLBACK(void *ptr, unsigned int size, FMOD_MEMORY_TYPE type)
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
    Callback to free a block of memory.

	[PARAMETERS]
    'ptr'   Pointer to a pre-existing block of memory to be freed.
    'type'  Type of memory to be freed.

	[RETURN_VALUE]
    void

	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Linux64, Macintosh, Xbox360, PlayStation 2, PlayStation Portable, PlayStation 3, Wii, Solaris, iPhone

	[SEE_ALSO]
    Memory_Initialize
    Memory_GetStats
    FMOD_MEMORY_ALLOCCALLBACK
    FMOD_MEMORY_REALLOCCALLBACK
    FMOD_MEMORY_TYPE
]
*/
/*
void F_CALLBACK FMOD_MEMORY_FREECALLBACK(void *ptr, FMOD_MEMORY_TYPE type)
{
#if 0
	 // here purely for documentation purposes.
#endif
}
*/



