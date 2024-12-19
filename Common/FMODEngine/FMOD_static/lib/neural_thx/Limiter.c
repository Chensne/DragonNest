//					        _____________
//					      _________________
//					    _____/~\___________/\_
//					  ______/\_/\_________/\____
//					 ______/\___/\_______/\______
//					 _____/\_____/\_____/\_______
//					 ____/\_______/\___/\________
//					  __/\_________/\_/\________
//					   /\___________/~\_______
//					     ___________________
//					        _____________
//
//***************************************************************************//
//*																			*//
//*  Project     : Neural Audio												*//
//*  File        : Limiter.c												*//
//*  Description : Limiter													*//
//*  Author(s)   : Jeff Thompson											*//
//*																			*//
//*					Copyright (c) Neural Audio Corp. 2008					*//
//*																			*//
//***************************************************************************//
#include "Neural_THX_Encoders.h"
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "fmod_types.h"

//***************************************************************************//
//*                           Limiter_INIT(...)                             *//
//***************************************************************************//
int Limiter_INIT(float * TempBuffer,
				 Limiter_Struct * pPtr)
{
	pPtr->TempBuffer = TempBuffer;
	PeakConverter_INIT(0.0f,0.0f,0.0f,0.0f,0,&pPtr->EnvSmoother);
	return NRLSUR_OK;
}

//***************************************************************************//
//*                             Compressor(...)                             *//
//***************************************************************************//
int Limiter(float * AudioIn,
			float * AudioOut,
			float MaxAudioValue,
			float Knee,		//in dB referenced to zero dB
			float Ceiling,	//in dB referenced to zero dB
			float RiseTime,	//in msec
			float FallTime,	//in msec
			unsigned int Framesize,
			unsigned int SampleRate,
			Limiter_Struct * pPtr)
{
	int n;
	float fTemp;

	if(Knee > Ceiling) Knee = Ceiling;

	CopyArray(AudioIn,pPtr->TempBuffer,Framesize);

	//First normalize audio
	if(MaxAudioValue < (float)FLT_EPSILON)
		MaxAudioValue = (float)FLT_EPSILON;
	fTemp = 1.0f / MaxAudioValue;
	ScaleArray(pPtr->TempBuffer,fTemp,Framesize);

	//First convert the audio to the dB domain
	AbsValue(pPtr->TempBuffer,Framesize);
	MinClip(pPtr->TempBuffer,(float)FLT_EPSILON,Framesize);
	MaxClip(pPtr->TempBuffer,1.0f,Framesize);
	FastLog10_ARRAY(pPtr->TempBuffer,pPtr->TempBuffer,Framesize);
	ScaleArray(pPtr->TempBuffer,20.0f,Framesize);

	//Smooth the envelope
	PeakConverter(pPtr->TempBuffer,
				  pPtr->TempBuffer,
				  RiseTime,	//in msec
				  FallTime,	//in msec
				  -60.0f,
				  0.0f,
				  Framesize,
				  SampleRate,
				  &pPtr->EnvSmoother);

	//Apply the knee and ceiling parameters
	for(n = 0; n < (int)Framesize; n++){
		fTemp = pPtr->TempBuffer[n];
		pPtr->TempBuffer[n] = 0.0f;
		if(fTemp > Knee){
			pPtr->TempBuffer[n] = 0.5f * (Knee - fTemp);
			fTemp = fTemp + pPtr->TempBuffer[n];
		}
		if(fTemp > Ceiling){
			pPtr->TempBuffer[n] += Ceiling - fTemp;
			fTemp = fTemp + pPtr->TempBuffer[n];
		}
	}

	//Convert back to linear domain
	MaxClip(pPtr->TempBuffer,0.0f,Framesize);
	ScaleArray(pPtr->TempBuffer,0.05f,Framesize);
	FastPow10_ARRAY(pPtr->TempBuffer,pPtr->TempBuffer,Framesize);

	//Limit the audio signal
	Multiply2(AudioIn,pPtr->TempBuffer,AudioOut,Framesize);
	Ceiling = FMOD_POW(10.0f,0.05f*Ceiling) * MaxAudioValue;
	SaturateArray(AudioOut,Ceiling,Framesize);
	
	return NRLSUR_OK;
}
