//////////////////////////////////////////////////////////////////////
//						___________________							//
//						  |											//
//						  |	 |	  |	 \	 /							//
//						  |	 |	  |	  \ /							//
//						  |	 |----|	   /							//
//					 	  |	 |	  |	  / \							//
//					 	  |	 |	  |	 /	 \							//
//					  	___________________							//
//																	//
//				 Neural-THX (R) Surround Technology					//
//																	//
//					Copyright (c) 2008 THX Ltd.						//
//																	//
//		THX is a trademark of THX Ltd., which may be registered		//
//			in some jurisdictions.									//
//		All Rights Reserved.										//
//																	//
//		THX Confidential Information								//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name				:	Neural_THX_Global.h
//
//	Author(s)			:	Mark Gugler (mgugler@thx.com)
//
//	Created On			:	08/20/2007
//
//	Last Modified		:	03/03/2008
//
//	Version				:	1.61
//
//	References			:	
//
//	Description			:	Encapsulates any enumerations, structures, and 
//							defines needed by the Neural-THX API
//
//	Revision History	:	08/20/2007	Build basic framework and add in new
//										structs for the encoder
//							08/21/2007	Add in more comments and update some values
//										based on what was received from Neural
//							08/22/2007	Add in a new structure to encapsulate 
//										parameter structures into one interface
//							08/23/2007	Finish adding functions to the new struct
//										Add 522 parameters to settings struct
//							08/31/2007	Updating comments for clearly outlining
//										what values represent
//							09/04/2007	Update comments and prepare for 722
//										encoder integration
//							02/15/2008	Got 722 encoder so integrating and testing
//							02/26/2008	Added some defines and stripped out any
//										remaining memory allocation from the API
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __NEURAL_THX_GLOBAL__
#define __NEURAL_THX_GLOBAL__

