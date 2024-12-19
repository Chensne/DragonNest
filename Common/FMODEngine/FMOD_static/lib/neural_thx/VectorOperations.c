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
//*  File        : VectorOperations.c										*//
//*  Description : Basic vector (array) operations							*//
//*  Author(s)   : Jeff Thompson											*//
//*																			*//
//*					Copyright (c) Neural Audio Corp. 2006					*//
//*																			*//
//***************************************************************************//
#include "Neural_THX_Encoders.h"
#include <math.h>
#include <float.h>

//***************************************************************************//
//*								Add2(...)									*//
//***************************************************************************//
void Add2(float * PtrIn0,
		  float * PtrIn1,
		  float * PtrOut0,
		  unsigned int Framesize)
{
	unsigned int index;
    
	for(index = 0; index < Framesize; index++){
		PtrOut0[index] = PtrIn0[index] + PtrIn1[index];
	}
}

//***************************************************************************//
//*								Add2_x2(...)								*//
//***************************************************************************//
void Add2_x2(float * PtrIn0,
			 float * PtrIn1,
			 float * PtrOut0,
			 float * PtrIn2,
			 float * PtrIn3,
			 float * PtrOut1,
			 unsigned int Framesize)
{
	unsigned int index;
    
	for(index = 0; index < Framesize; index++){
		PtrOut0[index] = PtrIn0[index] + PtrIn1[index];
		PtrOut1[index] = PtrIn2[index] + PtrIn3[index];
	}
}

//***************************************************************************//
//*								Add2_x3(...)								*//
//***************************************************************************//
void Add2_x3(float * PtrIn0,
			 float * PtrIn1,
			 float * PtrOut0,
			 float * PtrIn2,
			 float * PtrIn3,
			 float * PtrOut1,
			 float * PtrIn4,
			 float * PtrIn5,
			 float * PtrOut2,
			 unsigned int Framesize)
{
	unsigned int index;
    
	for(index = 0; index < Framesize; index++){
		PtrOut0[index] = PtrIn0[index] + PtrIn1[index];
		PtrOut1[index] = PtrIn2[index] + PtrIn3[index];
		PtrOut2[index] = PtrIn4[index] + PtrIn5[index];
	}
}

//***************************************************************************//
//*								Add2_x4(...)								*//
//***************************************************************************//
void Add2_x4(float * PtrIn0,
			 float * PtrIn1,
			 float * PtrOut0,
			 float * PtrIn2,
			 float * PtrIn3,
			 float * PtrOut1,
			 float * PtrIn4,
			 float * PtrIn5,
			 float * PtrOut2,
			 float * PtrIn6,
			 float * PtrIn7,
			 float * PtrOut3,
			 unsigned int Framesize)
{
	unsigned int index;
    
	for(index = 0; index < Framesize; index++){
		PtrOut0[index] = PtrIn0[index] + PtrIn1[index];
		PtrOut1[index] = PtrIn2[index] + PtrIn3[index];
		PtrOut2[index] = PtrIn4[index] + PtrIn5[index];
		PtrOut3[index] = PtrIn6[index] + PtrIn7[index];
	}
}

//***************************************************************************//
//*								Add3(...)									*//
//***************************************************************************//
void Add3(float * PtrIn0,
		  float * PtrIn1,
		  float * PtrIn2,
		  float * PtrOut0,
		  unsigned int Framesize)
{
	unsigned int index;
    
	for(index = 0; index < Framesize; index++){
		PtrOut0[index] = PtrIn0[index] + PtrIn1[index] + PtrIn2[index];
	}
}

//***************************************************************************//
//*								Add4(...)									*//
//***************************************************************************//
void Add4(float * PtrIn0,
		  float * PtrIn1,
		  float * PtrIn2,
		  float * PtrIn3,
		  float * PtrOut0,
		  unsigned int Framesize)
{
	unsigned int index;
    
	for(index = 0; index < Framesize; index++){
		PtrOut0[index] = PtrIn0[index] + PtrIn1[index] + PtrIn2[index] + PtrIn3[index];
	}
}

