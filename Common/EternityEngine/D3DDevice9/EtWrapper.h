#pragma once

#if defined(_DEBUG) || defined(_RDEBUG)
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif
#include <d3dx9.h>
#include "EtMathWrapperD3D.h"

#define ET_PI					( (FLOAT )3.141592654f )
#define EtToRadian( degree )	( ( degree ) * ( ET_PI / 180.0f ) )
#define EtToDegree( radian )	( ( radian ) * ( 180.0f / ET_PI ) )
#ifndef EtClamp
#define EtClamp( value , minValue, maxValue )  ( ( (value) > (maxValue) ) ? (maxValue) : ( (value) < (minValue) ? (minValue) : (value) ) )
#endif
#ifndef EtAcos
#define EtAcos( value ) (  acosf( EtClamp( (value) , -1.0f, 1.0f ) ) )			// -1 과 1 사이를 벗어값이 들어갔을때 acosf 가 NaN(Not a Number)을 리턴해서 크래쉬가 나는것을 방지한다.
#endif

#ifndef EtMin
#define EtMin( a, b) ((a)<(b)?(a):(b))
#endif
 
#ifndef EtMax
#define EtMax( a, b) ((a)>(b)?(a):(b))
#endif

#ifndef EtArrayFill
#define EtArrayFill( arr, val )  do{ std::fill( arr, arr + _countof(arr), val ); }while( false ) 
#endif


typedef D3DXHANDLE						EtHandle;
typedef IDirect3DVertexDeclaration9		EtVertexDecl;
typedef D3DVIEWPORT9					EtViewPort;
typedef IDirect3DBaseTexture9			EtBaseTexture;
typedef IDirect3DTexture9				EtTexture;
typedef IDirect3DSurface9				EtSurface;
typedef IDirect3DVertexBuffer9			EtVertexBuffer;
typedef IDirect3DIndexBuffer9			EtIndexBuffer;
typedef ID3DXEffect						EtEffect;

enum EtTextureType
{
	ETTEXTURE_NORMAL		= D3DRTYPE_TEXTURE,
	ETTEXTURE_VOLUME		= D3DRTYPE_VOLUMETEXTURE,
	ETTEXTURE_CUBE			= D3DRTYPE_CUBETEXTURE,
	ETTEXTURE_RENDER_TARGET	= D3DRTYPE_SURFACE,
};

enum MeshStreamType
{
	MST_POSITION	= D3DDECLUSAGE_POSITION,
	MST_BONEWEIGHT	= D3DDECLUSAGE_BLENDWEIGHT,
	MST_BONEINDEX	= D3DDECLUSAGE_BLENDINDICES,
	MST_NORMAL		= D3DDECLUSAGE_NORMAL,
	MST_PSIZE		= D3DDECLUSAGE_PSIZE,
	MST_TEXCOORD	= D3DDECLUSAGE_TEXCOORD,
	MST_TANGENT		= D3DDECLUSAGE_TANGENT,
	MST_BINORMAL	= D3DDECLUSAGE_BINORMAL,
	MST_TESSFACTOR	= D3DDECLUSAGE_TESSFACTOR,
	MST_POSITIONT	= D3DDECLUSAGE_POSITIONT,
	MST_COLOR		= D3DDECLUSAGE_COLOR,
	MST_FOG			= D3DDECLUSAGE_FOG,
	MST_DEPTH		= D3DDECLUSAGE_DEPTH,
	MST_SAMPLE		= D3DDECLUSAGE_SAMPLE,
};

enum PrimitiveType 
{
	PT_POINTLIST             = D3DPT_POINTLIST,
	PT_LINELIST              = D3DPT_LINELIST,
	PT_LINESTRIP             = D3DPT_LINESTRIP,
	PT_TRIANGLELIST          = D3DPT_TRIANGLELIST,
	PT_TRIANGLESTRIP         = D3DPT_TRIANGLESTRIP,
	PT_TRIANGLEFAN           = D3DPT_TRIANGLEFAN,
};

enum CullMode 
{
	CULL_NONE                = D3DCULL_NONE,
	CULL_CW                  = D3DCULL_CW,
	CULL_CCW                 = D3DCULL_CCW,
};

