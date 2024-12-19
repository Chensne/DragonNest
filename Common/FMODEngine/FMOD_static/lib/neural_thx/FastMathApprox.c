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
//*  File        : FastMathApprox.c											*//
//*  Description : Fast math approximations									*//
//*  Author(s)   : Jeff Thompson											*//
//*																			*//
//*					Copyright (c) Neural Audio Corp. 2008					*//
//*																			*//
//***************************************************************************//
#include "Neural_THX_Encoders.h"
#include <math.h>
#include <float.h>
#include "fmod_types.h"

//***************************************************************************//
//*                            FastMag(...)                                 *//
//***************************************************************************//
#ifdef USE_MAG_APPROX
float FastMag(float r, float i)
{
	return FastSqrt(r*r + i*i);
}
#else
float FastMag(float r, float i)
{
	return (float)sqrtf(r*r + i*i);
}
#endif

//***************************************************************************//
//*                         FastMag_ARRAY(...)                              *//
//***************************************************************************//
#ifdef USE_MAG_APPROX
void FastMag_ARRAY(float * PtrInRe,
				   float * PtrInIm,
				   float * PtrOut,
				   unsigned int Framesize)
{
	int n;
	for(n = 0; n < (int)Framesize; n++){
		PtrOut[n] = PtrInRe[n]*PtrInRe[n] + PtrInIm[n]*PtrInIm[n];
	}
	FastSqrt_ARRAY(PtrOut,PtrOut,Framesize);
}
#else
void FastMag_ARRAY(float * PtrInRe,
				   float * PtrInIm,
				   float * PtrOut,
				   unsigned int Framesize)
{
	int n;
	for(n = 0; n < (int)Framesize; n++){
		PtrOut[n] = (float)sqrtf(PtrInRe[n]*PtrInRe[n] + PtrInIm[n]*PtrInIm[n]);
	}
}
#endif

//***************************************************************************//
//*                          FastPhase_INIT(...)                            *//
//***************************************************************************//
void FastPhase_INIT(FastPhase_Struct * pParams,
					float * TempBuffer0,
					float * TempBuffer1,
					float * TempBuffer2)
{
	pParams->Quadrant    = TempBuffer0;
	pParams->Numerator   = TempBuffer1;
	pParams->Denominator = TempBuffer2;
}

//***************************************************************************//
//*                             FastPhase(...)                              *//
//***************************************************************************//
#ifdef USE_PHASE_APPROX
float FastPhase(float r, float i)
{
	float temp, phase;
	float Quadrant = 0, Numerator = 0, Denominator = 0;

	//Shift by 0 degrees
	Quadrant = 0;
	Numerator = i;
	Denominator = r;

	if(i > r && i > -r){
		//Shift by 90 degrees
		Quadrant = 1;
		Numerator = -r;
		Denominator = i;
	}
	if(r <= i && r <= -i){
		//Shift by 180 degrees
		Quadrant = 2;
		Numerator = i;
		Denominator = r;
	}
	if(i <= r && i <= -r){
		//Shift by 270 degrees
		Quadrant = 3;
		Numerator = -r;
		Denominator = i;
	}

	if((float)fabs(Denominator) < (float)FLT_EPSILON)
		Denominator = (float)FLT_EPSILON;

	Denominator = 1.0f / Denominator;

	Numerator *= Denominator;

	temp = Numerator * Numerator;
	phase  = 0.0208351f;
	phase *= temp;
	phase -= 0.085133f;
	phase *= temp;
	phase += 0.180141f;
	phase *= temp;
	phase -= 0.3302995f;
	phase *= temp;
	phase += 0.999866f;
	phase *= Numerator;

	if(Quadrant == 1){
		phase += 0.5f * (float)PI;
	}
	if(Quadrant == 2){
		phase += (float)PI;
	}
	if(Quadrant == 3){
		phase -= 0.5f * (float)PI;
	}

	if(phase > (float)PI)
		phase -= 2.0f * (float)PI;

	return phase;
}
#else
float FastPhase(float r, float i)
{
	float phase, temp;

	//Take the reciprocal of the real part
	temp = r;
	if(temp <= (float)FLT_EPSILON && temp >= -(float)FLT_EPSILON)
		temp = (float)FLT_EPSILON;
	temp = 1.0f / temp;

	//Now take the arctan of the imag / real
	temp = i * temp;
	phase = (float)atanf(temp);		//2 quadrant arctan

	//Make into a 4 quadrant arctan
	if(r < 0  &&  i >= 0){
		phase += (float)PI;
	}
	if(r < 0  &&  i < 0){
		phase -= (float)PI;
	}

	return phase;
}
#endif

