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
//*  File        : PeakConverter.c											*//
//*  Description : Peak converter with rise time / fall time				*//
//*  Author(s)   : Jeff Thompson											*//
//*																			*//
//*					Copyright (c) Neural Audio Corp. 2008					*//
//*																			*//
//***************************************************************************//
#include "Neural_THX_Encoders.h"
#include <math.h>

//***************************************************************************//
//*                          PeakConverter_INIT                             *//
//***************************************************************************//
int PeakConverter_INIT(float RiseTime,	//in msec
					   float FallTime,	//in msec
					   float MinValue,
					   float MaxValue,
					   unsigned int SampleRate,
					   PeakConverter_Struct * pParams)
{
	if(SampleRate == 0)
		return UNSUPPORTED_SAMPLERATE;
	if(MaxValue-MinValue <= 0.0f)
		return UNSUPPORTED_PARAMETER;

	if(RiseTime > 0)
		pParams->RiseSlew = (MaxValue-MinValue) / (RiseTime * 0.001f * (float)SampleRate);
	else
		pParams->RiseSlew = (MaxValue-MinValue);
	pParams->RiseTime = RiseTime;

	if(FallTime > 0)
		pParams->FallSlew = -(MaxValue-MinValue) / (FallTime * 0.001f * (float)SampleRate);
	else
		pParams->FallSlew = -(MaxValue-MinValue);
	pParams->FallTime = FallTime;

	pParams->MinValue = MinValue;
	pParams->MaxValue = MaxValue;
	pParams->PrevValue = 0.0f;
	pParams->SampleRate = SampleRate;

	return NRLSUR_OK;
}

//***************************************************************************//
//*	                          PeakConverter(...)                            *//
//***************************************************************************//
int PeakConverter(float * PtrIn,
				  float * PtrOut,
				  float RiseTime,	//in msec
				  float FallTime,	//in msec
				  float MinValue,
				  float MaxValue,
				  unsigned int Framesize,
				  unsigned int SampleRate,
				  PeakConverter_Struct * pParams)
{
	int n;
	int RetVal;
	float fTemp;

	if(SampleRate == 0)
		return UNSUPPORTED_SAMPLERATE;
	if(RiseTime < 0.0f)
		RiseTime = 0.0f;
	if(RiseTime > 5000.0f)
		RiseTime = 5000.0f;
	if(FallTime < 0.0f)
		FallTime = 0.0f;
	if(FallTime > 5000.0f)
		FallTime = 5000.0f;

	//If parameters have changed, then re-initialize
	if( RiseTime != pParams->RiseTime ||
		FallTime != pParams->FallTime ||
		MinValue != pParams->MinValue ||
		MaxValue != pParams->MaxValue ||
		SampleRate != pParams->SampleRate ) {
		RetVal = PeakConverter_INIT(RiseTime,FallTime,MinValue,MaxValue,SampleRate,pParams);
		if(RetVal != NRLSUR_OK)
			return RetVal;
	}

    //-----------------------------------------------------------//
    // Perform peak converter processing                         //
    //-----------------------------------------------------------//
    for(n = 0; n < (int)Framesize; n++){
		fTemp = PtrIn[n] - pParams->PrevValue;
		if(fTemp > pParams->RiseSlew)
			fTemp = pParams->RiseSlew;
		if(fTemp < pParams->FallSlew)
			fTemp = pParams->FallSlew;
		PtrOut[n] = pParams->PrevValue + fTemp;
		pParams->PrevValue = PtrOut[n];
    }

	return NRLSUR_OK;
}