//***************************************************************************//
//*								Add5(...)									*//
//***************************************************************************//
void Add5(float * PtrIn0,
		  float * PtrIn1,
		  float * PtrIn2,
		  float * PtrIn3,
		  float * PtrIn4,
		  float * PtrOut0,
		  unsigned int Framesize)
{
	unsigned int index;
    
	for(index = 0; index < Framesize; index++){
		PtrOut0[index] = PtrIn0[index] + PtrIn1[index] + PtrIn2[index] + PtrIn3[index] + PtrIn4[index];
	}
}

//***************************************************************************//
//*								Subtract2(...)								*//
//***************************************************************************//
void Subtract2(float * PtrIn0,
			   float * PtrIn1,
			   float * PtrOut0,
			   unsigned int Framesize)
{
	unsigned int index;
    
	for(index = 0; index < Framesize; index++){
		PtrOut0[index] = PtrIn0[index] - PtrIn1[index];
	}
}

//***************************************************************************//
//*								Subtract2_x2(...)							*//
//***************************************************************************//
void Subtract2_x2(float * PtrIn0,
				  float * PtrIn1,
				  float * PtrOut0,
				  float * PtrIn2,
				  float * PtrIn3,
				  float * PtrOut1,
				  unsigned int Framesize)
{
	unsigned int index;
    
	for(index = 0; index < Framesize; index++){
		PtrOut0[index] = PtrIn0[index] - PtrIn1[index];
		PtrOut1[index] = PtrIn2[index] - PtrIn3[index];
	}
}

//***************************************************************************//
//*								Subtract2_x3(...)							*//
//***************************************************************************//
void Subtract2_x3(float * PtrIn0,
				  float * PtrIn1,
				  float * PtrOut0,
				  float * PtrIn2,
				  float * PtrIn3,
				  float * PtrOut1,
				  float * PtrIn4,
				  float * PtrIn5,
				  float * PtrOut2,
				  unsigned int Framesize)
{
	unsigned int index;
    
	for(index = 0; index < Framesize; index++){
		PtrOut0[index] = PtrIn0[index] - PtrIn1[index];
		PtrOut1[index] = PtrIn2[index] - PtrIn3[index];
		PtrOut2[index] = PtrIn4[index] - PtrIn5[index];
	}
}

//***************************************************************************//
//*								Subtract2_x4(...)							*//
//***************************************************************************//
void Subtract2_x4(float * PtrIn0,
				  float * PtrIn1,
				  float * PtrOut0,
				  float * PtrIn2,
				  float * PtrIn3,
				  float * PtrOut1,
				  float * PtrIn4,
				  float * PtrIn5,
				  float * PtrOut2,
				  float * PtrIn6,
				  float * PtrIn7,
				  float * PtrOut3,
				  unsigned int Framesize)
{
	unsigned int index;
    
	for(index = 0; index < Framesize; index++){
		PtrOut0[index] = PtrIn0[index] - PtrIn1[index];
		PtrOut1[index] = PtrIn2[index] - PtrIn3[index];
		PtrOut2[index] = PtrIn4[index] - PtrIn5[index];
		PtrOut3[index] = PtrIn6[index] - PtrIn7[index];
	}
}

//***************************************************************************//
//*								Multiply2(...)								*//
//***************************************************************************//
void Multiply2(float * PtrIn0,
			   float * PtrIn1,
			   float * PtrOut0,
			   unsigned int Framesize)
{
	unsigned int index;
    
	for(index = 0; index < Framesize; index++){
		PtrOut0[index] = PtrIn0[index] * PtrIn1[index];
	}
}

//***************************************************************************//
//*								Multiply2_x2(...)							*//
//***************************************************************************//
void Multiply2_x2(float * PtrIn0,
				  float * PtrIn1,
				  float * PtrOut0,
				  float * PtrIn2,
				  float * PtrIn3,
				  float * PtrOut1,
				  unsigned int Framesize)
{
	unsigned int index;
    
	for(index = 0; index < Framesize; index++){
		PtrOut0[index] = PtrIn0[index] * PtrIn1[index];
		PtrOut1[index] = PtrIn2[index] * PtrIn3[index];
	}
}

