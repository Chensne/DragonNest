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
//*  File        : Delay.h													*//
//*  Description : Fixed delay												*//
//*  Author(s)   : Jeff Thompson											*//
//*																			*//
//*					Copyright (c) Neural Audio Corp. 2008					*//
//*																			*//
//***************************************************************************//
#include "Neural_THX_Encoders.h"
#include <stdlib.h>

//***************************************************************************//
//*                           Delay_INIT(...)								*//
//***************************************************************************//
int Delay_INIT(unsigned int DelayAmount,
			   Delay_Struct * pPtr)
{
	int n;

	if(DelayAmount != NEURAL_FRAMESIZE) return UNSUPPORTED_FRAMESIZE;

	pPtr->Pntr = 0;
	pPtr->DelayAmount = DelayAmount;
	for(n = 0; n < NEURAL_FRAMESIZE+1; n++){
		pPtr->Buffer[n] = 0;
	}

	return NRLSUR_OK;
}


//***************************************************************************//
//*                           Delay(...)									*//
//***************************************************************************//
int Delay(float * PtrIn,
		  float * PtrOut,
		  unsigned int Framesize,
		  Delay_Struct * pPtr)
{
	int n, Pntr, DelayAmount;
	float * Buffer;

	if(Framesize != NEURAL_FRAMESIZE) return UNSUPPORTED_FRAMESIZE;

	Buffer = pPtr->Buffer;
	Pntr = pPtr->Pntr;
	DelayAmount = pPtr->DelayAmount;

    for(n = 0; n < NEURAL_FRAMESIZE; n++){
		Buffer[Pntr] = PtrIn[n];
		Pntr = ( (Pntr+1) >= (DelayAmount+1) ) ? 0 : (Pntr+1);
		PtrOut[n] = Buffer[Pntr];
    }

	pPtr->Pntr = Pntr;

	return NRLSUR_OK;
}
