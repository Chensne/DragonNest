#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_MYEARS

#include "fmod_downmix_myears.h"
#include "fmod_memory.h"
#include <memory.h>

#define MYEARS_LOGGING
#define MYEARS_SUPPORT_EXTERNALISE_FILTERS
//#define MYEARS_5POINT1_MODE

#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_WINDOWS64)
#define MYEARS_DATA_FILE "\\myears\\data.myears"
#define MYEARS_LOG_FILE "\\myears\\myears.log"
#elif defined(PLATFORM_MAC)
#define MYEARS_DATA_FILE "" // <- todo
#undef MYEARS_LOGGING
#elif defined(PLATFORM_IPHONE)
#define MYEARS_DATA_FILE "" // <- todo
#undef MYEARS_LOGGING
#endif

#ifdef MYEARS_LOGGING

#include <stdarg.h>
#include <time.h>

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#endif

#ifdef PLATFORM_WINDOWS64
#include <windows.h>
#endif

#endif // MYEARS_LOGGING



namespace FMOD
{

const int    DownmixMyEars::MYEARS_EXTERNALISE_DELAY    = 1056;         // 22ms @ 48kHz
const float  DownmixMyEars::MYEARS_EXTERNALISE_GAIN     = 0.031622f;    // -30dB
const float  DownmixMyEars::MYEARS_LFE_CUTOFF           = 120.0f;
const float  DownmixMyEars::MYEARS_LFE_GAIN             = 1.0f;

bool DownmixMyEars::dataExists() 
{
    char filename[256];

#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_WINDOWS64)
    // Hack!
    GetSystemDirectory(filename, 256);
    filename[2] = 0;
    strcat(filename, MYEARS_DATA_FILE);

	FILE* file = fopen(filename, "r");
    if (file)
    {
        fclose(file);
        return true;
    }
#endif

    return false;
}

int DownmixMyEars::getSampleRate()
{
    return MYEARS_SAMPLE_RATE;
}

bool DownmixMyEars::log(char* fmtstr, ...)
{
#ifdef MYEARS_LOGGING
    char dirname[256];

#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_WINDOWS64)
    GetSystemDirectory(dirname, 256);
    dirname[2] = 0;
    strcat(dirname, MYEARS_LOG_FILE);

	FILE* logfile = fopen(dirname, "a");
    if (logfile)
    {
        char str[256];
        va_list argptr;
        va_start(argptr, fmtstr);
        vsprintf(str, fmtstr, argptr);

        fprintf(logfile, str);
        fclose(logfile);

        return true;
    }
#endif

    return false;
#else
    return true;
#endif
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
FMOD_RESULT DownmixMyEars::init(unsigned int dspbuffersize, unsigned int rate, FMOD_SPEAKERMODE outputspeakermode, void *extradata)
{
    FMOD_RESULT result;

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "DownmixMyEars::init", "dspbuffersize: %d  rate: %d.\n", dspbuffersize, rate));

    mOutputSpeakerMode = outputspeakermode;
    for (int i = 0; i < MYEARS_SPEAKER_COUNT; ++i)
    {
        mLeftFilter[i] = 0;
        mRightFilter[i] = 0;
        mLeftFilterExt[i] = 0;
        mRightFilterExt[i] = 0;
    }
    mUseExtFilters = false;

    if (outputspeakermode != FMOD_SPEAKERMODE_STEREO)
    {
        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "DownmixMyEars::init", "outputspeakermode must be FMOD_SPEAKERMODE_STEREO\n"));

        return FMOD_ERR_INVALID_PARAM;
    }

    if (rate != MYEARS_SAMPLE_RATE)
    {
        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "DownmixMyEars::init", "rate must be %d\n", MYEARS_SAMPLE_RATE));

        return FMOD_ERR_INVALID_PARAM;
    }

#ifdef MYEARS_5POINT1_MODE
    mNumInputChannels   = 6;
#else
    mNumInputChannels   = 8;