//***************************************************************************//
//*                         FastPhase_ARRAY(...)                            *//
//***************************************************************************//
#ifdef USE_PHASE_APPROX
void FastPhase_ARRAY(float * PtrInRe,
					 float * PtrInIm,
					 float * PtrOutPhase,
					 unsigned int Framesize,
					 FastPhase_Struct * pParams)
{
	int n;
	float temp;

	for(n = 0; n < (int)Framesize; n++){
		//Shift by 0 degrees
		pParams->Quadrant[n] = 0;
		pParams->Numerator[n] = PtrInIm[n];
		pParams->Denominator[n] = PtrInRe[n];

		if(PtrInIm[n] > PtrInRe[n] && PtrInIm[n] > -PtrInRe[n]){
			//Shift by 90 degrees
			pParams->Quadrant[n] = 1;
			pParams->Numerator[n] = -PtrInRe[n];
			pParams->Denominator[n] = PtrInIm[n];
		}
		if(PtrInRe[n] <= PtrInIm[n] && PtrInRe[n] <= -PtrInIm[n]){
			//Shift by 180 degrees
			pParams->Quadrant[n] = 2;
			pParams->Numerator[n] = PtrInIm[n];
			pParams->Denominator[n] = PtrInRe[n];
		}
		if(PtrInIm[n] <= PtrInRe[n] && PtrInIm[n] <= -PtrInRe[n]){
			//Shift by 270 degrees
			pParams->Quadrant[n] = 3;
			pParams->Numerator[n] = -PtrInRe[n];
			pParams->Denominator[n] = PtrInIm[n];
		}
	}
	for(n = 0; n < (int)Framesize; n++){
		if((float)fabs(pParams->Denominator[n]) < (float)FLT_EPSILON)
			pParams->Denominator[n] = (float)FLT_EPSILON;
	}

#ifdef PC_BUILD
	for(n = 0; n < (int)Framesize; n++){
		pParams->Denominator[n] = 1.0f / pParams->Denominator[n];
	}
#endif
#ifdef DSP_BUILD
	DSPF_sp_vecrecip(pParams->Denominator, pParams->Denominator, Framesize);
#endif

	for(n = 0; n < (int)Framesize; n++){
		pParams->Numerator[n] *= pParams->Denominator[n];
	}

	for(n = 0; n < (int)Framesize; n++){
		temp = pParams->Numerator[n] * pParams->Numerator[n];
		PtrOutPhase[n]  = 0.0208351f;
		PtrOutPhase[n] *= temp;
		PtrOutPhase[n] -= 0.085133f;
		PtrOutPhase[n] *= temp;
		PtrOutPhase[n] += 0.180141f;
		PtrOutPhase[n] *= temp;
		PtrOutPhase[n] -= 0.3302995f;
		PtrOutPhase[n] *= temp;
		PtrOutPhase[n] += 0.999866f;
		PtrOutPhase[n] *= pParams->Numerator[n];
	}

	for(n = 0; n < (int)Framesize; n++){
		if(pParams->Quadrant[n] == 1){
			PtrOutPhase[n] += 0.5f * (float)PI;
		}
		if(pParams->Quadrant[n] == 2){
			PtrOutPhase[n] += (float)PI;
		}
		if(pParams->Quadrant[n] == 3){
			PtrOutPhase[n] -= 0.5f * (float)PI;
		}
	}
	for(n = 0; n < (int)Framesize; n++){
		if(PtrOutPhase[n] > (float)PI)
			PtrOutPhase[n] -= 2.0f * (float)PI;
	}
}
#else
void FastPhase_ARRAY(float * PtrInRe,
					 float * PtrInIm,
					 float * PtrOutPhase,
					 unsigned int Framesize,
					 FastPhase_Struct * pParams)
{
	int n;
	float temp;

	//Do the hard part first: the PHASE
	//Take the reciprocal of the real part
    for(n = 0; n < (int)Framesize; n++){
		PtrOutPhase[n] = PtrInRe[n];
		if(PtrOutPhase[n] <= (float)FLT_EPSILON && PtrOutPhase[n] >= -(float)FLT_EPSILON){
			PtrOutPhase[n] = (float)FLT_EPSILON;
		}
		PtrOutPhase[n] = 1.0f / PtrOutPhase[n];
    }
	//Now take the arctan of the imag / real
    for(n = 0; n < (int)Framesize; n++){
		temp = PtrInIm[n] * PtrOutPhase[n];
		PtrOutPhase[n] = atanf(temp);		//2 quadrant arctan
	}
    for(n = 0; n < (int)Framesize; n++){
		//Make into a 4 quadrant arctan
		if(PtrInRe[n] < 0  &&  PtrInIm[n] >= 0){
			PtrOutPhase[n] += (float)PI;
		}
		if(PtrInRe[n] < 0  &&  PtrInIm[n] < 0){
			PtrOutPhase[n] -= (float)PI;
		}
    }
}
#endif

