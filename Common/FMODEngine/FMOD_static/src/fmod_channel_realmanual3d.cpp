#include "fmod_settings.h"

#include "fmod_3d.h"
#include "fmod_channeli.h"
#include "fmod_channel_realmanual3d.h"
#include "fmod_soundi.h"
#include "fmod_speakermap.h"
#include "fmod_systemi.h"

#include <string.h>

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
ChannelRealManual3D::ChannelRealManual3D()
{
#ifdef FMOD_SUPPORT_SOFTWARE
    mAngleToListener = 0.0f;
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
FMOD_RESULT ChannelRealManual3D::alloc()
{
#ifdef FMOD_SUPPORT_SOFTWARE
    mAngleToListener = 0.0f;
#endif

    return ChannelReal::alloc();
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
FMOD_RESULT ChannelRealManual3D::set2DFreqVolumePanFor3D()
{
#ifdef FMOD_SUPPORT_3DSOUND
    float lrpan[FMOD_CHANNEL_MAXINPUTCHANNELS] = { 0 }, fbpan[FMOD_CHANNEL_MAXINPUTCHANNELS] = { 0 };
    int subchannel, numsubchannels;
    float lrpan_base = 0.0f, fbpan_base = 0.0f, angle = 0.0f;
    bool monospread = false;

    if (!(mMode & FMOD_3D))
    {
        return FMOD_OK;
    }
    if (mSound)
    {
        if (mSound->mSubSampleParent)
        {
            numsubchannels = mSound->mSubSampleParent->mChannels;
        }
        else
        {
            numsubchannels = mSound->mChannels;
        }
    }
    else if (mDSP)
    {
        numsubchannels = mDSP->mDescription.channels;
    }
    else
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    /*
        CALCULATE 3D PAN
    */
    if (mSystem->mNumListeners == 1)
    {
        FMOD_VECTOR currdiff;
        FMOD_VECTOR front, right;
        FMOD_VECTOR position;
        float       distance;

        position = mParent->mPosition3D;

        /*
            Distance between emitter and listener this frame 
        */
        if (mMode & FMOD_3D_HEADRELATIVE)
        {
            front.x = 0.0f;
            front.y = 0.0f;
            front.z = 1.0f;
            right.x = 1.0f;
            right.y = 0.0f;
            right.z = 0.0f;

            currdiff = position;
        }
        else
        {
            front    = mSystem->mListener[0].mFront;
            right    = mSystem->mListener[0].mRight;
            FMOD_Vector_Subtract(&position, &mSystem->mListener[0].mPosition, &currdiff);        
        }

        if (mSystem->mFlags & FMOD_INIT_3D_RIGHTHANDED)
        {
            front.z = -front.z;
            currdiff.z = -currdiff.z;
        }

        /*
            Normalize difference vector
        */
        distance = FMOD_Vector_GetLength(&currdiff);

        if (distance <= 0)
        {
            currdiff.x = 0;
            currdiff.y = 0;
            currdiff.z = 0;
        }
        else
        {
            /*
                Normalize
            */
            currdiff.x /= (float)distance;
            currdiff.y /= (float)distance;
            currdiff.z /= (float)distance;
        }

        /*
            Base L/R and F/B pan, and the base direction of the sound
        */
        lrpan_base = FMOD_Vector_DotProduct(&currdiff, &right);
        fbpan_base = FMOD_Vector_DotProduct(&currdiff, &front);

        if (lrpan_base > 1.01f || lrpan_base < -1.01f)
        {
            return FMOD_ERR_INVALID_VECTOR;
        }
        if (fbpan_base > 1.01f || fbpan_base < -1.01f)
        {
            return FMOD_ERR_INVALID_VECTOR;
        }

        /*
            Clamp (floating point accuracy issues on some platforms)
        */
        lrpan_base = lrpan_base >  1.0f ? 1.0f  : lrpan_base;
        lrpan_base = lrpan_base < -1.0f ? -1.0f : lrpan_base;
        fbpan_base = fbpan_base >  1.0f ? 1.0f  : fbpan_base;
        fbpan_base = fbpan_base < -1.0f ? -1.0f : fbpan_base;

        angle = FMOD_AngleSort_GetValue(lrpan_base, fbpan_base);
        
#ifdef FMOD_SUPPORT_SOFTWARE
        mAngleToListener = FMOD_ACOS(fbpan_base) * (180.0f / FMOD_PI);
#endif

        /*
            Spread the pan field for each part of a sound using the spread angle.
            If a stereo sound has a spread of 90 degrees, the pan will be rotated 45 degrees left for left channel, 45 degrees right for right channel.
        */         
        float radian_spread = mParent->mSpread * FMOD_PI / 180.0f;

        if (mParent->mSpread != 0.0f && numsubchannels == 1)
        {
            monospread = true;
            numsubchannels = 2;
        }

        for (subchannel = 0; subchannel < numsubchannels; subchannel++)
        {
            if (mParent->mSpread == 0.0f || numsubchannels == 1)
            {
                lrpan[subchannel] = lrpan_base;
                fbpan[subchannel] = fbpan_base;
            }
            else
            {
                //
                // Map input channel to speaker position index in spread
                // We don't have access to the sound's channel format so make a guess
                // For spread pos -1.0 for leftmost, 1.0 for rightmost
                //
                float spread_pos = 0.0f;
                switch(numsubchannels)
                {
                    case 4: // Could be either L/R/LR/RR or L/R/C/B so last 2 subs get centre
                        switch(subchannel)
                        {
                            case 0: spread_pos = -1.0f; break; // L
                            case 1: spread_pos =  1.0f; break; // R
                            case 2: spread_pos =  0.0f; break; // unknown
                            case 3: spread_pos =  0.0f; break; // unknown
                        }
                        break;

                    case 6:
                        switch(subchannel)
                        {
                            case 0: spread_pos = -0.5f; break; // L
                            case 1: spread_pos =  0.5f; break; // R
                            case 2: spread_pos =  0.0f; break; // C
                            case 3: spread_pos =  0.0f; break;  // LFE
                            case 4: spread_pos = -1.0f; break; // LR
                            case 5: spread_pos =  1.0f; break; // RR
                        }
                        break;
                    case 8:
                        switch(subchannel)
                        {
                            case 0: spread_pos = -0.333f;   break; // L
                            case 1: spread_pos =  0.333f;   break; // R
                            case 2: spread_pos =  0.0f;     break; // C
                            case 3: spread_pos =  0.0f;     break; // LFE
                            case 4: spread_pos = -1.0f;     break; // LR
                            case 5: spread_pos =  1.0f;     break; // RR
                            case 6: spread_pos = -0.667f;   break; // LS
                            case 7: spread_pos =  0.667f;   break; // RS
                        }
                        break;
                    default:
                        spread_pos = -1.0f + 2.0f * (float)subchannel / (float)(numsubchannels - 1);
                        break;
                }
                float theta = -0.5f * spread_pos * radian_spread; 
                float cost  = FMOD_COS(theta);
                float sint  = FMOD_SIN(theta);

                lrpan[subchannel] = (cost * lrpan_base) - (sint * fbpan_base);
                fbpan[subchannel] = (cost * fbpan_base) + (sint * lrpan_base);   
            }
        }
    }

    /*
        CALL THE DRIVER TO SET THE VOLUME, FREQUENCY AND PAN
    */

    setFrequency(mParent->mFrequency);

    if (mParent->mSpeakerMode != FMOD_SPEAKERMODE_MONO &&
        mParent->mSpeakerMode != FMOD_SPEAKERMODE_RAW)
    {
        float speakerlevel[FMOD_SPEAKER_MAX][FMOD_CHANNEL_MAXINPUTCHANNELS];  /* 8 * 16 * sizeof(float) = 512 bytes */
        int numspeakers = 0, numrealspeakers = 0;
        int subchannel;

        FMOD_memset(speakerlevel, 0, sizeof(float) * FMOD_SPEAKER_MAX * FMOD_CHANNEL_MAXINPUTCHANNELS);

        for (subchannel = 0; subchannel < numsubchannels; subchannel++)
        {
            speakerlevel[FMOD_SPEAKER_LOW_FREQUENCY][subchannel] = mSystem->mSpeaker[FMOD_SPEAKER_LOW_FREQUENCY].mActive ? mParent->mSpeakerLFE : 0;
        }

        if (mParent->mSpeakerMode == FMOD_SPEAKERMODE_STEREO)
        {
            numspeakers = 2;
            numrealspeakers = 2;
        }
        else if (mParent->mSpeakerMode == FMOD_SPEAKERMODE_PROLOGIC)
        {
            numspeakers = 5;
            numrealspeakers = 6;
        }
        else if (mParent->mSpeakerMode == FMOD_SPEAKERMODE_QUAD)
        {
            numspeakers = 4;
            numrealspeakers = 4;
        }
        else if (mParent->mSpeakerMode == FMOD_SPEAKERMODE_SURROUND)
        {
            numspeakers = 5;
            numrealspeakers = 5;
        }
        else if (mParent->mSpeakerMode == FMOD_SPEAKERMODE_5POINT1)
        {
            numspeakers = 5;
            numrealspeakers = 6;
        }
        else if (mParent->mSpeakerMode == FMOD_SPEAKERMODE_7POINT1)
        {
            numspeakers = 7;
            numrealspeakers = 8;
        }

        if (mParent->m3DPanLevel > 0.0f)   /* 0.0 = pure 2d.  Less than this has some 3d component.  1 = pure 3d. */
        {
            for (subchannel = 0; subchannel < numsubchannels; subchannel++)
            {
                if (fbpan[subchannel] == 0 && lrpan[subchannel] == 0)
                {               
                    float level = 1.4142135623730950488016887242097f / numspeakers;
                
                    speakerlevel[FMOD_SPEAKER_FRONT_LEFT][subchannel]    = mSystem->mSpeaker[FMOD_SPEAKER_FRONT_LEFT].mActive ? level : 0;
                    speakerlevel[FMOD_SPEAKER_FRONT_RIGHT][subchannel]   = mSystem->mSpeaker[FMOD_SPEAKER_FRONT_RIGHT].mActive ? level : 0;
                    speakerlevel[FMOD_SPEAKER_FRONT_CENTER][subchannel]  = mSystem->mSpeaker[FMOD_SPEAKER_FRONT_CENTER].mActive ? level : 0;
                    speakerlevel[FMOD_SPEAKER_BACK_LEFT][subchannel]     = mSystem->mSpeaker[FMOD_SPEAKER_BACK_LEFT].mActive ? level : 0;
                    speakerlevel[FMOD_SPEAKER_BACK_RIGHT][subchannel]    = mSystem->mSpeaker[FMOD_SPEAKER_BACK_RIGHT].mActive ? level : 0;
                    speakerlevel[FMOD_SPEAKER_SIDE_LEFT][subchannel]     = mSystem->mSpeaker[FMOD_SPEAKER_SIDE_LEFT].mActive ? level : 0;
                    speakerlevel[FMOD_SPEAKER_SIDE_RIGHT][subchannel]    = mSystem->mSpeaker[FMOD_SPEAKER_SIDE_RIGHT].mActive ? level : 0;
                }
                else
                {
                    int speaker;
                    float subchannelangle;

                    if (lrpan[subchannel] == lrpan_base && fbpan[subchannel] == fbpan_base)
                    {
                        subchannelangle = angle;
                    }
                    else
                    {
                        subchannelangle = FMOD_AngleSort_GetValue(lrpan[subchannel], fbpan[subchannel]);
                    }

                    if (!mSystem->mSpeakerList[0])                                /* No speakers are active. */
                    {
                        
                    }
                    else if (!mSystem->mSpeakerList[1])   /* Only 1 speaker is active. */
                    {
                        speakerlevel[mSystem->mSpeakerList[0]->mSpeaker][subchannel] = 1.0f;
                    }
                    else
                    {
                        for (speaker = 1; speaker < numspeakers + 1; speaker++)
                        {
                            FMOD_SPEAKERCONFIG *spkA, *spkB;

                            spkA = mSystem->mSpeakerList[speaker-1];
                            if (mSystem->mSpeakerList[speaker])
                            {
                                spkB = mSystem->mSpeakerList[speaker];
                            }
                            else
                            {
                                spkB = mSystem->mSpeakerList[0];
                            }

                            if (spkA->mXZAngle == spkB->mXZAngle)   /* 2 speakers are at the same location so skip to the next pair */
                            {
                                continue;
                            }
    
                            if (FMOD_AngleSort_IsClockwise(spkA->mXZAngle, subchannelangle, spkB->mXZAngle))
                            {
                                /*
                                    VBAP only makes sense if source direction is in the vector space spanned by positive gains.
                                    So if the speaker angles are 180 deg apart then the best we can do is resolve the lateral
                                    (side-to-side) component of the source's direction.
                                */
                                if (spkA->mPairUseVBAP)
                                {
                                    /*
                                        Use Vector-Based Amplitude Panning by soving the equations:
                                        1. [x_src] = [x_spkA  x_spkB] * [gainA']
                                           [y_src]   [y_spkA  y_spkB]   [gainB']
                                        2. gainA = k * gainA'
                                        3. gainB = k * gainB'
                                        4. gainA ^ 2 + gainB ^ 2 = 1
                                        Note: there is no need to evaluate the determinant when inverting the speaker matrix
                                              in Eq.1 since gains will we normalised for constant power.
                                    */
                                    float gainA = lrpan[subchannel] * spkB->mXZNormal.z - fbpan[subchannel] * spkB->mXZNormal.x;
                                    float gainB = fbpan[subchannel] * spkA->mXZNormal.x - lrpan[subchannel] * spkA->mXZNormal.z;
                                    float norm = spkA->mPairVBAPSign / FMOD_SQRT(gainA * gainA + gainB * gainB);
                                    speakerlevel[spkA->mSpeaker][subchannel] = gainA * norm;
                                    speakerlevel[spkB->mSpeaker][subchannel] = gainB * norm;
                                }
                                else
                                {
                                    /*
                                        Resolve lateral component using the dot product, solving the following equations:
                                        1. u_src = u_spkA * gainA + u_spkB * gainB
                                        2. gainA ^ 2 + gainB ^ 2 = 1
                                        where 'u_' denotes the lateral component of the source and speaker unit direction vectors
                                    */
                                    float lateral = (lrpan[subchannel] * spkA->mXZNormal.x + fbpan[subchannel] * spkA->mXZNormal.z);
                                    float d = FMOD_SQRT(2.0f - lateral * lateral);
                                    speakerlevel[spkA->mSpeaker][subchannel] = (d + lateral) * 0.5f;
                                    speakerlevel[spkB->mSpeaker][subchannel] = (d - lateral) * 0.5f;
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }

        if (numsubchannels > 1)
        {
            int speaker;                       
            float levels[DSP_MAXLEVELS_OUT * DSP_MAXLEVELS_IN] = { 0 };
            int numinputlevels;

 			if (mParent->m3DPanLevel < 1.0f)
            {
                float _3dmix = mParent->m3DPanLevel, _2dmix = 1.0f - mParent->m3DPanLevel;

                if (mParent->mLastPanMode == FMOD_CHANNEL_PANMODE_PAN || mParent->mLastPanMode == FMOD_CHANNEL_PANMODE_SPEAKERMIX)
                {
                    float fl = 0.0f, fr = 0.0f, c = 0.0f, lfe = 0.0f, bl = 0.0f, br = 0.0f, sl = 0.0f, sr = 0.0f;

                    if (mParent->mLastPanMode == FMOD_CHANNEL_PANMODE_PAN)
                    {
                        float pan = (mParent->mPan + 1.0f) / 2.0f;
                        float l, r;

                        if (pan <= 0.5f)
                        {
                            l = 1.0f;
                            r = pan * 2.0f;
                        }
                        else
                        {
                            l = (1.0f - pan) * 2.0f;
                            r = 1.0f;
                        }
        
                        if (numsubchannels == 2 && !(mSound->mDefaultChannelMask & SPEAKER_ALLMONO))
                        {
                            fl  = l; fr  = r; c   = 0; lfe = 0; bl  = 0; br  = 0; sl  = 0; sr  = 0;
                        }
                        else
                        {
                            fl  = l; fr  = r; c   = 1.0f; lfe = 1.0f; bl = l; br = r; sl = l; sr = r;
                        }
                    }
                    else if (mParent->mLastPanMode == FMOD_CHANNEL_PANMODE_SPEAKERMIX)
                    {
                        fl  = mParent->mSpeakerFL;
                        fr  = mParent->mSpeakerFR;
                        c   = mParent->mSpeakerC;
                        lfe = mParent->mSpeakerLFE;
                        bl  = mParent->mSpeakerBL;
                        br  = mParent->mSpeakerBR;
                        sl  = mParent->mSpeakerSL;
                        sr  = mParent->mSpeakerSR;
                    }

                    FMOD_SPEAKERMAPTYPE speakermap = FMOD_SPEAKERMAPTYPE_DEFAULT;

                    if (mSound)
                    {
                        if (mSound->mDefaultChannelMask & SPEAKER_ALLMONO)
                        {
                            speakermap = FMOD_SPEAKERMAPTYPE_ALLMONO;
                        }
                        else if (mSound->mDefaultChannelMask & SPEAKER_ALLSTEREO)
                        {
                            speakermap = FMOD_SPEAKERMAPTYPE_ALLSTEREO;
                        }
                        else if (mSound->mDefaultChannelMask & SPEAKER_PROTOOLS)
                        {
                            speakermap = FMOD_SPEAKERMAPTYPE_51_PROTOOLS;
                        }
                    }

                    DSPI::calculateSpeakerLevels(fl, 
                             fr, 
                             c, 
                             lfe, 
                             bl, 
                             br, 
                             sl, 
                             sr, 
                             mParent->mSpeakerMode,
                             numsubchannels,
                             speakermap,
                             levels, 
                             &numinputlevels);

                    for (subchannel = 0; subchannel < numsubchannels; subchannel++)
                    {                
                        speakerlevel[FMOD_SPEAKER_FRONT_LEFT][subchannel]   = (speakerlevel[FMOD_SPEAKER_FRONT_LEFT][subchannel]   * _3dmix) + (levels[(FMOD_SPEAKER_FRONT_LEFT   * numinputlevels) + subchannel] * _2dmix);
                        speakerlevel[FMOD_SPEAKER_FRONT_RIGHT][subchannel]  = (speakerlevel[FMOD_SPEAKER_FRONT_RIGHT][subchannel]  * _3dmix) + (levels[(FMOD_SPEAKER_FRONT_RIGHT  * numinputlevels) + subchannel] * _2dmix);
                        speakerlevel[FMOD_SPEAKER_FRONT_CENTER][subchannel] = (speakerlevel[FMOD_SPEAKER_FRONT_CENTER][subchannel] * _3dmix) + (levels[(FMOD_SPEAKER_FRONT_CENTER * numinputlevels) + subchannel] * _2dmix);
                        speakerlevel[FMOD_SPEAKER_BACK_LEFT][subchannel]    = (speakerlevel[FMOD_SPEAKER_BACK_LEFT][subchannel]    * _3dmix) + (levels[(FMOD_SPEAKER_BACK_LEFT    * numinputlevels) + subchannel] * _2dmix);
                        speakerlevel[FMOD_SPEAKER_BACK_RIGHT][subchannel]   = (speakerlevel[FMOD_SPEAKER_BACK_RIGHT][subchannel]   * _3dmix) + (levels[(FMOD_SPEAKER_BACK_RIGHT   * numinputlevels) + subchannel] * _2dmix);
                        speakerlevel[FMOD_SPEAKER_SIDE_LEFT][subchannel]    = (speakerlevel[FMOD_SPEAKER_SIDE_LEFT][subchannel]    * _3dmix) + (levels[(FMOD_SPEAKER_SIDE_LEFT    * numinputlevels) + subchannel] * _2dmix);
                        speakerlevel[FMOD_SPEAKER_SIDE_RIGHT][subchannel]   = (speakerlevel[FMOD_SPEAKER_SIDE_RIGHT][subchannel]   * _3dmix) + (levels[(FMOD_SPEAKER_SIDE_RIGHT   * numinputlevels) + subchannel] * _2dmix);
                    }
                }
                else if (mParent->mLastPanMode == FMOD_CHANNEL_PANMODE_SPEAKERLEVELS && mParent->mLevels)
                {
                    numinputlevels = mSystem->mMaxInputChannels;

                    for (subchannel = 0; subchannel < numsubchannels; subchannel++)
                    {
                        speakerlevel[FMOD_SPEAKER_FRONT_LEFT][subchannel]  = (speakerlevel[FMOD_SPEAKER_FRONT_LEFT][subchannel]  * _3dmix) + (mParent->mLevels[(FMOD_SPEAKER_FRONT_LEFT  * numinputlevels) + subchannel] * _2dmix);
                        speakerlevel[FMOD_SPEAKER_FRONT_RIGHT][subchannel] = (speakerlevel[FMOD_SPEAKER_FRONT_RIGHT][subchannel] * _3dmix) + (mParent->mLevels[(FMOD_SPEAKER_FRONT_RIGHT * numinputlevels) + subchannel] * _2dmix);
                        
                        if (numspeakers > 2)
                        {
                            speakerlevel[FMOD_SPEAKER_FRONT_CENTER][subchannel] = (speakerlevel[FMOD_SPEAKER_FRONT_CENTER][subchannel] * _3dmix) + (mParent->mLevels[(FMOD_SPEAKER_FRONT_CENTER * numinputlevels) + subchannel] * _2dmix);
                            
                            if (numspeakers > 3)
                            {
                                speakerlevel[FMOD_SPEAKER_BACK_LEFT][subchannel]  = (speakerlevel[FMOD_SPEAKER_BACK_LEFT][subchannel]  * _3dmix) + (mParent->mLevels[(FMOD_SPEAKER_BACK_LEFT  * numinputlevels) + subchannel] * _2dmix);
                                speakerlevel[FMOD_SPEAKER_BACK_RIGHT][subchannel] = (speakerlevel[FMOD_SPEAKER_BACK_RIGHT][subchannel] * _3dmix) + (mParent->mLevels[(FMOD_SPEAKER_BACK_RIGHT * numinputlevels) + subchannel] * _2dmix);
                                
                                if (numspeakers > 5)
                                {
                                    speakerlevel[FMOD_SPEAKER_SIDE_LEFT][subchannel]  = (speakerlevel[FMOD_SPEAKER_SIDE_LEFT][subchannel]  * _3dmix) + (mParent->mLevels[(FMOD_SPEAKER_SIDE_LEFT  * numinputlevels) + subchannel] * _2dmix);
                                    speakerlevel[FMOD_SPEAKER_SIDE_RIGHT][subchannel] = (speakerlevel[FMOD_SPEAKER_SIDE_RIGHT][subchannel] * _3dmix) + (mParent->mLevels[(FMOD_SPEAKER_SIDE_RIGHT * numinputlevels) + subchannel] * _2dmix);
                                }
                            }
                        }
                    }
                }
            }

            if (mParent->mSpeakerMode == FMOD_SPEAKERMODE_PROLOGIC && (mSound->mMode & FMOD_SOFTWARE)) /* Downmix logic. */
            {
                for (subchannel = 0; subchannel < numsubchannels; subchannel++)
                {                
                    DSPI::calculateSpeakerLevels(speakerlevel[FMOD_SPEAKER_FRONT_LEFT][subchannel], 
                                                 speakerlevel[FMOD_SPEAKER_FRONT_RIGHT][subchannel], 
                                                 speakerlevel[FMOD_SPEAKER_FRONT_CENTER][subchannel], 
                                                 speakerlevel[FMOD_SPEAKER_LOW_FREQUENCY][subchannel], 
                                                 speakerlevel[FMOD_SPEAKER_BACK_LEFT][subchannel], 
                                                 speakerlevel[FMOD_SPEAKER_BACK_RIGHT][subchannel], 
                                                 speakerlevel[FMOD_SPEAKER_SIDE_LEFT][subchannel], 
                                                 speakerlevel[FMOD_SPEAKER_SIDE_RIGHT][subchannel], 
                                                 mParent->mSpeakerMode,
                                                 1,
                                                 FMOD_SPEAKERMAPTYPE_DEFAULT,
                                                 levels, 
                                                 &numinputlevels);

                    speakerlevel[FMOD_SPEAKER_FRONT_LEFT][subchannel]   = levels[0];
                    speakerlevel[FMOD_SPEAKER_FRONT_RIGHT][subchannel]  = levels[1];
                    speakerlevel[FMOD_SPEAKER_FRONT_CENTER][subchannel] = 0;
                    speakerlevel[FMOD_SPEAKER_BACK_LEFT][subchannel]    = 0;
                    speakerlevel[FMOD_SPEAKER_BACK_RIGHT][subchannel]   = 0;
                    speakerlevel[FMOD_SPEAKER_SIDE_LEFT][subchannel]    = 0;
                    speakerlevel[FMOD_SPEAKER_SIDE_RIGHT][subchannel]   = 0;
                }
            }

            if (monospread)
            {
                numsubchannels = 1; /* Put it back to normal */
            }

            for (speaker = 0; speaker < numrealspeakers; speaker++)
            {                
                if (monospread)
                {
                    speakerlevel[speaker][0] = (speakerlevel[speaker][0] + speakerlevel[speaker][1]) / 2.0f;
                }
                setSpeakerLevels(speaker, speakerlevel[speaker], numsubchannels);
            }
        }
        else
        {
            if (mParent->m3DPanLevel < 1.0f)
            {
                float _3dmix = mParent->m3DPanLevel, _2dmix = 1.0f - mParent->m3DPanLevel;

                if (mParent->mLastPanMode == FMOD_CHANNEL_PANMODE_PAN)
                {
                    float pan = (mParent->mPan + 1.0f) / 2.0f;
                    float l = FMOD_SQRT(1.0f - pan);
                    float r = FMOD_SQRT(pan);
    
                    speakerlevel[FMOD_SPEAKER_FRONT_LEFT][0]   = (speakerlevel[FMOD_SPEAKER_FRONT_LEFT][0]   * _3dmix) + (l * _2dmix);
                    speakerlevel[FMOD_SPEAKER_FRONT_RIGHT][0]  = (speakerlevel[FMOD_SPEAKER_FRONT_RIGHT][0]  * _3dmix) + (r * _2dmix);
                    speakerlevel[FMOD_SPEAKER_FRONT_CENTER][0] = (speakerlevel[FMOD_SPEAKER_FRONT_CENTER][0] * _3dmix);
                    speakerlevel[FMOD_SPEAKER_BACK_LEFT][0]    = (speakerlevel[FMOD_SPEAKER_BACK_LEFT][0]    * _3dmix);
                    speakerlevel[FMOD_SPEAKER_BACK_RIGHT][0]   = (speakerlevel[FMOD_SPEAKER_BACK_RIGHT][0]   * _3dmix);
                    speakerlevel[FMOD_SPEAKER_SIDE_LEFT][0]    = (speakerlevel[FMOD_SPEAKER_SIDE_LEFT][0]    * _3dmix);
                    speakerlevel[FMOD_SPEAKER_SIDE_RIGHT][0]   = (speakerlevel[FMOD_SPEAKER_SIDE_RIGHT][0]   * _3dmix);
                }
                else if (mParent->mLastPanMode == FMOD_CHANNEL_PANMODE_SPEAKERMIX)
                {
                    speakerlevel[FMOD_SPEAKER_FRONT_LEFT][0]   = (speakerlevel[FMOD_SPEAKER_FRONT_LEFT][0]   * _3dmix) + (mParent->mSpeakerFL * _2dmix);
                    speakerlevel[FMOD_SPEAKER_FRONT_RIGHT][0]  = (speakerlevel[FMOD_SPEAKER_FRONT_RIGHT][0]  * _3dmix) + (mParent->mSpeakerFR * _2dmix);
                    speakerlevel[FMOD_SPEAKER_FRONT_CENTER][0] = (speakerlevel[FMOD_SPEAKER_FRONT_CENTER][0] * _3dmix) + (mParent->mSpeakerC  * _2dmix);
                    speakerlevel[FMOD_SPEAKER_BACK_LEFT][0]    = (speakerlevel[FMOD_SPEAKER_BACK_LEFT][0]    * _3dmix) + (mParent->mSpeakerBL * _2dmix);
                    speakerlevel[FMOD_SPEAKER_BACK_RIGHT][0]   = (speakerlevel[FMOD_SPEAKER_BACK_RIGHT][0]   * _3dmix) + (mParent->mSpeakerBR * _2dmix);
                    speakerlevel[FMOD_SPEAKER_SIDE_LEFT][0]    = (speakerlevel[FMOD_SPEAKER_SIDE_LEFT][0]    * _3dmix) + (mParent->mSpeakerSL * _2dmix);
                    speakerlevel[FMOD_SPEAKER_SIDE_RIGHT][0]   = (speakerlevel[FMOD_SPEAKER_SIDE_RIGHT][0]   * _3dmix) + (mParent->mSpeakerSR * _2dmix);
                }
                else if (mParent->mLastPanMode == FMOD_CHANNEL_PANMODE_SPEAKERLEVELS && mParent->mLevels)
                {
                    speakerlevel[FMOD_SPEAKER_FRONT_LEFT][0]  = (speakerlevel[FMOD_SPEAKER_FRONT_LEFT][0]  * _3dmix) + (mParent->mLevels[mSystem->mMaxInputChannels * FMOD_SPEAKER_FRONT_LEFT]  * _2dmix);
                    speakerlevel[FMOD_SPEAKER_FRONT_RIGHT][0] = (speakerlevel[FMOD_SPEAKER_FRONT_RIGHT][0] * _3dmix) + (mParent->mLevels[mSystem->mMaxInputChannels * FMOD_SPEAKER_FRONT_RIGHT] * _2dmix);

                    if (numspeakers > 2)
                    {
                        speakerlevel[FMOD_SPEAKER_FRONT_CENTER][0] = (speakerlevel[FMOD_SPEAKER_FRONT_CENTER][0] * _3dmix) + (mParent->mLevels[mSystem->mMaxInputChannels * FMOD_SPEAKER_FRONT_CENTER] * _2dmix);
                    
                        if (numspeakers > 3)
                        {
                            speakerlevel[FMOD_SPEAKER_BACK_LEFT][0]  = (speakerlevel[FMOD_SPEAKER_BACK_LEFT][0]  * _3dmix) + (mParent->mLevels[mSystem->mMaxInputChannels * FMOD_SPEAKER_BACK_LEFT]  * _2dmix);
                            speakerlevel[FMOD_SPEAKER_BACK_RIGHT][0] = (speakerlevel[FMOD_SPEAKER_BACK_RIGHT][0] * _3dmix) + (mParent->mLevels[mSystem->mMaxInputChannels * FMOD_SPEAKER_BACK_RIGHT] * _2dmix);
                            
                            if (numspeakers > 5)
                            {
                                speakerlevel[FMOD_SPEAKER_SIDE_LEFT][0]  = (speakerlevel[FMOD_SPEAKER_SIDE_LEFT][0]  * _3dmix) + (mParent->mLevels[mSystem->mMaxInputChannels * FMOD_SPEAKER_SIDE_LEFT]  * _2dmix);
                                speakerlevel[FMOD_SPEAKER_SIDE_RIGHT][0] = (speakerlevel[FMOD_SPEAKER_SIDE_RIGHT][0] * _3dmix) + (mParent->mLevels[mSystem->mMaxInputChannels * FMOD_SPEAKER_SIDE_RIGHT] * _2dmix);
                            }
                        }
                    }
                }
            }

            setSpeakerMix(speakerlevel[FMOD_SPEAKER_FRONT_LEFT][0],
                          speakerlevel[FMOD_SPEAKER_FRONT_RIGHT][0], 
                          speakerlevel[FMOD_SPEAKER_FRONT_CENTER][0], 
                          mParent->mSpeakerLFE,
                          speakerlevel[FMOD_SPEAKER_BACK_LEFT][0],   
                          speakerlevel[FMOD_SPEAKER_BACK_RIGHT][0],  
                          speakerlevel[FMOD_SPEAKER_SIDE_LEFT][0],   
                          speakerlevel[FMOD_SPEAKER_SIDE_RIGHT][0]);
        }
    }

    setVolume(mParent->mVolume);
#endif

    return FMOD_OK;
}


}

