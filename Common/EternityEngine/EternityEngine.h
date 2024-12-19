#pragma once

#include "AssertX.h"
#include "Singleton.h"
#include "SmartPtr.h"
#include "Stream.h"
#include "EtError.h"
#include "SundriesFunc.h"

#define DEVICE_D3D9		1
#define DEVICE_D3D10	2
#define DEVICE_XBOX360	3

#pragma warning(disable:4819)
#ifndef RENDER_DEVICE
#define RENDER_DEVICE	DEVICE_D3D9
#endif
#if RENDER_DEVICE == DEVICE_D3D9
#if defined(_DEBUG) || defined(_RDEBUG)
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif
#include <d3d9.h>
#include <d3dx9.h>
#include "D3DDevice9\\EtD3DDevice9.h"
#elif RENDER_DEVICE == DEVICE_D3D10
#elif RENDER_DEVICE == DEVICE_XBOX360
#endif
#pragma warning(default:4819)

#define MIN_RES_WIDTH 1024
#define MIN_RES_HEIGHT 768

#define PRE_FIX_MATERIAL_DUMP

#include "EtType.h"
#include "EtObject.h"
#include "EtAniObject.h"
#include "EtCamera.h"
#include "EtSkin.h"
#include "EtAni.h"
#include "EtLight.h"
#include "EtTerrainArea.h"
#include "EtPostProcessMng.h"
#include "EtPostProcessFilter.h"
#include "EtBillboardEffect.h"
#include "EtEffectObject.h"
#include "EtLineTrailEffect.h"
#include "EtSwordTrail.h"
#include "EtLoader.h"
#include "EtEffectHelper.h"
#include "EtCustomRender.h"
#include "EtLineEffect.h"
#include "EtEffectTrail.h"

namespace EternityEngine
{
//--------------------------------------------------------------------------------
bool InitializeEngine( HWND hWnd, int nWidth, int nHeight, bool bWindow = false, bool bEnableMultiThread = false, SGraphicOption *pOption = NULL, bool bVSync = true, bool bShaderDebug = false );
void ReinitializeEngine( int nWidth, int nHeight );
void FinalizeEngine();
void SetGraphicOption( SGraphicOption &Option );
void SaveBackbuffer( const char *pFileName, D3DXIMAGE_FILEFORMAT Format = D3DXIFF_DDS );
void SaveBackbuffer( WCHAR *pFileName, D3DXIMAGE_FILEFORMAT Format = D3DXIFF_DDS );


//--------------------------------------------------------------------------------
void ClearScreen( DWORD dwColor, float fZ = 1.0f, DWORD dwStencil = 0, bool bClearColor = true, bool bClearZ = true, bool bClearStencil = true );
void RenderFrame( float fElapsedTime = 0.0166667f );
void ShowFrame( RECT *pTargetRect = NULL );


//--------------------------------------------------------------------------------
void CaptureScreen( const char *pFileName, int nWidth = INT_MAX, int nHeight = INT_MAX );
void SetCaptureScreenClearColor( DWORD dwColor );


//--------------------------------------------------------------------------------
void SetGlobalAmbient( EtColor *pAmbient );
EtColor GetGlobalAmbient();


//--------------------------------------------------------------------------------
EtResourceHandle LoadMesh( const char *pFileName );
EtResourceHandle LoadAni( const char *pFileName );
EtResourceHandle LoadSkin( const char *pFileName );


//--------------------------------------------------------------------------------
EtObjectHandle CreateStaticObject( const char *pSkinName, bool bShow = true );
EtAniObjectHandle CreateAniObject( const char *pSkinName, const char *pAniName, bool bShow = true );
EtAniObjectHandle CreateAniObject( EtSkinHandle hSkin, const char *pAniName, bool bShow = true );
EtCameraHandle CreateCamera( SCameraInfo *pSCameraInfo = NULL, bool bProcess = true );
EtLightHandle CreateLight( SLightInfo *pSLightInfo = NULL, EtObjectHandle hCastLightObject = CEtObject::Identity() );
CEtPostProcessFilter *CreateFilter( ScreenFilter Filter );
EtTextureHandle LoadTexture( const char *pFileName, EtTextureType = ETTEXTURE_NORMAL );
EtTextureHandle LoadEnvTexture( const char *pFileName, EtTextureType = ETTEXTURE_NORMAL );
EtTextureHandle CreateRenderTargetTexture( int nWidth, int nHeight, EtFormat Format = FMT_A8R8G8B8, EtUsage Usage = USAGE_DEFAULT, EtPool Pool  = POOL_DEFAULT );

int LoadParticleData( const char *pFileName );
void DeleteParticleData( int nIndex );
EtBillboardEffectHandle CreateBillboardEffect( int nParticleDataIndex, EtMatrix *pWorldMat, bool bReuseParticleInstance = true );
EtLineTrailHandle CreateLineTrail( EtTextureHandle hTexture, float fLifeTime, float fLineWidth, float fMinSegment = 50.0f );
EtEffectTrailHandle CreateEffectTrail( EtTextureHandle hTexture, float fLifeTime, float fLineWidth, float fMinSegment = 50.0f );
EtLineHandle CreateLine( EtTextureHandle hTexture, float fLifeTime, float fLineWidth, float fUMoveSpeed );
EtSwordTrailHandle CreateSwordTrail( EtAniObjectHandle hAniObject, EtTextureHandle hTexture, EtTextureHandle hNormalTexture, float fLifeTime, float fMinSegment = 50.0f);
int LoadEffectData( const char *pFileName );
void DeleteEffectData( int nIndex );
EtEffectObjectHandle CreateEffectObject( int nEffectDataIndex, EtMatrix *pWorldMat );

//--------------------------------------------------------------------------------
EtTerrainHandle CreateTerrain( STerrainInfo *pTerrainInfo );


//--------------------------------------------------------------------------------
EtObjectHandle Pick( int nX, int nY );

//--------------------------------------------------------------------------------
int MergeAni( EtAniHandle hTargetAni, EtAniHandle hMergeAni );
int SaveAni( EtAniHandle hAni, const char *pFileName );


//--------------------------------------------------------------------------------
void DrawLine2D( EtVector2 &Start, EtVector2 &End, DWORD dwColor = 0xffffffff );
void DrawTriangle2D( EtVector2 &Point1, EtVector2 &Point2, EtVector2 &Point3, DWORD dwColor = 0xffffffff );
void DrawLine3D( EtVector3 &Start, EtVector3 &End, DWORD dwColor = 0xffffffff, EtMatrix *matWorld = NULL );
void DrawPoint3D( EtVector3 &Point, DWORD dwColor, EtMatrix *matWorld = NULL, bool bIgnoreZBuffer = true );
void DrawTriangle3D( EtVector3 &Point1, EtVector3 &Point2, EtVector3 &Point3, DWORD dwColor = 0xffffffff, EtMatrix *matWorld = NULL );
void DrawTriangle3D( SPrimitiveDraw3D *pBuffer, DWORD dwTriangleCount );
void DrawQuad3DWithTex( EtVector3 *pVertices, EtVector2 *pTexCoord,
					   EtBaseTexture *pTexture, EtMatrix *matWorld = NULL );
void DrawQuad2DWithTex( EtVector4 *pVertices, EtVector2 *pTexCoord,
					   EtBaseTexture *pTexture );
void DrawText2D( EtVector2 &Position, const char *pString, DWORD dwColor = 0xffffffff );
void DrawText3D( EtVector3 &Position, const char *pString, DWORD dwColor = 0xffffffff );
};