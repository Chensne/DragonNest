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
//*  File        : Neural_THX_722_Encode.c									*//
//*  Description : Neural Surround Downmix implementation					*//
//*  Author(s)   : Jeff Thompson											*//
//*																			*//
//*					Copyright (c) Neural Audio Corp. 2008					*//
//*																			*//
//***************************************************************************//
#include "Neural_THX_Encoders.h"
#include <stdlib.h>

//***************************************************************************//
//*                     Neural_THX_722_Encode_INIT(...)                     *//
//***************************************************************************//
int Neural_THX_722_Encode_INIT(unsigned int Framesize,
							   unsigned int ChanConfig,
							   unsigned int SampleRate,
							   Neural_THX_722_Encode_Struct * pParams)
{	
	//Forward and inverse overlapped FFT's
	FFT_Overlapped_Stereo_INIT(&pParams->FFTSourceLR,pParams->TempBuffer0,pParams->TempBuffer1,NEURAL_FRAMESIZE);
	FFT_Overlapped_INIT(&pParams->FFTSourceC,pParams->TempBuffer0,pParams->TempBuffer1,NEURAL_FRAMESIZE);
	FFT_Overlapped_Stereo_INIT(&pParams->FFTSourceLsRs,pParams->TempBuffer0,pParams->TempBuffer1,NEURAL_FRAMESIZE);
	FFT_Overlapped_Stereo_INIT(&pParams->FFTSourceLbRb,pParams->TempBuffer0,pParams->TempBuffer1,NEURAL_FRAMESIZE);
	IFFT_Overlapped_Stereo_INIT(&pParams->InvFFTDownmixLR,pParams->TempBuffer0,pParams->TempBuffer1,NEURAL_FRAMESIZE);

	//Initialize phase shifts
	FreqDomain_PhaseShift_INIT(-22.5f,NEURAL_FRAMESIZE,&pParams->PhaseShift_L);
	FreqDomain_PhaseShift_INIT(+22.5f,NEURAL_FRAMESIZE,&pParams->PhaseShift_R);
	FreqDomain_PhaseShift_INIT(-90.0f,NEURAL_FRAMESIZE,&pParams->PhaseShift_Ls);
	FreqDomain_PhaseShift_INIT(+90.0f,NEURAL_FRAMESIZE,&pParams->PhaseShift_Rs);

	LR4_LP_INIT(&pParams->LFE_LP);

	//Init final limiters
	Limiter_INIT(pParams->TempBuffer0,&pParams->FinalLimiterL);
	Limiter_INIT(pParams->TempBuffer0,&pParams->FinalLimiterR);

	return NRLSUR_OK;
}