#endif
    mNumOutputChannels  = 2;

    mLFECutoff = MYEARS_LFE_CUTOFF;
    mLFEGain = MYEARS_LFE_GAIN;

    if (extradata)
    {
        MyEarsExtraData *myearsdata = (MyEarsExtraData *)extradata;
        // read initialisation data here
    }

    if ((result = loadFile(MYEARS_DATA_FILE)) != FMOD_OK)
    {
        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "DownmixMyEars::init", "Error opening MyEars file: %s\n", MYEARS_DATA_FILE));

        return result;
    }
    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "DownmixMyEars::init", "Successfully loaded MyEars file: %s\n", MYEARS_DATA_FILE));

    initLFEFilter();

#ifdef MYEARS_LOGGING
    time_t now = time(0);
    log("%s  Loaded '%s'\n  Externalisation = %s\n  LFEGain = %f\n", ctime(&now), MYEARS_DATA_FILE, mUseExtFilters ? "On" : "Off", mLFEGain);
#endif

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "DownmixMyEars::init", "done.\n"));

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
FMOD_RESULT DownmixMyEars::shutdown()
{
    for (int i = 0; i < MYEARS_SPEAKER_COUNT; ++i)
    {
        unloadFilter(mLeftFilter[i]);
        unloadFilter(mRightFilter[i]);
        unloadFilter(mLeftFilterExt[i]);
        unloadFilter(mRightFilterExt[i]);

        mLeftFilter[i] = 0;
        mRightFilter[i] = 0;
        mLeftFilterExt[i] = 0;
        mRightFilterExt[i] = 0;
    }

    return FMOD_OK;
}


void DownmixMyEars::initFilter(MyEarsFilter *filter)
{
    if (filter->buffer)
    {
        FMOD_memset(filter->buffer, 0, filter->delay * sizeof(float));
        filter->bufferpos = filter->buffer;
    }
    FMOD_memset(filter->state, 0, MYEARS_FILTER_ORDER * sizeof(float));
}


float DownmixMyEars::applyFilter(float sample, MyEarsFilter *filter)
{
    // apply tiny dc offset to prevent float denorm values

    sample += 1.0e-20f;

    // apply ITD delay

    if (filter->buffer)
    {
        *filter->bufferpos = sample;
        if (++filter->bufferpos >= filter->bufferend)
        {
            filter->bufferpos = filter->buffer;
        }
        sample = *filter->bufferpos;
    }

    float *state = filter->state;
    float *nextstate = state + 1;
    float *a = filter->a + 1;
    float *b = filter->b;

    // apply filter in transposed Direct Form II

    float y = *(b++) * sample + *state;                             //  0
    *(state++) = *(b++) * sample - *(a++) * y + *(nextstate++);     //  1
    *(state++) = *(b++) * sample - *(a++) * y + *(nextstate++);     //  2
    *(state++) = *(b++) * sample - *(a++) * y + *(nextstate++);     //  3
    *(state++) = *(b++) * sample - *(a++) * y + *(nextstate++);     //  4
    *(state++) = *(b++) * sample - *(a++) * y + *(nextstate++);     //  5
    *(state++) = *(b++) * sample - *(a++) * y + *(nextstate++);     //  6
    *(state++) = *(b++) * sample - *(a++) * y + *(nextstate++);     //  7
    *(state++) = *(b++) * sample - *(a++) * y + *(nextstate++);     //  8
    *(state++) = *(b++) * sample - *(a++) * y + *(nextstate++);     //  9
    *(state++) = *(b++) * sample - *(a++) * y + *(nextstate++);     // 10
    *(state++) = *(b++) * sample - *(a++) * y + *(nextstate++);     // 11
    *(state++) = *(b++) * sample - *(a++) * y + *(nextstate++);     // 12
    *(state++) = *(b++) * sample - *(a++) * y + *nextstate;         // 13
    *state = *b * sample - *a * y;                                  // 14

    return y;
}


