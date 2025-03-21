#ifndef _FMOD_PROFILE_H
#define _FMOD_PROFILE_H

#ifndef _FMOD_SETTINGS_H
#include "fmod_settings.h"
#endif

#ifdef FMOD_SUPPORT_PROFILE

#ifndef _FMOD_H
#include "fmod.h"
#endif
#ifndef _FMOD_LINKEDLIST_H
#include "fmod_linkedlist.h"
#endif
#ifndef _FMOD_LOCALCRITICALSECTION_H
#include "fmod_localcriticalsection.h"
#endif
#ifndef _FMOD_PROFILE_PKT_H
#include "fmod_profile_pkt.h"
#endif
#ifndef _FMOD_MEMORYTRACKER_H
#include "fmod_memorytracker.h"
#endif

namespace FMOD
{
    class SystemI;
    class ProfileModule;

    const int FMOD_PROFILE_UPDATE_SPEED              = 50;
    const int FMOD_PROFILE_MAX_DATATYPES             = 32;
    const int FMOD_PROFILE_CLIENT_READ_BUFFERSIZE    = 16384;
    const int FMOD_PROFILE_CLIENT_WRITE_BUFFERSIZE   = 16384;

    const unsigned int FMOD_PROFILECLIENT_FLAGS_DEAD = 0x00000001;

    typedef struct
    {
        unsigned char type;
        unsigned char subtype;
        unsigned int  updatetime;                        // How often the client wants to get this data type
        unsigned int  lastdatatime;                      // Time of last update

    } ProfileDataType;


    /*
        The Profile class gets input from ProfileModules and distributes it to ProfileClients
    */
    class Profile
    {
        DECLARE_MEMORYTRACKER

        private :

        void                    *mListenSocket;
        LinkedListNode           mClientHead;           // List of connected client apps. Accessed from multiple threads - protected with mCrit.
        LinkedListNode           mModuleHead;           // List of registered profiler modules
        FMOD_OS_CRITICALSECTION *mCrit;
        unsigned int             mTimeSinceLastUpdate;
        unsigned int             mInitialTimestamp;


        public :

        Profile();

        FMOD_RESULT init(unsigned short port);
        FMOD_RESULT release();
        FMOD_RESULT update(SystemI *system, unsigned int dt);
        FMOD_RESULT registerModule(ProfileModule *module);
        FMOD_RESULT unRegisterModule(ProfileModule *module);
        FMOD_RESULT addPacket(ProfilePacketHeader *packet);
    };


    /*
        A ProfileModule collects data from FMOD and adds it to the profile data stream
    */
    class ProfileModule : public LinkedListNode
    {
        public :

        unsigned int mUpdateTime;
        unsigned int mTimeSinceLastUpdate;


        ProfileModule();

        virtual FMOD_RESULT init();
        virtual FMOD_RESULT release();
        virtual FMOD_RESULT update(SystemI *system, unsigned int dt);
    };


    /*
        A ProfileClient represents a remote app which is connected to the profile data stream
    */
    class ProfileClient : public LinkedListNode
    {
        private :

        unsigned int    mFlags;
        void           *mSocket;
        char           *mBuffer;                                // Buffer that data packets will be accumulated in before sending
        char           *mBufferWritePos;
        char           *mBufferReadPos;
        unsigned int    mBufferSize;
        ProfileDataType mDataType[FMOD_PROFILE_MAX_DATATYPES];  // Array of data types that this client wants to see

        FMOD_RESULT     readData();                             // Read data packets from the client app and act on them
        FMOD_RESULT     sendData();                             // Send all data in this client's queue to the client


        public :

        ProfileClient();

        FMOD_RESULT init(void *socket);
        FMOD_RESULT release();
        FMOD_RESULT update(unsigned int dt);
        FMOD_RESULT addPacket(ProfilePacketHeader *packet);                         // Add a data packet to this client's queue
        FMOD_RESULT requestDataType(unsigned char type, unsigned char subtype, unsigned int updatetime);
        bool        wantsData(ProfilePacketHeader *packet);                         // Returns true if this client wants a packet of this type/subtype at this time
        bool        isDead() { return (mFlags & FMOD_PROFILECLIENT_FLAGS_DEAD); }   // Returns true if this client's connection is broken in any way
    };


    FMOD_RESULT FMOD_Profile_Create(unsigned short port);
    FMOD_RESULT FMOD_Profile_Release();
    FMOD_RESULT FMOD_Profile_Update(SystemI *system, unsigned int dt);

    extern Profile *g_profile;
}

#endif // FMOD_SUPPORT_PROFILE

#endif // _FMOD_PROFILE_H