//***************************************************************************//
//*                            FastSqrt(...)                                *//
//***************************************************************************//
#ifdef USE_SQRT_APPROX
float FastSqrt(float x)
{
	float xhalf = 0.5f*x;
	int i = *(int*)&x;			// get bits for floating value
	i = 0x5f3759df - (i>>1);	// gives initial guess y0
	x = *(float*)&i;			// convert bits back to float
	x = x*(1.5f-xhalf*x*x);		// Newton step, repeating increases accuracy
	x = x*(1.5f-xhalf*x*x);		// Newton step, repeating increases accuracy

	x = 2.0f*xhalf*x;			// convert InvSqrt into Sqrt
	return x;
}
#else
float FastSqrt(float x)
{
	return (float)sqrtf(x);
}
#endif

//***************************************************************************//
//*                         FastSqrt_ARRAY(...)                             *//
//***************************************************************************//
#ifdef USE_SQRT_APPROX
void FastSqrt_ARRAY(float * PtrIn,
					float * PtrOut,
					unsigned int Framesize)
{
	float x, xhalf;
	int n, i;
	static float Temp[NEURAL_FRAMESIZE];

	for(n = 0; n < (int)Framesize; n++){
		x = PtrIn[n];
		i = *(int*)&x;				// get bits for floating value
		i = 0x5f3759df - (i>>1);	// gives initial guess y0
		Temp[n] = *(float*)&i;		// convert bits back to float
	}
	for(n = 0; n < (int)Framesize; n++){
		x = Temp[n];
		xhalf = 0.5f*PtrIn[n];
		x = x*(1.5f-xhalf*x*x);		// Newton step, repeating increases accuracy
		x = x*(1.5f-xhalf*x*x);		// Newton step, repeating increases accuracy

		x = 2.0f*xhalf*x;			// convert InvSqrt into Sqrt

		PtrOut[n] = x;
	}

	/*
	for(n = 0; n < (int)Framesize; n++){
		x = LocalPtrIn[n];
		xhalf = 0.5f*x;
		i = *(int*)&x;				// get bits for floating value
		i = 0x5f3759df - (i>>1);	// gives initial guess y0
		x = *(float*)&i;			// convert bits back to float
		x = x*(1.5f-xhalf*x*x);		// Newton step, repeating increases accuracy
		x = x*(1.5f-xhalf*x*x);		// Newton step, repeating increases accuracy

		x = 2.0f*xhalf*x;			// convert InvSqrt into Sqrt

		LocalPtrOut[n] = x;
	}
	*/
}
#else
void FastSqrt_ARRAY(float * PtrIn,
					float * PtrOut,
					unsigned int Framesize)
{
	int n;
	for(n = 0; n < (int)Framesize; n++){
		PtrOut[n] = (float)sqrtf(PtrIn[n]);
	}
}
#endif

//***************************************************************************//
//*                             FastLog2(...)                               *//
//***************************************************************************//
#ifdef USE_LOG_APPROX
float FastLog2(float x)
{
	ieee754_float32_t log2val, partial;
	union {
		ieee754_float32_t	f;
		int					i;
	} fi;
	int mantisse;

	ieee754_float32_t * LocalLogTable = (ieee754_float32_t *)LogTable;

	if((1<<LOG2_SIZE_L2) != LOG2_SIZE)
		return 0.0f;

	fi.f = x;
	mantisse = fi.i & 0x7fffff;
	log2val = (ieee754_float32_t)(((fi.i>>23) & 0xFF)-0x7f);
	partial = (ieee754_float32_t)((mantisse & ((1<<(23-LOG2_SIZE_L2))-1)));
	partial *= 0.00006103515625f;	//1.0f/((1<<(23-LOG2_SIZE_L2)));


	mantisse >>= (23-LOG2_SIZE_L2);

	// log2val += log_table[mantisse];  without interpolation the results are not good //
	log2val += LocalLogTable[mantisse]*(1.0f-partial) + LocalLogTable[mantisse+1]*partial;

	return log2val;
}
#else
float FastLog2(float x)
{
	return 1.44269504088896f * logf(x);
}
#endif