#include "Neural_THX_Encoders.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global Defines
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NOTE:: Only comment out the encoders you won't use over the course of the game
//		This was implemented to not have any memory allocation
//		throughout the entire API.
#define USING_725
#define USING_722
// #define USING_522

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Definitions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name			:	Neural_THX_Encoder_Params
//
//	Created On		:	08/22/2007
//
//	Last Modified	:	02/28/2008
//
//	Description		:	Parameter structure encapsulating all other param structs
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct Neural_THX_Encoder_Params
{
#ifdef USING_725
	Neural_THX_725_Encode_Struct	t725;
#endif
#ifdef USING_522
	Neural_THX_522_Encode_Struct	t522;
#endif
#ifdef USING_722
	Neural_THX_722_Encode_Struct	t722;
#endif

	void *							pParams;
	Neural_THX_Encoder_Params()
	{
		pParams = NULL;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	Name		:	Init
	//
	//	Params		:	int - Channel Configuration to know which structure to use
	//
	//	Return		:	void
	//
	//	Purpose		:	Initialize the pointer needed by this encoder :)
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Init(int nChanConfig)
	{
		// see which configuration we have :)
		if(nChanConfig == NEURAL_THX_7_5_GAMING)
		{
#ifdef USING_725
			pParams = &t725;
#endif
		}
		else if(nChanConfig == NEURAL_THX_5_2_GAMING)
		{
#ifdef USING_522
			pParams = &t522;
#endif
		}
		else if(nChanConfig == NEURAL_THX_7_2_GAMING)
		{
#ifdef USING_722
			pParams = &t722;
#endif
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	Name		:	Release
	//
	//	Params		:	void
	//
	//	Return		:	void
	//
	//	Purpose		:	Releases memory if any has been allocated through the init
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Release(void)
	{
		pParams = NULL;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	Name		:	ValidStruct
	//
	//	Params		:	void
	//
	//	Return		:	bool - whether any of the pointers are valid or not
	//
	//	Purpose		:	Checks to see if the init was successful
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool ValidStruct(void)
	{
		if(pParams == NULL)
		{
			return false;
		}
		// return yes
		return true;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name			:	Neural_THX_Encoder_Settings
//
//	Created On		:	08/20/2007
//
//	Last Modified	:	02/28/2008
//
//	Description		:	Settings structure for the Neural-THX encoder
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct Neural_THX_Encoder_Settings
{
	// The Number of audio samples in each mono channel buffers passed to the
	//	encoder
	// NOTE: The encoder currently only supports an audio frame size of NEURAL_FRAMESIZE
	unsigned int	nFramesize;
	// A configuration value indicating that the encoder is to perform a Neural-THX
	//	compatible encoding.
	unsigned int	nChanConfig;
	// The sample rate of the input/output audio.
	// NOTE: The encoder currently supports sample rates of 32 kHz, 44.1 kHz, and
	//			48 kHz.
	unsigned int	nSampleRate;
	// Whether to use Final Limiting or not
	// NOTE: Used in 522 & 722 encoder
	THX_bool		bUseFinalLimiting;
	// LFE Cutoff when converting to 2 channels
	// NOTE: Used in 522 & 722 encoder
	float			fLFECutOff;

	// Default Constructor Set to defaults
	Neural_THX_Encoder_Settings()
	{
		nFramesize			= NEURAL_FRAMESIZE;
		nChanConfig			= NEURAL_THX_7_5_GAMING;
		nSampleRate			= SAMPLERATE_44_1;
		bUseFinalLimiting	= true;
		fLFECutOff			= 0.0f;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name			:	Neural_THX_Channel_Format
//
//	Created On		:	08/20/2007
//
//	Last Modified	:	02/28/2008
//
//	Description		:	Surround Setup structure
//
//						m_fL[n] equals 1 Sample point for the channel
//						while variable_name[n] =s a sample frame
//						NOTE: The encoder uses 32 bit max for sample points
//							which requires the user to compensate based on
//							the bytes each sample point takes
//							(i.e. bitsPerSample / 8 = nBytesPerSample rounded up)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct Neural_THX_Channel_Format	// double check if we need to support the framesize
{
	// Front
	float m_fL[NEURAL_FRAMESIZE];	
	float m_fC[NEURAL_FRAMESIZE];	
	float m_fR[NEURAL_FRAMESIZE];	
	// Sides/Surround & LFE
	float m_fLs[NEURAL_FRAMESIZE];
	float m_fRs[NEURAL_FRAMESIZE];
	float m_fLFE[NEURAL_FRAMESIZE];
	// Back
	float m_fLb[NEURAL_FRAMESIZE];
	float m_fRb[NEURAL_FRAMESIZE];			

	// Constructor set everything to NULL
	Neural_THX_Channel_Format()
	{
		// Fill with silence
		// CPS - FMOD_memset(this->m_fL,0,sizeof(float) * NEURAL_FRAMESIZE);
		// CPS - FMOD_memset(this->m_fC,0,sizeof(float) * NEURAL_FRAMESIZE);
		// CPS - FMOD_memset(this->m_fR,0,sizeof(float) * NEURAL_FRAMESIZE);
		// CPS - FMOD_memset(this->m_fLs,0,sizeof(float) * NEURAL_FRAMESIZE);
		// CPS - FMOD_memset(this->m_fRs,0,sizeof(float) * NEURAL_FRAMESIZE);
		// CPS - FMOD_memset(this->m_fLFE,0,sizeof(float) * NEURAL_FRAMESIZE);
		// CPS - FMOD_memset(this->m_fLb,0,sizeof(float) * NEURAL_FRAMESIZE);
		// CPS - FMOD_memset(this->m_fRb,0,sizeof(float) * NEURAL_FRAMESIZE);

		//ZeroMemory(this->m_fL,	sizeof(float) * NEURAL_FRAMESIZE);
		//ZeroMemory(this->m_fC,	sizeof(float) * NEURAL_FRAMESIZE);
		//ZeroMemory(this->m_fR,	sizeof(float) * NEURAL_FRAMESIZE);
		//ZeroMemory(this->m_fLs, sizeof(float) * NEURAL_FRAMESIZE);
		//ZeroMemory(this->m_fRs, sizeof(float) * NEURAL_FRAMESIZE);
		//ZeroMemory(this->m_fLFE,sizeof(float) * NEURAL_FRAMESIZE);
		//ZeroMemory(this->m_fLb, sizeof(float) * NEURAL_FRAMESIZE);
		//ZeroMemory(this->m_fRb, sizeof(float) * NEURAL_FRAMESIZE);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////	Name		:	Convert
	////
	////	Params		:	void * - Source of channels
	////
	////	Return		:	void
	////
	////	Purpose		:	Used to provide conversion support on any platform
	////					* converts any source into this format for use by
	////						the Neural-THX encoder
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//void Convert(void *pSource)
	//{
	//	// TODO : Use this function to transfer your channels into this
	//	//			structure which will later be passed into the encoder
	//}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	Name		:	Initialize
	//
	//	Params		:	void * - Source of channels
	//
	//	Return		:	void
	//
	//	Purpose		:	Used to set all channels in one easy to use function
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//void Initialize(float *fL	= NULL,
	//				float *fC	= NULL,
	//				float *fR	= NULL,
	//				float *fLs	= NULL,
	//				float *fRs	= NULL,
	//				float *fLFE	= NULL,
	//				float *fLb	= NULL,
	//				float *fRb	= NULL)
	//{
	//	// Copy over the information into the structure's variables
	//	if(fL)
	//		FMOD_memcpy(this->m_fL,fL,sizeof(float) * NEURAL_FRAMESIZE);
	//	if(fC)
	//		FMOD_memcpy(this->m_fC,fC,sizeof(float) * NEURAL_FRAMESIZE);
	//	if(fR)
	//		FMOD_memcpy(this->m_fR,fR,sizeof(float) * NEURAL_FRAMESIZE);
	//	if(fLs)
	//		FMOD_memcpy(this->m_fLs,fLs,sizeof(float) * NEURAL_FRAMESIZE);
	//	if(fRs)
	//		FMOD_memcpy(this->m_fRs,fRs,sizeof(float) * NEURAL_FRAMESIZE);
	//	if(fLFE)
	//		FMOD_memcpy(this->m_fLFE,fLFE,sizeof(float) * NEURAL_FRAMESIZE);
	//	if(fLb)
	//		FMOD_memcpy(this->m_fLb,fLb,sizeof(float) * NEURAL_FRAMESIZE);
	//	if(fRb)
	//		FMOD_memcpy(this->m_fRb,fRb,sizeof(float) * NEURAL_FRAMESIZE);
	//}
};


#endif // __NEURAL_THX_GLOBAL__

