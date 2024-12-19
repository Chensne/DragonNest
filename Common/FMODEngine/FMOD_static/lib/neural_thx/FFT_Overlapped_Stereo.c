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
//*  File        : FFT_Overlapped_Stereo.c									*//
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
//*                    FFT_Overlapped_Stereo_INIT(...)                      *//
//***************************************************************************//
int FFT_Overlapped_Stereo_INIT(FFT_Overlapped_Stereo_Struct * pPersistent,
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
		pPersistent->pOverlappedBuffer0[n] = 0;
		pPersistent->pOverlappedBuffer1[n] = 0;
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
//*                        FFT_Overlapped_Stereo(...)                       *//
//***************************************************************************//
int FFT_Overlapped_Stereo(float * PtrIn0,
						  float * PtrOutReal0,
						  float * PtrOutImag0,
						  float * PtrIn1,
						  float * PtrOutReal1,
						  float * PtrOutImag1,
						  unsigned int Framesize,
						  FFT_Overlapped_Stereo_Struct * pPersistent)
{
	unsigned int n;
	float Win;

	if(Framesize != HALFFFTSIZE) return UNSUPPORTED_FRAMESIZE;

	// Put input0 values into every even index as "real" data and input1 values into every odd index as "imag" data
	for(n = 0; n < HALFFFTSIZE; n++){
		Win = pPersistent->pSineWin[n];
		pPersistent->pIn[n          ].r = Win * pPersistent->pOverlappedBuffer0[n];
		pPersistent->pIn[n          ].i = Win * pPersistent->pOverlappedBuffer1[n];
		pPersistent->pIn[FFTSIZE-n-1].r = Win * PtrIn0[HALFFFTSIZE-n-1];
		pPersistent->pIn[FFTSIZE-n-1].i = Win * PtrIn1[HALFFFTSIZE-n-1];
		//Store in pOverlappedBuffer for next call
		pPersistent->pOverlappedBuffer0[n] = PtrIn0[n];
		pPersistent->pOverlappedBuffer1[n] = PtrIn1[n];
	}

	// FFT call
	THX_kiss_fft(pPersistent->state,
		pPersistent->pIn,
		pPersistent->pOut);

	// Pull the information out ...
	// Now parse through the complex output to separate the two input signals
	PtrOutReal0[0] = pPersistent->pOut[0].r;	//Real value of DC coefficient
	PtrOutImag0[0] = 0;
	PtrOutReal1[0] = pPersistent->pOut[0].i;	//Imag value of DC coefficient
	PtrOutImag1[0] = 0;
	for(n = 1; n < HALFFFTSIZE; n++){
		PtrOutReal0[n] = (float)0.5 * ( pPersistent->pOut[n].r + pPersistent->pOut[FFTSIZE-n].r);	// Real output
		PtrOutImag0[n] = (float)0.5 * ( pPersistent->pOut[n].i - pPersistent->pOut[FFTSIZE-n].i);	// Imag output
		PtrOutReal1[n] = (float)0.5 * ( pPersistent->pOut[n].i + pPersistent->pOut[FFTSIZE-n].i);	// Real output
		PtrOutImag1[n] = (float)0.5 * (-pPersistent->pOut[n].r + pPersistent->pOut[FFTSIZE-n].r);	// Imag output
	}

	return NRLSUR_OK;
}

//***************************************************************************//
//*                    IFFT_Overlapped_Stereo_INIT(...)                     *//
//***************************************************************************//
int IFFT_Overlapped_Stereo_INIT(IFFT_Overlapped_Stereo_Struct * pPersistent,
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
		pPersistent->pOverlappedBuffer0[n] = 0;
		pPersistent->pOverlappedBuffer1[n] = 0;
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
//*                      IFFT_Overlapped_Stereo(...)                        *//
//***************************************************************************//
int IFFT_Overlapped_Stereo(float * PtrInReal0,
						   float * PtrInImag0,
						   float * PtrOut0,
						   float * PtrInReal1,
						   float * PtrInImag1,
						   float * PtrOut1,
						   unsigned int Framesize,
						   IFFT_Overlapped_Stereo_Struct * pPersistent)
{
	unsigned int n;
	float Win;
	float coeff;

	if(Framesize != HALFFFTSIZE) return UNSUPPORTED_FRAMESIZE;

	//Sum X1 + j*X2
	for(n = 0; n < HALFFFTSIZE; n++){
		pPersistent->pIn[n].r =   PtrInReal0[n] - PtrInImag1[n];		//Real
		pPersistent->pIn[n].i = -(PtrInImag0[n] + PtrInReal1[n]);	//Imag
	}
	//Use the input buffers as temporary buffers
	pPersistent->pReal[HALFFFTSIZE] = 0;
	pPersistent->pImag[HALFFFTSIZE] = 0;
	for(n = 0; n < HALFFFTSIZE-1; n++){
		pPersistent->pIn[FFTSIZE-n-1].r =    PtrInReal0[n+1] + PtrInImag1[n+1];
		pPersistent->pIn[FFTSIZE-n-1].i = -(-PtrInImag0[n+1] + PtrInReal1[n+1]);
	}

	// FFT call
	THX_kiss_fft(pPersistent->state,
		pPersistent->pIn,
		pPersistent->pOut);

	coeff = (float)FFTSIZE;
	coeff = 1/coeff;
	for(n = 0; n < FFTSIZE; n++)
	{
		pPersistent->pOut[n].i = -coeff * pPersistent->pOut[n].i;
		pPersistent->pOut[n].r =  coeff * pPersistent->pOut[n].r;
	}

	for(n = 0; n < HALFFFTSIZE; n++){
		Win = pPersistent->pSineWin[n];
		PtrOut0[n] = Win * pPersistent->pOut[n].r + pPersistent->pOverlappedBuffer0[n];	// Real output
		PtrOut1[n] = Win * pPersistent->pOut[n].i + pPersistent->pOverlappedBuffer1[n];	// Real output
	}
	for(n = 0; n < HALFFFTSIZE; n++){
		Win = pPersistent->pSineWin[n];
		pPersistent->pOverlappedBuffer0[HALFFFTSIZE-n-1] = Win * pPersistent->pOut[FFTSIZE-n-1].r;
		pPersistent->pOverlappedBuffer1[HALFFFTSIZE-n-1] = Win * pPersistent->pOut[FFTSIZE-n-1].i;
	}

	return NRLSUR_OK;
}
