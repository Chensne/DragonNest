//////////////////////////////////////////////////////////////////////////
//		        _____________											//
//		      _________________				___________________			//
//		    _____/~\___________/\_			  |							//
//		  ______/\_/\_________/\____		  |	|	 |	\	/			//
//		 ______/\___/\_______/\______		  |	|	 |	 \ /			//
//		 _____/\_____/\_____/\_______		  |	|----|	  /				//
//		 ____/\_______/\___/\________		  |	|	 |	 / \			//
//		  __/\_________/\_/\________		  |	|	 |	/	\			//
//		   /\___________/~\_______			___________________			//
//		     ___________________										//
//		        _____________											//
//																		//
//																		//
//					Neural-THX (R) Surround Technology					//
//																		//
//				Copyright (c) 2008 Neural Audio & THX Ltd.				//
//																		//
//		Neural Surround is a trademark of Neural Audio Corporation.		//
//		THX is a trademark of THX Ltd., which may be registered in		//
//			some jurisdictions.											//
//																		//
//		All Rights Reserved.											//
//																		//
//		THX & Neural Confidential Information							//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name				:	Neural_THX_Encoders.h
//
//	Author(s)			:	Mark Gugler (mgugler@thx.com)
//							Jeff Thompson (jeff@neuralaudio.com)
//							Aaron Warner
//
//	Created On			:	08/21/2007
//
//	Last Modified		:	03/03/2008
//
//	Version				:	1.61
//
//	References			:	Link to library that is built for the platform you
//							are working on.
//
//	Description			:	Encapsulates all header files needed by each encoder 
//
//	Revision History	:	08/21/2007 Build and hope things start to work
//							08/23/2007 Add in 522 encoders and functionality
//							08/24/2007 Prune unnecessary if checks and clean up
//							09/04/2007 Clean up and double checking things
//							11/28/2007 Modify and strip out unneeded functions
//							12/14/2007 Plugging in new 256 framesize code
//							12/17/2007 Final polishing and tests, integrating
//										kiss_fft back into the encoder.
//							12/19/2007 - 01/29/2008 Testing platform specific FFT
//										implementations
//							02/15/2008 Received 722 encoder and integrating,testing
//										and basic optimizing
//							02/25/2008 Took memory allocation out of the kiss_fft
//										and made sure we were working...
//							08/01/2008 Updating to new Limiter functionality
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __NEURAL_THX_ENCODERS_H__
#define __NEURAL_THX_ENCODERS_H__

#include "THX__kiss_fft_guts.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/*                                                                          */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  NAME                                                                    */
/*      brev_table                                                          */
/*                                                                          */
/*  USAGE                                                                   */
/*      This data is required to bit-reverse the output in the              */
/*      sp_fftSPxSP routine. Paste this array onto your code                */
/*      for use of the routine.                                             */
/*                                                                          */
/* ======================================================================== */

//#pragma DATA_SECTION(BRev, ".tables");
static const unsigned char BRev[64] = {
0x0, 0x20, 0x10, 0x30, 0x8, 0x28, 0x18, 0x38,
0x4, 0x24, 0x14, 0x34, 0xc, 0x2c, 0x1c, 0x3c,
0x2, 0x22, 0x12, 0x32, 0xa, 0x2a, 0x1a, 0x3a,
0x6, 0x26, 0x16, 0x36, 0xe, 0x2e, 0x1e, 0x3e,
0x1, 0x21, 0x11, 0x31, 0x9, 0x29, 0x19, 0x39,
0x5, 0x25, 0x15, 0x35, 0xd, 0x2d, 0x1d, 0x3d,
0x3, 0x23, 0x13, 0x33, 0xb, 0x2b, 0x1b, 0x3b,
0x7, 0x27, 0x17, 0x37, 0xf, 0x2f, 0x1f, 0x3f
};

extern unsigned char * GetBrevTable();

//***************************************************************************//
//*																			*//
//*  Project     : Neural Audio												*//
//*  File        : Common.h													*//
//*  Description : Sets common parameters, options, and variables			*//
//*  Author(s)   : Jeff Thompson											*//
//*																			*//
//*					Copyright (c) Neural Audio Corp. 2007					*//
//*																			*//
//***************************************************************************//

//***************************************************************************//
//*                              DEFINITIONS								*//
//***************************************************************************//
//522 Build
#define NEURAL_522ENCODER_BUILD_MAJOR	03
#define NEURAL_522ENCODER_BUILD_MINOR	00
//725 Build
#define NEURAL_725ENCODER_BUILD_MAJOR	02
#define NEURAL_725ENCODER_BUILD_MINOR	00
//722 Build
#define NEURAL_722ENCODER_BUILD_MAJOR	01
#define NEURAL_722ENCODER_BUILD_MINOR	00

//Only one build type should be defined
#define PC_BUILD

//Only one cross-sharing amount should be defined
#define CROSSSHARE_8DB					//CROSSSHARE_8DB should be defined

//The audio block size used by the Neural encoder
#define	NEURAL_FRAMESIZE				256			//Must be 256 for Downmix

#ifdef PC_BUILD
#define AUDIO_VAL_MAX					2147483647		//32 bit audio
#define AUDIO_VAL_NORM					4.656612875246e-10f
#endif

//Define this flag to perform compensation delay on the front channels
//Removing compensation delay saves some memory requirements
#define COMPENSATION_FRONT_DELAY

//Define supported channel configurations
#define NEURAL_THX_5_2_GAMING			1
#define NEURAL_THX_7_2_GAMING			2				
#define NEURAL_THX_6_5_GAMING			3				//Not currently supported
#define NEURAL_THX_7_5_GAMING			4

//Algorithm constants
#define BACK_CROSS_SHARE				0.35481338923357545843321870226449f

//Define low frequency handling tables
#define NUMLOWFREQBINS	5	//Smooth the low frequency bins to compensate for small framesize
static const float LowFreqSynthApFactors_32k[]		= {	0.0f,	0.22f,	0.47f,	0.58f,	0.71f	};
static const float LowFreqSmoothingFactors_32k[]	= {	0.11f,	0.28f,	0.55f,	0.69f,	0.95f	};
static const float LowFreqSynthApFactors_44k[]		= {	0.0f,	0.3f,	0.65f,	0.9f,	0.98f	};
static const float LowFreqSmoothingFactors_44k[]	= {	0.08f,	0.15f,	0.3f,	0.5f,	0.8f	};
static const float LowFreqSynthApFactors_48k[]		= {	0.0f,	0.33f,	0.7f,	0.98f,	0.999f	};
static const float LowFreqSmoothingFactors_48k[]	= {	0.07f,	0.13f,	0.28f,	0.46f,	0.74f	};

//Define supported sample rates
#define SAMPLERATE_32_0					32000
#define SAMPLERATE_44_1					44100
#define SAMPLERATE_48_0					48000

//Error codes
#define NRLSUR_OK						0
#define BASE_ERROR						-100
#define INIT_ERROR						BASE_ERROR-1
#define UNSUPPORTED_SAMPLERATE			BASE_ERROR-2
#define UNSUPPORTED_CHANCONFIG			BASE_ERROR-3
#define UNSUPPORTED_FRAMESIZE			BASE_ERROR-4
#define UNSUPPORTED_PARAMETER			BASE_ERROR-5
#define UNKNOWN_ERROR					BASE_ERROR-6
#define NRLTHX_OK						NRLSUR_OK

//Some types used by Neural Encoder
#ifndef THX_bool
#define THX_bool  unsigned char
#endif
#ifndef NULL
#define NULL  0
#endif
#ifndef THX_true
#define THX_true  1
#endif
#ifndef THX_false
#define THX_false 0
#endif

