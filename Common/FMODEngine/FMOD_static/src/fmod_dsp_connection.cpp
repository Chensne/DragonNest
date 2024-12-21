/*$ preserve start $*/

#include "fmod_settings.h"

#include "fmod.hpp"
#include "fmod_dsp_connectioni.h"
#include "fmod_systemi.h"

namespace FMOD
{
/*$ preserve end $*/


FMOD_RESULT DSPConnection::getInput(DSP **input)
{
    FMOD_RESULT result;
    DSPConnectionI *dspconnectioni;

    result = DSPConnectionI::validate(this, &dspconnectioni);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspconnectioni->getInput((DSPI **)input);
    }
}


FMOD_RESULT DSPConnection::getOutput(DSP **output)
{
    FMOD_RESULT result;
    DSPConnectionI *dspconnectioni;

    result = DSPConnectionI::validate(this, &dspconnectioni);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspconnectioni->getOutput((DSPI **)output);
    }
}


FMOD_RESULT DSPConnection::setMix(float volume)
{
    FMOD_RESULT result;
    DSPConnectionI *dspconnectioni;

    result = DSPConnectionI::validate(this, &dspconnectioni);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspconnectioni->setMix(volume);
    }
}


FMOD_RESULT DSPConnection::getMix(float *volume)
{
    FMOD_RESULT result;
    DSPConnectionI *dspconnectioni;

    result = DSPConnectionI::validate(this, &dspconnectioni);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspconnectioni->getMix(volume);
    }
}


FMOD_RESULT DSPConnection::setLevels(FMOD_SPEAKER speaker, float *levels, int numlevels)
{
    FMOD_RESULT result;
    DSPConnectionI *dspconnectioni;

    result = DSPConnectionI::validate(this, &dspconnectioni);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspconnectioni->setLevels(speaker, levels, numlevels);
    }
}


FMOD_RESULT DSPConnection::getLevels(FMOD_SPEAKER speaker, float *levels, int numlevels)
{
    FMOD_RESULT result;
    DSPConnectionI *dspconnectioni;

    result = DSPConnectionI::validate(this, &dspconnectioni);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspconnectioni->getLevels(speaker, levels, numlevels);
    }
}


FMOD_RESULT DSPConnection::setUserData(void *_userdata)
{
    FMOD_RESULT result;
    DSPConnectionI *dspconnectioni;

    result = DSPConnectionI::validate(this, &dspconnectioni);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspconnectioni->setUserData(_userdata);
    }
}


FMOD_RESULT DSPConnection::getUserData(void **_userdata)
{
    FMOD_RESULT result;
    DSPConnectionI *dspconnectioni;

    result = DSPConnectionI::validate(this, &dspconnectioni);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspconnectioni->getUserData(_userdata);
    }
}


FMOD_RESULT DSPConnection::getMemoryInfo(unsigned int memorybits, unsigned int event_memorybits, unsigned int *memoryused, FMOD_MEMORY_USAGE_DETAILS *memoryused_details)
{
    FMOD_RESULT result;
    DSPConnectionI *dspconnectioni;

    result = DSPConnectionI::validate(this, &dspconnectioni);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspconnectioni->getMemoryInfo(memorybits, event_memorybits, memoryused, memoryused_details);
    }
}


/*$ preserve start $*/
}
/*$ preserve end $*/
