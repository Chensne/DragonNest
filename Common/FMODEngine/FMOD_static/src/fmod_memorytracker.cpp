#include "fmod_settings.h"
#include "fmod_types.h"
#include <string.h>

#ifdef FMOD_SUPPORT_MEMORYTRACKER

#ifndef _FMOD_MEMORYTRACKER_H
#include "fmod_memorytracker.h"
#endif

//#define FMOD_MEMORYTRACKER_TEST

#ifdef FMOD_MEMORYTRACKER_TEST
#include "crtdbg.h"
#endif

namespace FMOD
{

MemoryTracker::MemoryTracker()
{
#ifdef FMOD_MEMORYTRACKER_TEST
    test();
#endif

    clear();
}

void MemoryTracker::clear()
{
    FMOD_memset(&mMemUsed, 0, sizeof(FMOD_MEMORY_USAGE_DETAILS));    
    mTotalMemUsed = 0;
}

void MemoryTracker::add(bool eventobject, int bits, unsigned int numbytes)
{
    if (this)
    {
        if (eventobject)
        {
            switch(bits)
            {
            case FMOD_EVENT_MEMBITS_EVENTSYSTEM:
                mMemUsed.eventsystem += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_EVENT_MEMBITS_MUSICSYSTEM:
                mMemUsed.musicsystem += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_EVENT_MEMBITS_FEV:
                mMemUsed.fev += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_EVENT_MEMBITS_MEMORYFSB:
                mMemUsed.memoryfsb += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_EVENT_MEMBITS_EVENTPROJECT:
                mMemUsed.eventproject += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_EVENT_MEMBITS_EVENTGROUPI:
                mMemUsed.eventgroupi += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_EVENT_MEMBITS_SOUNDBANKCLASS:
                mMemUsed.soundbankclass += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_EVENT_MEMBITS_SOUNDBANKLIST:
                mMemUsed.soundbanklist += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_EVENT_MEMBITS_STREAMINSTANCE:
                mMemUsed.streaminstance += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_EVENT_MEMBITS_SOUNDDEFCLASS:
                mMemUsed.sounddefclass += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_EVENT_MEMBITS_SOUNDDEFDEFCLASS:
                mMemUsed.sounddefdefclass += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_EVENT_MEMBITS_SOUNDDEFPOOL:
                mMemUsed.sounddefpool += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_EVENT_MEMBITS_REVERBDEF:
                mMemUsed.reverbdef += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_EVENT_MEMBITS_EVENTREVERB:
                mMemUsed.eventreverb += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_EVENT_MEMBITS_USERPROPERTY:
                mMemUsed.userproperty += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_EVENT_MEMBITS_EVENTINSTANCE:
                mMemUsed.eventinstance += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_EVENT_MEMBITS_EVENTINSTANCE_COMPLEX:
                mMemUsed.eventinstance_complex += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_EVENT_MEMBITS_EVENTINSTANCE_SIMPLE:
                mMemUsed.eventinstance_simple += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_EVENT_MEMBITS_EVENTINSTANCE_LAYER:
                mMemUsed.eventinstance_layer += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_EVENT_MEMBITS_EVENTINSTANCE_SOUND:
                mMemUsed.eventinstance_sound += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_EVENT_MEMBITS_EVENTENVELOPE:
                mMemUsed.eventenvelope += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_EVENT_MEMBITS_EVENTENVELOPEDEF:
                mMemUsed.eventenvelopedef += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_EVENT_MEMBITS_EVENTPARAMETER:
                mMemUsed.eventparameter += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_EVENT_MEMBITS_EVENTCATEGORY:
                mMemUsed.eventcategory += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_EVENT_MEMBITS_EVENTENVELOPEPOINT:
                mMemUsed.eventenvelopepoint += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_EVENT_MEMBITS_EVENTINSTANCEPOOL:
                mMemUsed.eventinstancepool += numbytes;
                mTotalMemUsed += numbytes;
                break;
            }
        }
        else
        {
            switch(bits)
            {
            case FMOD_MEMBITS_OTHER:
                mMemUsed.other += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_MEMBITS_STRING:
                mMemUsed.string += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_MEMBITS_SYSTEM:
                mMemUsed.system += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_MEMBITS_PLUGINS:
                mMemUsed.plugins += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_MEMBITS_OUTPUT:
                mMemUsed.output += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_MEMBITS_CHANNEL:
                mMemUsed.channel += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_MEMBITS_CHANNELGROUP:
                mMemUsed.channelgroup += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_MEMBITS_CODEC:
                mMemUsed.codec += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_MEMBITS_FILE:
                mMemUsed.file += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_MEMBITS_SOUND:
                mMemUsed.sound += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_MEMBITS_SOUND_SECONDARYRAM:
                mMemUsed.secondaryram += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_MEMBITS_SOUNDGROUP:
                mMemUsed.soundgroup += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_MEMBITS_STREAMBUFFER:
                mMemUsed.streambuffer += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_MEMBITS_DSPCONNECTION:
                mMemUsed.dspconnection += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_MEMBITS_DSP:
                mMemUsed.dsp += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_MEMBITS_DSPCODEC:
                mMemUsed.dspcodec += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_MEMBITS_PROFILE:
                mMemUsed.profile += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_MEMBITS_RECORDBUFFER:
                mMemUsed.recordbuffer += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_MEMBITS_REVERB:
                mMemUsed.reverb += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_MEMBITS_REVERBCHANNELPROPS:
                mMemUsed.reverbchannelprops += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_MEMBITS_GEOMETRY:
                mMemUsed.geometry += numbytes;
                mTotalMemUsed += numbytes;
                break;
            case FMOD_MEMBITS_SYNCPOINT:
                mMemUsed.syncpoint += numbytes;
                mTotalMemUsed += numbytes;
                break;
            }
        }
    }
}

unsigned int MemoryTracker::getTotal()
{
    return mTotalMemUsed;
}

unsigned int MemoryTracker::getMemUsedFromBits(unsigned int memorybits, unsigned int event_memorybits)
{
    unsigned int used = 0;

    if (memorybits & FMOD_MEMBITS_OTHER)
    {
        used += mMemUsed.other;
    }
    if (memorybits & FMOD_MEMBITS_STRING)
    {
        used += mMemUsed.string;
    }
    if (memorybits & FMOD_MEMBITS_SYSTEM)
    {
        used += mMemUsed.system;
    }
    if (memorybits & FMOD_MEMBITS_PLUGINS)
    {
        used += mMemUsed.plugins;
    }
    if (memorybits & FMOD_MEMBITS_OUTPUT)
    {
        used += mMemUsed.output;
    }
    if (memorybits & FMOD_MEMBITS_CHANNEL)
    {
        used += mMemUsed.channel;
    }
    if (memorybits & FMOD_MEMBITS_CHANNELGROUP)
    {
        used += mMemUsed.channelgroup;
    }
    if (memorybits & FMOD_MEMBITS_CODEC)
    {
        used += mMemUsed.codec;
    }
    if (memorybits & FMOD_MEMBITS_FILE)
    {
        used += mMemUsed.file;
    }
    if (memorybits & FMOD_MEMBITS_SOUND)
    {
        used += mMemUsed.sound;
    }
    if (memorybits & FMOD_MEMBITS_SOUND_SECONDARYRAM)
    {
        used += mMemUsed.secondaryram;
    }
    if (memorybits & FMOD_MEMBITS_SOUNDGROUP)
    {
        used += mMemUsed.soundgroup;
    }
    if (memorybits & FMOD_MEMBITS_STREAMBUFFER)
    {
        used += mMemUsed.streambuffer;
    }
    if (memorybits & FMOD_MEMBITS_DSPCONNECTION)
    {
        used += mMemUsed.dspconnection;
    }
    if (memorybits & FMOD_MEMBITS_DSP)
    {
        used += mMemUsed.dsp;
    }
    if (memorybits & FMOD_MEMBITS_DSPCODEC)
    {
        used += mMemUsed.dspcodec;
    }
    if (memorybits & FMOD_MEMBITS_PROFILE)
    {
        used += mMemUsed.profile;
    }
    if (memorybits & FMOD_MEMBITS_RECORDBUFFER)
    {
        used += mMemUsed.recordbuffer;
    }
    if (memorybits & FMOD_MEMBITS_REVERB)
    {
        used += mMemUsed.reverb;
    }
    if (memorybits & FMOD_MEMBITS_REVERBCHANNELPROPS)
    {
        used += mMemUsed.reverbchannelprops;
    }
    if (memorybits & FMOD_MEMBITS_GEOMETRY)
    {
        used += mMemUsed.geometry;
    }
    if (memorybits & FMOD_MEMBITS_SYNCPOINT)
    {
        used += mMemUsed.syncpoint;
    }

    // event usage
    if (event_memorybits & FMOD_EVENT_MEMBITS_EVENTSYSTEM)
    {
        used += mMemUsed.eventsystem;
    }
    if (event_memorybits & FMOD_EVENT_MEMBITS_MUSICSYSTEM)
    {
        used += mMemUsed.musicsystem;
    }
    if (event_memorybits & FMOD_EVENT_MEMBITS_FEV)
    {
        used += mMemUsed.fev;
    }
    if (event_memorybits & FMOD_EVENT_MEMBITS_MEMORYFSB)
    {
        used += mMemUsed.memoryfsb;
    }
    if (event_memorybits & FMOD_EVENT_MEMBITS_EVENTPROJECT)
    {
        used += mMemUsed.eventproject;
    }
    if (event_memorybits & FMOD_EVENT_MEMBITS_EVENTGROUPI)
    {
        used += mMemUsed.eventgroupi;
    }
    if (event_memorybits & FMOD_EVENT_MEMBITS_SOUNDBANKCLASS)
    {
        used += mMemUsed.soundbankclass;
    }
    if (event_memorybits & FMOD_EVENT_MEMBITS_SOUNDBANKLIST)
    {
        used += mMemUsed.soundbanklist;
    }
    if (event_memorybits & FMOD_EVENT_MEMBITS_STREAMINSTANCE)
    {
        used += mMemUsed.streaminstance;
    }
    if (event_memorybits & FMOD_EVENT_MEMBITS_SOUNDDEFCLASS)
    {
        used += mMemUsed.sounddefclass;
    }
    if (event_memorybits & FMOD_EVENT_MEMBITS_SOUNDDEFDEFCLASS)
    {
        used += mMemUsed.sounddefdefclass;
    }
    if (event_memorybits & FMOD_EVENT_MEMBITS_SOUNDDEFPOOL)
    {
        used += mMemUsed.sounddefpool;
    }
    if (event_memorybits &FMOD_EVENT_MEMBITS_REVERBDEF )
    {
        used += mMemUsed.reverbdef;
    }
    if (event_memorybits & FMOD_EVENT_MEMBITS_EVENTREVERB)
    {
        used += mMemUsed.eventreverb;
    }
    if (event_memorybits & FMOD_EVENT_MEMBITS_USERPROPERTY)
    {
        used += mMemUsed.userproperty;
    }
    if (event_memorybits & FMOD_EVENT_MEMBITS_EVENTINSTANCE)
    {
        used += mMemUsed.eventinstance;
    }
    if (event_memorybits & FMOD_EVENT_MEMBITS_EVENTINSTANCE_COMPLEX)
    {
        used += mMemUsed.eventinstance_complex;
    }
    if (event_memorybits & FMOD_EVENT_MEMBITS_EVENTINSTANCE_SIMPLE)
    {
        used += mMemUsed.eventinstance_simple;
    }
    if (event_memorybits & FMOD_EVENT_MEMBITS_EVENTINSTANCE_LAYER)
    {
        used += mMemUsed.eventinstance_layer;
    }
    if (event_memorybits & FMOD_EVENT_MEMBITS_EVENTINSTANCE_SOUND)
    {
        used += mMemUsed.eventinstance_sound;
    }
    if (event_memorybits & FMOD_EVENT_MEMBITS_EVENTENVELOPE)
    {
        used += mMemUsed.eventenvelope;
    }
    if (event_memorybits & FMOD_EVENT_MEMBITS_EVENTENVELOPEDEF)
    {
        used += mMemUsed.eventenvelopedef;
    }
    if (event_memorybits & FMOD_EVENT_MEMBITS_EVENTPARAMETER)
    {
        used += mMemUsed.eventparameter;
    }
    if (event_memorybits & FMOD_EVENT_MEMBITS_EVENTCATEGORY)
    {
        used += mMemUsed.eventcategory;
    }
    if (event_memorybits & FMOD_EVENT_MEMBITS_EVENTENVELOPEPOINT)
    {
        used += mMemUsed.eventenvelopepoint;
    }

    return used;
}

void MemoryTracker::test()
{
#ifdef FMOD_MEMORYTRACKER_TEST

    clear();

    unsigned int i;

    for (i=1; i <= FMOD_MEMBITS_SYNCPOINT; i <<= 1)
    {
        add(false, i, i);
    }

    for (i=1; i <= FMOD_EVENT_MEMBITS_EVENTINSTANCEPOOL; i <<= 1)
    {
        add(true, i, i);
    }

    _ASSERTE(mMemUsed.other == 0x00000001);
    _ASSERTE(getMemUsedFromBits(0x00000001, 0) == 0x00000001);

    _ASSERTE(mMemUsed.string == 0x00000002);
    _ASSERTE(getMemUsedFromBits(0x00000002, 0) == 0x00000002);

    _ASSERTE(mMemUsed.system == 0x00000004);
    _ASSERTE(getMemUsedFromBits(0x00000004, 0) == 0x00000004);

    _ASSERTE(mMemUsed.plugins == 0x00000008);
    _ASSERTE(getMemUsedFromBits(0x00000008, 0) == 0x00000008);

    _ASSERTE(mMemUsed.output == 0x00000010);
    _ASSERTE(getMemUsedFromBits(0x00000010, 0) == 0x00000010);

    _ASSERTE(mMemUsed.channel == 0x00000020);
    _ASSERTE(getMemUsedFromBits(0x00000020, 0) == 0x00000020);

    _ASSERTE(mMemUsed.channelgroup == 0x00000040);
    _ASSERTE(getMemUsedFromBits(0x00000040, 0) == 0x00000040);

    _ASSERTE(mMemUsed.codec == 0x00000080);
    _ASSERTE(getMemUsedFromBits(0x00000080, 0) == 0x00000080);

    _ASSERTE(mMemUsed.file == 0x00000100);
    _ASSERTE(getMemUsedFromBits(0x00000100, 0) == 0x00000100);

    _ASSERTE(mMemUsed.sound == 0x00000200);
    _ASSERTE(getMemUsedFromBits(0x00000200, 0) == 0x00000200);

    _ASSERTE(mMemUsed.secondaryram == 0x00000400);
    _ASSERTE(getMemUsedFromBits(0x00000400, 0) == 0x00000400);

    _ASSERTE(mMemUsed.soundgroup == 0x00000800);
    _ASSERTE(getMemUsedFromBits(0x00000800, 0) == 0x00000800);

    _ASSERTE(mMemUsed.streambuffer == 0x00001000);
    _ASSERTE(getMemUsedFromBits(0x00001000, 0) == 0x00001000);

    _ASSERTE(mMemUsed.dspconnection == 0x00002000);
    _ASSERTE(getMemUsedFromBits(0x00002000, 0) == 0x00002000);

    _ASSERTE(mMemUsed.dsp == 0x00004000);
    _ASSERTE(getMemUsedFromBits(0x00004000, 0) == 0x00004000);

    _ASSERTE(mMemUsed.dspcodec == 0x00008000);
    _ASSERTE(getMemUsedFromBits(0x00008000, 0) == 0x00008000);

    _ASSERTE(mMemUsed.profile == 0x00010000);
    _ASSERTE(getMemUsedFromBits(0x00010000, 0) == 0x00010000);

    _ASSERTE(mMemUsed.recordbuffer == 0x00020000);
    _ASSERTE(getMemUsedFromBits(0x00020000, 0) == 0x00020000);

    _ASSERTE(mMemUsed.reverb == 0x00040000);
    _ASSERTE(getMemUsedFromBits(0x00040000, 0) == 0x00040000);

    _ASSERTE(mMemUsed.reverbchannelprops == 0x00080000);
    _ASSERTE(getMemUsedFromBits(0x00080000, 0) == 0x00080000);

    _ASSERTE(mMemUsed.geometry == 0x00100000);
    _ASSERTE(getMemUsedFromBits(0x00100000, 0) == 0x00100000);

    _ASSERTE(mMemUsed.syncpoint == 0x00200000);
    _ASSERTE(getMemUsedFromBits(0x00200000, 0) == 0x00200000);

    _ASSERTE(mMemUsed.eventsystem == 0x00000001);
    _ASSERTE(getMemUsedFromBits(0, 0x00000001) == 0x00000001);

    _ASSERTE(mMemUsed.musicsystem == 0x00000002);
    _ASSERTE(getMemUsedFromBits(0, 0x00000002) == 0x00000002);

    _ASSERTE(mMemUsed.fev == 0x00000004);
    _ASSERTE(getMemUsedFromBits(0, 0x00000004) == 0x00000004);

    _ASSERTE(mMemUsed.memoryfsb == 0x00000008);
    _ASSERTE(getMemUsedFromBits(0, 0x00000008) == 0x00000008);

    _ASSERTE(mMemUsed.eventproject == 0x00000010);
    _ASSERTE(getMemUsedFromBits(0, 0x00000010) == 0x00000010);

    _ASSERTE(mMemUsed.eventgroupi == 0x00000020);
    _ASSERTE(getMemUsedFromBits(0, 0x00000020) == 0x00000020);

    _ASSERTE(mMemUsed.soundbankclass == 0x00000040);
    _ASSERTE(getMemUsedFromBits(0, 0x00000040) == 0x00000040);

    _ASSERTE(mMemUsed.soundbanklist == 0x00000080);
    _ASSERTE(getMemUsedFromBits(0, 0x00000080) == 0x00000080);

    _ASSERTE(mMemUsed.streaminstance == 0x00000100);
    _ASSERTE(getMemUsedFromBits(0, 0x00000100) == 0x00000100);

    _ASSERTE(mMemUsed.sounddefclass == 0x00000200);
    _ASSERTE(getMemUsedFromBits(0, 0x00000200) == 0x00000200);

    _ASSERTE(mMemUsed.sounddefdefclass == 0x00000400);
    _ASSERTE(getMemUsedFromBits(0, 0x00000400) == 0x00000400);

    _ASSERTE(mMemUsed.sounddefpool == 0x00000800);
    _ASSERTE(getMemUsedFromBits(0, 0x00000800) == 0x00000800);

    _ASSERTE(mMemUsed.reverbdef == 0x00001000);
    _ASSERTE(getMemUsedFromBits(0, 0x00001000) == 0x00001000);

    _ASSERTE(mMemUsed.eventreverb == 0x00002000);
    _ASSERTE(getMemUsedFromBits(0, 0x00002000) == 0x00002000);

    _ASSERTE(mMemUsed.userproperty == 0x00004000);
    _ASSERTE(getMemUsedFromBits(0, 0x00004000) == 0x00004000);

    _ASSERTE(mMemUsed.eventinstance == 0x00008000);
    _ASSERTE(getMemUsedFromBits(0, 0x00008000) == 0x00008000);

    _ASSERTE(mMemUsed.eventinstance_complex == 0x00010000);
    _ASSERTE(getMemUsedFromBits(0, 0x00010000) == 0x00010000);

    _ASSERTE(mMemUsed.eventinstance_simple == 0x00020000);
    _ASSERTE(getMemUsedFromBits(0, 0x00020000) == 0x00020000);

    _ASSERTE(mMemUsed.eventinstance_layer == 0x00040000);
    _ASSERTE(getMemUsedFromBits(0, 0x00040000) == 0x00040000);

    _ASSERTE(mMemUsed.eventinstance_sound == 0x00080000);
    _ASSERTE(getMemUsedFromBits(0, 0x00080000) == 0x00080000);

    _ASSERTE(mMemUsed.eventenvelope == 0x00100000);
    _ASSERTE(getMemUsedFromBits(0, 0x00100000) == 0x00100000);

    _ASSERTE(mMemUsed.eventenvelopedef == 0x00200000);
    _ASSERTE(getMemUsedFromBits(0, 0x00200000) == 0x00200000);

    _ASSERTE(mMemUsed.eventparameter == 0x00400000);
    _ASSERTE(getMemUsedFromBits(0, 0x00400000) == 0x00400000);

    _ASSERTE(mMemUsed.eventcategory == 0x00800000);
    _ASSERTE(getMemUsedFromBits(0, 0x00800000) == 0x00800000);

    _ASSERTE(mMemUsed.eventenvelopepoint == 0x01000000);
    _ASSERTE(getMemUsedFromBits(0, 0x01000000) == 0x01000000);

    //_ASSERTE(mMemUsed.eventinstancepool == 0x02000000);
    //_ASSERTE(getMemUsedFromBits(0, 0x02000000) == 0x02000000);

#endif
}

}

#endif