//***************************************************************************//
//*																			*//
//*  Project     : Neural Surround											*//
//*  File        : VectorOperations.h										*//
//*  Description : Basic vector (array) operations							*//
//*  Author(s)   : Jeff Thompson											*//
//*																			*//
//*					Copyright (c) Neural Audio Corp. 2006					*//
//*																			*//
//***************************************************************************//

//***************************************************************************//
//*                           FUNCTION PROTOTYPES							*//
//***************************************************************************//

//---------------------------------------------------------------------------//
//-								Function List								-//
//---------------------------------------------------------------------------//
//- Add2(...)																-//
//- Add2_x2(...)															-//
//- Add2_x3(...)															-//
//- Add2_x4(...)															-//
//- Add3(...)																-//
//- Add4(...)																-//
//- Add5(...)																-//
//- Subtract2(...)															-//
//- Subtract2_x2(...)														-//
//- Subtract2_x3(...)														-//
//- Subtract2_x4(...)														-//
//- Multiply2(...)															-//
//- Multiply2_x2(...)														-//
//- Multiply2_x3(...)														-//
//- Multiply2_x4(...)														-//
//- Multiply3(...)															-//
//- Multiply4(...)															-//
//- Multiply5(...)															-//
//- ComplexMultiply2(...)													-//
//- Divide2(...)															-//
//- Divide2_x2(...)															-//
//- Divide2_x3(...)															-//
//- Divide2_x4(...)															-//
//- CopyArray(...)															-//
//- FillArray(...)															-//
//- OffsetArray(...)														-//
//- ScaleArray(...)															-//
//- SquareArray(...)														-//
//- AbsValue(...)															-//
//- MaxClip(...)															-//
//- MinClip(...)															-//
//- SaturateArray(...)														-//
//- ReverseArray(...)														-//
//---------------------------------------------------------------------------//

extern void Add2(float * PtrIn0,float * PtrIn1,float * PtrOut0,unsigned int Framesize);
extern void Add2_x2(float * PtrIn0,float * PtrIn1,float * PtrOut0,float * PtrIn2,float * PtrIn3,float * PtrOut1,unsigned int Framesize);
extern void Add2_x3(float * PtrIn0,float * PtrIn1,float * PtrOut0,float * PtrIn2,float * PtrIn3,float * PtrOut1,float * PtrIn4,float * PtrIn5,float * PtrOut2,unsigned int Framesize);
extern void Add2_x4(float * PtrIn0,float * PtrIn1,float * PtrOut0,float * PtrIn2,float * PtrIn3,float * PtrOut1,float * PtrIn4,float * PtrIn5,float * PtrOut2,float * PtrIn6,float * PtrIn7,float * PtrOut3,unsigned int Framesize);
extern void Add3(float * PtrIn0,float * PtrIn1,float * PtrIn2,float * PtrOut0,unsigned int Framesize);
extern void Add4(float * PtrIn0,float * PtrIn1,float * PtrIn2,float * PtrIn3,float * PtrOut0,unsigned int Framesize);
extern void Add5(float * PtrIn0,float * PtrIn1,float * PtrIn2,float * PtrIn3,float * PtrIn4,float * PtrOut0,unsigned int Framesize);
extern void Subtract2(float * PtrIn0,float * PtrIn1,float * PtrOut0,unsigned int Framesize);
extern void Subtract2_x2(float * PtrIn0,float * PtrIn1,float * PtrOut0,float * PtrIn2,float * PtrIn3,float * PtrOut1,unsigned int Framesize);
extern void Subtract2_x3(float * PtrIn0,float * PtrIn1,float * PtrOut0,float * PtrIn2,float * PtrIn3,float * PtrOut1,float * PtrIn4,float * PtrIn5,float * PtrOut2,unsigned int Framesize);
extern void Subtract2_x4(float * PtrIn0,float * PtrIn1,float * PtrOut0,float * PtrIn2,float * PtrIn3,float * PtrOut1,float * PtrIn4,float * PtrIn5,float * PtrOut2,float * PtrIn6,float * PtrIn7,float * PtrOut3,unsigned int Framesize);
extern void Multiply2(float * PtrIn0,float * PtrIn1,float * PtrOut0,unsigned int Framesize);
extern void Multiply2_x2(float * PtrIn0,float * PtrIn1,float * PtrOut0,float * PtrIn2,float * PtrIn3,float * PtrOut1,unsigned int Framesize);
extern void Multiply2_x3(float * PtrIn0,float * PtrIn1,float * PtrOut0,float * PtrIn2,float * PtrIn3,float * PtrOut1,float * PtrIn4,float * PtrIn5,float * PtrOut2,unsigned int Framesize);
extern void Multiply2_x4(float * PtrIn0,float * PtrIn1,float * PtrOut0,float * PtrIn2,float * PtrIn3,float * PtrOut1,float * PtrIn4,float * PtrIn5,float * PtrOut2,float * PtrIn6,float * PtrIn7,float * PtrOut3,unsigned int Framesize);
extern void Multiply3(float * PtrIn0,float * PtrIn1,float * PtrIn2,float * PtrOut0,unsigned int Framesize);
extern void Multiply4(float * PtrIn0,float * PtrIn1,float * PtrIn2,float * PtrIn3,float * PtrOut0,unsigned int Framesize);
extern void Multiply5(float * PtrIn0,float * PtrIn1,float * PtrIn2,float * PtrIn3,float * PtrIn4,float * PtrOut0,unsigned int Framesize);
extern void ComplexMultiply2(float * PtrInReal0,float * PtrInImag0,float * PtrInReal1,float * PtrInImag1,float * PtrOutReal,float * PtrOutImag,unsigned int Framesize);
extern void Divide2(float * PtrIn0,float * PtrIn1,float * PtrOut0,unsigned int Framesize);
extern void Divide2_x2(float * PtrIn0,float * PtrIn1,float * PtrOut0,float * PtrIn2,float * PtrIn3,float * PtrOut1,unsigned int Framesize);
extern void Divide2_x3(float * PtrIn0,float * PtrIn1,float * PtrOut0,float * PtrIn2,float * PtrIn3,float * PtrOut1,float * PtrIn4,float * PtrIn5,float * PtrOut2,unsigned int Framesize);
extern void Divide2_x4(float * PtrIn0,float * PtrIn1,float * PtrOut0,float * PtrIn2,float * PtrIn3,float * PtrOut1,float * PtrIn4,float * PtrIn5,float * PtrOut2,float * PtrIn6,float * PtrIn7,float * PtrOut3,unsigned int Framesize);
extern void CopyArray(float * PtrIn,float * PtrOut,unsigned int Framesize);
extern void FillArray(float * PtrArray,float FillValue, unsigned int Framesize);
extern void OffsetArray(float * PtrArray,float OffsetValue,unsigned int Framesize);
extern void ScaleArray(float * PtrArray,float ScaleValue,unsigned int Framesize);
extern void SquareArray(float * PtrArray,unsigned int Framesize);
extern void AbsValue(float * PtrArray,unsigned int Framesize);
extern void MaxClip(float * PtrArray,float MaxValue,unsigned int Framesize);
extern void MinClip(float * PtrArray,float MinValue,unsigned int Framesize);
extern void SaturateArray(float * PtrArray,float SaturationValue,unsigned int Framesize);
extern void ReverseArray(float * PtrArray,unsigned int Framesize);

//***************************************************************************//
//*																			*//
//*  Project     : Neural Audio												*//
//*  File        : FreqDomain_PhaseShift.h									*//
//*  Description : Shift a frequency domain signal in phase					*//
//*  Author(s)   : Jeff Thompson											*//
//*																			*//
//*					Copyright (c) Neural Audio Corp. 2007					*//
//*																			*//
//***************************************************************************//

