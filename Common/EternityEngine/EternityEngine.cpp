#ifdef _DEBUG 
#undef THIS_FILE 
static char THIS_FILE[]=__FILE__; 
#define new DEBUG_NEW 
#endif 

#include "Stdafx.h"
#include "EternityEngine.h"
#include "EtEngine.h"
#include "EtLoader.h"
#include "EtPrimitiveDraw.h"
#include "EtObject.h"
#include "EtAniObject.h"
#include "EtTerrain.h"
#include "EtLayeredMultiUVTerrain.h"
#include "EtCamera.h"
#include "EtLight.h"
#include "EtOptionController.h"
#include "EtEffectData.h"
#include "EtTextDraw.h"
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


namespace EternityEngine {

bool InitializeEngine( HWND hWnd, int nWidth, int nHeight, bool bWindow, bool bEnableMultiThread, SGraphicOption *pOption, bool bVSync, bool bShaderDebug )
{
	bool bResult = false;
	if( pOption )
	{
		bResult = GetEtEngine()->Initialize( hWnd, nWidth, nHeight, *pOption, bWindow, bEnableMultiThread, bVSync, bShaderDebug );
	}
	else
	{
		SGraphicOption Option;
		bResult = GetEtEngine()->Initialize( hWnd, nWidth, nHeight, Option, bWindow, bEnableMultiThread, bVSync, bShaderDebug );
	}
	return bResult;
}

void ReinitializeEngine( int nWidth, int nHeight )
{
	GetEtEngine()->Reinitialize( nWidth, nHeight );
}

void FinalizeEngine()
{
	GetEtEngine()->Finalize();
}

void SetGraphicOption( SGraphicOption &Option )
{
	GetEtOptionController()->SetGraphicOption( Option );
}

void SaveBackbuffer( const char *pFileName, D3DXIMAGE_FILEFORMAT Format )
{
	GetEtDevice()->SaveBackbuffer( pFileName, Format );
}

void SaveBackbuffer( WCHAR *pFileName, D3DXIMAGE_FILEFORMAT Format )
{
	GetEtDevice()->SaveBackbuffer( pFileName, Format );
}

void CaptureScreen( const char *pFileName, int nWidth, int nHeight )
{
	GetEtEngine()->CaptureScreen( pFileName, nWidth, nHeight );
}

void SetCaptureScreenClearColor( DWORD dwColor )
{
	GetEtEngine()->SetCaptureScreenClearColor( dwColor );
}

EtResourceHandle LoadMesh( const char *pFileName )
{
	return LoadResource( pFileName, RT_MESH );
}

EtResourceHandle LoadAni( const char *pFileName )
{
	return LoadResource( pFileName, RT_ANI );
}

EtResourceHandle LoadSkin( const char *pFileName )
{
	return LoadResource( pFileName, RT_SKIN );
}

void ClearScreen( DWORD dwColor, float fZ, DWORD dwStencil, bool bClearColor, bool bClearZ, bool bClearStencil )
{
	GetEtDevice()->ClearBuffer( dwColor, fZ, dwStencil, bClearColor, bClearZ, bClearStencil );
}

void RenderFrame( float fElapsedTime )
{
	GetEtEngine()->RenderFrame( fElapsedTime );
}

void ShowFrame( RECT *pTargetRect )
{
	GetEtEngine()->ShowFrame( pTargetRect );
}

EtObjectHandle CreateStaticObject( const char *pSkinName, bool bShow )
{
	EtResourceHandle hSkin;
	CEtObject *pObject;

	hSkin = LoadSkin( pSkinName );
	if( !hSkin )
	{
		return CEtObject::Identity();
	}
	ScopeLock<CSyncLock> Lock( CEtObject::s_SmartPtrLock );

	pObject = new CEtObject();
	pObject->Initialize( hSkin );
	pObject->ShowObject( bShow );

	return pObject->GetMySmartPtr();
}


EtAniObjectHandle CreateAniObject( const char *pSkinName, const char *pAniName, bool bShow )
{
	EtSkinHandle hSkin;
	EtAniHandle hAni;
	CEtAniObject *pObject;

	hSkin = LoadSkin( pSkinName );
	if( !hSkin )
	{
		return CEtAniObject::Identity();
	}
	if( pAniName && strlen(pAniName) > 0 ) {
		hAni = LoadAni( pAniName );
		if( !hAni )
		{
			SAFE_RELEASE_SPTR( hSkin );
			return CEtAniObject::Identity();
		}
	}

	ScopeLock<CSyncLock> Lock( CEtObject::s_SmartPtrLock );

	pObject = new CEtAniObject();
	pObject->Initialize( hSkin, hAni );
	pObject->ShowObject( bShow );

	return pObject->GetMySmartPtr();
}

EtAniObjectHandle CreateAniObject( EtSkinHandle hSkin, const char *pAniName, bool bShow )
{
	EtAniHandle hAni;
	CEtAniObject *pObject;

	if( !hSkin )
	{
		return CEtAniObject::Identity();
	}
	if( pAniName && strlen(pAniName) > 0 ) {
		hAni = LoadAni( pAniName );
		if( !hAni )
		{
			SAFE_RELEASE_SPTR( hSkin );
			return CEtAniObject::Identity();
		}
	}

	ScopeLock<CSyncLock> Lock( CEtObject::s_SmartPtrLock );

	pObject = new CEtAniObject();
	pObject->Initialize( hSkin, hAni );
	pObject->ShowObject( bShow );

	return pObject->GetMySmartPtr();
}

EtCameraHandle CreateCamera( SCameraInfo *pSCameraInfo, bool bProcess )
{
	EtCameraHandle hCamera;
	CEtCamera *pCamera;
	SCameraInfo DefaultSCameraInfo;

	pCamera = new CEtCamera( bProcess );
	if( !pSCameraInfo )
	{
		pSCameraInfo = &DefaultSCameraInfo;
	}
	pCamera->Initialize( pSCameraInfo );

	return pCamera->GetMySmartPtr();
}

void SetGlobalAmbient( EtColor *pAmbient )
{
	CEtLight::SetGlobalAmbient( pAmbient );
}

EtColor GetGlobalAmbient()
{
	return *CEtLight::GetGlobalAmbient();
}

EtLightHandle CreateLight( SLightInfo *pSLightInfo, EtObjectHandle hCastLightObject )
{
	CEtLight *pLight;
	SLightInfo DefaultSLightInfo;

	pLight = new CEtLight();
	if( !pSLightInfo )
	{
		pSLightInfo = &DefaultSLightInfo;
	}
	pLight->Initialize( pSLightInfo, hCastLightObject );

	return pLight->GetMySmartPtr();
}

CEtPostProcessFilter *CreateFilter( ScreenFilter Filter )
{
	return GetEtPostProcessMng()->CreateFilter( Filter );
}

EtTextureHandle LoadTexture( const char *pFileName, EtTextureType )
{
	return LoadResource( pFileName, RT_TEXTURE );
}

EtTextureHandle LoadEnvTexture( const char *pFileName, EtTextureType Type )
{
	/*
	CEtTexture *pTexture;

	pTexture = new CEtTexture();
	pTexture->Load( pFileName );
	*/
	EtTextureHandle hTexture = LoadTexture( pFileName );
	if( !hTexture ) return CEtTexture::Identity();
	CEtTexture::SetEnvTexture( hTexture->GetTexturePtr() );

	return hTexture;
}

EtTextureHandle CreateRenderTargetTexture( int nWidth, int nHeight, EtFormat Format, EtUsage Usage, EtPool Pool )
{
	CEtTexture *pTexture;

	pTexture = CEtTexture::CreateRenderTargetTexture( nWidth, nHeight, Format, Usage, Pool );

	return pTexture->GetMySmartPtr();
}

int LoadParticleData( const char *pFileName )
{
	EtBillboardEffectDataHandle hParticleData;

	hParticleData = LoadResource( pFileName, RT_PARTICLE );
	if( !hParticleData ) return -1;

	return hParticleData->GetMyIndex();
}

void DeleteParticleData( int nIndex )
{
	EtResourceHandle hResource = CEtResource::GetResource( nIndex );
	if( hResource && hResource->GetResourceType() == RT_PARTICLE )
		SAFE_RELEASE_SPTR( hResource );
}

EtBillboardEffectHandle CreateBillboardEffect( int nParticleDataIndex, EtMatrix *pWorldMat, bool bReuseParticleInstance )
{
	EtResourceHandle hBillboardEffectData = CEtResource::GetResource( nParticleDataIndex );
	if( !hBillboardEffectData || hBillboardEffectData->GetResourceType() != RT_PARTICLE )
	{
		ASSERT( 0 && "잘못된 리소스 타입 입니다." );
		return CEtResource::Identity();
	}

	CEtBillboardEffect *pBillboardEffect;

	ScopeLock<CSyncLock> Lock( CEtBillboardEffect::s_SmartPtrLock );

	pBillboardEffect = new CEtBillboardEffect();
	pBillboardEffect->Initialize( hBillboardEffectData, pWorldMat, bReuseParticleInstance );

	return pBillboardEffect->GetMySmartPtr();
}

EtLineTrailHandle CreateLineTrail( EtTextureHandle hTexture, float fLifeTime, float fLineWidth, float fMinSegment )
{
	CEtLineTrailEffect *pEffect;

	ScopeLock<CSyncLock> Lock( CEtLineTrailEffect::s_SmartPtrLock );

	pEffect = new CEtLineTrailEffect();
	pEffect->Initialize( hTexture, fLifeTime, fLineWidth, fMinSegment );

	return pEffect->GetMySmartPtr();
}

EtEffectTrailHandle CreateEffectTrail( EtTextureHandle hTexture, float fLifeTime, float fLineWidth, float fMinSegment )
{
	CEtEffectTrail *pEffect;

	ScopeLock<CSyncLock> Lock( CEtEffectTrail::s_SmartPtrLock );

	pEffect = new CEtEffectTrail();
	pEffect->Initialize( hTexture, fLifeTime, fLineWidth, fMinSegment );

	return pEffect->GetMySmartPtr();
}

EtLineHandle CreateLine( EtTextureHandle hTexture, float fLifeTime, float fLineWidth, float fUMoveSpeed )
{
	CEtLineEffect *pEffect;

	ScopeLock<CSyncLock> Lock( CEtLineEffect::s_SmartPtrLock );

	pEffect = new CEtLineEffect();
	pEffect->Initialize( hTexture, fLifeTime, fLineWidth, fUMoveSpeed );

	return pEffect->GetMySmartPtr();
}

EtSwordTrailHandle CreateSwordTrail( EtAniObjectHandle hAniObject, EtTextureHandle hTexture, EtTextureHandle hNormalTexture, float fLifeTime, float fMinSegment )
{
	CEtSwordTrail *pEffect;

	ScopeLock<CSyncLock> Lock( CEtSwordTrail::s_SmartPtrLock );

	pEffect = new CEtSwordTrail();
	pEffect->Initialize( hAniObject, hTexture, hNormalTexture, fLifeTime, fMinSegment );

	return pEffect->GetMySmartPtr();
}

int LoadEffectData( const char *pFileName )
{
	EtEffectDataHandle hEffectData;

	hEffectData = LoadResource( pFileName, RT_EFFECT );
	if( !hEffectData ) return -1;

	return hEffectData->GetMyIndex();
}

void DeleteEffectData( int nIndex )
{
	EtResourceHandle hResource = CEtResource::GetResource( nIndex );
	if( hResource && hResource->GetResourceType() == RT_EFFECT )
		SAFE_RELEASE_SPTR( hResource );
}

EtEffectObjectHandle CreateEffectObject( int nEffectDataIndex, EtMatrix *pWorldMat )
{
	EtResourceHandle hEffectData = CEtResource::GetResource( nEffectDataIndex );
	if( !hEffectData || hEffectData->GetResourceType() != RT_EFFECT ) {
		ASSERT( 0 && "잘못된 리소스 타입 입니다." );
		return CEtResource::Identity();
	}

	CEtEffectObject *pEffectObject;

	ScopeLock<CSyncLock> Lock( CEtEffectObject::s_SmartPtrLock );

	pEffectObject = new CEtEffectObject();
	pEffectObject->Initialize( hEffectData, pWorldMat );

	return pEffectObject->GetMySmartPtr();
}


EtTerrainHandle CreateTerrain( STerrainInfo *pTerrainInfo )
{
	EtTerrainHandle hTerrain;
	CEtTerrainArea *pTerrain;		

	ScopeLock<CSyncLock> Lock( CEtTerrainArea::s_SmartPtrLock );

	pTerrain = new CEtTerrainArea();
	pTerrain->Initialize( pTerrainInfo );

	return pTerrain->GetMySmartPtr();
}

EtObjectHandle Pick( int nX, int nY )
{
	return CEtObject::Pick( nX, nY );
}

int MergeAni( EtAniHandle hTargetAni, EtAniHandle hMergeAni )
{
	if( ( !hTargetAni ) || ( !hMergeAni ) )
	{
		return ETERR_INVALIDRESOURCEHANDLE;
	}

	return hTargetAni->MergeAni( hMergeAni.GetPointer() );
}

int SaveAni( EtAniHandle hAni, const char *pFileName )
{
	if( !hAni )
	{
		return ETERR_INVALIDRESOURCEHANDLE;
	}

	return hAni->Save( pFileName );
}

void DrawLine2D( EtVector2 &Start, EtVector2 &End, DWORD dwColor )
{
	CEtPrimitiveDraw::GetInstance().DrawLine2D( Start, End, dwColor );
}

void DrawTriangle2D( EtVector2 &Point1, EtVector2 &Point2, EtVector2 &Point3, DWORD dwColor )
{
	CEtPrimitiveDraw::GetInstance().DrawTriangle2D( Point1, Point2, Point3, dwColor );
}

void DrawLine3D( EtVector3 &Start, EtVector3 &End, DWORD dwColor, EtMatrix *matWorld )
{
	CEtPrimitiveDraw::GetInstance().DrawLine3D( Start, End, dwColor, matWorld );
}

void DrawPoint3D( EtVector3 &Point, DWORD dwColor, EtMatrix *matWorld, bool bIgnoreZBuffer )
{
	CEtPrimitiveDraw::GetInstance().DrawPoint3D( Point, dwColor, matWorld, bIgnoreZBuffer );
}

void DrawTriangle3D( EtVector3 &Point1, EtVector3 &Point2, EtVector3 &Point3, DWORD dwColor, EtMatrix *matWorld )
{
	CEtPrimitiveDraw::GetInstance().DrawTriangle3D( Point1, Point2, Point3, dwColor, matWorld );
}

void DrawTriangle3D( SPrimitiveDraw3D *pBuffer, DWORD dwTriangleCount )
{
	CEtPrimitiveDraw::GetInstance().DrawTriangle3D( pBuffer, dwTriangleCount );
}

void DrawQuad3DWithTex( EtVector3 *pVertices, EtVector2 *pTexCoord,
					   EtBaseTexture *pTexture, EtMatrix *matWorld )
{
	CEtPrimitiveDraw::GetInstance().DrawQuad3DWithTex( pVertices, pTexCoord, pTexture, matWorld );
}

void DrawQuad2DWithTex( EtVector4 *pVertices, EtVector2 *pTexCoord,
					   EtBaseTexture *pTexture )
{
	CEtPrimitiveDraw::GetInstance().DrawQuad2DWithTex( pVertices, pTexCoord, pTexture );
}

void DrawText2D( EtVector2 &Position, const char *pString, DWORD dwColor )
{
	CEtTextDraw::GetInstance().DrawText2D( Position, pString, dwColor );
}

void DrawText3D( EtVector3 &Position, const char *pString, DWORD dwColor )
{
	CEtTextDraw::GetInstance().DrawText3D( Position, pString, dwColor );
}
};