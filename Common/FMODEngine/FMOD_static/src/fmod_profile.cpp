#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_PROFILE

#include "fmod_profile.h"
#include "fmod_systemi.h"
#include "fmod_os_net.h"

namespace FMOD
{

FMOD_RESULT FMOD_Profile_Create(unsigned short port)
{
    if (gGlobal->gProfile)
    {
        return FMOD_OK;
    }

    gGlobal->gProfile = FMOD_Object_Alloc(Profile);
    if (!gGlobal->gProfile)
    {
        return FMOD_ERR_MEMORY;
    }

    FMOD_RESULT result = gGlobal->gProfile->init(port);
    if (result != FMOD_OK)
    {
        gGlobal->gProfile->release();
        gGlobal->gProfile = 0;
    }

    return result;
}


FMOD_RESULT FMOD_Profile_Release()
{
    if (gGlobal->gProfile)
    {
        FMOD_RESULT result;

        result = gGlobal->gProfile->release();
        gGlobal->gProfile = 0;
    }
    
    return FMOD_OK;
}


FMOD_RESULT FMOD_Profile_Update(SystemI *system, unsigned int dt)
{
    if (!gGlobal->gProfile)
    {
        return FMOD_ERR_UNINITIALIZED;
    }

    return gGlobal->gProfile->update(system, dt);
}


/*===================================================================*/


Profile::Profile()
{
    mListenSocket        = 0;
    mCrit                = 0;
    mTimeSinceLastUpdate = FMOD_PROFILE_UPDATE_SPEED;
    mInitialTimestamp    = 0;
}


FMOD_RESULT Profile::init(unsigned short port)
{
    FMOD_RESULT     result      = FMOD_OK;
    unsigned short  listenPort  = port ? port : FMOD_PROFILE_PORT;

    
    result = FMOD_OS_Net_Init();
    if (result != FMOD_OK)
    {
        return result;
    }

    result = FMOD_OS_Net_Listen(listenPort, &mListenSocket);
    if (result != FMOD_OK)
    {
        FMOD_OS_Net_Shutdown();
        return result;
    }

    result = FMOD_OS_CriticalSection_Create(&mCrit);
    if (result != FMOD_OK)
    {
        FMOD_OS_Net_Shutdown();
        return result;
    }

    result = FMOD_OS_Time_GetMs(&mInitialTimestamp);
    if (result != FMOD_OK)
    {
        FMOD_OS_Net_Shutdown();
        return result;
    }

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "Profile::init", "Profiler listening on port: %d\n", listenPort));

    return FMOD_OK;
}