//***************************************************************************//
//*                              DEFINITIONS								*//
//***************************************************************************//
#ifndef PI
#define PI		 3.1415926535897932384626433832795
#endif

//***************************************************************************//
//*							  Phase Shift Struct							*//
//***************************************************************************//
typedef struct
{
	float			Prev_PhaseShift_Degrees;
	float			ShiftScalar_PositiveFreqs_Real;
	float			ShiftScalar_PositiveFreqs_Imag;

} FreqDomain_PhaseShift_Struct;

//***************************************************************************//
//*                           FUNCTION PROTOTYPES							*//
//***************************************************************************//
//-----
//Phase shift by -90 degrees causes the output to lag the input by 90 degrees (similar to Hilbert Transform)
//Phase shift by +90 degrees causes the output to lead the input by 90 degrees
//-----
extern int FreqDomain_PhaseShift_INIT(float PhaseShift_Degrees,unsigned int Framesize,FreqDomain_PhaseShift_Struct * pPtr);
extern int FreqDomain_PhaseShift(float * PtrInReal,float * PtrInImag,float * PtrOutReal,float * PtrOutImag,float PhaseShift_Degrees,	unsigned int Framesize,unsigned int SampleRate,FreqDomain_PhaseShift_Struct * pPtr);

///////////////////////////////////////////////////////////////////////////////
///  FFT   ////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define swap(a,b) tempr=(a); (a)=(b); (b)=tempr;

//Sine Window 512 coefficients
//Jeff Thompson, Neural Audio
//Generated from script: 8-7-2007 14:8.14
static const float SineWindow[] = {
	0.003067956763f,	0.009203754782f,	0.015339206285f,	0.021474080275f,
	0.027608145779f,	0.033741171851f,	0.039872927588f,	0.046003182131f,
	0.052131704680f,	0.058258264500f,	0.064382630930f,	0.070504573390f,
	0.076623861392f,	0.082740264549f,	0.088853552583f,	0.094963495330f,
	0.101069862755f,	0.107172424957f,	0.113270952178f,	0.119365214811f,
	0.125454983412f,	0.131540028703f,	0.137620121586f,	0.143695033150f,
	0.149764534677f,	0.155828397654f,	0.161886393780f,	0.167938294975f,
	0.173983873387f,	0.180022901406f,	0.186055151663f,	0.192080397050f,
	0.198098410718f,	0.204108966093f,	0.210111836880f,	0.216106797076f,
	0.222093620973f,	0.228072083171f,	0.234041958584f,	0.240003022449f,
	0.245955050336f,	0.251897818154f,	0.257831102162f,	0.263754678975f,
	0.269668325573f,	0.275571819311f,	0.281464937926f,	0.287347459545f,
	0.293219162694f,	0.299079826308f,	0.304929229735f,	0.310767152750f,
	0.316593375556f,	0.322407678801f,	0.328209843579f,	0.333999651442f,
	0.339776884407f,	0.345541324964f,	0.351292756086f,	0.357030961233f,
	0.362755724367f,	0.368466829953f,	0.374164062971f,	0.379847208924f,
	0.385516053844f,	0.391170384302f,	0.396809987417f,	0.402434650859f,
	0.408044162865f,	0.413638312238f,	0.419216888363f,	0.424779681209f,
	0.430326481340f,	0.435857079922f,	0.441371268732f,	0.446868840162f,
	0.452349587234f,	0.457813303599f,	0.463259783552f,	0.468688822036f,
	0.474100214651f,	0.479493757660f,	0.484869248001f,	0.490226483288f,
	0.495565261826f,	0.500885382611f,	0.506186645345f,	0.511468850438f,
	0.516731799018f,	0.521975292937f,	0.527199134782f,	0.532403127877f,
	0.537587076296f,	0.542750784865f,	0.547894059173f,	0.553016705580f,
	0.558118531221f,	0.563199344014f,	0.568258952670f,	0.573297166698f,
	0.578313796412f,	0.583308652938f,	0.588281548223f,	0.593232295040f,
	0.598160706996f,	0.603066598540f,	0.607949784968f,	0.612810082429f,
	0.617647307938f,	0.622461279374f,	0.627251815495f,	0.632018735940f,
	0.636761861236f,	0.641481012809f,	0.646176012983f,	0.650846684996f,
	0.655492853000f,	0.660114342067f,	0.664710978203f,	0.669282588347f,
	0.673829000379f,	0.678350043130f,	0.682845546385f,	0.687315340892f,
	0.691759258364f,	0.696177131491f,	0.700568793943f,	0.704934080376f,
	0.709272826439f,	0.713584868781f,	0.717870045056f,	0.722128193929f,
	0.726359155084f,	0.730562769228f,	0.734738878096f,	0.738887324461f,
	0.743007952135f,	0.747100605980f,	0.751165131910f,	0.755201376897f,
	0.759209188978f,	0.763188417263f,	0.767138911936f,	0.771060524262f,
	0.774953106595f,	0.778816512381f,	0.782650596167f,	0.786455213599f,
	0.790230221437f,	0.793975477554f,	0.797690840943f,	0.801376171723f,
	0.805031331143f,	0.808656181588f,	0.812250586585f,	0.815814410807f,
	0.819347520077f,	0.822849781376f,	0.826321062846f,	0.829761233795f,
	0.833170164702f,	0.836547727224f,	0.839893794196f,	0.843208239642f,
	0.846490938774f,	0.849741768001f,	0.852960604930f,	0.856147328375f,
	0.859301818357f,	0.862423956111f,	0.865513624091f,	0.868570705971f,
	0.871595086656f,	0.874586652278f,	0.877545290207f,	0.880470889052f,
	0.883363338666f,	0.886222530149f,	0.889048355855f,	0.891840709392f,
	0.894599485631f,	0.897324580705f,	0.900015892016f,	0.902673318237f,
	0.905296759318f,	0.907886116488f,	0.910441292258f,	0.912962190428f,
	0.915448716088f,	0.917900775621f,	0.920318276709f,	0.922701128334f,
	0.925049240783f,	0.927362525650f,	0.929640895843f,	0.931884265582f,
	0.934092550404f,	0.936265667170f,	0.938403534063f,	0.940506070593f,
	0.942573197601f,	0.944604837261f,	0.946600913083f,	0.948561349916f,
	0.950486073949f,	0.952375012720f,	0.954228095109f,	0.956045251350f,
	0.957826413028f,	0.959571513082f,	0.961280485811f,	0.962953266874f,
	0.964589793290f,	0.966190003445f,	0.967753837093f,	0.969281235357f,
	0.970772140729f,	0.972226497079f,	0.973644249651f,	0.975025345067f,
	0.976369731330f,	0.977677357825f,	0.978948175319f,	0.980182135968f,
	0.981379193314f,	0.982539302287f,	0.983662419212f,	0.984748501802f,
	0.985797509168f,	0.986809401814f,	0.987784141645f,	0.988721691960f,
	0.989622017463f,	0.990485084256f,	0.991310859846f,	0.992099313142f,
	0.992850414460f,	0.993564135521f,	0.994240449453f,	0.994879330795f,
	0.995480755492f,	0.996044700901f,	0.996571145791f,	0.997060070339f,
	0.997511456140f,	0.997925286199f,	0.998301544934f,	0.998640218180f,
	0.998941293187f,	0.999204758618f,	0.999430604555f,	0.999618822495f,
	0.999769405351f,	0.999882347454f,	0.999957644552f,	0.999995293810f,
};

extern float * GetSineWindow();

//***************************************************************************//
//*																			*//
//*  Project     : Neural Surround											*//
//*  File        : FFT_Overlapped_Stereo.h									*//
//*  Description : A windowed and overlapped FFT and IFFT					*//
//*  Author(s)   : Jeff Thompson											*//
//*																			*//
//*					Copyright (c) Neural Audio Corp. 2007					*//
//*																			*//
//***************************************************************************//

