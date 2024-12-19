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
//*  Project     : Neural Audio - THX										*//
//*  File        : Neural_THX_725_Encode.c									*//
//*  Description : Downmixer for Neural - THX gaming mode from 7.1 to 5.1	*//
//*  Author(s)   : Jeff Thompson											*//
//*																			*//
//*					Copyright (c) Neural Audio Corp. 2008					*//
//*																			*//
//***************************************************************************//
#include "Neural_THX_Encoders.h"
#include <stdlib.h>
#include <math.h>

//***************************************************************************//
//*                     Neural_THX_725_Encode_INIT(...)                     *//
//***************************************************************************//
int Neural_THX_725_Encode_INIT(unsigned int Framesize,
							   unsigned int ChanConfig,
							   unsigned int SampleRate,
							   Neural_THX_725_Encode_Struct * pParams)
{
	//Initialize the FFT transforms
	FFT_Overlapped_Stereo_INIT(&pParams->Transform_LsRs,pParams->TempBuffer0,pParams->TempBuffer1,NEURAL_FRAMESIZE);
	FFT_Overlapped_Stereo_INIT(&pParams->Transform_LbRb,pParams->TempBuffer0,pParams->TempBuffer1,NEURAL_FRAMESIZE);
	IFFT_Overlapped_Stereo_INIT(&pParams->InvTransform_LsRs,pParams->TempBuffer0,pParams->TempBuffer1,NEURAL_FRAMESIZE);

	//Initialize phase shifts
	FreqDomain_PhaseShift_INIT(+90.0f,NEURAL_FRAMESIZE,&pParams->PhaseShift_Pos);
	FreqDomain_PhaseShift_INIT(-90.0f,NEURAL_FRAMESIZE,&pParams->PhaseShift_Neg);

#ifdef COMPENSATION_FRONT_DELAY
	//Initialize delays
	if(Delay_INIT(NEURAL_FRAMESIZE,&pParams->DelayChanL)   < 0) return INIT_ERROR;
	if(Delay_INIT(NEURAL_FRAMESIZE,&pParams->DelayChanR)   < 0) return INIT_ERROR;
	if(Delay_INIT(NEURAL_FRAMESIZE,&pParams->DelayChanC)   < 0) return INIT_ERROR;
	if(Delay_INIT(NEURAL_FRAMESIZE,&pParams->DelayChanLFE) < 0) return INIT_ERROR;
#endif

	//Init final limiters
	Limiter_INIT(pParams->TempBuffer0,&pParams->FinalLimiterLs);
	Limiter_INIT(pParams->TempBuffer0,&pParams->FinalLimiterRs);

	return NRLSUR_OK;
}


