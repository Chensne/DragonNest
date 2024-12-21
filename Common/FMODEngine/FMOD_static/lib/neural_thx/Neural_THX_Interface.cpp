
#include "Neural_THX_Interface.h"


NEURAL_THX_ENCODER::NEURAL_THX_ENCODER(void)
{
}

NEURAL_THX_ENCODER::~NEURAL_THX_ENCODER(void)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name		:	Init
//
//	Params		:	Neural_THX_Encoder_Settings - settings structure defining
//						global settings of encoder
//
//	Return		:	int - Neural-THX error code
//
//	Purpose		:	Used to set all channels in one easy to use function
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int NEURAL_THX_ENCODER::Init( Neural_THX_Encoder_Settings &tSettings, Neural_THX_Encoder_Params	&tParams )
{
	// Get prepared to catch errors
	int nErrorHandle = NRLTHX_OK;
	
	// Make sure the settings they passed us are valid
	
	// Check Configuration
	if( tSettings.nChanConfig != NEURAL_THX_7_5_GAMING &&
		tSettings.nChanConfig != NEURAL_THX_6_5_GAMING &&
		tSettings.nChanConfig != NEURAL_THX_7_2_GAMING &&
		tSettings.nChanConfig != NEURAL_THX_5_2_GAMING)
	{
		return UNSUPPORTED_CHANCONFIG;
	}
	// Check Samplerate
	if( tSettings.nSampleRate != SAMPLERATE_32_0 &&
		tSettings.nSampleRate != SAMPLERATE_44_1 &&
		tSettings.nSampleRate != SAMPLERATE_48_0 )
	{
		return UNSUPPORTED_SAMPLERATE;
	}
	// Check Framesize
	if( tSettings.nFramesize != NEURAL_FRAMESIZE)
	{
		return UNSUPPORTED_FRAMESIZE;
	}
	// Double check if we have a valid structure now
	if(!tParams.ValidStruct())
	{
		return UNSUPPORTED_PARAMETER;
	}

	// Branch for Initialization of encoder that the user wants
	switch(tSettings.nChanConfig)
	{
	case NEURAL_THX_7_5_GAMING:
		{
			// The 7 to 5 Encoder Init
			nErrorHandle = Neural_THX_725_Encode_INIT(	tSettings.nFramesize,
														tSettings.nChanConfig,
														tSettings.nSampleRate, 
														(Neural_THX_725_Encode_Struct *)(tParams.pParams));		
		}
		break;
	case NEURAL_THX_5_2_GAMING:
		{
			// The 5 To 2 Encoder Init
			nErrorHandle = Neural_THX_522_Encode_INIT(	tSettings.nFramesize,
														tSettings.nChanConfig,
														tSettings.nSampleRate, 
														(Neural_THX_522_Encode_Struct *)(tParams.pParams));		
		}
		break;
	case NEURAL_THX_6_5_GAMING: // !Supported Yet
		{
			// Temporary until it gets supported
			return UNSUPPORTED_CHANCONFIG;
		}
		break;
	case NEURAL_THX_7_2_GAMING: // !Supported Yet
		{
			nErrorHandle = Neural_THX_722_Encode_INIT(	tSettings.nFramesize,
														tSettings.nChanConfig,
														tSettings.nSampleRate, 
														(Neural_THX_722_Encode_Struct *)(tParams.pParams));		
		}
		break;

	default:
		{
			return UNKNOWN_ERROR;// should never reach this ... should :)
		}
		break;
	};
	// Double check if there was an error :) hopefully not
	NRLTHX_ERRORCHECK(nErrorHandle);
	// return the success (if it reaches here things went well most likely)
	return nErrorHandle;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name		:	Encode
//
//	Params		:	Neural_THX_Channel_Format - Channels in to down mix
//					Neural_THX_Channel_Format - Channels out (results)
//					Neural_THX_Encoder_Settings - settings structure defining
//						global settings of encoder
//
//	Return		:	int - Neural-THX error code
//
//	Purpose		:	Used to pass the information to the right encoder 
//						packaged in the library
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int NEURAL_THX_ENCODER::Encode( Neural_THX_Channel_Format &tChannelsIn,
			Neural_THX_Channel_Format &tChannelsOut,
			Neural_THX_Encoder_Settings &tSettings,
			Neural_THX_Encoder_Params	&tParams)
{
	// Get prepared to catch errors
	int nErrorHandle = NRLTHX_OK;
	// Make sure the settings they passed us are valid
	
	// Check Configuration
	if( tSettings.nChanConfig != NEURAL_THX_7_5_GAMING &&
		tSettings.nChanConfig != NEURAL_THX_6_5_GAMING &&
		tSettings.nChanConfig != NEURAL_THX_7_2_GAMING &&
		tSettings.nChanConfig != NEURAL_THX_5_2_GAMING)
	{
		return UNSUPPORTED_CHANCONFIG;
	}
	// Check Samplerate
	if( tSettings.nSampleRate != SAMPLERATE_32_0 &&
		tSettings.nSampleRate != SAMPLERATE_44_1 &&
		tSettings.nSampleRate != SAMPLERATE_48_0 )
	{
		return UNSUPPORTED_SAMPLERATE;
	}
	// Check Framesize
	if( tSettings.nFramesize != NEURAL_FRAMESIZE)
	{
		return UNSUPPORTED_FRAMESIZE;
	}
	// Check to see if we have a valid buffer
	if(!tParams.ValidStruct())
	{
		return UNSUPPORTED_PARAMETER;
	}

	// Prepare the channels to be sent to the corresponding encoder
	// NOTE: This section will be used if we support interleaved buffers

	// Branch - figure out which encoder the user wants by the settings struct
	// Send off the information to the right encoder and let it
	//	spit back out the results the user wants
	switch(tSettings.nChanConfig)
	{
	case NEURAL_THX_7_5_GAMING:
		{
			// The 7 To 5 Encoder
			nErrorHandle = Neural_THX_725_Encode(	tChannelsIn.m_fL,
													tChannelsIn.m_fR, 
													tChannelsIn.m_fC,
													tChannelsIn.m_fLFE,
													tChannelsIn.m_fLs,
													tChannelsIn.m_fRs,
													tChannelsIn.m_fLb,
													tChannelsIn.m_fRb,
													tChannelsOut.m_fL,
													tChannelsOut.m_fR,
													tChannelsOut.m_fC,
													tChannelsOut.m_fLFE,
													tChannelsOut.m_fLs,
													tChannelsOut.m_fRs,
													tSettings.bUseFinalLimiting,
													tSettings.nFramesize,
													tSettings.nChanConfig,
													tSettings.nSampleRate,
													(Neural_THX_725_Encode_Struct *)(tParams.pParams));
		}
		break;
	case NEURAL_THX_5_2_GAMING:
		{
			// The 5 To 2 Encoder :) 
			nErrorHandle = Neural_THX_522_Encode(	tChannelsIn.m_fL,
													tChannelsIn.m_fR,
													tChannelsIn.m_fC,
													tChannelsIn.m_fLFE,
													tChannelsIn.m_fLs,
													tChannelsIn.m_fRs,
													tChannelsOut.m_fL,
													tChannelsOut.m_fR,
													tSettings.bUseFinalLimiting,
													tSettings.fLFECutOff,
													tSettings.nFramesize,
													tSettings.nChanConfig,
													tSettings.nSampleRate,
													(Neural_THX_522_Encode_Struct *)(tParams.pParams));
		}
		break;
	case NEURAL_THX_6_5_GAMING: // !Supported Yet
		{
			// Temporary until it gets supported
			return UNSUPPORTED_CHANCONFIG;
		}
		break;
	case NEURAL_THX_7_2_GAMING: // !Supported Yet
		{
			nErrorHandle = Neural_THX_722_Encode(	tChannelsIn.m_fL,
													tChannelsIn.m_fR,
													tChannelsIn.m_fC,
													tChannelsIn.m_fLFE,
													tChannelsIn.m_fLs,
													tChannelsIn.m_fRs,
													tChannelsIn.m_fLb,
													tChannelsIn.m_fRb,
													tChannelsOut.m_fL,
													tChannelsOut.m_fR,
													tSettings.bUseFinalLimiting,
													tSettings.fLFECutOff,
													tSettings.nFramesize,
													tSettings.nChanConfig,
													tSettings.nSampleRate,
													(Neural_THX_722_Encode_Struct *)(tParams.pParams));
		}
		break;

	default:
		{
			return UNKNOWN_ERROR;// should never reach this ... should :)
		}
		break;
	};


	// Double check if there was an error :) hopefully not
	NRLTHX_ERRORCHECK(nErrorHandle);
	// return the errors
	return nErrorHandle;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name		:	Shutdown
//
//	Params		:	void
//
//	Return		:	int - Neural-THX error code
//
//	Purpose		:	Used to unitialize structures and release any memory needed
//						by the encoder
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int NEURAL_THX_ENCODER::Shutdown(void)
{
	// Get prepared to catch errors
	int nErrorHandle = NRLTHX_OK;

	// Return the errors
	return nErrorHandle;
}