//***************************************************************************//
//*                          FastLog2_ARRAY(...)                            *//
//***************************************************************************//
#ifdef USE_LOG_APPROX
void FastLog2_ARRAY(float * PtrIn,
					float * PtrOut,
					unsigned int Framesize)
{
	ieee754_float32_t log2val, partial;
	union {
		ieee754_float32_t	f;
		int					i;
	} fi;
	int mantisse;
	int n;

	ieee754_float32_t * LocalLogTable = (ieee754_float32_t *)LogTable;

	if((1<<LOG2_SIZE_L2) != LOG2_SIZE)
		return;

	for(n = 0; n < (int)Framesize; n++){
		fi.f = PtrIn[n];
		mantisse = fi.i & 0x7fffff;
		log2val = (ieee754_float32_t)(((fi.i>>23) & 0xFF)-0x7f);
		partial = (ieee754_float32_t)((mantisse & ((1<<(23-LOG2_SIZE_L2))-1)));
		partial *= 0.00006103515625f;	//1.0f/((1<<(23-LOG2_SIZE_L2)));


		mantisse >>= (23-LOG2_SIZE_L2);

		// log2val += log_table[mantisse];  without interpolation the results are not good //
		log2val += LocalLogTable[mantisse]*(1.0f-partial) + LocalLogTable[mantisse+1]*partial;

		PtrOut[n] = log2val;
	}
}
#else
void FastLog2_ARRAY(float * PtrIn,
					float * PtrOut,
					unsigned int Framesize)
{
	int n;
	float norm = 1.44269504088896f;
	for(n = 0; n < (int)Framesize; n++){
		PtrOut[n] = norm * logf(PtrIn[n]);
	}
}
#endif

//***************************************************************************//
//*                              FastLog10(...)                             *//
//***************************************************************************//
#ifdef USE_LOG_APPROX
float FastLog10(float x)
{
	float y = FastLog2(x);
	y *= LOG2OVERLOG10;
	return y;
}
#else
float FastLog10(float x)
{
	return (float)log10f(x);
}
#endif

//***************************************************************************//
//*                           FastLog10_ARRAY(...)                          *//
//***************************************************************************//
#ifdef USE_LOG_APPROX
void FastLog10_ARRAY(float * PtrIn,
					 float * PtrOut,
					 unsigned int Framesize)
{
	int n;

	FastLog2_ARRAY(PtrIn,PtrOut,Framesize);

	for(n = 0; n < (int)Framesize; n++){
		PtrOut[n] *= LOG2OVERLOG10;
	}
}
#else
void FastLog10_ARRAY(float * restrict PtrIn,
					 float * restrict PtrOut,
					 unsigned int Framesize)
{
	int n;
	for(n = 0; n < (int)Framesize; n++){
		PtrOut[n] = (float)log10f(PtrIn[n]);
	}
}
#endif

//***************************************************************************//
//*                              FastLogln(...)                             *//
//***************************************************************************//
#ifdef USE_LOG_APPROX
float FastLogln(float x)
{
	float y = FastLog2(x);
	y *= LOG2;
	return y;
}
#else
float FastLogln(float x)
{
	return (float)logf(x);
}
#endif

//***************************************************************************//
//*                           FastLogln_ARRAY(...)                          *//
//***************************************************************************//
#ifdef USE_LOG_APPROX
void FastLogln_ARRAY(float * PtrIn,
					 float * PtrOut,
					 unsigned int Framesize)
{
	int n;

	FastLog2_ARRAY(PtrIn,PtrOut,Framesize);

	for(n = 0; n < (int)Framesize; n++){
		PtrOut[n] *= LOG2;
	}
}
#else
void FastLogln_ARRAY(float * PtrIn,
					 float * PtrOut,
					 unsigned int Framesize)
{
	int n;
	for(n = 0; n < (int)Framesize; n++){
		PtrOut[n] = (float)logf(PtrIn[n]);
	}
}
#endif

//***************************************************************************//
//*                               FastPow(...)                              *//
//***************************************************************************//
#ifdef USE_POW_APPROX
float FastPow(float x, float y)
{
	return FastPow2(y*FastLog2(x));
}
#else
float FastPow(float x, float y)
{
	return (float)FMOD_POW(x,y);
}
#endif

