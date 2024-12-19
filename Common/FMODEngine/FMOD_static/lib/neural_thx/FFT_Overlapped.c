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
//*  Project     : Neural Surround											*//
//*  File        : FFT_Overlapped.c											*//
//*  Description : A windowed and overlapped FFT and IFFT					*//
//*  Author(s)   : Jeff Thompson											*//
//*																			*//
//*					Copyright (c) Neural Audio Corp. 2008					*//
//*																			*//
//***************************************************************************//
#include "Neural_THX_Encoders.h"
#include <stdlib.h>
#include <math.h>

//***************************************************************************//
//*                       FFT_Overlapped_INIT(...)                          *//
//***************************************************************************//
int FFT_Overlapped_INIT(FFT_Overlapped_Struct * pPersistent,
						float * pTempBuffer0,	//Size must be 2*Framesize
						float * pTempBuffer1,	//Size must be 2*Framesize
						unsigned int Framesize)
{
	int n, j;
	size_t memNeeded;

	if(Framesize != HALFFFTSIZE) return UNSUPPORTED_FRAMESIZE;

	pPersistent->pSineWin = (float*)GetSineWindow();

	pPersistent->pReal = pTempBuffer0;
	pPersistent->pImag = pTempBuffer1;

	for(n = 0; n < HALFFFTSIZE; n++){
		pPersistent->pOverlappedBuffer[n] = 0;
	}

	// Let's do something that will speed everything up
	memNeeded = FFT_MEM_CHUNK;
	pPersistent->state		= THX_kiss_fft_alloc(FFTSIZE,0,fft_buff,&memNeeded);
	// Lets make some memory for our buffers
	for(j = 0; j < FFTSIZE; j++)
	{
		pPersistent->pIn[j].r  = 0.0f;
		pPersistent->pIn[j].i  = 0.0f;
		pPersistent->pOut[j].r = 0.0f;
		pPersistent->pOut[j].i = 0.0f;
	}

	return NRLSUR_OK;
}

//***************************************************************************//
//*                           FFT_Overlapped(...)                           *//
//***************************************************************************//
int FFT_Overlapped(float * PtrIn,
				   float * PtrOutReal,
				   float * PtrOutImag,
				   unsigned int Framesize,
				   FFT_Overlapped_Struct * pPersistent)
{
	unsigned int n;
	float Win;

	if(Framesize != HALFFFTSIZE) return UNSUPPORTED_FRAMESIZE;

	// Put input values into every index as "real" data
	for(n = 0; n < HALFFFTSIZE; n++){
		Win = pPersistent->pSineWin[n];
		pPersistent->pIn[n          ].r = Win * pPersistent->pOverlappedBuffer[n];
		pPersistent->pIn[n          ].i = 0.0f;
		pPersistent->pIn[FFTSIZE-n-1].r = Win * PtrIn[HALFFFTSIZE-n-1];
		pPersistent->pIn[FFTSIZE-n-1].i = 0.0f;
		//Store in pOverlappedBuffer for next call
		pPersistent->pOverlappedBuffer[n] = PtrIn[n];
	}

	// FFT call
	THX_kiss_fft(pPersistent->state,
		pPersistent->pIn,
		pPersistent->pOut);
	
	// Now parse through the complex output to separate the input signal
	for(n = 0; n < HALFFFTSIZE; n++){
		PtrOutReal[n] = pPersistent->pOut[n].r;	// Real output
		PtrOutImag[n] = pPersistent->pOut[n].i;	// Imag output
	}

	return NRLSUR_OK;
}

//***************************************************************************//
//*                       IFFT_Overlapped_INIT(...)                         *//
//***************************************************************************//
int IFFT_Overlapped_INIT(IFFT_Overlapped_Struct * pPersistent,
						 float * pTempBuffer0,	//Size must be 2*Framesize
						 float * pTempBuffer1,	//Size must be 2*Framesize
						 unsigned int Framesize)
{
	int n, j;
	size_t memNeeded;

	if(Framesize != HALFFFTSIZE) return UNSUPPORTED_FRAMESIZE;

	pPersistent->pSineWin = (float*)GetSineWindow();

	pPersistent->pReal  = pTempBuffer0;
	pPersistent->pImag  = pTempBuffer1;

	for(n = 0; n < HALFFFTSIZE; n++){
		pPersistent->pOverlappedBuffer[n] = 0;
	}

	// Let's do something that will speed everything up
	memNeeded = FFT_MEM_CHUNK;
	pPersistent->state		= THX_kiss_fft_alloc(FFTSIZE,0,fft_buff,&memNeeded);
	// Lets make some memory for our buffers
	for(j = 0; j < FFTSIZE; j++)
	{
		pPersistent->pIn[j].r  = 0.0f;
		pPersistent->pIn[j].i  = 0.0f;
		pPersistent->pOut[j].r = 0.0f;
		pPersistent->pOut[j].i = 0.0f;
	}

	return NRLSUR_OK;
}

//***************************************************************************//
//*                         IFFT_Overlapped(...)                            *//
//***************************************************************************//
int IFFT_Overlapped(float * PtrInReal,
					float * PtrInImag,
					float * PtrOut,
					unsigned int Framesize,
					IFFT_Overlapped_Struct * pPersistent)
{
	unsigned int n;
	float Win;
	float coeff;

	if(Framesize != HALFFFTSIZE) return UNSUPPORTED_FRAMESIZE;

	for(n = 0; n < HALFFFTSIZE; n++){
		pPersistent->pIn[n].r = PtrInReal[n];	//Real
		pPersistent->pIn[n].i = PtrInImag[n];	//Imag
	}
	//Use the input buffers as temporary buffers
	pPersistent->pIn[HALFFFTSIZE].r = 0;
	pPersistent->pIn[HALFFFTSIZE].i = 0;
	for(n = 0; n < HALFFFTSIZE-1; n++){
		pPersistent->pIn[FFTSIZE-n-1].r =  pPersistent->pIn[n+1].r;
		pPersistent->pIn[FFTSIZE-n-1].i = -pPersistent->pIn[n+1].i;
	}

	// IFFT call
	coeff=(float)FFTSIZE;
	coeff=1/coeff;

	for(n=0;n<FFTSIZE;n++)
	{
		pPersistent->pIn[n].i = -pPersistent->pIn[n].i;		
	}
		
	THX_kiss_fft(pPersistent->state,
		pPersistent->pIn,
		pPersistent->pOut);

	for(n=0;n < (unsigned int)FFTSIZE;n++)
	{
		pPersistent->pIn[n].i = -coeff*pPersistent->pIn[n].i;		
		pPersistent->pIn[n].r = coeff*pPersistent->pIn[n].r;		
	}

	// Store all even indexes to output 0 and all odd indexes to output 1
	for(n = 0; n < HALFFFTSIZE; n++){
		Win = pPersistent->pSineWin[n];
		PtrOut[n] = Win * pPersistent->pIn[n].r + pPersistent->pOverlappedBuffer[n];	// Real output
	}
	for(n = 0; n < HALFFFTSIZE; n++){
		Win = pPersistent->pSineWin[n];
		pPersistent->pOverlappedBuffer[HALFFFTSIZE-n-1] = Win * pPersistent->pIn[FFTSIZE-n-1].r;
	}

	return NRLSUR_OK;
}