//***************************************************************************//
//*                              DEFINITIONS								*//
//***************************************************************************//
#if NEURAL_FRAMESIZE == 1024
#define  FFTSIZE		2048
#define  HALFFFTSIZE	1024
#elif NEURAL_FRAMESIZE == 256
#define  FFTSIZE		512
#define  HALFFFTSIZE	256
#endif

#ifndef FFT_MEM_CHUNK
#define FFT_MEM_CHUNK	4360	// Amount of Memory the Kiss_FFT needs
static char fft_buff[FFT_MEM_CHUNK];
#endif

//***************************************************************************//
//*							Forward FFT Structure							*//
//***************************************************************************//
typedef struct
{
	float			pOverlappedBuffer0[HALFFFTSIZE];
	float			pOverlappedBuffer1[HALFFFTSIZE];
	float			* pSineWin;

	float			* pReal;	//Size must be 2*Framesize
	float			* pImag;	//Size must be 2*Framesize

	THX_kiss_fft_cfg	state;
	THX_kiss_fft_cpx	pIn[FFTSIZE];
	THX_kiss_fft_cpx	pOut[FFTSIZE];
} FFT_Overlapped_Stereo_Struct;

//***************************************************************************//
//*							Inverse FFT Structure							*//
//***************************************************************************//
typedef struct
{
	float			pOverlappedBuffer0[HALFFFTSIZE];
	float			pOverlappedBuffer1[HALFFFTSIZE];
	float			* pSineWin;

	float			* pReal;	//Size must be 2*Framesize
	float			* pImag;	//Size must be 2*Framesize

	THX_kiss_fft_cfg	state;
	THX_kiss_fft_cpx	pIn[FFTSIZE];
	THX_kiss_fft_cpx	pOut[FFTSIZE];
} IFFT_Overlapped_Stereo_Struct;


//***************************************************************************//
//*                           FUNCTION PROTOTYPES							*//
//***************************************************************************//

extern int FFT_Overlapped_Stereo_INIT(FFT_Overlapped_Stereo_Struct * pPersistent,float * pTempBuffer0,float * pTempBuffer1,unsigned int Framesize);
extern int FFT_Overlapped_Stereo(float * PtrIn0,float * PtrOutReal0,float * PtrOutImag0,float * PtrIn1,float * PtrOutReal1,float * PtrOutImag1,unsigned int Framesize,FFT_Overlapped_Stereo_Struct * pPersistent);
extern int IFFT_Overlapped_Stereo_INIT(IFFT_Overlapped_Stereo_Struct * pPersistent,float * pTempBuffer0,float * pTempBuffer1,unsigned int Framesize);
extern int IFFT_Overlapped_Stereo(float * PtrInReal0,float * PtrInImag0,float * PtrOut0,float * PtrInReal1,float * PtrInImag1,float * PtrOut1,unsigned int Framesize,IFFT_Overlapped_Stereo_Struct * pPersistent);

//***************************************************************************//
//*																			*//
//*  Project     : Neural Audio												*//          
//*  File        : Delay.h													*//
//*  Description : Fixed delay												*//
//*  Author(s)   : Jeff Thompson											*//
//*																			*//
//*					Copyright (c) Neural Audio Corp. 2007					*//
//*																			*//
//***************************************************************************//

typedef struct
{
//***************************************************************************//
//*                   Internal Persistent Variables							*//
//***************************************************************************//
	float			Buffer[NEURAL_FRAMESIZE+1];
	unsigned int	Pntr;
	unsigned int	DelayAmount;
} Delay_Struct;

//***************************************************************************//
//*                           FUNCTION PROTOTYPES							*//
//***************************************************************************//

extern int Delay_INIT(unsigned int DelayAmount,Delay_Struct * pPtr);
extern int Delay(float * PtrIn,float * PtrOut,unsigned int Framesize,Delay_Struct * pPtr);

//***************************************************************************//
//*																			*//
//*  Project     : Neural Audio												*//
//*  File        : LR4_LP.h													*//
//*  Description : Linkwitz-Riley 4th order Low-Pass filter					*//
//*  Author(s)   : Jeff Thompson											*//
//*																			*//
//*					Copyright (c) Neural Audio Corp. 2005					*//
//*																			*//
//***************************************************************************//

//***************************************************************************//
//*                              DEFINITIONS								*//
//***************************************************************************//
#ifndef PI
#define PI		3.1415926535897932384626433832795
#endif

#define	TWOPOWTHREEHALVES	2.8284271247461900976033774484194

typedef struct
{
//***************************************************************************//
//*                   Internal Persistent Variables							*//
//***************************************************************************//
	float   PrevCutOff;
	float   PrevStage00;
	float   PrevStage01;
	float   PrevStage10;
	float   PrevStage11;
	float   b0;
	float   b1;
	float   b2;
	float   a0;
	float   a1;
	float	PrevSampleRate;

} LR4_LP_Struct;

//***************************************************************************//
//*                           FUNCTION PROTOTYPES							*//
//***************************************************************************//

extern void LR4_LP_INIT(LR4_LP_Struct * pPtr);
extern void LR4_LP(float * PtrIn,float * PtrOut,float Cutoff,unsigned int Framesize,unsigned int SampleRate,LR4_LP_Struct * pPtr);

//***************************************************************************//
//*																			*//
//*  Project     : Neural Surround											*//
//*  File        : FFT_Overlapped.h											*//
//*  Description : A windowed and overlapped FFT and IFFT					*//
//*  Author(s)   : Jeff Thompson											*//
//*																			*//
//*					Copyright (c) Neural Audio Corp. 2007					*//
//*																			*//
//***************************************************************************//

//***************************************************************************//
//*                              DEFINITIONS								*//
//***************************************************************************//
#if NEURAL_FRAMESIZE == 1024
#define  FFTSIZE		2048
#define  HALFFFTSIZE	1024
#elif NEURAL_FRAMESIZE == 256
#define  FFTSIZE		512
#define  HALFFFTSIZE	256
#endif

#ifndef FFT_MEM_CHUNK
#define FFT_MEM_CHUNK	4360	// Amount of Memory the Kiss_FFT needs
static char fft_buff[FFT_MEM_CHUNK];
#endif

//***************************************************************************//
//*							Forward FFT Structure							*//
//***************************************************************************//
typedef struct
{
	float			pOverlappedBuffer[HALFFFTSIZE];
	float			* pSineWin;

	float			* pReal;	//Size must be 2*Framesize
	float			* pImag;	//Size must be 2*Framesize

	THX_kiss_fft_cfg	state;
	THX_kiss_fft_cpx	pIn[FFTSIZE];
	THX_kiss_fft_cpx	pOut[FFTSIZE];
} FFT_Overlapped_Struct;

//***************************************************************************//
//*							Inverse FFT Structure							*//
//***************************************************************************//
typedef struct
{
	float			pOverlappedBuffer[HALFFFTSIZE];
	float			* pSineWin;

	float			* pReal;	//Size must be 2*Framesize
	float			* pImag;	//Size must be 2*Framesize

	THX_kiss_fft_cfg	state;
	THX_kiss_fft_cpx	pIn[FFTSIZE];
	THX_kiss_fft_cpx	pOut[FFTSIZE];
} IFFT_Overlapped_Struct;


//***************************************************************************//
//*                           FUNCTION PROTOTYPES							*//
//***************************************************************************//

