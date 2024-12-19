#ifndef _FMOD_REVERBI_H
#define _FMOD_REVERBI_H

#include "fmod_settings.h"

#include "fmod.hpp"
#include "fmod_memory.h"
#include "fmod_linkedlist.h"

#ifndef _FMOD_MEMORYTRACKER_H
#include "fmod_memorytracker.h"
#endif

namespace FMOD
{

    class SystemI;
    class ChannelI;
	class DSPI;
    class DSPConnectionI;

    typedef enum
    {
        FMOD_REVERB_PHYSICAL    = 0x1,                       
        FMOD_REVERB_VIRTUAL     = 0x2,
    } FMOD_REVERB_MODE;

    #define FMOD_REVERB_MAXINSTANCES 4
    typedef struct FMOD_REVERB_STDPROPERTIES
    {                                  
        int          Environment;       
        float        Room;              
        float        RoomHF;
        float        RoomLF;           
        float        DecayTime;
        float        DecayHFRatio;
        float        Reflections;
        float        ReflectionsDelay;  
        float        Reverb;
        float        ReverbDelay;
        float        HFReference;       
        float        LFReference;       
        float        RoomRolloffFactor;
        float        Diffusion;
        float        Density;          
    } FMOD_REVERB_STDPROPERTIES;

    struct FMOD_REVERB_CHANNELDATA
    {
        FMOD_REVERB_CHANNELPROPERTIES  mChanProps;     // Channel properties which determine input levels
        DSPConnectionI                *mDSPConnection;
        float                          mPresenceGain;
    };

    struct FMOD_REVERB_INSTANCE
    {
        DSPI                           *mDSP;             // DSP which implements the reverb
        FMOD_REVERB_CHANNELDATA        *mChannelData;     // Channeldata for each instance
        FMOD_REVERB_PROPERTIES          mProps;           // ID3L2 listener properties determines nature of reverb
    };

    class ReverbI : public LinkedListNode
    {
        DECLARE_MEMORYTRACKER

        public:
            ReverbI();
#ifdef PLATFORM_PS2
            virtual ~ReverbI() {}
#endif

            static FMOD_RESULT      validate(Reverb *reverb, ReverbI **reverbi);

            FMOD_RESULT             init(SystemI *system, bool is_3d, FMOD_REVERB_MODE reverb_mode=FMOD_REVERB_PHYSICAL);
            FMOD_RESULT             release(bool freethis=true);

            // Reverb manipulation.
            FMOD_RESULT             set3DAttributes(const FMOD_VECTOR *position, float mindistance, float maxdistance);
            FMOD_RESULT             get3DAttributes(FMOD_VECTOR *position, float *mindistance, float *maxdistance);
            FMOD_RESULT             setProperties(const FMOD_REVERB_PROPERTIES* properties);
            FMOD_RESULT             getProperties(FMOD_REVERB_PROPERTIES* properties);
            FMOD_RESULT             setActive(bool active);
            FMOD_RESULT             getActive(bool *active);

            // Userdata set/get.
            FMOD_RESULT             setUserData(void *userdata);
            FMOD_RESULT             getUserData(void **userdata);    
            FMOD_RESULT             getMemoryInfo(unsigned int memorybits, unsigned int event_memorybits, unsigned int *memoryused, FMOD_MEMORY_USAGE_DETAILS *memoryused_details);

        public:

            FMOD_RESULT             setChanProperties(int instance, const int index, const FMOD_REVERB_CHANNELPROPERTIES* props, DSPConnectionI *connection = 0);
            FMOD_RESULT             getChanProperties(int instance, const int index, FMOD_REVERB_CHANNELPROPERTIES* props, DSPConnectionI **connection = 0);
            FMOD_RESULT             resetChanProperties(int instance, int channel);
            FMOD_RESULT             resetConnectionPointer(int instance, int channel);

            FMOD_RESULT             createDSP(int instance);
            FMOD_RESULT             releaseDSP(int instance);
            FMOD_RESULT             setPresenceGain(int instance, int index, float lingain);
            FMOD_RESULT             getPresenceGain(int instance, int index, float *lingain);

            bool                    is3d()                  { return m3D;}
            FMOD_RESULT             setGain(float lingain)  { mGain = lingain; return FMOD_OK;}
            float                   getGain()               { return mGain; }
            FMOD_REVERB_MODE        getMode()               { return mMode; }      


            void                    setDisableIfNoEnvironment(bool dis);
            void                    calculateDistanceGain(FMOD_VECTOR *p, float *linear_gain, float *linear_coeff);
            static void             sumRoomProps(FMOD_REVERB_STDPROPERTIES *accprops, FMOD_REVERB_PROPERTIES *addprops, float factor);
            static void             sumProps(FMOD_REVERB_STDPROPERTIES *accprops, FMOD_REVERB_PROPERTIES *addprops, float factor);
            static void             factorProps(FMOD_REVERB_PROPERTIES *quotientprops, FMOD_REVERB_STDPROPERTIES *divprops, float factor);

            FMOD_REVERB_INSTANCE    mInstance[FMOD_REVERB_MAXINSTANCES];

        private:
            SystemI                *mSystem;        // Instance of the system
            void                   *mUserData;
            float                   mGain;          // Linear gain of component for listener's locale

            bool                    mDisableIfNoEnvironment; // Call setActive() with truth value of (environment==-1)
            bool                    m3D;            // Use this reverb as a 3D per-channel effect. Set by any of the radius/position functions. Cannot be unset!
            bool                    mActive;
            FMOD_REVERB_MODE        mMode;          // The type of 3D reverb this is (if it's 3d)
            FMOD_VECTOR             mPosition;      // Spherical centre
            float                   mMinRadius;     // Spherical inner radius (0dB locus)
            float                   mMaxRadius;     // Spherical outer radius (-infinity dB locus)
            float                   mMinMaxDistance;// Distance between min and max distance
    };
}

#endif
