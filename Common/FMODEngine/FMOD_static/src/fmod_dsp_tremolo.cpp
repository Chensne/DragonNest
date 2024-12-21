#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_TREMOLO

#include "fmod.h"
#include "fmod_dspi.h"
#include "fmod_dsp_tremolo.h"
#ifdef PLATFORM_PS3_SPU
    #include "fmod_systemi_spu.h"
#else
    #include "fmod_systemi.h"
#endif

#include <stdio.h>

#ifdef PLATFORM_PS3
extern unsigned int _binary_spu_fmod_dsp_tremolo_pic_start[];
#endif

namespace FMOD
{

FMOD_DSP_DESCRIPTION_EX dsptremolo_desc;

#ifdef PLUGIN_EXPORTS

#ifdef __cplusplus
extern "C" {
#endif

    /*
        FMODGetDSPDescription is mandantory for every fmod plugin.  This is the symbol the registerplugin function searches for.
        Must be declared with F_API to make it export as stdcall.
    */
    F_DECLSPEC F_DLLEXPORT FMOD_DSP_DESCRIPTION_EX * F_API FMODGetDSPDescriptionEx()
    {
        return DSPTremolo::getDescriptionEx();
    }

#ifdef __cplusplus
}
#endif

#endif  /* PLUGIN_EXPORTS */

const DSPTremolo::speakerPhaseMap DSPTremolo::phaseMap[4] = 
{
    //         F/left  F/Right  Front  Woofer  B/Left  B/Right S/Left  S/Right 
    {   2,  {  -0.25f,  0.25f,  0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f    }},
    {   4,  {  -0.125f, 0.125f,-0.375f, 0.375f, 0.0f,   0.0f,   0.0f,   0.0f    }},
    {   6,  {  -0.2f,   0.2f,   0.0f,   0.0f,  -0.4f,   0.4f,   0.0f,   0.0f    }},
    {   8,  {  -0.143f, 0.143f, 0.0f,   0.0f,  -0.429f, 0.429f,-0.286f, 0.286f  }}
};


FMOD_DSP_PARAMETERDESC dsptremolo_param[8] =
{
    {  0.01f, 20.0f,    5.0f,   "Frequency",       "Hz",     "LFO frequency in Hz.  0.01 to 20.  Default = 5." },
    {  0.0f,  1.0f,     1.0f,   "Depth",           "",       "Tremolo depth.  0 to 1.  Default = 1." },
    {  0.0f,  1.0f,     0.0f,   "Shape",           "",       "LFO shape morph between triangle and sine.  0 to 1.  Default = 0." },
    { -1.0f,  1.0f,     0.0f,   "Skew",            "",       "Time-skewing of LFO cycle.  -1 to 1.  Default = 0." },
    {  0.0f,  1.0f,     0.5f,   "Duty",            "",       "LFO on-time.  0 to 1.  Default = 0.5." },
    {  0.0f,  1.0f,     0.0f,   "Square",          "",       "Flatness of the LFO shape.  0 to 1.  Default = 0." },
    {  0.0f,  1.0f,     0.0f,   "Phase",           "",       "Instantaneous LFO phase.  0 to 1.  Default = 0." },
    { -1.0f,  1.0f,     0.0f,   "Spread",          "",       "Rotation / auto-pan effect.  -1 to 1.  Default = 0." },
};

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_DSP_DESCRIPTION_EX *DSPTremolo::getDescriptionEx()
{
#ifndef PLATFORM_PS3_SPU
    FMOD_memset(&dsptremolo_desc, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));

    FMOD_strcpy(dsptremolo_desc.name, "FMOD Tremolo");
    dsptremolo_desc.version       = 0x00010100;
    dsptremolo_desc.create        = DSPTremolo::createCallback;
    dsptremolo_desc.release       = DSPTremolo::releaseCallback;
    dsptremolo_desc.reset         = DSPTremolo::resetCallback;