//***************************************************************************//
//*                            FastPow_ARRAY(...)                           *//
//***************************************************************************//
#ifdef USE_POW_APPROX
void FastPow_ARRAY(float * PtrIn0,
				   float * PtrIn1,
				   float * PtrOut,
				   unsigned int Framesize)
{
	int n;
	FastLog2_ARRAY(PtrIn0,PtrOut,Framesize);
	for(n = 0; n < (int)Framesize; n++){
		PtrOut[n] *= PtrIn1[n];
	}
	FastPow2_ARRAY(PtrOut,PtrOut,Framesize);

	//for(n = 0; n < (int)Framesize; n++){
	//	PtrOut[n] = FastPow2(PtrIn1[n]*FastLog2(PtrIn0[n]));
	//}
}
#else
void FastPow_ARRAY(float * PtrIn0,
				   float * PtrIn1,
				   float * PtrOut,
				   unsigned int Framesize)
{
	int n;
	for(n = 0; n < (int)Framesize; n++){
		PtrOut[n] = (float)FMOD_POW(PtrIn0[n],PtrIn1[n]);
	}
}
#endif

//***************************************************************************//
//*                              FastPow2(...)                              *//
//***************************************************************************//
#ifdef USE_POW_APPROX
float FastPow2(float i)
{
	float shift23=(1<<23);
	float PowBodge=0.33971f;
	float x;
	float y=i-FMOD_FLOOR(i);
	y=(y-y*y)*PowBodge;

	x=i+127-y;
	x*= shift23; //pow(2,23);
	*(int*)&x=(int)x;
	return x;
}
#else
float FastPow2(float x)
{
	return (float)FMOD_POW(2.0f,x);
}
#endif

//***************************************************************************//
//*                           FastPow2_ARRAY(...)                           *//
//***************************************************************************//
#ifdef USE_POW_APPROX
void FastPow2_ARRAY(float * PtrIn,
					float * PtrOut,
					unsigned int Framesize)
{
	int n;
	float shift23=(1<<23);
	float i, PowBodge, x, y;
	for(n = 0; n < (int)Framesize; n++){
		i = PtrIn[n];
		PowBodge=0.33971f;
		x=(float)((i>0.0f)?(int)i:(int)(i-1.0f));
		y=i-x; //floorf(i);
		y=(y-y*y)*PowBodge;

		PtrOut[n]=i+127-y;
	}
	for(n = 0; n < (int)Framesize; n++){
		x=PtrOut[n];  
		x*=shift23; //pow(2,23);
		*(int*)&x=(int)x;
		PtrOut[n] = x;
	}
}
#else
void FastPow2_ARRAY(float * PtrIn,
					float * PtrOut,
					unsigned int Framesize)
{
	int n;
	for(n = 0; n < (int)Framesize; n++){
		PtrOut[n] = (float)FMOD_POW(2.0f,PtrIn[n]);
	}
}
#endif

//***************************************************************************//
//*                              FastPow10(...)                             *//
//***************************************************************************//
#ifdef USE_POW_APPROX
float FastPow10(float x)
{
	return FastPow2(x * 3.32192809488736f);
}
#else
float FastPow10(float x)
{
	return (float)FMOD_POW(10.0f,x);
}
#endif

//***************************************************************************//
//*                           FastPow10_ARRAY(...)                          *//
//***************************************************************************//
#ifdef USE_POW_APPROX
void FastPow10_ARRAY(float * PtrIn,
					 float * PtrOut,
					 unsigned int Framesize)
{
	int n;
	for(n = 0; n < (int)Framesize; n++){
		PtrOut[n] = PtrIn[n] * 3.32192809488736f;
	}
	FastPow2_ARRAY(PtrOut,PtrOut,Framesize);
}
#else
void FastPow10_ARRAY(float * PtrIn,
					 float * PtrOut,
					 unsigned int Framesize)
{
	int n;
	for(n = 0; n < (int)Framesize; n++){
		PtrOut[n] = (float)FMOD_POW(10.0f,PtrIn[n]);
	}
}
#endif

//***************************************************************************//
//*                               FastExp(...)                              *//
//***************************************************************************//
#ifdef USE_POW_APPROX
float FastExp(float x)
{
	return FastPow2(x * 1.44269504064533f);
}
#else
float FastExp(float x)
{
	return (float)expf(x);
}
#endif