enum EtBlendOP 
{
	BLENDOP_ADD              = D3DBLENDOP_ADD,
	BLENDOP_SUBTRACT         = D3DBLENDOP_SUBTRACT,
	BLENDOP_REVSUBTRACT      = D3DBLENDOP_REVSUBTRACT,
	BLENDOP_MIN              = D3DBLENDOP_MIN,
	BLENDOP_MAX              = D3DBLENDOP_MAX,
};

enum EtBlendMode
{
	BLEND_ZERO               = D3DBLEND_ZERO,
	BLEND_ONE                = D3DBLEND_ONE,
	BLEND_SRCCOLOR           = D3DBLEND_SRCCOLOR,
	BLEND_INVSRCCOLOR        = D3DBLEND_INVSRCCOLOR,
	BLEND_SRCALPHA           = D3DBLEND_SRCALPHA,
	BLEND_INVSRCALPHA        = D3DBLEND_INVSRCALPHA,
	BLEND_DESTALPHA          = D3DBLEND_DESTALPHA,
	BLEND_INVDESTALPHA       = D3DBLEND_INVDESTALPHA,
	BLEND_DESTCOLOR          = D3DBLEND_DESTCOLOR,
	BLEND_INVDESTCOLOR       = D3DBLEND_INVDESTCOLOR,
	BLEND_SRCALPHASAT        = D3DBLEND_SRCALPHASAT,
	BLEND_BOTHSRCALPHA       = D3DBLEND_BOTHSRCALPHA,
	BLEND_BOTHINVSRCALPHA    = D3DBLEND_BOTHINVSRCALPHA,
	BLEND_BLENDFACTOR        = D3DBLEND_BLENDFACTOR,
	BLEND_INVBLENDFACTOR     = D3DBLEND_INVBLENDFACTOR,
};

enum EtFormat
{
	FMT_UNKNOWN         = D3DFMT_UNKNOWN,
	FMT_R8G8B8          = D3DFMT_R8G8B8,
	FMT_A8R8G8B8        = D3DFMT_A8R8G8B8,
	FMT_X8R8G8B8        = D3DFMT_X8R8G8B8,
	FMT_R5G6B5          = D3DFMT_R5G6B5,
	FMT_X1R5G5B5        = D3DFMT_X1R5G5B5,
	FMT_A1R5G5B5        = D3DFMT_A1R5G5B5,
	FMT_A4R4G4B4        = D3DFMT_A4R4G4B4,
	FMT_R3G3B2          = D3DFMT_R3G3B2,
	FMT_A8              = D3DFMT_A8,
	FMT_A8R3G3B2        = D3DFMT_A8R3G3B2,
	FMT_X4R4G4B4        = D3DFMT_X4R4G4B4,
	FMT_A2B10G10R10     = D3DFMT_A2B10G10R10,
	FMT_A8B8G8R8        = D3DFMT_A8B8G8R8,
	FMT_X8B8G8R8        = D3DFMT_X8B8G8R8,
	FMT_G16R16          = D3DFMT_G16R16,
	FMT_A2R10G10B10     = D3DFMT_A2R10G10B10,
	FMT_A16B16G16R16    = D3DFMT_A16B16G16R16,
	FMT_A8P8            = D3DFMT_A8P8,
	FMT_P8              = D3DFMT_P8,
	FMT_L8              = D3DFMT_L8,
	FMT_A8L8            = D3DFMT_A8L8,
	FMT_A4L4            = D3DFMT_A4L4,
	FMT_V8U8            = D3DFMT_V8U8,
	FMT_L6V5U5          = D3DFMT_L6V5U5,
	FMT_X8L8V8U8        = D3DFMT_X8L8V8U8,
	FMT_Q8W8V8U8        = D3DFMT_Q8W8V8U8,
	FMT_V16U16          = D3DFMT_V16U16,
	FMT_A2W10V10U10     = D3DFMT_A2W10V10U10,
	FMT_UYVY            = D3DFMT_UYVY,
	FMT_R8G8_B8G8       = D3DFMT_R8G8_B8G8,
	FMT_YUY2            = D3DFMT_YUY2,
	FMT_G8R8_G8B8       = D3DFMT_G8R8_G8B8,
	FMT_DXT1            = D3DFMT_DXT1,
	FMT_DXT2            = D3DFMT_DXT2,
	FMT_DXT3            = D3DFMT_DXT3,
	FMT_DXT4            = D3DFMT_DXT4,
	FMT_DXT5            = D3DFMT_DXT5,
	FMT_D16_LOCKABLE    = D3DFMT_D16_LOCKABLE,
	FMT_D32             = D3DFMT_D32,
	FMT_D15S1           = D3DFMT_D15S1,
	FMT_D24S8           = D3DFMT_D24S8,
	FMT_D24X8           = D3DFMT_D24X8,
	FMT_D24X4S4         = D3DFMT_D24X4S4,
	FMT_D16             = D3DFMT_D16,
	FMT_D32F_LOCKABLE   = D3DFMT_D32F_LOCKABLE,
	FMT_D24FS8          = D3DFMT_D24FS8,
	FMT_L16             = D3DFMT_L16,
	FMT_VERTEXDATA      = D3DFMT_VERTEXDATA,
	FMT_INDEX16         = D3DFMT_INDEX16,
	FMT_INDEX32         = D3DFMT_INDEX32,
	FMT_Q16W16V16U16    = D3DFMT_Q16W16V16U16,
	FMT_MULTI2_ARGB8    = D3DFMT_MULTI2_ARGB8,
	FMT_R16F            = D3DFMT_R16F,
	FMT_G16R16F         = D3DFMT_G16R16F,
	FMT_A16B16G16R16F   = D3DFMT_A16B16G16R16F,
	FMT_R32F            = D3DFMT_R32F,
	FMT_G32R32F         = D3DFMT_G32R32F,
	FMT_A32B32G32R32F   = D3DFMT_A32B32G32R32F,
	FMT_CxV8U8          = D3DFMT_CxV8U8,
};