//***************************************************************************//
//*								Multiply2_x3(...)							*//
//***************************************************************************//
void Multiply2_x3(float * PtrIn0,
				  float * PtrIn1,
				  float * PtrOut0,
				  float * PtrIn2,
				  float * PtrIn3,
				  float * PtrOut1,
				  float * PtrIn4,
				  float * PtrIn5,
				  float * PtrOut2,
				  unsigned int Framesize)
{
	unsigned int index;
    
	for(index = 0; index < Framesize; index++){
		PtrOut0[index] = PtrIn0[index] * PtrIn1[index];
		PtrOut1[index] = PtrIn2[index] * PtrIn3[index];
		PtrOut2[index] = PtrIn4[index] * PtrIn5[index];
	}
}

//***************************************************************************//
//*								Multiply2_x4(...)							*//
//***************************************************************************//
void Multiply2_x4(float * PtrIn0,
				  float * PtrIn1,
				  float * PtrOut0,
				  float * PtrIn2,
				  float * PtrIn3,
				  float * PtrOut1,
				  float * PtrIn4,
				  float * PtrIn5,
				  float * PtrOut2,
				  float * PtrIn6,
				  float * PtrIn7,
				  float * PtrOut3,
				  unsigned int Framesize)
{
	unsigned int index;
    
	for(index = 0; index < Framesize; index++){
		PtrOut0[index] = PtrIn0[index] * PtrIn1[index];
		PtrOut1[index] = PtrIn2[index] * PtrIn3[index];
		PtrOut2[index] = PtrIn4[index] * PtrIn5[index];
		PtrOut3[index] = PtrIn6[index] * PtrIn7[index];
	}
}

//***************************************************************************//
//*								Multiply3(...)								*//
//***************************************************************************//
void Multiply3(float * PtrIn0,
			   float * PtrIn1,
			   float * PtrIn2,
			   float * PtrOut0,
			   unsigned int Framesize)
{
	unsigned int index;
    
	for(index = 0; index < Framesize; index++){
		PtrOut0[index] = PtrIn0[index] * PtrIn1[index] * PtrIn2[index];
	}
}

//***************************************************************************//
//*								Multiply4(...)								*//
//***************************************************************************//
void Multiply4(float * PtrIn0,
			   float * PtrIn1,
			   float * PtrIn2,
			   float * PtrIn3,
			   float * PtrOut0,
			   unsigned int Framesize)
{
	unsigned int index;
    
	for(index = 0; index < Framesize; index++){
		PtrOut0[index] = PtrIn0[index] * PtrIn1[index] * PtrIn2[index] * PtrIn3[index];
	}
}

//***************************************************************************//
//*								Multiply5(...)								*//
//***************************************************************************//
void Multiply5(float * PtrIn0,
			   float * PtrIn1,
			   float * PtrIn2,
			   float * PtrIn3,
			   float * PtrIn4,
			   float * PtrOut0,
			   unsigned int Framesize)
{
	unsigned int index;
    
	for(index = 0; index < Framesize; index++){
		PtrOut0[index] = PtrIn0[index] * PtrIn1[index] * PtrIn2[index] * PtrIn3[index] * PtrIn4[index];
	}
}

//***************************************************************************//
//*						   ComplexMultiply2(...)							*//
//***************************************************************************//
void ComplexMultiply2(float * PtrInReal0,
					  float * PtrInImag0,
					  float * PtrInReal1,
					  float * PtrInImag1,
					  float * PtrOutReal,
					  float * PtrOutImag,
					  unsigned int Framesize)
{
	unsigned int index;
	float Real0, Real1, Imag0, Imag1;

    for (index = 0; index < Framesize; index++) {
		Real0 = PtrInReal0[index];
		Real1 = PtrInReal1[index];
		Imag0 = PtrInImag0[index];
		Imag1 = PtrInImag1[index];
		PtrOutReal[index] = Real0 * Real1 - Imag0 * Imag1;
		PtrOutImag[index] = Real0 * Imag1 + Real1 * Imag0;
    }
}