//***************************************************************************//
//*                         Neural_THX_725_Encode(...)                      *//
//***************************************************************************//
int Neural_THX_725_Encode(float * PtrInL,		//7.1 In
						  float * PtrInR,
						  float * PtrInC,
						  float * PtrInLFE,
						  float * PtrInLs,
						  float * PtrInRs,
						  float * PtrInLb,
						  float * PtrInRb,
						  float * PtrOutL,		//5.1 Out
						  float * PtrOutR,
						  float * PtrOutC,
						  float * PtrOutLFE,
						  float * PtrOutLs,
						  float * PtrOutRs,
						  THX_bool UseFinalLimiting,
						  unsigned int Framesize,
						  unsigned int ChanConfig,
						  unsigned int SampleRate,
						  Neural_THX_725_Encode_Struct * pParams)
{
	//Neural Audio - THX Gaming Downmix
	//Downmix 7.1 format to 5.1 format
	float * PhaseShift_RealLb_Pos = &pParams->TempBuffer0[0*NEURAL_FRAMESIZE];
	float * PhaseShift_ImagLb_Pos = &pParams->TempBuffer0[1*NEURAL_FRAMESIZE];
	float * PhaseShift_RealRb_Pos = &pParams->TempBuffer0[2*NEURAL_FRAMESIZE];
	float * PhaseShift_ImagRb_Pos = &pParams->TempBuffer0[3*NEURAL_FRAMESIZE];
	float * PhaseShift_RealLb_Neg = &pParams->TempBuffer1[0*NEURAL_FRAMESIZE];
	float * PhaseShift_ImagLb_Neg = &pParams->TempBuffer1[1*NEURAL_FRAMESIZE];
	float * PhaseShift_RealRb_Neg = &pParams->TempBuffer1[2*NEURAL_FRAMESIZE];
	float * PhaseShift_ImagRb_Neg = &pParams->TempBuffer1[3*NEURAL_FRAMESIZE];

	//***********************************************************
	//***** Surround channel downmixing                     *****
	//***********************************************************
	//Convert all input channels to freq domain
	FFT_Overlapped_Stereo(PtrInLs,pParams->Input_RealLs,pParams->Input_ImagLs,
						  PtrInRs,pParams->Input_RealRs,pParams->Input_ImagRs,
						  NEURAL_FRAMESIZE,&pParams->Transform_LsRs);
	FFT_Overlapped_Stereo(PtrInLb,pParams->Input_RealLb,pParams->Input_ImagLb,
						  PtrInRb,pParams->Input_RealRb,pParams->Input_ImagRb,
						  NEURAL_FRAMESIZE,&pParams->Transform_LbRb);

	//Phase shift Lb channel
	FreqDomain_PhaseShift(pParams->Input_RealLb,pParams->Input_ImagLb,
					      PhaseShift_RealLb_Pos,PhaseShift_ImagLb_Pos,
						  +90.0f,NEURAL_FRAMESIZE,SampleRate,&pParams->PhaseShift_Pos);
	FreqDomain_PhaseShift(pParams->Input_RealLb,pParams->Input_ImagLb,
					      PhaseShift_RealLb_Neg,PhaseShift_ImagLb_Neg,
						  -90.0f,NEURAL_FRAMESIZE,SampleRate,&pParams->PhaseShift_Neg);
	//Phase shift Rb channel
	FreqDomain_PhaseShift(pParams->Input_RealRb,pParams->Input_ImagRb,
					      PhaseShift_RealRb_Pos,PhaseShift_ImagRb_Pos,
						  +90.0f,NEURAL_FRAMESIZE,SampleRate,&pParams->PhaseShift_Pos);
	FreqDomain_PhaseShift(pParams->Input_RealRb,pParams->Input_ImagRb,
					      PhaseShift_RealRb_Neg,PhaseShift_ImagRb_Neg,
						  -90.0f,NEURAL_FRAMESIZE,SampleRate,&pParams->PhaseShift_Neg);

	//-----
	//Generate Left Surround
	//-----
	//Real
	Add2(pParams->Input_RealLs,PhaseShift_RealLb_Pos,pParams->Dmix_RealLs,NEURAL_FRAMESIZE);
	ScaleArray(PhaseShift_RealRb_Pos,BACK_CROSS_SHARE,NEURAL_FRAMESIZE);
	Add2(pParams->Dmix_RealLs,PhaseShift_RealRb_Pos,pParams->Dmix_RealLs,NEURAL_FRAMESIZE);
	//Imag
	Add2(pParams->Input_ImagLs,PhaseShift_ImagLb_Pos,pParams->Dmix_ImagLs,NEURAL_FRAMESIZE);
	ScaleArray(PhaseShift_ImagRb_Pos,BACK_CROSS_SHARE,NEURAL_FRAMESIZE);
	Add2(pParams->Dmix_ImagLs,PhaseShift_ImagRb_Pos,pParams->Dmix_ImagLs,NEURAL_FRAMESIZE);

	//-----
	//Generate Right Surround
	//-----
	//Real
	Add2(pParams->Input_RealRs,PhaseShift_RealRb_Neg,pParams->Dmix_RealRs,NEURAL_FRAMESIZE);
	ScaleArray(PhaseShift_RealLb_Neg,BACK_CROSS_SHARE,NEURAL_FRAMESIZE);
	Add2(pParams->Dmix_RealRs,PhaseShift_RealLb_Neg,pParams->Dmix_RealRs,NEURAL_FRAMESIZE);
	//Imag
	Add2(pParams->Input_ImagRs,PhaseShift_ImagRb_Neg,pParams->Dmix_ImagRs,NEURAL_FRAMESIZE);
	ScaleArray(PhaseShift_ImagLb_Neg,BACK_CROSS_SHARE,NEURAL_FRAMESIZE);
	Add2(pParams->Dmix_ImagRs,PhaseShift_ImagLb_Neg,pParams->Dmix_ImagRs,NEURAL_FRAMESIZE);


	//***********************************************************
	//***** Convert downmixed channels back to time domain  *****
	//***********************************************************
	IFFT_Overlapped_Stereo(pParams->Dmix_RealLs,pParams->Dmix_ImagLs,PtrOutLs,
						   pParams->Dmix_RealRs,pParams->Dmix_ImagRs,PtrOutRs,
						   NEURAL_FRAMESIZE,&pParams->InvTransform_LsRs);

	if(UseFinalLimiting){
		//Perform final limiting on the output signals
		Limiter(PtrOutLs,PtrOutLs,
				(float)AUDIO_VAL_MAX,
				-6.0f,-0.1f,
				0.0f,500.0f,
				NEURAL_FRAMESIZE,SampleRate,&pParams->FinalLimiterLs);
		Limiter(PtrOutRs,PtrOutRs,
				(float)AUDIO_VAL_MAX,
				-6.0f,-0.1f,
				0.0f,500.0f,
				NEURAL_FRAMESIZE,SampleRate,&pParams->FinalLimiterRs);
	}

#ifdef COMPENSATION_FRONT_DELAY
	//***********************************************************
	//***** Front channel handling                          *****
	//***********************************************************
	//Delay Front channels to compensate for Surround downmixing

	//-----
	//Left Front
	//-----
	Delay(PtrInL, PtrOutL,NEURAL_FRAMESIZE,&pParams->DelayChanL);
	//-----
	//Right Front
	//-----
	Delay(PtrInR, PtrOutR,NEURAL_FRAMESIZE,&pParams->DelayChanR);
	//-----
	//Center
	//-----
	Delay(PtrInC, PtrOutC,NEURAL_FRAMESIZE,&pParams->DelayChanC);
	//-----
	//LFE
	//-----
	Delay(PtrInLFE, PtrOutLFE,NEURAL_FRAMESIZE,&pParams->DelayChanLFE);
#else
	//Else just copy the input buffer to output buffers with no delay
	CopyArray(PtrInL,  PtrOutL,  NEURAL_FRAMESIZE);
	CopyArray(PtrInR,  PtrOutR,  NEURAL_FRAMESIZE);
	CopyArray(PtrInC,  PtrOutC,  NEURAL_FRAMESIZE);
	CopyArray(PtrInLFE,PtrOutLFE,NEURAL_FRAMESIZE);
#endif

	//***********************************************************
	//***** That's all folks!                               *****
	//***********************************************************
	SaturateArray(PtrOutL,  (float)AUDIO_VAL_MAX,NEURAL_FRAMESIZE);
	SaturateArray(PtrOutR,  (float)AUDIO_VAL_MAX,NEURAL_FRAMESIZE);
	SaturateArray(PtrOutC,  (float)AUDIO_VAL_MAX,NEURAL_FRAMESIZE);
	SaturateArray(PtrOutLFE,(float)AUDIO_VAL_MAX,NEURAL_FRAMESIZE);
	SaturateArray(PtrOutLs, (float)AUDIO_VAL_MAX,NEURAL_FRAMESIZE);
	SaturateArray(PtrOutRs, (float)AUDIO_VAL_MAX,NEURAL_FRAMESIZE);

	return NRLSUR_OK;
}