enum EtMultiSampleType
{
	MULTISAMPLE_NONE = D3DMULTISAMPLE_NONE,
	MULTISAMPLE_NONMASKABLE = D3DMULTISAMPLE_NONMASKABLE,
	MULTISAMPLE_2_SAMPLES = D3DMULTISAMPLE_2_SAMPLES,
	MULTISAMPLE_3_SAMPLES = D3DMULTISAMPLE_3_SAMPLES,
	MULTISAMPLE_4_SAMPLES = D3DMULTISAMPLE_4_SAMPLES,
	MULTISAMPLE_5_SAMPLES = D3DMULTISAMPLE_5_SAMPLES,
	MULTISAMPLE_6_SAMPLES = D3DMULTISAMPLE_6_SAMPLES,
	MULTISAMPLE_7_SAMPLES = D3DMULTISAMPLE_7_SAMPLES,
	MULTISAMPLE_8_SAMPLES = D3DMULTISAMPLE_8_SAMPLES,
	MULTISAMPLE_9_SAMPLES = D3DMULTISAMPLE_9_SAMPLES,
	MULTISAMPLE_10_SAMPLES = D3DMULTISAMPLE_10_SAMPLES,
	MULTISAMPLE_11_SAMPLES = D3DMULTISAMPLE_11_SAMPLES,
	MULTISAMPLE_12_SAMPLES = D3DMULTISAMPLE_12_SAMPLES,
	MULTISAMPLE_13_SAMPLES = D3DMULTISAMPLE_13_SAMPLES,
	MULTISAMPLE_14_SAMPLES = D3DMULTISAMPLE_14_SAMPLES,
	MULTISAMPLE_15_SAMPLES = D3DMULTISAMPLE_15_SAMPLES,
	MULTISAMPLE_16_SAMPLES = D3DMULTISAMPLE_16_SAMPLES,
};