    #ifdef PLATFORM_PS3
    dsptremolo_desc.read          = (FMOD_DSP_READCALLBACK)_binary_spu_fmod_dsp_tremolo_pic_start;    /* SPU PIC entry address */
    #else
    dsptremolo_desc.read          = DSPTremolo::readCallback;
    #endif

    dsptremolo_desc.numparameters = sizeof(dsptremolo_param) / sizeof(dsptremolo_param[0]);
    dsptremolo_desc.paramdesc     = dsptremolo_param;
    dsptremolo_desc.setparameter  = DSPTremolo::setParameterCallback;
    dsptremolo_desc.getparameter  = DSPTremolo::getParameterCallback;
#ifdef FMOD_SUPPORT_MEMORYTRACKER
    dsptremolo_desc.getmemoryused = &DSPTremolo::getMemoryUsedCallback;
#endif

    dsptremolo_desc.mType         = FMOD_DSP_TYPE_TREMOLO;
    dsptremolo_desc.mCategory     = FMOD_DSP_CATEGORY_FILTER;
    dsptremolo_desc.mSize         = sizeof(DSPTremolo);
#else
    dsptremolo_desc.read          = DSPTremolo::readCallback;                  /* We only care about read function on SPU */
#endif
    return &dsptremolo_desc;
}