//***************************************************************************//
//*								Divide2(...)								*//
//***************************************************************************//
void Divide2(float * PtrIn0,
			 float * PtrIn1,
			 float * PtrOut0,
			 unsigned int Framesize)
{
	unsigned int index;

    for (index = 0; index<Framesize; index++) {
		if(PtrIn1[index] == 0){
			PtrIn1[index] += (float)FLT_EPSILON;
		}
    }

	// Function to take the reciprical of the second input buffer
	for(index = 0; index < Framesize; index++){
		PtrOut0[index] = (float)1.0 / PtrIn1[index];
	}

	// Now multiply the recipricals by the first input buffer
    for (index = 0; index<Framesize; index++) {
		PtrOut0[index] *= PtrIn0[index];
    }
}

//***************************************************************************//
//*								Divide2_x2(...)								*//
//***************************************************************************//
void Divide2_x2(float * PtrIn0,
				float * PtrIn1,
				float * PtrOut0,
				float * PtrIn2,
				float * PtrIn3,
				float * PtrOut1,
				unsigned int Framesize)
{
	unsigned int index;

    for (index = 0; index<Framesize; index++) {
		if(PtrIn1[index] == 0){
			PtrIn1[index] += (float)FLT_EPSILON;
		}
		if(PtrIn3[index] == 0){
			PtrIn3[index] += (float)FLT_EPSILON;
		}
    }

	// Function to take the reciprical of the second input buffer
	for(index = 0; index < Framesize; index++){
		PtrOut0[index] = (float)1.0 / PtrIn1[index];
		PtrOut1[index] = (float)1.0 / PtrIn3[index];
	}

	// Now multiply the recipricals by the first input buffer
    for (index = 0; index<Framesize; index++) {
		PtrOut0[index] *= PtrIn0[index];
		PtrOut1[index] *= PtrIn2[index];
    }
}

//***************************************************************************//
//*								Divide2_x3(...)								*//
//***************************************************************************//
void Divide2_x3(float * PtrIn0,
				float * PtrIn1,
				float * PtrOut0,
				float * PtrIn2,
				float * PtrIn3,
				float * PtrOut1,
				float * PtrIn4,
				float * PtrIn5,
				float * PtrOut2,
				unsigned int Framesize)
{
	unsigned int index;

    for (index = 0; index<Framesize; index++) {
		if(PtrIn1[index] == 0){
			PtrIn1[index] += (float)FLT_EPSILON;
		}
		if(PtrIn3[index] == 0){
			PtrIn3[index] += (float)FLT_EPSILON;
		}
		if(PtrIn5[index] == 0){
			PtrIn5[index] += (float)FLT_EPSILON;
		}
    }

	// Function to take the reciprical of the second input buffer
	for(index = 0; index < Framesize; index++){
		PtrOut0[index] = (float)1.0 / PtrIn1[index];
		PtrOut1[index] = (float)1.0 / PtrIn3[index];
		PtrOut2[index] = (float)1.0 / PtrIn5[index];
	}

	// Now multiply the recipricals by the first input buffer
    for (index = 0; index<Framesize; index++) {
		PtrOut0[index] *= PtrIn0[index];
		PtrOut1[index] *= PtrIn2[index];
		PtrOut2[index] *= PtrIn4[index];
    }
}

//***************************************************************************//
//*								Divide2_x4(...)								*//
//***************************************************************************//
void Divide2_x4(float * PtrIn0,
				float * PtrIn1,
				float * PtrOut0,
				float * PtrIn2,
				float * PtrIn3,
				float * PtrOut1,
				float * PtrIn4,
				float * PtrIn5,
				float * PtrOut2,
				float * PtrIn6,
				float * PtrIn7,
				float * PtrOut3,
				unsigned int Framesize)
{
	unsigned int index;

    for (index = 0; index<Framesize; index++) {
		if(PtrIn1[index] == 0){
			PtrIn1[index] += (float)FLT_EPSILON;
		}
		if(PtrIn3[index] == 0){
			PtrIn3[index] += (float)FLT_EPSILON;
		}
		if(PtrIn5[index] == 0){
			PtrIn5[index] += (float)FLT_EPSILON;
		}
		if(PtrIn7[index] == 0){
			PtrIn7[index] += (float)FLT_EPSILON;
		}
    }

	// Function to take the reciprical of the second input buffer
	for(index = 0; index < Framesize; index++){
		PtrOut0[index] = (float)1.0 / PtrIn1[index];
		PtrOut1[index] = (float)1.0 / PtrIn3[index];
		PtrOut2[index] = (float)1.0 / PtrIn5[index];
		PtrOut3[index] = (float)1.0 / PtrIn7[index];
	}

	// Now multiply the recipricals by the first input buffer
    for (index = 0; index<Framesize; index++) {
		PtrOut0[index] *= PtrIn0[index];
		PtrOut1[index] *= PtrIn2[index];
		PtrOut2[index] *= PtrIn4[index];
		PtrOut3[index] *= PtrIn6[index];
    }
}

