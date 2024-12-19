#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_NEURAL

#include "fmod_downmix_neuralthx.h"
#include "fmod_memory.h"

namespace FMOD
{

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
FMOD_RESULT DownmixNeural::init(unsigned int dspbuffersize, unsigned int rate, FMOD_SPEAKERMODE outputspeakermode, void *extradata)
{
    int err;

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "DownmixNeural::init", "dspbuffersize: %d  rate: %d.\n", dspbuffersize, rate));

    if (dspbuffersize % NEURAL_FRAMESIZE)
    {
        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "DownmixNeural::init", "dspbuffersize: %d must be multiple of 256\n", dspbuffersize));

        return FMOD_ERR_INVALID_PARAM;
    }

    mOutputSpeakerMode = outputspeakermode;
    switch(mOutputSpeakerMode)
    {
        case FMOD_SPEAKERMODE_STEREO:
        {
            mNeuralMode         = NEURAL_THX_7_2_GAMING;
            mNumInputChannels   = 8;
            mNumOutputChannels  = 2;

            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "DownmixNeural::init", "Initialising to NEURAL_7_2_GAMING.\n"));

            break;
        }
        case FMOD_SPEAKERMODE_5POINT1:
        {
            mNeuralMode         = NEURAL_THX_7_5_GAMING;
            mNumInputChannels   = 8;
            mNumOutputChannels  = 6;

            FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "DownmixNeural::init", "Initialising to NEURAL_7_5_GAMING.\n"));

            break;
        }
        default:
        {
            FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "DownmixNeural::init", "outputspeakermode must be FMOD_SPEAKERMODE_STEREO or FMOD_SPEAKERMODE_5POINT1\n"));
            return FMOD_ERR_INVALID_PARAM;
        }
    }

    bool    usefinallimiting    = false;
    float   lfecutoff           = 0.0f;

    if (extradata)
    {
        NeuralExtraData *neuraldata = (NeuralExtraData *)extradata;

        usefinallimiting    = neuraldata->usefinallimiting;
        lfecutoff           = neuraldata->lfecutoff;
    }

    mNeuralParameters.Init(mNeuralMode);
    mNeuralSettings.nChanConfig = mNeuralMode;
    mNeuralSettings.nSampleRate = rate;

    err = mNeuralEncoder.Init(mNeuralSettings, mNeuralParameters);
    if (err != NRLTHX_OK)
    {
        FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "DownmixNeural::init", "Neural Init returned: %d\n", err));
        return FMOD_ERR_OUTPUT_INIT;
    }

    FLOG((FMOD_DEBUG_LEVEL_LOG, __FILE__, __LINE__, "DownmixNeural::init", "done.\n"));

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
FMOD_RESULT DownmixNeural::shutdown()
{
    mNeuralEncoder.Shutdown();

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
FMOD_RESULT DownmixNeural::encode(float *inbuffer, float *outbuffer, unsigned int numsamples)
{
    int err;
    Neural_THX_Channel_Format channels_in, channels_out;
    int numiterations = numsamples / NEURAL_FRAMESIZE;

    float *in  = inbuffer;
    float *out = outbuffer;

    for (int i = 0; i < numiterations; i++)
    {
        if (mNumInputChannels == 8)
        {
            for (int j = 0; j < NEURAL_FRAMESIZE; j++)
            {
                channels_in.m_fL  [j] = in[0];  // front left
                channels_in.m_fR  [j] = in[1];  // front right
                channels_in.m_fC  [j] = in[2];  // center
                channels_in.m_fLFE[j] = in[3];  // lfe

                #ifdef PLATFORM_PS3
                /*
                    Back and sides swapped around on PS3
                */
                channels_in.m_fLs [j] = in[4];  // surround left
                channels_in.m_fRs [j] = in[5];  // surround right

                channels_in.m_fLb [j] = in[6];  // back left
                channels_in.m_fRb [j] = in[7];  // back right
                #else
                channels_in.m_fLb [j] = in[4];  // back left
                channels_in.m_fRb [j] = in[5];  // back right
                
                channels_in.m_fLs [j] = in[6];  // side left
                channels_in.m_fRs [j] = in[7];  // side right
                #endif

                in += 8;
            }
        }
        else if (mNumInputChannels == 6)
        {
            for (int j = 0; j < NEURAL_FRAMESIZE; j++)
            {
                channels_in.m_fL  [j] = in[0];  // front left
                channels_in.m_fR  [j] = in[1];  // front right
                channels_in.m_fC  [j] = in[2];  // center
                channels_in.m_fLFE[j] = in[3];  // lfe
                channels_in.m_fLs [j] = in[4];  // surround left
                channels_in.m_fRs [j] = in[5];  // surround right

                in += 6;
            }
        }

        /*
            Encode
        */
        err = mNeuralEncoder.Encode(channels_in, channels_out, mNeuralSettings, mNeuralParameters);
        if (err != NRLTHX_OK)
        {
            FLOG((FMOD_DEBUG_LEVEL_ERROR, __FILE__, __LINE__, "DownmixNeural::init", "Neural Encode returned: %d\n", err));
            return FMOD_ERR_INVALID_PARAM;
        }

        if (mNumOutputChannels == 6)
        {
            for (int j = 0; j < NEURAL_FRAMESIZE; j++)
            {
                out[0] = channels_out.m_fL  [j];  // front left
                out[1] = channels_out.m_fR  [j];  // front right
                out[2] = channels_out.m_fC  [j];  // center
                out[3] = channels_out.m_fLFE[j];  // lfe      
                out[4] = channels_out.m_fLs [j];  // surround left
                out[5] = channels_out.m_fRs [j];  // surround right
                
                out += 6;
            }
        }
        else if (mNumOutputChannels == 2)
        {
            for (int j = 0; j < NEURAL_FRAMESIZE; j++)
            {
                out[0] = channels_out.m_fL  [j];  // front left
                out[1] = channels_out.m_fR  [j];  // front right

                out += 2;
            }
        }
    }

    return FMOD_OK;
}

}

#endif //FMOD_INIT_DTS_NEURALSURROUND
