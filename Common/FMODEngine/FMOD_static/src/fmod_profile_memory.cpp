#include "fmod_settings.h"

#if defined(FMOD_SUPPORT_PROFILE) && defined(FMOD_SUPPORT_PROFILE_MEMORY)

#include "fmod_profile_memory.h"
#include "fmod_systemi.h"
#include "fmod_memory.h"

namespace FMOD
{

ProfileMemory *g_profile_memory = 0;


FMOD_RESULT FMOD_ProfileMemory_Create()
{
    FMOD_RESULT result;

    if (g_profile_memory)
    {
        return FMOD_OK;
    }

    g_profile_memory = FMOD_Object_Alloc(ProfileMemory);
    if (!g_profile_memory)
    {
        return FMOD_ERR_MEMORY;
    }

    result = g_profile_memory->init();
    if (result != FMOD_OK)
    {
        return result;
    }

    return g_profile->registerModule(g_profile_memory);
}


FMOD_RESULT FMOD_ProfileMemory_Release()
{
    FMOD_RESULT result = FMOD_OK;

    if (g_profile_memory)
    {
        result = g_profile->unRegisterModule(g_profile_memory);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = g_profile_memory->release();
        g_profile_memory = 0;
    }

    return result;
}


/*===================================================================*/


ProfileMemory::ProfileMemory()
{
    mUpdateTime = 1000;
    mBuffer     = 0;
}


FMOD_RESULT ProfileMemory::init()
{
    mBuffer = (char *)FMOD_Memory_Alloc(FMOD_PROFILE_CLIENT_WRITE_BUFFERSIZE);
    if (!mBuffer)
    {
        return FMOD_ERR_MEMORY;
    }

    return FMOD_OK;
}


FMOD_RESULT ProfileMemory::release()
{
    if (mBuffer)
    {
        FMOD_Memory_Free(mBuffer);
    }

    FMOD_Memory_Free(this);

    return FMOD_OK;
}


FMOD_RESULT ProfileMemory::update(FMOD::SystemI *system, unsigned int dt)
{
    FMOD_RESULT result;
    MemPool    *mempool = gGlobal->gSystemPool;

    ProfilePacketMemoryDump  *pkt      = (ProfilePacketMemoryDump *)mBuffer;
    ProfilePacketMemoryBlock *blockptr = (ProfilePacketMemoryBlock *)(mBuffer + sizeof(ProfilePacketMemoryDump));

    pkt->hdr.size      = sizeof(pkt);
    pkt->hdr.timestamp = 0;
    pkt->hdr.type      = FMOD_PROFILE_DATATYPE_MEMORY;
    pkt->hdr.subtype   = FMOD_PROFILE_DATASUBTYPE_MEMORY_DUMP;
    pkt->hdr.version   = FMOD_PROFILE_MEMORY_VERSION;
    pkt->hdr.flags     = 0;

    pkt->blocksize     = mempool->mBlockSize;
    pkt->sizeblocks    = mempool->mSizeBlocks;
    pkt->numallocs     = 0;

    unsigned int bytesalloced = 0;
    unsigned int realbytesalloced = 0;
    unsigned int wastage = 0;

    for (int blockoffset=0; blockoffset < mempool->mSizeBlocks; )
    {
        int byteoffset = blockoffset / 8;
        int bitoffset  = blockoffset & 7;

        if (mempool->mBitmap[byteoffset] & (1 << bitoffset))
        {
            // Will we run out of buffer room if we use the next block?
            if (sizeof(ProfilePacketMemoryDump) + (sizeof(ProfilePacketMemoryBlock) * (pkt->numallocs + 1)) > FMOD_PROFILE_CLIENT_WRITE_BUFFERSIZE)
            {
                /* Need to handle this gracefully... */
                return FMOD_ERR_INTERNAL;
            }

            // got an alloced block
            // if so, output it, see how big it is and skip to just past it
            MemBlockHeader *block = (MemBlockHeader *)(mempool->mData + (blockoffset * mempool->mBlockSize));

//AJS            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "ProfileMemory::update", "%d  %d  %d\n", block->mSize, block->mNumBlocks, block->mBlockOffset));

            blockptr->mSize        = block->mSize;
            blockptr->mNumBlocks   = block->mNumBlocks;
            blockptr->mBlockOffset = block->mBlockOffset;
            blockptr++;

            blockoffset += block->mNumBlocks;

            pkt->numallocs++;

            // it doesn't match exactly with memool internal vars - not taking into account the headers?

            bytesalloced += block->mSize;
            realbytesalloced += (block->mNumBlocks * mempool->mBlockSize);
            wastage = (realbytesalloced - bytesalloced);
        }
        else
        {
            blockoffset++;
        }
    }

//AJS    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "ProfileMemory::update", "%d allocs\n", pkt->numallocs));

    pkt->hdr.size = sizeof(ProfilePacketMemoryDump) + (pkt->numallocs * sizeof(ProfilePacketMemoryBlock));

    result = g_profile->addPacket((ProfilePacketHeader *)pkt);
    if (result != FMOD_OK)
    {
        return result;
    }

    return FMOD_OK;
}


}

#endif  // FMOD_SUPPORT_PROFILE && FMOD_SUPPORT_PROFILE_MEMORY