enum EtFVF
{
	FVF_RESERVED0			= D3DFVF_RESERVED0,
	FVF_POSITION_MASK		= D3DFVF_POSITION_MASK,
	FVF_XYZ					= D3DFVF_XYZ,
	FVF_XYZRHW				= D3DFVF_XYZRHW,
	FVF_XYZB1				= D3DFVF_XYZB1,
	FVF_XYZB2				= D3DFVF_XYZB2,
	FVF_XYZB3				= D3DFVF_XYZB3,
	FVF_XYZB4				= D3DFVF_XYZB4,
	FVF_XYZB5				= D3DFVF_XYZB5,
	FVF_XYZW				= D3DFVF_XYZW,
	FVF_NORMAL				= D3DFVF_NORMAL,
	FVF_PSIZE				= D3DFVF_PSIZE,
	FVF_DIFFUSE				= D3DFVF_DIFFUSE,
	FVF_SPECULAR			= D3DFVF_SPECULAR,
	FVF_TEXCOUNT_MASK		= D3DFVF_TEXCOUNT_MASK,
	FVF_TEXCOUNT_SHIFT		= D3DFVF_TEXCOUNT_SHIFT,
	FVF_TEX0				= D3DFVF_TEX0,
	FVF_TEX1				= D3DFVF_TEX1,
	FVF_TEX2				= D3DFVF_TEX2,
	FVF_TEX3				= D3DFVF_TEX3,
	FVF_TEX4				= D3DFVF_TEX4,
	FVF_TEX5				= D3DFVF_TEX5,
	FVF_TEX6				= D3DFVF_TEX6,
	FVF_TEX7				= D3DFVF_TEX7,
	FVF_TEX8				= D3DFVF_TEX8,
	FVF_LASTBETA_UBYTE4		= D3DFVF_LASTBETA_UBYTE4,
	FVF_LASTBETA_D3DCOLOR	= D3DFVF_LASTBETA_D3DCOLOR,
};

enum EtUsage
{
	USAGE_DEFAULT		= 0,
	USAGE_RENDERTARGET	= D3DUSAGE_RENDERTARGET,
	USAGE_DEPTHSTENCIL	= D3DUSAGE_DEPTHSTENCIL,
	USAGE_DYNAMIC		= D3DUSAGE_DYNAMIC,            
	USAGE_WRITEONLY		= D3DUSAGE_WRITEONLY,
	USAGE_AUTOGENMIPMAP = D3DUSAGE_AUTOGENMIPMAP,
	USAGE_QUERY_VERTEXTEXTURE = D3DUSAGE_QUERY_VERTEXTEXTURE,
};

enum EtPool 
{
	POOL_DEFAULT		= D3DPOOL_DEFAULT,
	POOL_MANAGED		= D3DPOOL_MANAGED,
	POOL_SYSTEMMEM		= D3DPOOL_SYSTEMMEM,
	POOL_SCRATCH		= D3DPOOL_SCRATCH,
};

enum EtCmpFunc
{
	CF_NEVER			= D3DCMP_NEVER,
	CF_LESS				= D3DCMP_LESS,
	CF_EQUAL			= D3DCMP_EQUAL,
	CF_LESSEQUAL		= D3DCMP_LESSEQUAL,
	CF_GREATER			= D3DCMP_GREATER,
	CF_NOTEQUAL			= D3DCMP_NOTEQUAL,
	CF_GREATEREQUAL		= D3DCMP_GREATEREQUAL,
	CF_ALWAYS			= D3DCMP_ALWAYS,
};

enum EtColorWrite
{
	CW_RED			= D3DCOLORWRITEENABLE_RED,
	CW_GREEN		= D3DCOLORWRITEENABLE_GREEN,
	CW_BLUE			= D3DCOLORWRITEENABLE_BLUE,
	CW_ALPHA		= D3DCOLORWRITEENABLE_ALPHA,
	CW_ALL			= D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA,
};

enum EtDeviceCheck
{
	DC_OK			= D3D_OK,
	DC_CAN_RESET	= D3DERR_DEVICENOTRESET,
	DC_CANNOT_RESET	= D3DERR_DEVICELOST,
};

enum EtTextureAddress
{	
	TADDRESS_WRAP = D3DTADDRESS_WRAP,
	TADDRESS_MIRROR = D3DTADDRESS_MIRROR,
	TADDRESS_CLAMP = D3DTADDRESS_CLAMP,
	TADDRESS_BORDER = D3DTADDRESS_BORDER,
	TADDRESS_MIRRORONCE = D3DTADDRESS_MIRRORONCE,
};

#define ETLOCK_READONLY				D3DLOCK_READONLY
#define ETLOCK_DISCARD				D3DLOCK_DISCARD
#define ETLOCK_NOOVERWRITE			D3DLOCK_NOOVERWRITE
#define ETLOCK_NOSYSLOCK			D3DLOCK_NOSYSLOCK
#define ETLOCK_DONOTWAIT			D3DLOCK_DONOTWAIT
#define ETLOCK_NO_DIRTY_UPDATE		D3DLOCK_NO_DIRTY_UPDATE