#if MYEARS_LFE_ORDER == 2

void DownmixMyEars::initLFEFilter()
{
    float damp = sqrtf(0.5f);
    float c = 1.0f / FMOD_TAN(FMOD_PI * mLFECutoff / ((float)MYEARS_SAMPLE_RATE));
    float cc = c * c;

    mLFEFilter.b[0] = 1.0f / (1.0f + 2.0f*damp*c + cc) * mLFEGain;
    mLFEFilter.b[1] = 2.0f * mLFEFilter.b[0];
    mLFEFilter.b[2] = mLFEFilter.b[0];

    mLFEFilter.a[0] = 1.0f;
    mLFEFilter.a[1] = 2.0f * mLFEFilter.b[0] * (1.0f - cc);
    mLFEFilter.a[2] = mLFEFilter.b[0] * (1.0f - 2.0f * damp*c + cc);

    mLFEFilter.state[0] = 0.0f;
    mLFEFilter.state[1] = 0.0f;
}


float DownmixMyEars::applyLFEFilter(float sample)
{
    // apply tiny dc offset to prevent float denorm values

    sample += 1.0e-20f;

    // apply filter in transposed Direct Form II

    float y             = mLFEFilter.b[0] * sample                       + mLFEFilter.state[0]; //  0
    mLFEFilter.state[0] = mLFEFilter.b[1] * sample - mLFEFilter.a[1] * y + mLFEFilter.state[1]; //  1
    mLFEFilter.state[1] = mLFEFilter.b[2] * sample - mLFEFilter.a[2] * y;                       //  2

    return y;
}

#elif MYEARS_LFE_ORDER == 4

static int convolve(float *in1, int len1, float *in2, int len2, float *out)
{
    int convlen = len1 + len2 - 1;

    for (int i = 0; i < convlen; i++)
    {
	    float sum = 0.0f;

        int n_lo = i - len2 + 1;
        if (n_lo < 0)
        {
            n_lo = 0;
        }

	    int n_hi = len1 - 1;
        if (n_hi > i)
        {
            n_hi = i;
        }

	    float *p1 = in1 + n_lo;
	    float *p2 = in2 + i - n_lo;
	    for (int n = n_lo; n <= n_hi; n++)
        {
		    sum += *p1 * *p2;
		    p1++;
		    p2--;
		}

	    *out++ = sum;
	}

    return convlen;
}

// 4th order Linkwitz-Riley
void DownmixMyEars::initLFEFilter()
{
    float damp = sqrtf(0.5f);
    float c = 1.0f / FMOD_TAN(FMOD_PI * mLFECutoff / ((float)MYEARS_SAMPLE_RATE));
    float cc = c * c;

    float b[3];
    float a[3];

    b[0] = 1.0f / (1.0f + 2.0f * damp * c + cc);
    b[1] = 2.0f * b[0];
    b[2] = b[0];

    a[0] = 1.0f;
    a[1] = 2.0f * b[0] * (1.0f - cc);
    a[2] = b[0] * (1.0f - 2.0f * damp*c + cc);

    convolve(b, 3, b, 3, mLFEFilter.b);
    convolve(a, 3, a, 3, mLFEFilter.a);

    for (int i = 0; i < MYEARS_LFE_ORDER; i++)
    {
        mLFEFilter.state[i] = 0.0f;
    }
}


float DownmixMyEars::applyLFEFilter(float sample)
{
    // apply tiny dc offset to prevent float denorm values
    static float dither = 1.0e-20f;

    sample += dither;
    dither = -dither;

    float *state = mLFEFilter.state;
    float *nextstate = state + 1;
    float *b = mLFEFilter.b;
    float *a = mLFEFilter.a + 1;

    // apply filter in transposed Direct Form II

    float y = *(b++) * sample + *state;                             //  0
    *(state++) = *(b++) * sample - *(a++) * y + *(nextstate++);     //  1
    *(state++) = *(b++) * sample - *(a++) * y + *(nextstate++);     //  2
    *(state++) = *(b++) * sample - *(a++) * y + *nextstate;         //  3
    *state = *b * sample - *a * y;                                  //  4

    return y * mLFEGain;
}