extern int FFT_Overlapped_INIT(FFT_Overlapped_Struct * pPersistent,float * pTempBuffer0,float * pTempBuffer1,unsigned int Framesize);
extern int FFT_Overlapped(float * PtrIn,float * PtrOutReal,float * PtrOutImag,unsigned int Framesize,FFT_Overlapped_Struct * pPersistent);
extern int IFFT_Overlapped_INIT(IFFT_Overlapped_Struct * pPersistent,float * pTempBuffer0,	float * pTempBuffer1,	unsigned int Framesize);
extern int IFFT_Overlapped(float * PtrInReal,float * PtrInImag,float * PtrOut,unsigned int Framesize,IFFT_Overlapped_Struct * pPersistent);

//***************************************************************************//
//*																			*//
//*  Project     : Neural Audio												*//
//*  File        : FastMathApprox.h											*//
//*  Description : Fast math approximations									*//
//*  Author(s)   : Jeff Thompson											*//
//*																			*//
//*					Copyright (c) Neural Audio Corp. 2007					*//
//*																			*//
//***************************************************************************//
#define USE_MAG_APPROX
#define USE_PHASE_APPROX
#define USE_SQRT_APPROX
#define USE_LOG_APPROX
#define USE_POW_APPROX
#define USE_SIN_APPROX
#define USE_COS_APPROX

//***************************************************************************//
//*                              DEFINITIONS                                *//
//***************************************************************************//
#ifndef PI
#define PI	3.1415926535897932384626434
#endif

typedef float	ieee754_float32_t;

#define			LOG2_SIZE		512
#define			LOG2_SIZE_L2	9

#define			LOG2			0.69314718055994530942f
#define			LOG10			2.30258509299404568402f
#define			LOG2OVERLOG10	0.30102999566398119521f

#define			EXP_A			1512775.39519518569384f
#define			EXP_C			45799//60801//

static const float LogTable[] = {
0.000000000000f,	0.002815015614f,	0.005624548998f,	0.008428622037f,	0.011227255687f,	0.014020469971f,
0.016808288172f,	0.019590727985f,	0.022367812693f,	0.025139562786f,	0.027905996889f,	0.030667135492f,
0.033423002809f,	0.036173611879f,	0.038918990642f,	0.041659150273f,	0.044394120574f,	0.047123912722f,
0.049848549068f,	0.052568051964f,	0.055282436311f,	0.057991724461f,	0.060695931315f,	0.063395082951f,
0.066089190543f,	0.068778276443f,	0.071462363005f,	0.074141465127f,	0.076815597713f,	0.079484783113f,
0.082149043679f,	0.084808386862f,	0.087462842464f,	0.090112417936f,	0.092757143080f,	0.095397025347f,
0.098032079637f,	0.100662335753f,	0.103287808597f,	0.105908505619f,	0.108524456620f,	0.111135669053f,
0.113742165267f,	0.116343960166f,	0.118941076100f,	0.121533520520f,	0.124121308327f,	0.126704469323f,
0.129283010960f,	0.131856963038f,	0.134426325560f,	0.136991113424f,	0.139551356435f,	0.142107054591f,
0.144658237696f,	0.147204920650f,	0.149747118354f,	0.152284845710f,	0.154818102717f,	0.157346934080f,
0.159871339798f,	0.162391334772f,	0.164906933904f,	0.167418152094f,	0.169925004244f,	0.172427505255f,
0.174925684929f,	0.177419543266f,	0.179909095168f,	0.182394355536f,	0.184875339270f,	0.187352076173f,
0.189824566245f,	0.192292809486f,	0.194756850600f,	0.197216689587f,	0.199672341347f,	0.202123820782f,
0.204571142793f,	0.207014322281f,	0.209453359246f,	0.211888298392f,	0.214319124818f,	0.216745853424f,
0.219168514013f,	0.221587121487f,	0.224001675844f,	0.226412191987f,	0.228818684816f,	0.231221184134f,
0.233619675040f,	0.236014187336f,	0.238404735923f,	0.240791335702f,	0.243173986673f,	0.245552703738f,
0.247927516699f,	0.250298410654f,	0.252665430307f,	0.255028575659f,	0.257387846708f,	0.259743273258f,
0.262094855309f,	0.264442592859f,	0.266786545515f,	0.269126683474f,	0.271463036537f,	0.273795604706f,
0.276124417782f,	0.278449445963f,	0.280770778656f,	0.283088356256f,	0.285402208567f,	0.287712365389f,
0.290018856525f,	0.292321622372f,	0.294620752335f,	0.296916216612f,	0.299208015203f,	0.301496207714f,
0.303780734539f,	0.306061685085f,	0.308339029551f,	0.310612767935f,	0.312882959843f,	0.315149575472f,
0.317412614822f,	0.319672107697f,	0.321928083897f,	0.324180543423f,	0.326429486275f,	0.328674912453f,
0.330916881561f,	0.333155363798f,	0.335390359163f,	0.337621897459f,	0.339850008488f,	0.342074662447f,
0.344295918941f,	0.346513718367f,	0.348728150129f,	0.350939184427f,	0.353146821260f,	0.355351090431f,
0.357551991940f,	0.359749555588f,	0.361943781376f,	0.364134669304f,	0.366322219372f,	0.368506461382f,
0.370687395334f,	0.372865051031f,	0.375039428473f,	0.377210527658f,	0.379378378391f,	0.381542950869f,
0.383704304695f,	0.385862410069f,	0.388017296791f,	0.390168964863f,	0.392317414284f,	0.394462704659f,
0.396604776382f,	0.398743689060f,	0.400879442692f,	0.403012037277f,	0.405141472816f,	0.407267779112f,
0.409390926361f,	0.411510974169f,	0.413627922535f,	0.415741771460f,	0.417852520943f,	0.419960170984f,
0.422064751387f,	0.424166291952f,	0.426264762878f,	0.428360164165f,	0.430452555418f,	0.432541906834f,
0.434628218412f,	0.436711549759f,	0.438791841269f,	0.440869182348f,	0.442943483591f,	0.445014834404f,
0.447083234787f,	0.449148654938f,	0.451211124659f,	0.453270643950f,	0.455327212811f,	0.457380890846f,
0.459431618452f,	0.461479455233f,	0.463524371386f,	0.465566396713f,	0.467605561018f,	0.469641804695f,
0.471675217152f,	0.473705738783f,	0.475733429193f,	0.477758258581f,	0.479780256748f,	0.481799423695f,
0.483815789223f,	0.485829323530f,	0.487840026617f,	0.489847958088f,	0.491853088140f,	0.493855446577f,
0.495855033398f,	0.497851848602f,	0.499845892191f,	0.501837193966f,	0.503825724125f,	0.505811572075f,
0.507794618607f,	0.509774982929f,	0.511752665043f,	0.513727605343f,	0.515699863434f,	0.517669379711f,
0.519636273384f,	0.521600425243f,	0.523561954498f,	0.525520801544f,	0.527477025986f,	0.529430568218f,
0.531381487846f,	0.533329725266f,	0.535275399685f,	0.537218391895f,	0.539158821106f,	0.541096627712f,
0.543031811714f,	0.544964432716f,	0.546894431114f,	0.548821926117f,	0.550746798515f,	0.552669107914f,
0.554588854313f,	0.556506037712f,	0.558420717716f,	0.560332834721f,	0.562242448330f,	0.564149498940f,
0.566054046154f,	0.567956089973f,	0.569855630398f,	0.571752667427f,	0.573647201061f,	0.575539231300f,
0.577428817749f,	0.579315960407f,	0.581200599670f,	0.583082795143f,	0.584962487221f,	0.586839795113f,
0.588714659214f,	0.590587079525f,	0.592457056046f,	0.594324588776f,	0.596189737320f,	0.598052501678f,
0.599912822247f,	0.601770758629f,	0.603626370430f,	0.605479538441f,	0.607330322266f,	0.609178721905f,
0.611024796963f,	0.612868487835f,	0.614709854126f,	0.616548836231f,	0.618385493755f,	0.620219826698f,
0.622051835060f,	0.623881518841f,	0.625708818436f,	0.627533912659f,	0.629356622696f,	0.631177067757f,
0.632995188236f,	0.634811043739f,	0.636624634266f,	0.638435900211f,	0.640244960785f,	0.642051696777f,
0.643856167793f,	0.645658433437f,	0.647458434105f,	0.649256169796f,	0.651051700115f,	0.652844965458f,
0.654636025429f,	0.656424880028f,	0.658211469650f,	0.659995913506f,	0.661778092384f,	0.663558125496f,
0.665335893631f,	0.667111515999f,	0.668884992599f,	0.670656263828f,	0.672425329685f,	0.674192249775f,
0.675957024097f,	0.677719652653f,	0.679480075836f,	0.681238412857f,	0.682994604111f,	0.684748649597f,
0.686500549316f,	0.688250303268f,	0.689997971058f,	0.691743493080f,	0.693486928940f,	0.695228278637f,
0.696967542171f,	0.698704659939f,	0.700439691544f,	0.702172696590f,	0.703903555870f,	0.705632388592f,
0.707359135151f,	0.709083795547f,	0.710806429386f,	0.712526977062f,	0.714245498180f,	0.715961992741f,
0.717676401138f,	0.719388842583f,	0.721099197865f,	0.722807526588f,	0.724513828754f,	0.726218163967f,
0.727920472622f,	0.729620754719f,	0.731319010258f,	0.733015298843f,	0.734709620476f,	0.736401915550f,
0.738092243671f,	0.739780604839f,	0.741466999054f,	0.743151366711f,	0.744833827019f,	0.746514320374f,
0.748192846775f,	0.749869406223f,	0.751544058323f,	0.753216743469f,	0.754887521267f,	0.756556332111f,
0.758223235607f,	0.759888172150f,	0.761551260948f,	0.763212382793f,	0.764871597290f,	0.766528904438f,
0.768184304237f,	0.769837856293f,	0.771489441395f,	0.773139178753f,	0.774787068367f,	0.776433050632f,
0.778077125549f,	0.779719352722f,	0.781359732151f,	0.782998204231f,	0.784634828568f,	0.786269605160f,
0.787902534008f,	0.789533674717f,	0.791162908077f,	0.792790293694f,	0.794415891171f,	0.796039581299f,
0.797661542892f,	0.799281597137f,	0.800899922848f,	0.802516341209f,	0.804131031036f,	0.805743873119f,
0.807354927063f,	0.808964192867f,	0.810571610928f,	0.812177300453f,	0.813781201839f,	0.815383315086f,
0.816983640194f,	0.818582177162f,	0.820178985596f,	0.821774005890f,	0.823367238045f,	0.824958741665f,
0.826548516750f,	0.828136503696f,	0.829722762108f,	0.831307232380f,	0.832890033722f,	0.834471046925f,
0.836050331593f,	0.837627947330f,	0.839203774929f,	0.840777933598f,	0.842350363731f,	0.843921065331f,
0.845490038395f,	0.847057342529f,	0.848622918129f,	0.850186824799f,	0.851749062538f,	0.853309571743f,
0.854868412018f,	0.856425523758f,	0.857980966568f,	0.859534800053f,	0.861086905003f,	0.862637341022f,
0.864186167717f,	0.865733265877f,	0.867278754711f,	0.868822574615f,	0.870364725590f,	0.871905267239f,
0.873444139957f,	0.874981343746f,	0.876516938210f,	0.878050923347f,	0.879583239555f,	0.881113946438f,
0.882643043995f,	0.884170532227f,	0.885696351528f,	0.887220621109f,	0.888743221760f,	0.890264272690f,
0.891783714294f,	0.893301546574f,	0.894817769527f,	0.896332383156f,	0.897845447063f,	0.899356901646f,
0.900866806507f,	0.902375102043f,	0.903881847858f,	0.905386984348f,	0.906890571117f,	0.908392608166f,
0.909893095493f,	0.911391973495f,	0.912889361382f,	0.914385139942f,	0.915879368782f,	0.917372107506f,
0.918863236904f,	0.920352876186f,	0.921840965748f,	0.923327505589f,	0.924812495708f,	0.926295995712f,
0.927777945995f,	0.929258406162f,	0.930737316608f,	0.932214736938f,	0.933690667152f,	0.935165047646f,
0.936637938023f,	0.938109338284f,	0.939579188824f,	0.941047608852f,	0.942514479160f,	0.943979918957f,
0.945443809032f,	0.946906268597f,	0.948367238045f,	0.949826717377f,	0.951284706593f,	0.952741265297f,
0.954196333885f,	0.955649912357f,	0.957102060318f,	0.958552718163f,	0.960001945496f,	0.961449682713f,
0.962895989418f,	0.964340865612f,	0.965784311295f,	0.967226266861f,	0.968666791916f,	0.970105886459f,
0.971543550491f,	0.972979784012f,	0.974414587021f,	0.975847959518f,	0.977279901505f,	0.978710472584f,
0.980139553547f,	0.981567263603f,	0.982993602753f,	0.984418451786f,	0.985841929913f,	0.987264037132f,
0.988684713840f,	0.990103960037f,	0.991521835327f,	0.992938339710f,	0.994353413582f,	0.995767176151f,
0.997179508209f,	0.998590409756f,	1.000000000000f,	
};