FMOD_RESULT Profile::release()
{
    FMOD_RESULT     result;
    LinkedListNode *node, *nextnode;

    if (mListenSocket)
    {
        FMOD_OS_Net_Close(mListenSocket);
    }

    for (node = mClientHead.getNext(); node != &mClientHead; node = nextnode)
    {
        ProfileClient *client = (ProfileClient *)node;

        nextnode = node->getNext();
        client->removeNode();
        result = client->release();
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    for (node = mModuleHead.getNext(); node != &mModuleHead; node = nextnode)
    {
        ProfileModule *module = (ProfileModule *)node;

        nextnode = node->getNext();
        module->removeNode();
        result = module->release();
        if (result != FMOD_OK)
        {
            return result;
        }

        /*
            now set the globals pointer to null
        */
        if (FMOD::gGlobal->gProfileChannel == (ProfileChannel*)module)
        {
            FMOD::gGlobal->gProfileChannel = 0;
        }
        if (FMOD::gGlobal->gProfileCodec == (ProfileCodec*)module)
        {
            FMOD::gGlobal->gProfileCodec = 0;
        }
        if (FMOD::gGlobal->gProfileCpu == (ProfileCpu*)module)
        {
            FMOD::gGlobal->gProfileCpu = 0;
        }
        if (FMOD::gGlobal->gProfileDsp == (ProfileDsp*)module)
        {
            FMOD::gGlobal->gProfileDsp = 0;
        }
    }

    if (mCrit)
    {
        FMOD_OS_CriticalSection_Free(mCrit);
    }

    FMOD_OS_Net_Shutdown();
    FMOD_Memory_Free(this);

    return FMOD_OK;
}


FMOD_RESULT Profile::update(FMOD::SystemI *system, unsigned int dt)
{
    FMOD_RESULT          result;
    LinkedListNode      *node, *nextnode;
    unsigned int         profiledt;
    LocalCriticalSection crit(mCrit, false);

	mTimeSinceLastUpdate += dt;
    if (mTimeSinceLastUpdate < (unsigned int)FMOD_PROFILE_UPDATE_SPEED)
	{
		return FMOD_OK;
	}
    profiledt = mTimeSinceLastUpdate;
    mTimeSinceLastUpdate = 0;

    /*
        Accept connections from new clients
    */
    void *clientsocket;
	result = FMOD_OS_Net_Accept(mListenSocket, &clientsocket);
    if (result == FMOD_OK)
    {
        ProfileClient *client = FMOD_Object_Alloc(ProfileClient);
        if (!client)
        {
            return FMOD_ERR_MEMORY;
        }

        result = client->init(clientsocket);
        if (result != FMOD_OK)
        {
            return result;
        }

        crit.enter();
        client->addBefore(&mClientHead);
        crit.leave();
    }

    /*
        Update all ProfileModules
    */
    for (node = mModuleHead.getNext(); node != &mModuleHead; node = node->getNext())
    {
        ProfileModule *module = (ProfileModule *)node;

        if (!module->mUpdateTime || (module->mUpdateTime && ((module->mTimeSinceLastUpdate += profiledt) > module->mUpdateTime)))
        {
            result = module->update(system, module->mTimeSinceLastUpdate);
            if (result != FMOD_OK)
            {
                return result;
            }

            module->mTimeSinceLastUpdate = 0;
        }
    }


    crit.enter();

    /*
        Update all ProfileClients
    */
    for (node = mClientHead.getNext(); node != &mClientHead; node = node->getNext())
    {
        ProfileClient *client = (ProfileClient *)node;

        result = client->update(profiledt);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    /*
        Remove dead ProfileClients
    */
    for (node = mClientHead.getNext(); node != &mClientHead; node = nextnode)
    {
        ProfileClient *client = (ProfileClient *)node;

        nextnode = node->getNext();

        if (client->isDead())
        {
            client->removeNode();

            result = client->release();
            if (result != FMOD_OK)
            {
                return result;
            }
        }
    }

    crit.leave();

    return FMOD_OK;
}


FMOD_RESULT Profile::registerModule(ProfileModule *module)
{
    module->addBefore(&mModuleHead);

    return FMOD_OK;
}


FMOD_RESULT Profile::unRegisterModule(ProfileModule *module)
{
    module->removeNode();

    return FMOD_OK;
}


/*
    This could be called from multiple threads
*/
FMOD_RESULT Profile::addPacket(ProfilePacketHeader *packet)
{
    FMOD_RESULT     result;
    LinkedListNode *node;
    unsigned int    t;
    LocalCriticalSection crit(mCrit, true);

    result = FMOD_OS_Time_GetMs(&t);
    if (result != FMOD_OK)
    {
        return result;
    }
    packet->timestamp = t - mInitialTimestamp;

    /*
        Add this packet to all clients that want it
    */
    for (node = mClientHead.getNext(); node != &mClientHead; node = node->getNext())
    {
        ProfileClient *client = (ProfileClient *)node;

        if (client->wantsData(packet))
        {
            result = client->addPacket(packet);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
    }

    return FMOD_OK;
}

#ifdef FMOD_SUPPORT_MEMORYTRACKER

}

#ifdef FMOD_SUPPORT_PROFILE_DSP
    #include "fmod_profile_dsp.h"
#endif
#ifdef FMOD_SUPPORT_PROFILE_CPU
    #include "fmod_profile_cpu.h"
#endif
#ifdef FMOD_SUPPORT_PROFILE_CHANNEL
    #include "fmod_profile_channel.h"
#endif
#ifdef FMOD_SUPPORT_PROFILE_CODEC
    #include "fmod_profile_codec.h"
#endif

namespace FMOD
{

FMOD_RESULT Profile::getMemoryUsedImpl(MemoryTracker *tracker)
{
    tracker->add(false, FMOD_MEMBITS_PROFILE, sizeof(*this));

    if (mCrit)
    {
        tracker->add(false, FMOD_MEMBITS_PROFILE, gSizeofCriticalSection);
    }

#ifdef FMOD_SUPPORT_PROFILE_DSP
    if (gGlobal->gProfileDsp)
    {
        tracker->add(false, FMOD_MEMBITS_PROFILE, sizeof(ProfileDsp));

        if (gGlobal->gProfileDsp->mNodeStack)
        {
            tracker->add(false, FMOD_MEMBITS_PROFILE, sizeof(FMOD::DSPI*) * gGlobal->gProfileDsp->mMaxStackNodes);
        }

        // Packet to be sent over the network
        if (gGlobal->gProfileDsp->mDataPacket)
        {
            tracker->add(false, FMOD_MEMBITS_PROFILE, sizeof(ProfilePacketDspNetwork) + (gGlobal->gProfileDsp->mMaxPacketNodes * sizeof(ProfileDspRawNode)));
        }
    }
#endif

#ifdef FMOD_SUPPORT_PROFILE_CPU
    if (gGlobal->gProfileCpu)
    {
        tracker->add(false, FMOD_MEMBITS_PROFILE, sizeof(ProfileCpu));
    }
#endif

#ifdef FMOD_SUPPORT_PROFILE_CHANNEL
    if (gGlobal->gProfileChannel)
    {
        tracker->add(false, FMOD_MEMBITS_PROFILE, sizeof(ProfileChannel));
    }
#endif

#ifdef FMOD_SUPPORT_PROFILE_CODEC
    if (gGlobal->gProfileCodec)
    {
        tracker->add(false, FMOD_MEMBITS_PROFILE, sizeof(ProfileCodec));
    }
#endif

    return FMOD_OK;
}

#endif


/*===================================================================*/


ProfileModule::ProfileModule()
{
    mUpdateTime          = 0;
    mTimeSinceLastUpdate = 0;
}


FMOD_RESULT ProfileModule::init()
{
    return FMOD_OK;
}


FMOD_RESULT ProfileModule::release()
{
    return FMOD_OK;
}


FMOD_RESULT ProfileModule::update(SystemI *system, unsigned int dt)
{
    return FMOD_OK;
}


/*===================================================================*/


ProfileClient::ProfileClient()
{
    mSocket          = (void *)-1;
    mBuffer          = 0;
    mBufferReadPos   = 0;
    mBufferWritePos  = 0;
    mBufferSize      = 0;
    mFlags           = 0;

    for (int i = 0; i < FMOD_PROFILE_MAX_DATATYPES; i++)
    {
        mDataType[i].type         = FMOD_PROFILE_DATATYPE_NONE;
        mDataType[i].subtype      = 0;
        mDataType[i].updatetime   = 0;
        mDataType[i].lastdatatime = 0;
    }
}


FMOD_RESULT ProfileClient::init(void *socket)
{
    mBufferSize  = FMOD_PROFILE_CLIENT_WRITE_BUFFERSIZE;
    mBuffer      = (char *)FMOD_Memory_Alloc(mBufferSize);
    if (!mBuffer)
    {
        return FMOD_ERR_MEMORY;
    }

    mBufferReadPos   = mBuffer;
    mBufferWritePos  = mBuffer;
    mSocket          = socket;

    return FMOD_OK;
}


FMOD_RESULT ProfileClient::release()
{
    FMOD_OS_Net_Close(mSocket);

    if (mBuffer)
    {
        FMOD_Memory_Free(mBuffer);
    }

    FMOD_Memory_Free(this);

    return FMOD_OK;
}


FMOD_RESULT ProfileClient::update(unsigned int dt)
{
    FMOD_RESULT result;

    if (isDead())
    {
        return FMOD_OK;
    }
    
    result = readData();
    if (result != FMOD_OK)
    {
        return result;
    }

    result = sendData();
    if (result == FMOD_ERR_NET_WOULD_BLOCK)
    {
        // Try again later (next update)
        return FMOD_OK;
    }
    else if (result != FMOD_OK)
    {
        mFlags |= FMOD_PROFILECLIENT_FLAGS_DEAD;
        return result;
    }

    return FMOD_OK;
}


FMOD_RESULT ProfileClient::addPacket(ProfilePacketHeader *packet)
{
    FMOD_RESULT result;

    if (isDead())
    {
        return FMOD_OK;
    }

    if (packet->size > mBufferSize)
    {
        unsigned int bufferreadoffset   = (unsigned int)(mBufferReadPos - mBuffer);
        unsigned int bufferwriteoffset  = (unsigned int)(mBufferWritePos - mBuffer);
        
        mBufferSize = ((packet->size / FMOD_PROFILE_CLIENT_WRITE_BUFFERSIZE) + 1) * FMOD_PROFILE_CLIENT_WRITE_BUFFERSIZE;
        
        mBuffer = (char *)FMOD_Memory_ReAlloc(mBuffer, mBufferSize);
        if (!mBuffer)
        {
            return FMOD_ERR_MEMORY;
        }

        mBufferReadPos   = mBuffer + bufferreadoffset;
        mBufferWritePos  = mBuffer + bufferwriteoffset;
    }

    if ((mBufferWritePos + packet->size) > (mBuffer + mBufferSize))
    {
        result = sendData();
        if (result != FMOD_OK)
        {
            mFlags |= FMOD_PROFILECLIENT_FLAGS_DEAD;
            return FMOD_OK;
        }
    }

    for (int i = 0; i < FMOD_PROFILE_MAX_DATATYPES; i++)
    {
        if ((mDataType[i].type == packet->type) && (mDataType[i].subtype == packet->subtype))
        {
            mDataType[i].lastdatatime = packet->timestamp;
            break;
        }
    }

    /*
        Copy the packet, then endian swap the copied data in-place (if needed)
    */
    FMOD_memcpy(mBufferWritePos, packet, packet->size);
#ifdef PLATFORM_ENDIAN_BIG
    {
        ProfilePacketHeader *bufferedPacket = (ProfilePacketHeader*)mBufferWritePos;

        bufferedPacket->size       = FMOD_SWAPENDIAN_DWORD(bufferedPacket->size);
        bufferedPacket->timestamp  = FMOD_SWAPENDIAN_DWORD(bufferedPacket->timestamp);
    }
#endif
    mBufferWritePos += packet->size;

    return FMOD_OK;
}


FMOD_RESULT ProfileClient::requestDataType(unsigned char type, unsigned char subtype, unsigned int updatetime)
{
    int i;

    for (i = 0; i < FMOD_PROFILE_MAX_DATATYPES; i++)
    {
        if ((mDataType[i].type == type) && (mDataType[i].subtype == subtype))
        {
            if (updatetime)
            {
                mDataType[i].updatetime = updatetime;
            }
            else
            {
                mDataType[i].type = FMOD_PROFILE_DATATYPE_NONE;
            }

            return FMOD_OK;
        }
    }

    for (i = 0; i < FMOD_PROFILE_MAX_DATATYPES; i++)
    {
        if (mDataType[i].type == FMOD_PROFILE_DATATYPE_NONE)
        {
            mDataType[i].type         = type;
            mDataType[i].subtype      = subtype;
            mDataType[i].updatetime   = updatetime;
            mDataType[i].lastdatatime = 0;

            return FMOD_OK;
        }
    }

    return FMOD_OK;
}


bool ProfileClient::wantsData(ProfilePacketHeader *packet)
{
    if (isDead())
    {
        return false;
    }

    for (int i = 0; i < FMOD_PROFILE_MAX_DATATYPES; i++)
    {
        if ((mDataType[i].type == packet->type) && (mDataType[i].subtype == packet->subtype))
        {
            return ((packet->timestamp - mDataType[i].lastdatatime) > mDataType[i].updatetime);
        }
    }

    return false;
}


FMOD_RESULT ProfileClient::readData()
{
    FMOD_RESULT          result;
    unsigned int         bytesread, bytestoread;
    char                 packetbuffer[FMOD_PROFILE_CLIENT_READ_BUFFERSIZE];
    ProfilePacketHeader *packetheader = (ProfilePacketHeader *)(&packetbuffer[0]);

    if (isDead())
    {
        return FMOD_OK;
    }

    for (;;)
    {
        /*
            Read a packet header
        */
        bytestoread = sizeof(ProfilePacketHeader);
        result = FMOD_OS_Net_Read(mSocket, (char *)packetheader, bytestoread, &bytesread);
        if (result == FMOD_ERR_NET_WOULD_BLOCK)
        {
            /*
                No packets to be read
            */
            break;
        }
        else if (result != FMOD_OK)
        {
            /*
                Consider any problems fatal and kill this client
            */
            mFlags |= FMOD_PROFILECLIENT_FLAGS_DEAD;
            return FMOD_OK;
        }

        if (bytesread != bytestoread)       // This needs to be handled gracefully
        {
            mFlags |= FMOD_PROFILECLIENT_FLAGS_DEAD;
            return FMOD_OK;
        }

        /*
            Endian swap the packet header if needed
        */
#ifdef PLATFORM_ENDIAN_BIG
        packetheader->size       = FMOD_SWAPENDIAN_DWORD(packetheader->size);
        packetheader->timestamp  = FMOD_SWAPENDIAN_DWORD(packetheader->timestamp);
#endif

        /*
            Read the packet data
        */
        for (;;)
        {
            bytestoread = packetheader->size - sizeof(ProfilePacketHeader);
            result = FMOD_OS_Net_Read(mSocket, ((char *)packetheader) + sizeof(ProfilePacketHeader), bytestoread, &bytesread);
            if (result == FMOD_OK)
            {
                break;
            }

            if (result != FMOD_ERR_NET_WOULD_BLOCK)
            {
                /*
                    Consider any problems fatal and kill this client
                */
                mFlags |= FMOD_PROFILECLIENT_FLAGS_DEAD;
                return FMOD_OK;
            }

            FMOD_OS_Time_Sleep(1);
        }

        if (bytesread != bytestoread)       // This needs to be handled gracefully
        {
            mFlags |= FMOD_PROFILECLIENT_FLAGS_DEAD;
            return FMOD_OK;
        }

        /*
            Now actually process the packet
        */
        switch (packetheader->type)
        {
            case FMOD_PROFILE_DATATYPE_CONTROL :
            {
                switch (packetheader->subtype)
                {
                    case FMOD_PROFILE_DATASUBTYPE_CONTROL_REQUESTDATA :
                    {
                        ProfilePacketControlRequestData *p = (ProfilePacketControlRequestData *)packetheader;

                        /*
                            Endian swap the subtype parameters if needed
                        */
#ifdef PLATFORM_ENDIAN_BIG
                        p->updatetime = FMOD_SWAPENDIAN_DWORD(p->updatetime);
#endif

                        result = requestDataType(p->type, p->subtype, p->updatetime);
                        if (result != FMOD_OK)
                        {
                            return result;
                        }

                        break;
                    }
                }

                break;
            }

            default :
            {
                break;
            }
        }
    }

    return FMOD_OK;
}


FMOD_RESULT ProfileClient::sendData()
{
    FMOD_RESULT  result        = FMOD_OK;
    unsigned int byteswritten  = 0;
    unsigned int bytestowrite  = (unsigned int)(mBufferWritePos - mBufferReadPos);

    if (isDead() || !bytestowrite)
    {
        return FMOD_OK;
    }

    while (bytestowrite)
    {
        unsigned int chunksize = FMOD_MIN((unsigned int)FMOD_PROFILE_CLIENT_WRITE_BUFFERSIZE, bytestowrite);
        
        result = FMOD_OS_Net_Write(mSocket, mBufferReadPos, chunksize, &byteswritten);
        if (result != FMOD_OK)
        {
            return result;
        }

        mBufferReadPos += byteswritten;
        bytestowrite   -= byteswritten;
    }

    // Buffer now fully emptied
    mBufferReadPos = mBufferWritePos = mBuffer;
    return FMOD_OK;
}


}

#endif //FMOD_SUPPORT_PROFILE