#endif


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
FMOD_RESULT DownmixMyEars::encode(float *inbuffer, float *outbuffer, unsigned int numsamples)
{
    float *in  = inbuffer;
    float *out = outbuffer;

#ifdef MYEARS_SUPPORT_EXTERNALISE_FILTERS
    if (mUseExtFilters)
    {
        switch(mNumInputChannels)
        {
        case 8:
            for (unsigned int i = 0; i < numsamples; i++)
            {
                float extleft;
                float extright;
                float lfe;

                out[0]      = applyFilter(*in  , mLeftFilter[FMOD_SPEAKER_FRONT_LEFT]);
                out[1]      = applyFilter(*in  , mRightFilter[FMOD_SPEAKER_FRONT_LEFT]);
                extleft     = applyFilter(*in  , mLeftFilterExt[FMOD_SPEAKER_FRONT_LEFT]);
                extright    = applyFilter(*in++, mRightFilterExt[FMOD_SPEAKER_FRONT_LEFT]);
                out[0]     += applyFilter(*in  , mLeftFilter[FMOD_SPEAKER_FRONT_RIGHT]);
                out[1]     += applyFilter(*in  , mRightFilter[FMOD_SPEAKER_FRONT_RIGHT]);
                extleft    += applyFilter(*in  , mLeftFilterExt[FMOD_SPEAKER_FRONT_RIGHT]);
                extright   += applyFilter(*in++, mRightFilterExt[FMOD_SPEAKER_FRONT_RIGHT]);
                out[0]     += applyFilter(*in  , mLeftFilter[FMOD_SPEAKER_FRONT_CENTER]);
                out[1]     += applyFilter(*in  , mRightFilter[FMOD_SPEAKER_FRONT_CENTER]);
                extleft    += applyFilter(*in  , mLeftFilterExt[FMOD_SPEAKER_FRONT_CENTER]);
                extright   += applyFilter(*in++, mRightFilterExt[FMOD_SPEAKER_FRONT_CENTER]);
                lfe         = applyLFEFilter(*in++);
                out[0]     += lfe;
                out[1]     += lfe;
                out[0]     += applyFilter(*in  , mLeftFilter[FMOD_SPEAKER_BACK_LEFT]);
                out[1]     += applyFilter(*in  , mRightFilter[FMOD_SPEAKER_BACK_LEFT]);
                extleft    += applyFilter(*in  , mLeftFilterExt[FMOD_SPEAKER_BACK_LEFT]);
                extright   += applyFilter(*in++, mRightFilterExt[FMOD_SPEAKER_BACK_LEFT]);
                out[0]     += applyFilter(*in  , mLeftFilter[FMOD_SPEAKER_BACK_RIGHT]);
                out[1]     += applyFilter(*in  , mRightFilter[FMOD_SPEAKER_BACK_RIGHT]);
                extleft    += applyFilter(*in  , mLeftFilterExt[FMOD_SPEAKER_BACK_RIGHT]);
                extright   += applyFilter(*in++, mRightFilterExt[FMOD_SPEAKER_BACK_RIGHT]);
                out[0]     += applyFilter(*in  , mLeftFilter[FMOD_SPEAKER_SIDE_LEFT]);
                out[1]     += applyFilter(*in  , mRightFilter[FMOD_SPEAKER_SIDE_LEFT]);
                extleft    += applyFilter(*in  , mLeftFilterExt[FMOD_SPEAKER_SIDE_LEFT]);
                extright   += applyFilter(*in++, mRightFilterExt[FMOD_SPEAKER_SIDE_LEFT]);
                out[0]     += applyFilter(*in  , mLeftFilter[FMOD_SPEAKER_SIDE_RIGHT]);
                out[1]     += applyFilter(*in  , mRightFilter[FMOD_SPEAKER_SIDE_RIGHT]);
                extleft    += applyFilter(*in  , mLeftFilterExt[FMOD_SPEAKER_SIDE_RIGHT]);
                extright   += applyFilter(*in++, mRightFilterExt[FMOD_SPEAKER_SIDE_RIGHT]);

                out[0]     += extleft * MYEARS_EXTERNALISE_GAIN;
                out[1]     += extright * MYEARS_EXTERNALISE_GAIN;

                out[0] *= 0.5f;
                out[1] *= 0.5f;

                out += 2;
            }
            break;

        case 6:
            for (unsigned int i = 0; i < numsamples; i++)
            {
                float extleft;
                float extright;
                float lfe;

                out[0]      = applyFilter(*in  , mLeftFilter[FMOD_SPEAKER_FRONT_LEFT]);
                out[1]      = applyFilter(*in  , mRightFilter[FMOD_SPEAKER_FRONT_LEFT]);
                extleft     = applyFilter(*in  , mLeftFilterExt[FMOD_SPEAKER_FRONT_LEFT]);
                extright    = applyFilter(*in++, mRightFilterExt[FMOD_SPEAKER_FRONT_LEFT]);
                out[0]     += applyFilter(*in  , mLeftFilter[FMOD_SPEAKER_FRONT_RIGHT]);
                out[1]     += applyFilter(*in  , mRightFilter[FMOD_SPEAKER_FRONT_RIGHT]);
                extleft    += applyFilter(*in  , mLeftFilterExt[FMOD_SPEAKER_FRONT_RIGHT]);
                extright   += applyFilter(*in++, mRightFilterExt[FMOD_SPEAKER_FRONT_RIGHT]);
                out[0]     += applyFilter(*in  , mLeftFilter[FMOD_SPEAKER_FRONT_CENTER]);
                out[1]     += applyFilter(*in  , mRightFilter[FMOD_SPEAKER_FRONT_CENTER]);
                extleft    += applyFilter(*in  , mLeftFilterExt[FMOD_SPEAKER_FRONT_CENTER]);
                extright   += applyFilter(*in++, mRightFilterExt[FMOD_SPEAKER_FRONT_CENTER]);
                lfe         = applyLFEFilter(*in++);
                out[0]     += lfe;
                out[1]     += lfe;
                out[0]     += applyFilter(*in  , mLeftFilter[FMOD_SPEAKER_BACK_LEFT]);
                out[1]     += applyFilter(*in  , mRightFilter[FMOD_SPEAKER_BACK_LEFT]);
                extleft    += applyFilter(*in  , mLeftFilterExt[FMOD_SPEAKER_BACK_LEFT]);
                extright   += applyFilter(*in++, mRightFilterExt[FMOD_SPEAKER_BACK_LEFT]);
                out[0]     += applyFilter(*in  , mLeftFilter[FMOD_SPEAKER_BACK_RIGHT]);
                out[1]     += applyFilter(*in  , mRightFilter[FMOD_SPEAKER_BACK_RIGHT]);
                extleft    += applyFilter(*in  , mLeftFilterExt[FMOD_SPEAKER_BACK_RIGHT]);
                extright   += applyFilter(*in++, mRightFilterExt[FMOD_SPEAKER_BACK_RIGHT]);

                out[0]     += extleft * MYEARS_EXTERNALISE_GAIN;
                out[1]     += extright * MYEARS_EXTERNALISE_GAIN;

                out[0] *= 0.5f;
                out[1] *= 0.5f;

                out += 2;
            }
            break;

        default:
            return FMOD_ERR_INTERNAL;

        }
    }
    else
#endif
    {
        switch(mNumInputChannels)
        {
        case 8:
            for (unsigned int i = 0; i < numsamples; i++)
            {
                float lfe;

                out[0]  = applyFilter(*in  , mLeftFilter[FMOD_SPEAKER_FRONT_LEFT]);
                out[1]  = applyFilter(*in++, mRightFilter[FMOD_SPEAKER_FRONT_LEFT]);
                out[0] += applyFilter(*in  , mLeftFilter[FMOD_SPEAKER_FRONT_RIGHT]);
                out[1] += applyFilter(*in++, mRightFilter[FMOD_SPEAKER_FRONT_RIGHT]);
                out[0] += applyFilter(*in  , mLeftFilter[FMOD_SPEAKER_FRONT_CENTER]);
                out[1] += applyFilter(*in++, mRightFilter[FMOD_SPEAKER_FRONT_CENTER]);
                lfe     = applyLFEFilter(*in++);
                out[0] += lfe;
                out[1] += lfe;
                out[0] += applyFilter(*in  , mLeftFilter[FMOD_SPEAKER_BACK_LEFT]);
                out[1] += applyFilter(*in++, mRightFilter[FMOD_SPEAKER_BACK_LEFT]);
                out[0] += applyFilter(*in  , mLeftFilter[FMOD_SPEAKER_BACK_RIGHT]);
                out[1] += applyFilter(*in++, mRightFilter[FMOD_SPEAKER_BACK_RIGHT]);
                out[0] += applyFilter(*in  , mLeftFilter[FMOD_SPEAKER_SIDE_LEFT]);
                out[1] += applyFilter(*in++, mRightFilter[FMOD_SPEAKER_SIDE_LEFT]);
                out[0] += applyFilter(*in  , mLeftFilter[FMOD_SPEAKER_SIDE_RIGHT]);
                out[1] += applyFilter(*in++, mRightFilter[FMOD_SPEAKER_SIDE_RIGHT]);

                out[0] *= 0.5f;
                out[1] *= 0.5f;

                out += 2;
            }
            break;

        case 6:
            for (unsigned int i = 0; i < numsamples; i++)
            {
                float lfe;

                out[0]  = applyFilter(*in  , mLeftFilter[FMOD_SPEAKER_FRONT_LEFT]);
                out[1]  = applyFilter(*in++, mRightFilter[FMOD_SPEAKER_FRONT_LEFT]);
                out[0] += applyFilter(*in  , mLeftFilter[FMOD_SPEAKER_FRONT_RIGHT]);
                out[1] += applyFilter(*in++, mRightFilter[FMOD_SPEAKER_FRONT_RIGHT]);
                out[0] += applyFilter(*in  , mLeftFilter[FMOD_SPEAKER_FRONT_CENTER]);
                out[1] += applyFilter(*in++, mRightFilter[FMOD_SPEAKER_FRONT_CENTER]);
                lfe     = applyLFEFilter(*in++);
                out[0] += lfe;
                out[1] += lfe;
                out[0] += applyFilter(*in  , mLeftFilter[FMOD_SPEAKER_BACK_LEFT]);
                out[1] += applyFilter(*in++, mRightFilter[FMOD_SPEAKER_BACK_LEFT]);
                out[0] += applyFilter(*in  , mLeftFilter[FMOD_SPEAKER_BACK_RIGHT]);
                out[1] += applyFilter(*in++, mRightFilter[FMOD_SPEAKER_BACK_RIGHT]);

                out[0] *= 0.5f;
                out[1] *= 0.5f;

                out += 2;
            }
            break;

        default:
            return FMOD_ERR_INTERNAL;
        
        }
    }

    return FMOD_OK;
}


