#ifndef _FMOD_MEMORYTRACKER_H
#define _FMOD_MEMORYTRACKER_H

#ifndef _FMOD_SETTINGS_H
#include "fmod_settings.h"
#endif

#ifdef FMOD_SUPPORT_MEMORYTRACKER

#ifndef _FMOD_MEMORYINFO_H
#include "fmod_memoryinfo.h"
#endif

#define DECLARE_MEMORYTRACKER_DEFAULT(i, x)                            \
    public :                                                        \
    bool __mMemoryTrackerVisited;                                   \
    virtual FMOD_RESULT getMemoryUsed(MemoryTracker *tracker)       \
    {                                                               \
        if (tracker)                                                \
        {                                                           \
            if (!__mMemoryTrackerVisited)                           \
            {                                                       \
                tracker->add((i), (x), sizeof(*this));                   \
                __mMemoryTrackerVisited = true;                     \
            }                                                       \
        }                                                           \
        else                                                        \
        {                                                           \
            __mMemoryTrackerVisited = false;                        \
        }                                                           \
        return FMOD_OK;                                             \
    }                                                               \
    private :

#define DECLARE_MEMORYTRACKER                                       \
    public :                                                        \
    bool __mMemoryTrackerVisited;                                   \
    virtual FMOD_RESULT getMemoryUsedImpl(MemoryTracker *tracker);  \
    virtual FMOD_RESULT getMemoryUsed(MemoryTracker *tracker)       \
    {                                                               \
        if (tracker)                                                \
        {                                                           \
            if (!__mMemoryTrackerVisited)                           \
            {                                                       \
                FMOD_RESULT result = getMemoryUsedImpl(tracker);    \
                if (result != FMOD_OK)                              \
                {                                                   \
                    return result;                                  \
                }                                                   \
                __mMemoryTrackerVisited = true;                     \
            }                                                       \
        }                                                           \
        else                                                        \
        {                                                           \
            FMOD_RESULT result = getMemoryUsedImpl(tracker);        \
            if (result != FMOD_OK)                                  \
            {                                                       \
                return result;                                      \
            }                                                       \
            __mMemoryTrackerVisited = false;                        \
        }                                                           \
        return FMOD_OK;                                             \
    }                                                               \
    private :

#define DECLARE_MEMORYTRACKER_NONVIRTUAL                            \
    public :                                                        \
    bool __mMemoryTrackerVisited;                                   \
    FMOD_RESULT getMemoryUsedImpl(MemoryTracker *tracker);          \
    FMOD_RESULT getMemoryUsed(MemoryTracker *tracker)               \
    {                                                               \
        if (tracker)                                                \
        {                                                           \
            if (!__mMemoryTrackerVisited)                           \
            {                                                       \
                FMOD_RESULT result = getMemoryUsedImpl(tracker);    \
                if (result != FMOD_OK)                              \
                {                                                   \
                    return result;                                  \
                }                                                   \
                __mMemoryTrackerVisited = true;                     \
            }                                                       \
        }                                                           \
        else                                                        \
        {                                                           \
            FMOD_RESULT result = getMemoryUsedImpl(tracker);        \
            if (result != FMOD_OK)                                  \
            {                                                       \
                return result;                                      \
            }                                                       \
            __mMemoryTrackerVisited = false;                        \
        }                                                           \
        return FMOD_OK;                                             \
    }                                                               \
    private :

#define DECLARE_MEMORYTRACKER_NONVIRTUAL_EXPORT                     \
    public :                                                        \
    bool __mMemoryTrackerVisited;                                   \
    FMOD_RESULT F_API getMemoryUsedImpl(MemoryTracker *tracker);          \
    FMOD_RESULT getMemoryUsed(MemoryTracker *tracker)               \
    {                                                               \
        if (tracker)                                                \
        {                                                           \
            if (!__mMemoryTrackerVisited)                           \
            {                                                       \
                FMOD_RESULT result = getMemoryUsedImpl(tracker);    \
                if (result != FMOD_OK)                              \
                {                                                   \
                    return result;                                  \
                }                                                   \
                __mMemoryTrackerVisited = true;                     \
            }                                                       \
        }                                                           \
        else                                                        \
        {                                                           \
            FMOD_RESULT result = getMemoryUsedImpl(tracker);        \
            if (result != FMOD_OK)                                  \
            {                                                       \
                return result;                                      \
            }                                                       \
            __mMemoryTrackerVisited = false;                        \
        }                                                           \
        return FMOD_OK;                                             \
    }                                                               \
    private :

#define DECLARE_MEMORYTRACKER_ONLY                                                   \
    public :                                                                         \
    virtual FMOD_RESULT getMemoryUsed(MemoryTracker *tracker) { return FMOD_ERR_INTERNAL; }  \
    private :

#define GETMEMORYINFO_IMPL                                          \
                                                                    \
    if (memoryused)                                                 \
    {                                                               \
        *memoryused = 0;                                            \
    }                                                               \
                                                                    \
    MemoryTracker tracker;                                          \
    CHECK_RESULT(getMemoryUsed(0));                                 \
    CHECK_RESULT(getMemoryUsed(&tracker));                          \
                                                                    \
    if (memoryused_details)                                           \
    {                                                               \
        *memoryused_details = tracker.getMemUsedDetails();          \
    }                                                               \
                                                                    \
    if (memoryused)                                                 \
    {                                                               \
        *memoryused = tracker.getMemUsedFromBits(memorybits, event_memorybits); \
    }                                                               \
                                                                    \
    return FMOD_OK;


namespace FMOD
{
    class EventSystemI;

    class MemoryTracker
    {
        private :

        FMOD_MEMORY_USAGE_DETAILS mMemUsed;
        unsigned int mTotalMemUsed;


        public :

        MemoryTracker();

        void         clear();
        void         add(bool eventobject, int bits, unsigned int numbytes);
        unsigned int getTotal();
        FMOD_MEMORY_USAGE_DETAILS getMemUsedDetails() { return mMemUsed; }
        unsigned int getMemUsedFromBits(unsigned int memorybits, unsigned int event_memorybits);
        void         test();
    };
}

#else

#define DECLARE_MEMORYTRACKER_DEFAULT(x)
#define DECLARE_MEMORYTRACKER
#define DECLARE_MEMORYTRACKER_NONVIRTUAL
#define DECLARE_MEMORYTRACKER_NONVIRTUAL_EXPORT
#define DECLARE_MEMORYTRACKER_ONLY

#endif

#endif
