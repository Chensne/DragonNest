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
//	Name				:	Neural_THX_Interface.h
//
//	Author(s)			:	Mark Gugler (mgugler@thx.com)
//
//	Created On			:	08/20/2007
//
//	Last Modified		:	03/03/2008
//
//	Version				:	1.61
//
//	References			:	Linked to Neural_THX_Encoder.h & .lib
//
//	Description			:	Interfaces the Encoder packaged in the library
//
//	Revision History	:	08/20/2007	Build basic framework and comment
//										tasks as best as possible
//							08/21/2007	More comments and changes of format
//							08/22/2007	Library attempts to fix linking
//							08/23/2007	Skipping libray for now, filling in
//										functionality of the functions below.
//										Also added in a shutdown to release memory
//							08/28/2007	Adapt the structure of allocating memory
//										to developer's feedback
//							09/04/2007	Update comments and prepare for 722
//										encoder integration
//							02/15/2008	Got 722 encoder so integrating and testing
//							02/26/2008	Making interface compatible with new param
//										structure as we stripped out memory allocations
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __NEURAL_THX_ENCODER__
#define __NEURAL_THX_ENCODER__

#include "Neural_THX_Encoders.h"
#include "Neural_THX_Global.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Macros
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define NRLTHX_FAILED(error) (error == NRLTHX_OK || error == NRLSUR_OK ? false : true) 
#define NRLTHX_ERRORCHECK(error) if(NRLTHX_FAILED(error)){return error;}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name			:	NEURAL_THX_ENCODER
//
//	Created On		:	08/20/2007
//
//	Last Modified	:	08/23/2007
//
//	Description		:	Interface class used to encapsulate all the
//						Neural-THX encoders into one object
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class NEURAL_THX_ENCODER
{
public:
	NEURAL_THX_ENCODER(void);
	virtual ~NEURAL_THX_ENCODER(void);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	Name		:	Init
	//
	//	Params		:	Neural_THX_Encoder_Settings - settings structure defining
	//						global settings of encoder
	//					Neural_THX_Encoder_Params - Parameters structure used by 
	//						the encoder
	//
	//	Return		:	int - Neural-THX error code
	//
	//	Purpose		:	Used to initialize any needs by the encoder
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int Init(Neural_THX_Encoder_Settings &tSettings,Neural_THX_Encoder_Params &tParams);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	Name		:	Encode
	//
	//	Params		:	Neural_THX_Channel_Format - Channels in to down mix
	//					Neural_THX_Channel_Format - Channels out (results)
	//					Neural_THX_Encoder_Settings - settings structure defining
	//						global settings of encoder
	//					Neural_THX_Encoder_Params - Parameters structure used by
	//						the encoder
	//
	//	Return		:	int - Neural-THX error code
	//
	//	Purpose		:	Used to pass the information to the right encoder 
	//						packaged in the library
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int Encode( Neural_THX_Channel_Format	&tChannelsIn,
				Neural_THX_Channel_Format	&tChannelsOut,
				Neural_THX_Encoder_Settings &tSettings,
				Neural_THX_Encoder_Params	&tParams);

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
	int Shutdown(void);
};



#endif //__NEURAL_THX_ENCODER__