//***************************************************************************//
//*                           FUNCTION PROTOTYPES                           *//
//***************************************************************************//

//----- Magnitude approximations -----
extern float FastMag(float r, float i);				//Good accuracy
extern void FastMag_ARRAY(float * PtrInRe,float * PtrInIm,float * PtrOut,unsigned int Framesize);

//----- Fast phase approximations -----
typedef struct
{
	float * Quadrant;
	float * Numerator;
	float * Denominator;
	
} FastPhase_Struct;

extern void FastPhase_INIT(FastPhase_Struct * pParams,float * TempBuffer0,float * TempBuffer1,float * TempBuffer2);
extern float FastPhase(float r, float i);				//Good accuracy
extern void FastPhase_ARRAY(float * PtrInRe,float * PtrInIm,float * PtrOutPhase,unsigned int Framesize,FastPhase_Struct * pParams);
//----- Square root approximations -----
extern float FastSqrt(float x);						//Good accuracy
extern void FastSqrt_ARRAY(float * PtrIn,float * PtrOut,unsigned int Framesize);
//----- Log approximations -----
extern float FastLog2(float x);						//Good accuracy
extern void FastLog2_ARRAY(float * PtrIn,float * PtrOut,unsigned int Framesize);
extern float FastLog10(float x);						//Good accuracy
extern void FastLog10_ARRAY(float * PtrIn,	float * PtrOut,unsigned int Framesize);
extern float FastLogln(float x);						//Good accuracy
extern void FastLogln_ARRAY(float * PtrIn,	float * PtrOut,unsigned int Framesize);
//----- Pow approximations -----
extern float FastPow(float x, float y);				//Okay accuracy
extern void FastPow_ARRAY(float * PtrIn0,float * PtrIn1,float * PtrOut,unsigned int Framesize);
extern float FastPow2(float x);						//Okay accuracy
extern void FastPow2_ARRAY(float * PtrIn,float * PtrOut,unsigned int Framesize);
extern float FastPow10(float x);						//Okay accuracy
extern void FastPow10_ARRAY(float * PtrIn,	float * PtrOut,unsigned int Framesize);
//----- Exponential approximations -----
extern float FastExp(float x);							//Okay accuracy
extern void FastExp_ARRAY(float * PtrIn,float * PtrOut,unsigned int Framesize);
//----- Trig approximations -----
//Input must be between -PI <= x <= PI
extern float FastSin(float x);							//Good accuracy
extern void FastSin_ARRAY(float * PtrIn,float * PtrOut,unsigned int Framesize);
//Input must be between -PI <= x <= PI
extern float FastCos(float x);							//Good accuracy
extern void FastCos_ARRAY(float * PtrIn,float * PtrOut,unsigned int Framesize);