//***************************************************************************//
//*                         Neural_THX_722_Encode(...)                      *//
//***************************************************************************//
int Neural_THX_722_Encode(float * PtrInL,
						  float * PtrInR,
						  float * PtrInC,
						  float * PtrInLFE,
						  float * PtrInLs,
						  float * PtrInRs,
						  float * PtrInLb,
						  float * PtrInRb,
						  float * PtrOutL,
						  float * PtrOutR,
						  THX_bool UseFinalLimiting,
						  float LFE_Cutoff,
						  unsigned int Framesize,
						  unsigned int ChanConfig,
						  unsigned int SampleRate,
						  Neural_THX_722_Encode_Struct * pParams)
{
	//Neural Surround Downmix

	//***********************************************************
	//***** Low-pass filter the LFE channel (if selected)	*****
	//***********************************************************
	if(LFE_Cutoff > 40.0f && LFE_Cutoff < 200.0f){
		LR4_LP(PtrInLFE,PtrInLFE,LFE_Cutoff,NEURAL_FRAMESIZE,SampleRate,&pParams->LFE_LP);
	}

	//First sum LFE channel into Center channel
	Add2(PtrInC,PtrInLFE,PtrInC,NEURAL_FRAMESIZE);

	//****************************************************************************************************
	//***** Begin downmix by transforming all signals to frequency domain and performing phase shift *****
	//****************************************************************************************************
	//Convert all input channels to freq domain
	//L-R channels
	FFT_Overlapped_Stereo(PtrInL,pParams->Input_RealL,pParams->Input_ImagL,
						  PtrInR,pParams->Input_RealR,pParams->Input_ImagR,
						  NEURAL_FRAMESIZE,&pParams->FFTSourceLR);
	//C channel
	FFT_Overlapped(PtrInC,pParams->Input_RealC,pParams->Input_ImagC,
				   NEURAL_FRAMESIZE,&pParams->FFTSourceC);
	//Ls-Rs channels
	FFT_Overlapped_Stereo(PtrInLs,pParams->Input_RealLs,pParams->Input_ImagLs,
						  PtrInRs,pParams->Input_RealRs,pParams->Input_ImagRs,
						  NEURAL_FRAMESIZE,&pParams->FFTSourceLsRs);
	//Lb-Rb channels
	FFT_Overlapped_Stereo(PtrInLb,pParams->Input_RealLb,pParams->Input_ImagLb,
						  PtrInRb,pParams->Input_RealRb,pParams->Input_ImagRb,
						  NEURAL_FRAMESIZE,&pParams->FFTSourceLbRb);
	
	//Phase shift L channel
	FreqDomain_PhaseShift(pParams->Input_RealL,pParams->Input_ImagL,
					      pParams->Input_RealL,pParams->Input_ImagL,
						  -22.5f,NEURAL_FRAMESIZE,SampleRate,&pParams->PhaseShift_L);
	//Phase shift R channel
	FreqDomain_PhaseShift(pParams->Input_RealR,pParams->Input_ImagR,
					      pParams->Input_RealR,pParams->Input_ImagR,
						  +22.5f,NEURAL_FRAMESIZE,SampleRate,&pParams->PhaseShift_R);
	//Phase shift Ls channel
	FreqDomain_PhaseShift(pParams->Input_RealLs,pParams->Input_ImagLs,
					      pParams->Input_RealLs,pParams->Input_ImagLs,
						  -90.0f,NEURAL_FRAMESIZE,SampleRate,&pParams->PhaseShift_Ls);
	//Phase shift Rs channel
	FreqDomain_PhaseShift(pParams->Input_RealRs,pParams->Input_ImagRs,
					      pParams->Input_RealRs,pParams->Input_ImagRs,
						  +90.0f,NEURAL_FRAMESIZE,SampleRate,&pParams->PhaseShift_Rs);
	//Phase shift Lb channel
	FreqDomain_PhaseShift(pParams->Input_RealLb,pParams->Input_ImagLb,
					      pParams->Input_RealLb,pParams->Input_ImagLb,
						  -90.0f,NEURAL_FRAMESIZE,SampleRate,&pParams->PhaseShift_Ls);
	//Phase shift Rb channel
	FreqDomain_PhaseShift(pParams->Input_RealRb,pParams->Input_ImagRb,
					      pParams->Input_RealRb,pParams->Input_ImagRb,
						  +90.0f,NEURAL_FRAMESIZE,SampleRate,&pParams->PhaseShift_Rs);


	
	//Scale center and lfe channels by -3dB
	ScaleArray(pParams->Input_RealC,(float)MINUS_3DB,NEURAL_FRAMESIZE);
	ScaleArray(pParams->Input_ImagC,(float)MINUS_3DB,NEURAL_FRAMESIZE);
		
	//Add center and lfe channels to left/right phase shifted channels
	//L channel
	Add2(pParams->Input_RealL,pParams->Input_RealC,pParams->Downmix_RealL,NEURAL_FRAMESIZE);
	Add2(pParams->Input_ImagL,pParams->Input_ImagC,pParams->Downmix_ImagL,NEURAL_FRAMESIZE);
	//R channel
	Add2(pParams->Input_RealR,pParams->Input_RealC,pParams->Downmix_RealR,NEURAL_FRAMESIZE);
	Add2(pParams->Input_ImagR,pParams->Input_ImagC,pParams->Downmix_ImagR,NEURAL_FRAMESIZE);

		
	//***********************************************************
	//***** Combine the front channels with the surrounds 	*****
	//***********************************************************
	ScaleArray(pParams->Input_RealLs,(float)SUR_CROSSSHARENORM,NEURAL_FRAMESIZE);
	ScaleArray(pParams->Input_ImagLs,(float)SUR_CROSSSHARENORM,NEURAL_FRAMESIZE);
	ScaleArray(pParams->Input_RealRs,(float)SUR_CROSSSHARENORM,NEURAL_FRAMESIZE);
	ScaleArray(pParams->Input_ImagRs,(float)SUR_CROSSSHARENORM,NEURAL_FRAMESIZE);
	//----- Back channels -----
	ScaleArray(pParams->Input_RealLb,(float)BACK_CROSSSHARENORM,NEURAL_FRAMESIZE);
	ScaleArray(pParams->Input_ImagLb,(float)BACK_CROSSSHARENORM,NEURAL_FRAMESIZE);
	ScaleArray(pParams->Input_RealRb,(float)BACK_CROSSSHARENORM,NEURAL_FRAMESIZE);
	ScaleArray(pParams->Input_ImagRb,(float)BACK_CROSSSHARENORM,NEURAL_FRAMESIZE);


	//----- Surround channels -----
	Add2(pParams->Downmix_RealL,pParams->Input_RealLs,pParams->Downmix_RealL,NEURAL_FRAMESIZE);
	Add2(pParams->Downmix_ImagL,pParams->Input_ImagLs,pParams->Downmix_ImagL,NEURAL_FRAMESIZE);

	Add2(pParams->Downmix_RealR,pParams->Input_RealRs,pParams->Downmix_RealR,NEURAL_FRAMESIZE);
	Add2(pParams->Downmix_ImagR,pParams->Input_ImagRs,pParams->Downmix_ImagR,NEURAL_FRAMESIZE);

	CopyArray(pParams->Input_RealLs,pParams->TempBuffer0,NEURAL_FRAMESIZE);
	ScaleArray(pParams->TempBuffer0,-SUR_CROSSSHARECONSTANT,Framesize);
	Add2(pParams->Downmix_RealR,pParams->TempBuffer0,pParams->Downmix_RealR,NEURAL_FRAMESIZE);
	CopyArray(pParams->Input_ImagLs,pParams->TempBuffer0,NEURAL_FRAMESIZE);
	ScaleArray(pParams->TempBuffer0,-SUR_CROSSSHARECONSTANT,Framesize);
	Add2(pParams->Downmix_ImagR,pParams->TempBuffer0,pParams->Downmix_ImagR,NEURAL_FRAMESIZE);

	CopyArray(pParams->Input_RealRs,pParams->TempBuffer0,NEURAL_FRAMESIZE);
	ScaleArray(pParams->TempBuffer0,-SUR_CROSSSHARECONSTANT,Framesize);
	Add2(pParams->Downmix_RealL,pParams->TempBuffer0,pParams->Downmix_RealL,NEURAL_FRAMESIZE);
	CopyArray(pParams->Input_ImagRs,pParams->TempBuffer0,NEURAL_FRAMESIZE);
	ScaleArray(pParams->TempBuffer0,-SUR_CROSSSHARECONSTANT,Framesize);
	Add2(pParams->Downmix_ImagL,pParams->TempBuffer0,pParams->Downmix_ImagL,NEURAL_FRAMESIZE);

	//----- Back channels -----
	Add2(pParams->Downmix_RealL,pParams->Input_RealLb,pParams->Downmix_RealL,NEURAL_FRAMESIZE);
	Add2(pParams->Downmix_ImagL,pParams->Input_ImagLb,pParams->Downmix_ImagL,NEURAL_FRAMESIZE);

	Add2(pParams->Downmix_RealR,pParams->Input_RealRb,pParams->Downmix_RealR,NEURAL_FRAMESIZE);
	Add2(pParams->Downmix_ImagR,pParams->Input_ImagRb,pParams->Downmix_ImagR,NEURAL_FRAMESIZE);

	CopyArray(pParams->Input_RealLb,pParams->TempBuffer0,NEURAL_FRAMESIZE);
	ScaleArray(pParams->TempBuffer0,-BACK_CROSSSHARECONSTANT,Framesize);
	Add2(pParams->Downmix_RealR,pParams->TempBuffer0,pParams->Downmix_RealR,NEURAL_FRAMESIZE);
	CopyArray(pParams->Input_ImagLb,pParams->TempBuffer0,NEURAL_FRAMESIZE);
	ScaleArray(pParams->TempBuffer0,-BACK_CROSSSHARECONSTANT,Framesize);
	Add2(pParams->Downmix_ImagR,pParams->TempBuffer0,pParams->Downmix_ImagR,NEURAL_FRAMESIZE);

	CopyArray(pParams->Input_RealRb,pParams->TempBuffer0,NEURAL_FRAMESIZE);
	ScaleArray(pParams->TempBuffer0,-BACK_CROSSSHARECONSTANT,Framesize);
	Add2(pParams->Downmix_RealL,pParams->TempBuffer0,pParams->Downmix_RealL,NEURAL_FRAMESIZE);
	CopyArray(pParams->Input_ImagRb,pParams->TempBuffer0,NEURAL_FRAMESIZE);
	ScaleArray(pParams->TempBuffer0,-BACK_CROSSSHARECONSTANT,Framesize);
	Add2(pParams->Downmix_ImagL,pParams->TempBuffer0,pParams->Downmix_ImagL,NEURAL_FRAMESIZE);


	//***********************************************************
	//***** Inverse filterbank back to time signals         *****
	//***********************************************************
	IFFT_Overlapped_Stereo(pParams->Downmix_RealL,pParams->Downmix_ImagL,PtrOutL,
						   pParams->Downmix_RealR,pParams->Downmix_ImagR,PtrOutR,
						   NEURAL_FRAMESIZE,&pParams->InvFFTDownmixLR);

	if(UseFinalLimiting){
		//Perform final limiting on the output signals
		Limiter(PtrOutL,PtrOutL,
				(float)AUDIO_VAL_MAX,
				-6.0f,-0.1f,
				0.0f,500.0f,
				NEURAL_FRAMESIZE,SampleRate,&pParams->FinalLimiterL);
		Limiter(PtrOutR,PtrOutR,
				(float)AUDIO_VAL_MAX,
				-6.0f,-0.1f,
				0.0f,500.0f,
				NEURAL_FRAMESIZE,SampleRate,&pParams->FinalLimiterR);
	}


	//That's all folks
	SaturateArray(PtrOutL,(float)AUDIO_VAL_MAX,NEURAL_FRAMESIZE);
	SaturateArray(PtrOutR,(float)AUDIO_VAL_MAX,NEURAL_FRAMESIZE);

	return NRLSUR_OK;
}
