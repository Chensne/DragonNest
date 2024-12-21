/*$ preserve start $*/

#include "fmod_settings.h"

#include "fmod_dspi.h"

namespace FMOD
{
/*$ preserve end $*/


FMOD_RESULT DSP::release()
{
    FMOD_RESULT result;
    DSPI *dspi;

    result = DSPI::validate(this, &dspi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspi->release();
    }
}


FMOD_RESULT DSP::getSystemObject(System **system)
{
    FMOD_RESULT result;
    DSPI *dspi;

    result = DSPI::validate(this, &dspi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspi->getSystemObject(system);
    }
}


FMOD_RESULT DSP::addInput(DSP *target, DSPConnection **connection)
{
    FMOD_RESULT result;
    DSPI *dspi;

    result = DSPI::validate(this, &dspi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspi->addInput((DSPI *)target, (DSPConnectionI **)connection);
    }
}


FMOD_RESULT DSP::disconnectFrom(DSP *target)
{
    FMOD_RESULT result;
    DSPI *dspi;

    result = DSPI::validate(this, &dspi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspi->disconnectFrom((DSPI *)target);
    }
}


FMOD_RESULT DSP::disconnectAll(bool inputs, bool outputs)
{
    FMOD_RESULT result;
    DSPI *dspi;

    result = DSPI::validate(this, &dspi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspi->disconnectAll(inputs, outputs);
    }
}


FMOD_RESULT DSP::remove()
{
    FMOD_RESULT result;
    DSPI *dspi;

    result = DSPI::validate(this, &dspi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspi->remove();
    }
}


FMOD_RESULT DSP::getNumInputs(int *numinputs)
{
    FMOD_RESULT result;
    DSPI *dspi;

    result = DSPI::validate(this, &dspi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspi->getNumInputs(numinputs);
    }
}


FMOD_RESULT DSP::getNumOutputs(int *numoutputs)
{
    FMOD_RESULT result;
    DSPI *dspi;

    result = DSPI::validate(this, &dspi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspi->getNumOutputs(numoutputs);
    }
}


FMOD_RESULT DSP::getInput(int index, DSP **input, DSPConnection **inputconnection)
{
    FMOD_RESULT result;
    DSPI *dspi;

    result = DSPI::validate(this, &dspi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspi->getInput(index, (DSPI **)input, (DSPConnectionI **)inputconnection);
    }
}


FMOD_RESULT DSP::getOutput(int index, DSP **output, DSPConnection **outputconnection)
{
    FMOD_RESULT result;
    DSPI *dspi;

    result = DSPI::validate(this, &dspi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspi->getOutput(index, (DSPI **)output, (DSPConnectionI **)outputconnection);
    }
}


FMOD_RESULT DSP::setActive(bool active)
{
    FMOD_RESULT result;
    DSPI *dspi;

    result = DSPI::validate(this, &dspi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspi->setActive(active);
    }
}


FMOD_RESULT DSP::getActive(bool *active)
{
    FMOD_RESULT result;
    DSPI *dspi;

    result = DSPI::validate(this, &dspi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspi->getActive(active);
    }
}


FMOD_RESULT DSP::setBypass(bool bypass)
{
    FMOD_RESULT result;
    DSPI *dspi;

    result = DSPI::validate(this, &dspi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspi->setBypass(bypass);
    }
}


FMOD_RESULT DSP::getBypass(bool *bypass)
{
    FMOD_RESULT result;
    DSPI *dspi;

    result = DSPI::validate(this, &dspi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspi->getBypass(bypass);
    }
}


FMOD_RESULT DSP::setSpeakerActive(FMOD_SPEAKER speaker, bool active)
{
    FMOD_RESULT result;
    DSPI *dspi;

    result = DSPI::validate(this, &dspi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspi->setSpeakerActive(speaker, active);
    }
}


FMOD_RESULT DSP::getSpeakerActive(FMOD_SPEAKER speaker, bool *active)
{
    FMOD_RESULT result;
    DSPI *dspi;

    result = DSPI::validate(this, &dspi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspi->getSpeakerActive(speaker, active);
    }
}


FMOD_RESULT DSP::reset()
{
    FMOD_RESULT result;
    DSPI *dspi;

    result = DSPI::validate(this, &dspi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspi->reset();
    }
}


FMOD_RESULT DSP::setParameter(int index, float value)
{
    FMOD_RESULT result;
    DSPI *dspi;

    result = DSPI::validate(this, &dspi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspi->setParameter(index, value);
    }
}


FMOD_RESULT DSP::getParameter(int index, float *value, char *valuestr, int valuestrlen)
{
    FMOD_RESULT result;
    DSPI *dspi;

    result = DSPI::validate(this, &dspi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspi->getParameter(index, value, valuestr, valuestrlen);
    }
}


FMOD_RESULT DSP::getNumParameters(int *numparams)
{
    FMOD_RESULT result;
    DSPI *dspi;

    result = DSPI::validate(this, &dspi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspi->getNumParameters(numparams);
    }
}


FMOD_RESULT DSP::getParameterInfo(int index, char *name, char *label, char *description, int descriptionlen, float *min, float *max)
{
    FMOD_RESULT result;
    DSPI *dspi;

    result = DSPI::validate(this, &dspi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspi->getParameterInfo(index, name, label, description, descriptionlen, min, max);
    }
}


FMOD_RESULT DSP::showConfigDialog(void *hwnd, bool show)
{
    FMOD_RESULT result;
    DSPI *dspi;

    result = DSPI::validate(this, &dspi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspi->showConfigDialog(hwnd, show);
    }
}


FMOD_RESULT DSP::getInfo(char *name, unsigned int *version, int *channels, int *configwidth, int *configheight)
{
    FMOD_RESULT result;
    DSPI *dspi;

    result = DSPI::validate(this, &dspi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspi->getInfo(name, version, channels, configwidth, configheight);
    }
}


FMOD_RESULT DSP::getType(FMOD_DSP_TYPE *type)
{
    FMOD_RESULT result;
    DSPI *dspi;

    result = DSPI::validate(this, &dspi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspi->getType(type);
    }
}


FMOD_RESULT DSP::setDefaults(float frequency, float volume, float pan, int priority)
{
    FMOD_RESULT result;
    DSPI *dspi;

    result = DSPI::validate(this, &dspi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspi->setDefaults(frequency, volume, pan, priority);
    }
}


FMOD_RESULT DSP::getDefaults(float *frequency, float *volume, float *pan, int *priority)
{
    FMOD_RESULT result;
    DSPI *dspi;

    result = DSPI::validate(this, &dspi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspi->getDefaults(frequency, volume, pan, priority);
    }
}


FMOD_RESULT DSP::setUserData(void *_userdata)
{
    FMOD_RESULT result;
    DSPI *dspi;

    result = DSPI::validate(this, &dspi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspi->setUserData(_userdata);
    }
}


FMOD_RESULT DSP::getUserData(void **_userdata)
{
    FMOD_RESULT result;
    DSPI *dspi;

    result = DSPI::validate(this, &dspi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspi->getUserData(_userdata);
    }
}


FMOD_RESULT DSP::getMemoryInfo(unsigned int memorybits, unsigned int event_memorybits, unsigned int *memoryused, FMOD_MEMORY_USAGE_DETAILS *memoryused_details)
{
    FMOD_RESULT result;
    DSPI *dspi;

    result = DSPI::validate(this, &dspi);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return dspi->getMemoryInfo(memorybits, event_memorybits, memoryused, memoryused_details);
    }
}


/*$ preserve start $*/
}
/*$ preserve end $*/