FMOD_RESULT DownmixMyEars::loadFile(const char *filename)
{
    FMOD_RESULT result;

	FILE* file = fopen(filename, "r");
	if (!file)
	{
        return FMOD_ERR_FILE_NOTFOUND;
    }

    for(int i = 0; i < MYEARS_SPEAKER_COUNT; ++i)
    {
        if (i != FMOD_SPEAKER_LOW_FREQUENCY)
        {
            if ((result = loadFilter(file, &mRightFilter[i])) != FMOD_OK)
            {
                fclose(file);
                return result;
            }
            if ((result = loadFilter(file, &mLeftFilter[i])) != FMOD_OK)
            {
                fclose(file);
                return result;
            }
            if ((result = loadFilter(file, &mRightFilterExt[i], true)) != FMOD_OK)
            {
                fclose(file);
                return result;
            }
            if ((result = loadFilter(file, &mLeftFilterExt[i], true)) != FMOD_OK)
            {
                fclose(file);
                return result;
            }

            initFilter(mRightFilter[i]);
            initFilter(mLeftFilter[i]);
            initFilter(mRightFilterExt[i]);
            initFilter(mLeftFilterExt[i]);
        }
        else
        {
            mRightFilter[i] = 0;
            mLeftFilter[i] = 0;
            mRightFilterExt[i] = 0;
            mLeftFilterExt[i] = 0;
        }
    }

    int ext;
    if (fscanf(file, "%d", &ext) == EOF) // enable externalisation filters
    {
        fclose(file);
        return FMOD_ERR_FORMAT;
    }

    if (fscanf(file, ",%f", &mLFEGain) == EOF) // LFE gain (optional)
    {
        mLFEGain = MYEARS_LFE_GAIN;
    }

    mUseExtFilters = (ext != 0);

    fclose(file);

    return FMOD_OK;
}