//***************************************************************************//
//*																			*//
//*  Project     : Neural Audio												*//
//*  File        : PeakConverter.h											*//
//*  Description : Peak converter with rise time / fall time				*//
//*  Author(s)   : Jeff Thompson											*//
//*																			*//
//*					Copyright (c) Neural Audio Corp. 2007					*//
//*																			*//
//***************************************************************************//

//***************************************************************************//
//*                        Peak Converter Struct                            *//
//***************************************************************************//
typedef struct
{
	unsigned int SampleRate;
	float RiseTime;
	float FallTime;
	float MinValue;
	float MaxValue;
	float PrevValue;
	float RiseSlew;
	float FallSlew;

} PeakConverter_Struct;

//***************************************************************************//
//*                           FUNCTION PROTOTYPES							*//
//***************************************************************************//
extern int PeakConverter_INIT(float RiseTime,float FallTime,float MinValue,float MaxValue,unsigned int SampleRate,PeakConverter_Struct * pParams);
extern int PeakConverter(float * PtrIn,float * PtrOut,float RiseTime,float FallTime,float MinValue,float MaxValue,unsigned int Framesize,unsigned int SampleRate,PeakConverter_Struct * pParams);

//***************************************************************************//
//*																			*//
//*  Project     : Neural Audio												*//
//*  File        : Limiter.h												*//
//*  Description : Limiter													*//
//*  Author(s)   : Jeff Thompson											*//
//*																			*//
//*					Copyright (c) Neural Audio Corp. 2007					*//
//*																			*//
//***************************************************************************//

//***************************************************************************//
//*                              DEFINITIONS								*//
//***************************************************************************//
typedef struct
{
	PeakConverter_Struct EnvSmoother;
	float * TempBuffer;

} Limiter_Struct;

//***************************************************************************//
//*                           FUNCTION PROTOTYPES							*//
//***************************************************************************//

extern int Limiter_INIT(float * TempBuffer,Limiter_Struct * pPtr);
extern int Limiter(float * AudioIn,float * AudioOut,float MaxAudioValue,float Knee,	float Ceiling,float RiseTime,float FallTime,unsigned int Framesize,unsigned int SampleRate,Limiter_Struct * pPtr);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////   Neural Encoders
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//***************************************************************************//
//*																			*//
//*  Project     : Neural Audio - THX										*//
//*  File        : Neural_THX_725_Encode.h									*//
//*  Description : Downmixer for Neural - THX gaming mode from 7.1 to 5.1	*//
//*  Author(s)   : Jeff Thompson											*//
//*																			*//
//*					Copyright (c) Neural Audio Corp. 2007					*//
//*																			*//
//***************************************************************************//

//***************************************************************************//
//*                              DEFINITIONS                                *//
//***************************************************************************//

//***************************************************************************//
//*                           Downmixer structure                           *//
//***************************************************************************//
typedef struct
{
	//FFT transforms for input/output channels
	FFT_Overlapped_Stereo_Struct Transform_LsRs;
	FFT_Overlapped_Stereo_Struct Transform_LbRb;
	IFFT_Overlapped_Stereo_Struct InvTransform_LsRs;

	//Phase shift for back channels
	FreqDomain_PhaseShift_Struct PhaseShift_Pos;
	FreqDomain_PhaseShift_Struct PhaseShift_Neg;

#ifdef COMPENSATION_FRONT_DELAY
	//Delays for all other channels
	Delay_Struct DelayChanL;
	Delay_Struct DelayChanR;
	Delay_Struct DelayChanC;
	Delay_Struct DelayChanLFE;
#endif

	//Final limiters
	Limiter_Struct FinalLimiterLs;
	Limiter_Struct FinalLimiterRs;

	float Input_RealLs[NEURAL_FRAMESIZE];
	float Input_ImagLs[NEURAL_FRAMESIZE];
	float Input_RealRs[NEURAL_FRAMESIZE];
	float Input_ImagRs[NEURAL_FRAMESIZE];
	float Input_RealLb[NEURAL_FRAMESIZE];
	float Input_ImagLb[NEURAL_FRAMESIZE];
	float Input_RealRb[NEURAL_FRAMESIZE];
	float Input_ImagRb[NEURAL_FRAMESIZE];

	float Dmix_RealLs[NEURAL_FRAMESIZE];
	float Dmix_ImagLs[NEURAL_FRAMESIZE];
	float Dmix_RealRs[NEURAL_FRAMESIZE];
	float Dmix_ImagRs[NEURAL_FRAMESIZE];

	//Temporary buffers
	float Placeholder;
	float TempBuffer0[4*NEURAL_FRAMESIZE];
	float TempBuffer1[4*NEURAL_FRAMESIZE];

} Neural_THX_725_Encode_Struct;

//***************************************************************************//
//*                           FUNCTION PROTOTYPES							*//
//***************************************************************************//

extern int Neural_THX_725_Encode_INIT(unsigned int Framesize,unsigned int ChanConfig,unsigned int SampleRate,Neural_THX_725_Encode_Struct * pParams);
extern int Neural_THX_725_Encode(float * PtrInL,float * PtrInR,float * PtrInC,float * PtrInLFE,float * PtrInLs,float * PtrInRs,float * PtrInLb,float * PtrInRb,float * PtrOutL,float * PtrOutR,float * PtrOutC,float * PtrOutLFE,float * PtrOutLs,float * PtrOutRs,THX_bool UseFinalLimiting,unsigned int Framesize,unsigned int ChanConfig,unsigned int SampleRate,Neural_THX_725_Encode_Struct * pParams);

//***************************************************************************//
//*																			*//
//*  Project     : Neural Audio - THX										*//
//*  File        : Neural_THX_522_Encode.h									*//
//*  Description : Neural Surround Downmix implementation					*//
//*  Author(s)   : Jeff Thompson											*//
//*																			*//
//*					Copyright (c) Neural Audio Corp. 2007					*//
//*																			*//
//***************************************************************************//

//***************************************************************************//
//*                              DEFINITIONS								*//
//***************************************************************************//
//Algorithm constants for cross-sharing (CROSSSHARE_XDB defined in Common.h)
#ifdef CROSSSHARE_9DB
#define CROSSSHARECONSTANT			0.354813389233575f	//-9dB
#define CROSSSHARENORM				0.942435265799133f	//Use for -9dB
#endif
#ifdef CROSSSHARE_8DB
#define CROSSSHARECONSTANT			0.398107170553497f	//-8dB
#define CROSSSHARENORM				0.929081864744324f	//Use for -8dB
#endif
#ifdef CROSSSHARE_7DB
#define CROSSSHARECONSTANT			0.446683592150963f	//-7dB
#define CROSSSHARENORM				0.913051186507875f	//Use for -7dB
#endif
#ifdef CROSSSHARE_6DB
#define CROSSSHARECONSTANT			0.501187233627272f	//-6dB
#define CROSSSHARENORM				0.894002232148723f	//Use for -6dB
#endif

#define MINUS_3DB					0.707945784384138f

