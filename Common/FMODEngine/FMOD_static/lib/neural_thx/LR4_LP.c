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
//*  File        : LR4_LP.c													*//
//*  Description : Linkwitz-Riley 4th order Low-Pass filter					*//
//*  Author(s)   : Jeff Thompson											*//
//*																			*//
//*					Copyright (c) Neural Audio Corp. 2005					*//
//*																			*//
//***************************************************************************//
#include "Neural_THX_Encoders.h"
#include <math.h>
#include "fmod_types.h"

//***************************************************************************//
//*                           LR4_LP_INIT(...)								*//
//***************************************************************************//
void LR4_LP_INIT(LR4_LP_Struct * pPtr)
{
	pPtr->PrevCutOff = -1.0f;
	pPtr->PrevStage00 = 0.0f;
	pPtr->PrevStage01 = 0.0f;
	pPtr->PrevStage10 = 0.0f;
	pPtr->PrevStage11 = 0.0f;
	pPtr->b0 = 0.0f;
	pPtr->b1 = 0.0f;
	pPtr->b2 = 0.0f;
	pPtr->a0 = 0.0f;
	pPtr->a1 = 0.0f;
	pPtr->PrevSampleRate = -1.0f;
}


//***************************************************************************//
//*									LR4_LP(...)								*//
//***************************************************************************//
void LR4_LP(float * PtrIn,
			float * PtrOut,
			float Cutoff,	//in Hertz
			unsigned int Framesize,
			unsigned int SampleRate,
			LR4_LP_Struct * pPtr)
{
	int n;
	float midStage = 0.0f;	// Local buffer

	//----- Note that this only works for fourth order LR filters -----//

	//----- Calculate the filter coefficients -----//
	if( Cutoff != pPtr->PrevCutOff || (float)SampleRate != pPtr->PrevSampleRate ){
		//----- Pre-warp the digital cutoff frequency to an analog equivalent -----//
		float omegac = (float)2.0 * FMOD_TAN( (float)PI*(Cutoff) / (float)SampleRate );
		float norm = (float)4.0 + omegac*(float)TWOPOWTHREEHALVES + omegac*omegac;
		norm = (float)1.0 / norm;
		pPtr->b0 = (omegac*omegac) * norm;
		pPtr->b1 = (float)2.0*pPtr->b0;
		pPtr->b2 = pPtr->b0;
		pPtr->a0 = ((float)2.0*omegac*omegac - (float)8.0) * norm;
		pPtr->a1 = ((float)4.0 - omegac*(float)TWOPOWTHREEHALVES + omegac*omegac) * norm;
		pPtr->PrevCutOff = Cutoff;
		pPtr->PrevSampleRate = (float)SampleRate;
	}

    //------------------------------------------------------------------//
    // Perform the filtering through a cascade of second order stages	//
    //------------------------------------------------------------------//
	// Stage 1
    for(n = 0; n < (int)Framesize; n++){
		midStage = PtrIn[n]
				   - pPtr->PrevStage00 * pPtr->a0
				   - pPtr->PrevStage01 * pPtr->a1;
		PtrOut[n] = midStage * pPtr->b0
					+ pPtr->PrevStage00 * pPtr->b1
					+ pPtr->PrevStage01 * pPtr->b2;
		pPtr->PrevStage01 = pPtr->PrevStage00;
		pPtr->PrevStage00 = midStage;
	}
	
	// Stage 2
    for(n = 0; n < (int)Framesize; n++){
		midStage = PtrOut[n]
				   - pPtr->PrevStage10 * pPtr->a0
				   - pPtr->PrevStage11 * pPtr->a1;
		PtrOut[n] = midStage * pPtr->b0
					+ pPtr->PrevStage10 * pPtr->b1
					+ pPtr->PrevStage11 * pPtr->b2;
		pPtr->PrevStage11 = pPtr->PrevStage10;
		pPtr->PrevStage10 = midStage;
    }
}