FMOD_RESULT DownmixMyEars::loadFilter(FILE *file, MyEarsFilter **filter, bool externaliser)
{
    int dummy;
    int i;

    *filter = (MyEarsFilter *)FMOD_Memory_Calloc(sizeof(MyEarsFilter));
    if (!filter)
    {
        return FMOD_ERR_MEMORY;
    }

    // 'a' coefficients
    if (fscanf(file, "%d,", &dummy) == EOF) // ignore 'a' coefficient delays
    {
        goto MYEARS_FILTER_LOAD_ERROR;
    }
    for (i = 0; i < MYEARS_FILTER_ORDER; ++i)
    {
        if (fscanf(file, "%f,", &(*filter)->a[i]) == EOF)
        {
            goto MYEARS_FILTER_LOAD_ERROR;
        }
    }
    if (fscanf(file, "%f\n", &(*filter)->a[i]) == EOF)
    {
        goto MYEARS_FILTER_LOAD_ERROR;
    }

    // 'b' coefficients
    if (fscanf(file, "%d,", &(*filter)->delay) == EOF)
    {
        goto MYEARS_FILTER_LOAD_ERROR;
    }
    for (i = 0; i < MYEARS_FILTER_ORDER; ++i)
    {
        if (fscanf(file, "%f,", &(*filter)->b[i]) == EOF)
        {
            goto MYEARS_FILTER_LOAD_ERROR;
        }
    }
    if (fscanf(file, "%f\n", &(*filter)->b[i]) == EOF)
    {
        goto MYEARS_FILTER_LOAD_ERROR;
    }

    if (externaliser)
    {
        (*filter)->delay += MYEARS_EXTERNALISE_DELAY;
    }

    if ((*filter)->delay)
    {
        (*filter)->buffer = (float *)FMOD_Memory_Calloc((*filter)->delay * sizeof(float));
        if (!(*filter)->buffer)
        {
            FMOD_Memory_Free(*filter);
            return FMOD_ERR_MEMORY;
        }

        (*filter)->bufferend = (*filter)->buffer + (*filter)->delay;
    }
    else
    {
        (*filter)->buffer = 0;
    }

    return FMOD_OK;

MYEARS_FILTER_LOAD_ERROR:
    FMOD_Memory_Free(*filter);
    return FMOD_ERR_FORMAT;
}


void DownmixMyEars::unloadFilter(MyEarsFilter *filter)
{
    if (filter)
    {
        if (filter->buffer)
        {
            FMOD_Memory_Free(filter->buffer);
        }
        FMOD_Memory_Free(filter);
    }
}


}

#endif //FMOD_SUPPORT_MYEARS