//***************************************************************************//
//*                           Encoder structure								*//
//***************************************************************************//
typedef struct
{
	//Forward and inverse overlapped FFT's
	FFT_Overlapped_Stereo_Struct	FFTSourceLR;
	FFT_Overlapped_Struct			FFTSourceC;
	FFT_Overlapped_Stereo_Struct	FFTSourceLsRs;
	IFFT_Overlapped_Stereo_Struct	InvFFTDownmixLR;

	//Phase shifts on L, R, Ls, Rs
	FreqDomain_PhaseShift_Struct	PhaseShift_L;
	FreqDomain_PhaseShift_Struct	PhaseShift_R;
	FreqDomain_PhaseShift_Struct	PhaseShift_Ls;
	FreqDomain_PhaseShift_Struct	PhaseShift_Rs;

	//Low-pass of the LFE channel
	LR4_LP_Struct LFE_LP;

	//Final limiters
	Limiter_Struct FinalLimiterL;
	Limiter_Struct FinalLimiterR;
		
	//Some temporary buffers
	float Placeholder;
	float TempBuffer0[4*NEURAL_FRAMESIZE];
	float TempBuffer1[4*NEURAL_FRAMESIZE];
	
	//Working buffers
	float Input_RealL[NEURAL_FRAMESIZE];
	float Input_ImagL[NEURAL_FRAMESIZE];
	float Input_RealR[NEURAL_FRAMESIZE];
	float Input_ImagR[NEURAL_FRAMESIZE];
	float Input_RealC[NEURAL_FRAMESIZE];
	float Input_ImagC[NEURAL_FRAMESIZE];
	float Input_RealLs[NEURAL_FRAMESIZE];
	float Input_ImagLs[NEURAL_FRAMESIZE];
	float Input_RealRs[NEURAL_FRAMESIZE];
	float Input_ImagRs[NEURAL_FRAMESIZE];

	float Downmix_RealL[NEURAL_FRAMESIZE];
	float Downmix_ImagL[NEURAL_FRAMESIZE];
	float Downmix_RealR[NEURAL_FRAMESIZE];
	float Downmix_ImagR[NEURAL_FRAMESIZE];
	
} Neural_THX_522_Encode_Struct;

//***************************************************************************//
//*                           FUNCTION PROTOTYPES							*//
//***************************************************************************//

extern int Neural_THX_522_Encode_INIT(unsigned int Framesize,unsigned int ChanConfig,unsigned int SampleRate,Neural_THX_522_Encode_Struct * pParams);
extern int Neural_THX_522_Encode(float * PtrInL,float * PtrInR,float * PtrInC,float * PtrInLFE,float * PtrInLs,float * PtrInRs,float * PtrOutL,float * PtrOutR,THX_bool UseFinalLimiting,float LFE_Cutoff,unsigned int Framesize,unsigned int ChanConfig,unsigned int SampleRate,Neural_THX_522_Encode_Struct * pParams);

//***************************************************************************//
//*																			*//
//*  Project     : Neural Audio - THX										*//
//*  File        : Neural_THX_722_Encode.h									*//
//*  Description : Neural Surround Downmix implementation					*//
//*  Author(s)   : Jeff Thompson											*//
//*																			*//
//*					Copyright (c) Neural Audio Corp. 2007					*//
//*																			*//
//***************************************************************************//

//***************************************************************************//
//*                              DEFINITIONS								*//
//***************************************************************************//
//Algorithm constants for cross-sharing (CROSSSHARE_XDB defined in Common.h)
#ifdef CROSSSHARE_9DB
#define SUR_CROSSSHARECONSTANT			0.354813389233575f	//-9dB
#define SUR_CROSSSHARENORM				0.942435265799133f	//Use for -9dB
#endif
#ifdef CROSSSHARE_8DB
#define SUR_CROSSSHARECONSTANT			0.398107170553497f	//-8dB
#define SUR_CROSSSHARENORM				0.929081864744324f	//Use for -8dB
#define BACK_CROSSSHARECONSTANT			0.575439937337157f	//-4.8dB
#define BACK_CROSSSHARENORM				0.866741480219858f	//Use for -4.8dB
#endif
#ifdef CROSSSHARE_7DB
#define SUR_CROSSSHARECONSTANT			0.446683592150963f	//-7dB
#define SUR_CROSSSHARENORM				0.913051186507875f	//Use for -7dB
#endif
#ifdef CROSSSHARE_6DB
#define SUR_CROSSSHARECONSTANT			0.501187233627272f	//-6dB
#define SUR_CROSSSHARENORM				0.894002232148723f	//Use for -6dB
#endif

#define MINUS_3DB					0.707945784384138f

//***************************************************************************//
//*                           Encoder structure								*//
//***************************************************************************//
typedef struct
{
	//Forward and inverse overlapped FFT's
	FFT_Overlapped_Stereo_Struct	FFTSourceLR;
	FFT_Overlapped_Struct			FFTSourceC;
	FFT_Overlapped_Stereo_Struct	FFTSourceLsRs;
	FFT_Overlapped_Stereo_Struct	FFTSourceLbRb;
	IFFT_Overlapped_Stereo_Struct	InvFFTDownmixLR;

	//Phase shifts on L, R, Ls, Rs
	FreqDomain_PhaseShift_Struct	PhaseShift_L;
	FreqDomain_PhaseShift_Struct	PhaseShift_R;
	FreqDomain_PhaseShift_Struct	PhaseShift_Ls;
	FreqDomain_PhaseShift_Struct	PhaseShift_Rs;

	//Low-pass of the LFE channel
	LR4_LP_Struct LFE_LP;

	//Final limiters
	Limiter_Struct FinalLimiterL;
	Limiter_Struct FinalLimiterR;
		
	//Some temporary buffers
	float Placeholder;
	float TempBuffer0[4*NEURAL_FRAMESIZE];
	float TempBuffer1[4*NEURAL_FRAMESIZE];
	
	//Working buffers
	float Input_RealL[NEURAL_FRAMESIZE];
	float Input_ImagL[NEURAL_FRAMESIZE];
	float Input_RealR[NEURAL_FRAMESIZE];
	float Input_ImagR[NEURAL_FRAMESIZE];
	float Input_RealC[NEURAL_FRAMESIZE];
	float Input_ImagC[NEURAL_FRAMESIZE];
	float Input_RealLs[NEURAL_FRAMESIZE];
	float Input_ImagLs[NEURAL_FRAMESIZE];
	float Input_RealRs[NEURAL_FRAMESIZE];
	float Input_ImagRs[NEURAL_FRAMESIZE];
	float Input_RealLb[NEURAL_FRAMESIZE];
	float Input_ImagLb[NEURAL_FRAMESIZE];
	float Input_RealRb[NEURAL_FRAMESIZE];
	float Input_ImagRb[NEURAL_FRAMESIZE];

	float Downmix_RealL[NEURAL_FRAMESIZE];
	float Downmix_ImagL[NEURAL_FRAMESIZE];
	float Downmix_RealR[NEURAL_FRAMESIZE];
	float Downmix_ImagR[NEURAL_FRAMESIZE];
	
} Neural_THX_722_Encode_Struct;


//***************************************************************************//
//*                           FUNCTION PROTOTYPES							*//
//***************************************************************************//

extern int Neural_THX_722_Encode_INIT(unsigned int Framesize,unsigned int ChanConfig,unsigned int SampleRate,Neural_THX_722_Encode_Struct * pParams);
extern int Neural_THX_722_Encode(float * PtrInL,float * PtrInR,float * PtrInC,float * PtrInLFE,float * PtrInLs,float * PtrInRs,float * PtrInLb,float * PtrInRb,float * PtrOutL,float * PtrOutR,THX_bool UseFinalLimiting,float LFE_Cutoff,unsigned int Framesize,unsigned int ChanConfig,unsigned int SampleRate,Neural_THX_722_Encode_Struct * pParams);

///////////////////////////////////////
// Closing brace for extern "C"
///////////////////////////////////////
#ifdef __cplusplus
} // Closing brace for extern "C"
#endif

///////////////////////////////////////
// end of file
///////////////////////////////////////
#endif  //__NEURAL_THX_ENCODERS_H__