#ifndef PLATFORM_PS3_SPU

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT DSPTremolo::createInternal()
{
    int count;

    init();
    
    mChannels = 0;
	mOldSpeakerMask = 0xFFFF;

    for (count = 0; count < mDescription.numparameters; count++)
    {
        FMOD_RESULT result;

        result = setParameter(count, mDescription.paramdesc[count].defaultval);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    /*
        Do a forced update here to make sure everything is calculated correctly first.
    */
    {
        mSystem->getSoftwareFormat(&mOutputRate, 0, &mChannels, 0, 0, 0);

        mTransitionRampFactor = 1.0f / (float)TREMOLO_TRANSITION;
        mInvTableSize = 1.0f / (float)TREMOLO_TABLE_SIZE;

        mFrequency  = mFrequencyUpdate;
        mDepth      = mDepthUpdate;
        mShape      = mShapeUpdate;
        mSkew       = mSkewUpdate;
        mDuty       = mDutyUpdate;
        mSquare     = mSquareUpdate;
        mPhase      = mPhaseUpdate;
        mSpread     = mSpreadUpdate;

        mLFOMin = 1.0f - mDepth;

        resetInternal();

        createLFOTable();
        updateWaveform();
        updateTiming();
        applyPhase();
    }

    return FMOD_OK;
}

#endif //!PLATFORM_PS3_SPU

void DSPTremolo::createLFOTable()
{
    float angle = -FMOD_PI_2;
    float anglestep = FMOD_PI * mInvTableSize;
    float ramp = 0.0f;
    float rampstep = 1.0f * mInvTableSize;

    for (int i = 0; i <= TREMOLO_TABLE_SIZE; i++)
    {
        mLFOTable[i] = mShape * (FMOD_SIN(angle) * 0.5f + 0.5f);
        mLFOTable[i] += (1.0f - mShape) * ramp;

        angle += anglestep;
        ramp += rampstep;
    }
}


float DSPTremolo::readLFOTable(int index, bool forward, float *ramp)
{
    if (forward)
    {
        if (index == TREMOLO_TABLE_SIZE)
        {
            *ramp = 0.0f;
        }
        else
        {
            *ramp = (mLFOTable[index + 1] - mLFOTable[index]) * mUpRampFactor;
        }
    }
    else
    {
        if (index == 0)
        {
            *ramp = 0.0f;
        }
        else
        {
            *ramp = (mLFOTable[index - 1] - mLFOTable[index]) * mDownRampFactor;
        }
    }

    return mLFOMin + mDepth * mLFOTable[index];
}


void DSPTremolo::updateWaveform()
{
    float skew = (mSkew + 1.0f) * 0.5f;
    float ontime = (1.0f - mDuty) * skew;
    float offtime = skew + (1.0f - skew) * mDuty;
    float halfuptime = (1.0f - mSquare) * FMOD_MIN(ontime, skew - ontime);
    float halfdowntime = (1.0f - mSquare) * FMOD_MIN(offtime - skew, 1.0f - offtime);

    mKUp = ontime - halfuptime;
    mKHigh = ontime + halfuptime;
    mKDown = offtime - halfdowntime;
    mKLow = offtime + halfdowntime;
}


void DSPTremolo::updateTiming()
{
    float phase[TREMOLO_MAX_CHANNELS];
    float nextramp[TREMOLO_MAX_CHANNELS];
    int ch;

    for (ch = 0; ch < mChannels; ch++) 
    {
        phase[ch] = (float)mLFOPos[ch] * mInvPeriod;
        nextramp[ch] = (float)mLFONextRamp[ch] * mInvPeriod;
    }

    mPeriod = (float)mOutputRate / mFrequency;
    mInvPeriod = 1.0f / mPeriod;
    mPeriodSamples = (int)(floor(mPeriod));
    mPeriodAdjust = mPeriod - floor(mPeriod);
    mPeriodOffset = 0.0f;
    mPeriodAddSample = 0;

    mTableUpPeriod = mPeriod * (mKHigh - mKUp) * mInvTableSize;
    mTableDownPeriod = mPeriod * (mKLow - mKDown) * mInvTableSize;

    if (mTableUpPeriod <= 0.0f)
    {
        mTableUpPeriod = 0.0f;
        mUpRampFactor = 0.0f;
    }
    else
    {
        mUpRampFactor = mDepth / mTableUpPeriod;
    }

    if (mTableDownPeriod <= 0.0f)
    {
        mTableDownPeriod = 0.0f;
        mDownRampFactor = 0.0f;
    }
    else
    {
        mDownRampFactor = mDepth / mTableDownPeriod;
    }

    mPosUp      = (int)(mPeriod * mKUp);
    mPosHigh    = (int)(mPeriod * mKHigh);
    mPosDown    = (int)(mPeriod * mKDown);
    mPosLow     = (int)(mPeriod * mKLow);

    for (ch = 0; ch < mChannels; ch++) 
    {
        mLFOPos[ch] = (int)(phase[ch] * mPeriod);
        if ((float)mLFOPos[ch] >= mPeriod)
        {
            mLFOPos[ch] = 0;
        }
        mLFONextRamp[ch] = (int)(nextramp[ch] * mPeriod);
        if (mLFONextRamp[ch] >= mPeriodSamples)
        {
            mLFONextRamp[ch] = 0;
        }
    }
}


void DSPTremolo::applyPhase()
{
    int map = -1;
    int i, ch;

    for (i = 0; i < sizeof(phaseMap)/sizeof(phaseMap[0]); i++)
    {
        if (phaseMap[i].channelcount == mChannels && mChannels <= TREMOLO_MAX_CHANNEL_MAPS)
        {
            map = i;
        }
    }

    for (ch = 0; ch < mChannels; ch++) 
    {
        if (map >= 0)
        {
            mLFOPos[ch] = (int)((mPhase - phaseMap[map].phase[ch] * mSpread) * mPeriod);
        }
        else
        {
            mLFOPos[ch] = (int)((mPhase - ((float)i / (float)mChannels * mSpread)) * mPeriod);
        }

        if (mLFOPos[ch] >= mPeriodSamples)
        {
            mLFOPos[ch] -= mPeriodSamples;
        }
        else if (mLFOPos[ch] < 0)
        {
            mLFOPos[ch] += mPeriodSamples;
        }
    }
}

#ifndef PLATFORM_PS3_SPU

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT DSPTremolo::releaseInternal()
{
    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT DSPTremolo::resetInternal()
{
    for (int i = 0; i < TREMOLO_MAX_CHANNELS; i++)
    {
        mLFOPos[i] = 0;
        mLFOLevel[i] = 0.0f;
        mLFORamp[i] = 0.0f;
        mLFONextRamp[i] = 0;
    }

    mUpdatePhase = false;

    return FMOD_OK;
}

#endif // !PLATFORM_PS3_SPU


void DSPTremolo::getRampValues(int samplepos, float *p_lfolevel, float *p_lforamp, int *p_nextramppos)
{
    int tableindex;

    if (samplepos >= mPosLow) // LFO bottom
    {
        *p_lfolevel = 1.0f - mDepth;
        *p_lforamp = 0.0f;

        *p_nextramppos = mPosUp;
    }
    else if (samplepos >= mPosDown) // LFO downward slope 
    {
        tableindex = ((samplepos - mPosDown) * TREMOLO_TABLE_SIZE) / (mPosLow - mPosDown);

        if (tableindex + 1 == TREMOLO_TABLE_SIZE)
        {
            *p_nextramppos = mPosLow;
        }
        else
        {
            *p_nextramppos = mPosDown + (int)(mTableDownPeriod * (float)(tableindex + 1) + 0.5f);
        }

        if (samplepos != mPosDown + (int)(mTableDownPeriod * (float)(tableindex) + 0.5f))
        {
            *p_lfolevel = readLFOTable(TREMOLO_TABLE_SIZE - tableindex, false, p_lforamp);
            *p_lfolevel += *p_lforamp * ((float)samplepos - ((float)mPosDown + tableindex * mTableDownPeriod));
        }
        else
        {
            if (tableindex == 0)
            {
                *p_lfolevel = readLFOTable(TREMOLO_TABLE_SIZE - tableindex, true, p_lforamp);
            }
            else
            {
                readLFOTable(TREMOLO_TABLE_SIZE - tableindex, false, p_lforamp);
            }
        }
    }
    else if (samplepos >= mPosHigh) // LFO top
    {
        *p_lfolevel = 1.0f;
        *p_lforamp = 0.0f;

        *p_nextramppos = mPosDown;
    }
    else if (samplepos >= mPosUp) // LFO upward slope
    {
        tableindex = ((samplepos - mPosUp) * TREMOLO_TABLE_SIZE) / (mPosHigh - mPosUp);

        if (tableindex + 1 == TREMOLO_TABLE_SIZE)
        {
            *p_nextramppos = mPosHigh;
        }
        else
        {
            *p_nextramppos = mPosUp + (int)(mTableUpPeriod * (float)(tableindex + 1) + 0.5f);
        }

        if (samplepos != mPosUp + (int)(mTableUpPeriod * (float)(tableindex) + 0.5f))
        {
            *p_lfolevel = readLFOTable(tableindex, true, p_lforamp);
            *p_lfolevel += *p_lforamp * ((float)samplepos - ((float)mPosUp + tableindex * mTableUpPeriod)); 
        }
        else
        {
            if (tableindex == 0)
            {
                *p_lfolevel = readLFOTable(tableindex, true, p_lforamp);
            }
            else
            {
                readLFOTable(tableindex, true, p_lforamp);
            }
        }
    }
    else // LFO bottom
    {
        *p_lfolevel = 1.0f - mDepth;
        *p_lforamp = 0.0f;

        *p_nextramppos = mPosUp;
    }

    if (*p_nextramppos >= mPeriodSamples)
    {
        *p_nextramppos = 0;
    }
}

float DSPTremolo::getLFOLevel(int samplepos)
{
    int tableindex;
    float level;
    float ramp;

    if (samplepos >= mPosLow) // LFO bottom
    {
        level = 1.0f - mDepth;
    }
    else if (samplepos >= mPosDown) // LFO downward slope 
    {
        tableindex = ((samplepos - mPosDown) * TREMOLO_TABLE_SIZE) / (mPosLow - mPosDown);

        level = readLFOTable(TREMOLO_TABLE_SIZE - tableindex, false, &ramp);
        level += ramp * ((float)samplepos - ((float)mPosDown + tableindex * mTableDownPeriod));
    }
    else if (samplepos >= mPosHigh) // LFO top
    {
        level = 1.0f;
    }
    else if (samplepos >= mPosUp) // LFO upward slope
    {
        tableindex = ((samplepos - mPosUp) * TREMOLO_TABLE_SIZE) / (mPosHigh - mPosUp);

        level = readLFOTable(tableindex, true, &ramp);
        level += ramp * ((float)samplepos - ((float)mPosUp + tableindex * mTableUpPeriod)); 
    }
    else // LFO bottom
    {
        level = 1.0f - mDepth;
    }

    return level;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT DSPTremolo::readInternal(float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int outchannels)
{   
    bool changedepth = false;
    bool changetiming = false;
    bool changeshape = false;
    bool changewaveform = false;
    bool changephase = false;
    bool transition = false;

    #define OO_32767 (1.0f / 32767.0f)

    if (!inbuffer)
    {
        return FMOD_OK;
    }

    int outputrate = 0;

    #ifdef PLATFORM_PS3
    outputrate = 48000;
    #else
    mSystem->getSoftwareFormat(&outputrate, 0, 0, 0, 0, 0);
    #endif

    /*
        Update parameters
    */
    if (mChannels != inchannels)
    {
        mChannels = inchannels;
        changephase = true;
    }
    if (mOutputRate != outputrate)
    {
        mOutputRate = outputrate;
        changetiming = true;
    }
    if (mFrequency != mFrequencyUpdate)
    {
        mFrequency = mFrequencyUpdate;
        changetiming = true;
    }
    if (mDepth != mDepthUpdate)
    {
        mDepth = mDepthUpdate;
        changedepth = true;
    }
    if (mShape != mShapeUpdate)
    {
        mShape = mShapeUpdate;
        changeshape = true;
    }
    if (mSkew != mSkewUpdate)
    {
        mSkew  = mSkewUpdate;
        changewaveform = true;
    }
    if (mDuty != mDutyUpdate)
    {
        mDuty  = mDutyUpdate;
        changewaveform = true;
    }
    if (mSquare != mSquareUpdate)
    {
        mSquare  = mSquareUpdate;
        changewaveform = true;
    }
    if (mUpdatePhase)
    {
        mUpdatePhase = false;
        mPhase = mPhaseUpdate;
        changephase = true;
    }
    if (mSpread != mSpreadUpdate)
    {
        mSpread  = mSpreadUpdate;
        changephase = true;
    }

    if (changedepth)
    {
        mLFOMin = 1.0f - mDepth;

        mUpRampFactor = mDepth / mTableUpPeriod;
        mDownRampFactor = mDepth / mTableDownPeriod;
        transition = true;
    }
    if (changeshape)
    {
        createLFOTable();
        transition = true;
    }
    if (changewaveform)
    {
        updateWaveform();
        changetiming = true;
        transition = true;
    }
    if (changetiming)
    {
        updateTiming();
        transition = true;
    }
    if (changephase)
    {
        applyPhase();
        transition = true;
    }

    if (!(speakermask & ((1 << inchannels)-1)) )
    {
        FMOD_memcpy(outbuffer, inbuffer, length * outchannels * sizeof(float));
        return FMOD_OK;
    }

    {
        float *in  = inbuffer;
        float *out = outbuffer;
        int ch;
        
        if (length--)
        {
            for (ch = 0; ch < inchannels; ch++)
            {
                if (transition)
                {
                    mLFONextRamp[ch] = mLFOPos[ch] + TREMOLO_TRANSITION;
                    if (mLFONextRamp[ch] >= mPeriodSamples)
                    {
                        mLFONextRamp[ch] = 0;
                    }
                    mLFORamp[ch] = (getLFOLevel(mLFONextRamp[ch]) - mLFOLevel[ch]) * mTransitionRampFactor;
                }
                else
                {
                    getRampValues(mLFOPos[ch], &mLFOLevel[ch], &mLFORamp[ch], &mLFONextRamp[ch]);
                }
                out[ch] = in[ch] * mLFOLevel[ch];
                mLFOLevel[ch] +=  mLFORamp[ch];

                if (++mLFOPos[ch] >= mPeriodSamples)
                {
                    if (ch == 0)
                    {
                        mPeriodOffset += mPeriodAdjust;
                        if (mPeriodOffset >= 1.0f)
                        {
                            mPeriodOffset -= 1.0f;
                            mPeriodAddSample = inchannels;
                        }
                    }

                    if (mPeriodAddSample > 0)
                    {
                        mLFOPos[ch] = -1;
                        mPeriodAddSample--;
                    }
                    else
                    {
                        mLFOPos[ch] = 0;
                    }
                }
            }

            in += inchannels;
            out += inchannels;
        }

        while (length--)
        {
            for (ch = 0; ch < inchannels; ch++)
            {
                if (mLFOPos[ch] >= mLFONextRamp[ch])
                {
                    getRampValues(mLFOPos[ch], &mLFOLevel[ch], &mLFORamp[ch], &mLFONextRamp[ch]);
                }

                out[ch] = in[ch] * mLFOLevel[ch];
                mLFOLevel[ch] +=  mLFORamp[ch];

                if (++mLFOPos[ch] >= mPeriodSamples)
                {
                    if (ch == 0)
                    {
                        mPeriodOffset += mPeriodAdjust;
                        if (mPeriodOffset >= 1.0f)
                        {
                            mPeriodOffset -= 1.0f;
                            mPeriodAddSample = inchannels;
                        }
                    }

                    if (mPeriodAddSample > 0)
                    {
                        mLFOPos[ch] = -1;
                        mPeriodAddSample--;
                    }
                    else
                    {
                        mLFOPos[ch] = 0;
                    }
                }
            }

            in += inchannels;
            out += inchannels;
        }
    }

    return FMOD_OK;
}

#ifndef PLATFORM_PS3_SPU

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT DSPTremolo::setParameterInternal(int index, float value)
{
    switch (index)
    {
        case FMOD_DSP_TREMOLO_FREQUENCY:
        {
            mFrequencyUpdate = value;
            break;
        }
        case FMOD_DSP_TREMOLO_DEPTH:
        {
            mDepthUpdate = value;
            break;
        }
        case FMOD_DSP_TREMOLO_SHAPE:
        {
            mShapeUpdate = value;
            break;
        }
        case FMOD_DSP_TREMOLO_SKEW:
        {
            mSkewUpdate = value;
            break;
        }
        case FMOD_DSP_TREMOLO_DUTY:
        {
            mDutyUpdate = value;
            break;
        }
        case FMOD_DSP_TREMOLO_SQUARE:
        {
            mSquareUpdate = value;
            break;
        }
        case FMOD_DSP_TREMOLO_PHASE:
        {
            mPhaseUpdate = value;
            mUpdatePhase = (value > 0.0f);
            break;
        }
        case FMOD_DSP_TREMOLO_SPREAD:
        {
            mSpreadUpdate = value;
            break;
        }
    }

    return FMOD_OK;
}

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT DSPTremolo::getParameterInternal(int index, float *value, char *valuestr)
{
    switch (index)
    {
        case FMOD_DSP_TREMOLO_FREQUENCY:
        {
            *value = mFrequencyUpdate;
            sprintf(valuestr, "%.02f", mFrequencyUpdate);
            break;
        }
        case FMOD_DSP_TREMOLO_DEPTH:
        {
            *value = mDepthUpdate;
            sprintf(valuestr, "%.02f", mDepthUpdate);
            break;
        }
        case FMOD_DSP_TREMOLO_SHAPE:
        {
            *value = mShapeUpdate;
            sprintf(valuestr, "%.02f", mShapeUpdate);
            break;
        }
        case FMOD_DSP_TREMOLO_SKEW:
        {
            *value = mSkewUpdate;
            sprintf(valuestr, "%.02f", mSkewUpdate);
            break;
        }
        case FMOD_DSP_TREMOLO_DUTY:
        {
            *value = mDutyUpdate;
            sprintf(valuestr, "%.02f", mDutyUpdate);
            break;
        }
        case FMOD_DSP_TREMOLO_SQUARE:
        {
            *value = mSquareUpdate;
            sprintf(valuestr, "%.02f", mSquareUpdate);
            break;
        }
        case FMOD_DSP_TREMOLO_PHASE:
        {
            *value = mPhaseUpdate;
            sprintf(valuestr, "%.02f", mPhaseUpdate);
            break;
        }
        case FMOD_DSP_TREMOLO_SPREAD:
        {
            *value = mSpreadUpdate;
            sprintf(valuestr, "%.02f", mSpreadUpdate);
            break;
        }
    }

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
 
	[REMARKS]

    [PLATFORMS]
 
	[SEE_ALSO]
]
*/

#ifdef FMOD_SUPPORT_MEMORYTRACKER

FMOD_RESULT DSPTremolo::getMemoryUsedImpl(MemoryTracker *tracker)
{
    // Size of this class is already accounted for (via description.mSize). Just add extra allocated memory here.

    return FMOD_OK;
}

#endif


/*
    ==============================================================================================================

    CALLBACK INTERFACE

    ==============================================================================================================
*/


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK DSPTremolo::createCallback(FMOD_DSP_STATE *dsp)
{
    DSPTremolo *tremolo = (DSPTremolo *)dsp;

    return tremolo->createInternal();
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK DSPTremolo::releaseCallback(FMOD_DSP_STATE *dsp)
{
    DSPTremolo *tremolo = (DSPTremolo *)dsp;

    return tremolo->releaseInternal();
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK DSPTremolo::resetCallback(FMOD_DSP_STATE *dsp)
{
    DSPTremolo *tremolo = (DSPTremolo *)dsp;

    return tremolo->resetInternal();
}

#endif // !PLATFORM_PS3_SPU

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK DSPTremolo::readCallback(FMOD_DSP_STATE *dsp, float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int outchannels)
{
    DSPTremolo *tremolo = (DSPTremolo *)dsp;

    return tremolo->readInternal(inbuffer, outbuffer, length, inchannels, outchannels);
}

#ifndef PLATFORM_PS3_SPU

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK DSPTremolo::setParameterCallback(FMOD_DSP_STATE *dsp, int index, float value)
{
    DSPTremolo *tremolo = (DSPTremolo *)dsp;

    return tremolo->setParameterInternal(index, value);
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    Win32, Win64, Linux, Macintosh, XBox, PlayStation 2, GameCube

	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK DSPTremolo::getParameterCallback(FMOD_DSP_STATE *dsp, int index, float *value, char *valuestr)
{
    DSPTremolo *tremolo = (DSPTremolo *)dsp;

    return tremolo->getParameterInternal(index, value, valuestr);
}


#ifdef FMOD_SUPPORT_MEMORYTRACKER
/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/
FMOD_RESULT F_CALLBACK DSPTremolo::getMemoryUsedCallback(FMOD_DSP_STATE *dsp, MemoryTracker *tracker)
{
    DSPTremolo *tremolo = (DSPTremolo *)dsp;    

    return tremolo->DSPTremolo::getMemoryUsed(tracker);
}
#endif

#endif // !PLATFORM_PS3_SPU

}

#endif