//***************************************************************************//
//*                           FastExp_ARRAY(...)                            *//
//***************************************************************************//
#ifdef USE_POW_APPROX
void FastExp_ARRAY(float * PtrIn,
				   float * PtrOut,
				   unsigned int Framesize)
{
	int n;
	for(n = 0; n < (int)Framesize; n++){
		PtrOut[n] = PtrIn[n] * 1.44269504064533f;
	}
	FastPow2_ARRAY(PtrOut,PtrOut,Framesize);
}
#else
void FastExp_ARRAY(float * PtrIn,
				   float * PtrOut,
				   unsigned int Framesize)
{
	int n;
	for(n = 0; n < (int)Framesize; n++){
		PtrOut[n] = (float)expf(PtrIn[n]);
	}
}
#endif

//***************************************************************************//
//*                               FastSin(...)                              *//
//***************************************************************************//
#ifdef USE_SIN_APPROX
float FastSin(float x)
{
	float ASqr = x*x;
    float Result = (float)-2.39e-08;
    Result *= ASqr;
    Result += (float)2.7526e-06;
    Result *= ASqr;
    Result -= (float)1.98409e-04;
    Result *= ASqr;
    Result += (float)8.3333315e-03;
    Result *= ASqr;
    Result -= (float)1.666666664e-01;
    Result *= ASqr;
    Result += (float)1.0;
    Result *= x;
    return Result;
}
#else
float FastSin(float x)
{
	return (float)sin(x);
}
#endif

//***************************************************************************//
//*                           FastSin_ARRAY(...)                            *//
//***************************************************************************//
#ifdef USE_SIN_APPROX
void FastSin_ARRAY(float * PtrIn,
				   float * PtrOut,
				   unsigned int Framesize)
{
	int n;
	float ASqr, Result;
	for(n = 0; n < (int)Framesize; n++){
		ASqr = PtrIn[n]*PtrIn[n];
		Result = (float)-2.39e-08;
		Result *= ASqr;
		Result += (float)2.7526e-06;
		Result *= ASqr;
		Result -= (float)1.98409e-04;
		Result *= ASqr;
		Result += (float)8.3333315e-03;
		Result *= ASqr;
		Result -= (float)1.666666664e-01;
		Result *= ASqr;
		Result += (float)1.0;
		Result *= PtrIn[n];
		PtrOut[n] = Result;
	}
}
#else
void FastSin_ARRAY(float * PtrIn,
				   float * PtrOut,
				   unsigned int Framesize)
{
	int n;
	for(n = 0; n < (int)Framesize; n++){
		PtrOut[n] = (float)sin(PtrIn[n]);
	}
}
#endif

//***************************************************************************//
//*                               FastCos(...)                              *//
//***************************************************************************//
#ifdef USE_COS_APPROX
float FastCos(float x)
{
	float ASqr = x*x;
    float Result = (float)-2.605e-07;
    Result *= ASqr;
    Result += (float)2.47609e-05;
    Result *= ASqr;
    Result -= (float)1.3888397e-03;
    Result *= ASqr;
    Result += (float)4.16666418e-02;
    Result *= ASqr;
    Result -= (float)4.999999963e-01;
    Result *= ASqr;
    Result += (float)1.0;
    return Result;
}
#else
float FastCos(float x)
{
	return (float)cos(x);
}
#endif

//***************************************************************************//
//*                           FastCos_ARRAY(...)                            *//
//***************************************************************************//
#ifdef USE_COS_APPROX
void FastCos_ARRAY(float * PtrIn,
				   float * PtrOut,
				   unsigned int Framesize)
{
	int n;
	float ASqr, Result;
	for(n = 0; n < (int)Framesize; n++){
		ASqr = PtrIn[n]*PtrIn[n];
		Result = (float)-2.605e-07;
		Result *= ASqr;
		Result += (float)2.47609e-05;
		Result *= ASqr;
		Result -= (float)1.3888397e-03;
		Result *= ASqr;
		Result += (float)4.16666418e-02;
		Result *= ASqr;
		Result -= (float)4.999999963e-01;
		Result *= ASqr;
		Result += (float)1.0;
		PtrOut[n] = Result;
	}
}
#else
void FastCos_ARRAY(float * PtrIn,
				   float * PtrOut,
				   unsigned int Framesize)
{
	int n;
	for(n = 0; n < (int)Framesize; n++){
		PtrOut[n] = (float)cos(PtrIn[n]);
	}
}
#endif