//***************************************************************************//
//*                           CopyArray(...)								*//
//***************************************************************************//
void CopyArray(float * PtrIn,
			   float * PtrOut,
			   unsigned int Framesize)
{
	unsigned int index;
    
	for(index = 0; index < Framesize; index++){
		PtrOut[index] = PtrIn[index];
	}
}

//***************************************************************************//
//*                           FillArray(...)								*//
//***************************************************************************//
void FillArray(float * PtrArray,
			   float FillValue,
			   unsigned int Framesize)
{
	unsigned int index;
    
	for(index = 0; index < Framesize; index++){
		PtrArray[index] = FillValue;
	}
}

//***************************************************************************//
//*                           OffsetArray(...)								*//
//***************************************************************************//
void OffsetArray(float * PtrArray,
				 float OffsetValue,
				 unsigned int Framesize)
{
	unsigned int index;
    
	for(index = 0; index < Framesize; index++){
		PtrArray[index] += OffsetValue;
	}
}

//***************************************************************************//
//*                           ScaleArray(...)								*//
//***************************************************************************//
void ScaleArray(float * PtrArray,
				float ScaleValue,
				unsigned int Framesize)
{
	unsigned int index;
    
	for(index = 0; index < Framesize; index++){
		PtrArray[index] *= ScaleValue;
	}
}

//***************************************************************************//
//*                           SquareArray(...)								*//
//***************************************************************************//
void SquareArray(float * PtrArray,
				 unsigned int Framesize)
{
	unsigned int index;
    
	for(index = 0; index < Framesize; index++){
		PtrArray[index] *= PtrArray[index];
	}
}

//***************************************************************************//
//*                             AbsValue(...)								*//
//***************************************************************************//
void AbsValue(float * PtrArray,
			  unsigned int Framesize)
{
	unsigned int index;

	for(index = 0; index < Framesize; index++){
    	PtrArray[index] = (float)fabs(PtrArray[index]);
    }
}

//***************************************************************************//
//*                             MaxClip(...)								*//
//***************************************************************************//
void MaxClip(float * PtrArray,
			 float MaxValue,
			 unsigned int Framesize)
{
	unsigned int index;

	for(index = 0; index < Framesize; index++){
    	if(PtrArray[index] > MaxValue)
			PtrArray[index] = MaxValue;
    }
}

//***************************************************************************//
//*                             MinClip(...)								*//
//***************************************************************************//
void MinClip(float * PtrArray,
			 float MinValue,
			 unsigned int Framesize)
{
	unsigned int index;

	for(index = 0; index < Framesize; index++){
    	if(PtrArray[index] < MinValue)
			PtrArray[index] = MinValue;
    }
}

//***************************************************************************//
//*                          SaturateArray(...)								*//
//***************************************************************************//
void SaturateArray(float * PtrArray,
				   float SaturationValue,
				   unsigned int Framesize)
{
	unsigned int index;
	float MaxValuePos, MaxValueNeg, Value;

	MaxValuePos = (float)fabs(SaturationValue);
	MaxValueNeg = -MaxValuePos;

    for(index = 0; index < Framesize; index++){
		Value = PtrArray[index];
		if(Value > MaxValuePos)
			Value = MaxValuePos;
		if(Value < MaxValueNeg)
			Value = MaxValueNeg;
		PtrArray[index] = Value;
    }
}

//***************************************************************************//
//*                          ReverseArray(...)								*//
//***************************************************************************//
void ReverseArray(float * PtrArray,
				  unsigned int Framesize)
{
	unsigned int index;
	float temp;

    for(index = 0; index < (Framesize>>1); index++){
		temp = PtrArray[index];
		PtrArray[index] = PtrArray[Framesize-index-1];
		PtrArray[Framesize-index-1] = temp;
    }
}
