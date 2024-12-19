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
//*  File        : FreqDomain_PhaseShift.c									*//
//*  Description : Shift a frequency domain signal in phase					*//
//*  Author(s)   : Jeff Thompson											*//
//*																			*//
//*					Copyright (c) Neural Audio Corp. 2008					*//
//*																			*//
//***************************************************************************//
#include "Neural_THX_Encoders.h"
#include <stdlib.h>
#include <math.h>
#include "fmod_types.h"

//***************************************************************************//
//*                    FreqDomain_PhaseShift_INIT(...)                      *//
//***************************************************************************//
//-----
//Phase shift by -90 degrees causes the output to lag the input by 90 degrees (similar to Hilbert Transform)
//Phase shift by +90 degrees causes the output to lead the input by 90 degrees
//-----
int FreqDomain_PhaseShift_INIT(float PhaseShift_Degrees,	//Amount of phase shift (-90 degrees < x < 90 degrees)
							   unsigned int Framesize,
							   FreqDomain_PhaseShift_Struct * pPtr)
{
	float PhaseShift_Radians;

	pPtr->Prev_PhaseShift_Degrees = PhaseShift_Degrees;
	if(PhaseShift_Degrees < -90.0f) PhaseShift_Degrees = -90.0f;
	if(PhaseShift_Degrees >  90.0f) PhaseShift_Degrees =  90.0f;
	PhaseShift_Radians = (float)PI * PhaseShift_Degrees / 180.0f;
	pPtr->ShiftScalar_PositiveFreqs_Real = (float)FMOD_COS( PhaseShift_Radians );
	pPtr->ShiftScalar_PositiveFreqs_Imag = (float)FMOD_SIN( PhaseShift_Radians );

	return NRLSUR_OK;
}


//***************************************************************************//
//*                       FreqDomain_PhaseShift(...)                        *//
//***************************************************************************//
int FreqDomain_PhaseShift(float * PtrInReal,
						  float * PtrInImag,
						  float * PtrOutReal,
						  float * PtrOutImag,
						  float PhaseShift_Degrees,	//Amount of phase shift (-90 degrees < x < 90 degrees)
						  unsigned int Framesize,
						  unsigned int SampleRate,
						  FreqDomain_PhaseShift_Struct * pPtr)
{
	int n;
	float PhaseShift_Radians, RealTemp, ImagTemp, RealShiftTemp, ImagShiftTemp;
	const float *LowFreqSynthApFactors;

	if(Framesize < NUMLOWFREQBINS) return UNSUPPORTED_FRAMESIZE;

	//Use the tables appropriate with the samplerate
	switch(SampleRate){
		case SAMPLERATE_32_0:
			LowFreqSynthApFactors = LowFreqSynthApFactors_32k;
			break;
		case SAMPLERATE_44_1:
			LowFreqSynthApFactors = LowFreqSynthApFactors_44k;
			break;
		case SAMPLERATE_48_0:
			LowFreqSynthApFactors = LowFreqSynthApFactors_48k;
			break;
		default:
			return UNSUPPORTED_PARAMETER;
	}

	//If the amount of phase shift changes, re-calculate scaling constants
	if(PhaseShift_Degrees != pPtr->Prev_PhaseShift_Degrees){
		pPtr->Prev_PhaseShift_Degrees = PhaseShift_Degrees;
		if(PhaseShift_Degrees < -90.0f) PhaseShift_Degrees = -90.0f;
		if(PhaseShift_Degrees >  90.0f) PhaseShift_Degrees =  90.0f;
		PhaseShift_Radians = (float)PI * PhaseShift_Degrees / 180.0f;
		pPtr->ShiftScalar_PositiveFreqs_Real = (float)FMOD_COS( PhaseShift_Radians );
		pPtr->ShiftScalar_PositiveFreqs_Imag = (float)FMOD_SIN( PhaseShift_Radians );
	}

	//Perform the phase shift which is a complex multiplication with pre-computed scalars
	for(n = 0; n < NUMLOWFREQBINS; n++){	//Special handling of the low frequency bins since an ideal Hilbert transform is not possible
		RealTemp = PtrInReal[n];
		ImagTemp = PtrInImag[n];
		RealShiftTemp = 1.0f * (1.0f - LowFreqSynthApFactors[n]) + pPtr->ShiftScalar_PositiveFreqs_Real * LowFreqSynthApFactors[n];
		ImagShiftTemp = 0.0f * (1.0f - LowFreqSynthApFactors[n]) + pPtr->ShiftScalar_PositiveFreqs_Imag * LowFreqSynthApFactors[n];
		PtrOutReal[n] = RealTemp * RealShiftTemp - ImagTemp * ImagShiftTemp;
		PtrOutImag[n] = RealTemp * ImagShiftTemp + ImagTemp * RealShiftTemp;
	}
	for(n = NUMLOWFREQBINS; n < (int)Framesize; n++){
		RealTemp = PtrInReal[n];
		ImagTemp = PtrInImag[n];
		PtrOutReal[n] = RealTemp * pPtr->ShiftScalar_PositiveFreqs_Real - ImagTemp * pPtr->ShiftScalar_PositiveFreqs_Imag;
		PtrOutImag[n] = RealTemp * pPtr->ShiftScalar_PositiveFreqs_Imag + ImagTemp * pPtr->ShiftScalar_PositiveFreqs_Real;
	}

	return NRLSUR_OK;
}
