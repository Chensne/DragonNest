#include "Stdafx.h"
#include "EtCamera.h"
#include "EtLight.h"
#include "EtShadowMap.h"
#include "EtObject.h"
#include "EtAniObject.h"
#include "EtEffectObject.h"
#include "EtBillboardEffect.h"
#include "EtTerrainArea.h"
#include "EtWater.h"
#include "EtOptionController.h"
#include "EtBackBufferMng.h"
#include "EtMergedSkin.h"
#include "EtSkyBoxRTT.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

struct PCEmissiveInfo {
	EmissiveParam Param[ 5 ];
	int nArrayCount;
};

struct CoefficientInfo {
	float fA[4];
	float fB[4];
	float fC[4];
	float fD[4];
};

#define EMISSIVE_INDEX 0
#define COEFFICIENT_INDEX 1

extern float g_fTotalElapsedTime;

int VariableCountOne( void *pDummy, void *pDummy2 )
{
	return 1;
}

int VariableCountEmissive( void *pMaterialPtr, void *pCustomParamList )
{
	if( ( !pCustomParamList ) || ( !pMaterialPtr ) )
	{
		return 0;
	}

	CEtMaterial *pMaterial = (CEtMaterial*)pMaterialPtr;
	std::vector< SCustomParam > &vecCustomParam = *(std::vector< SCustomParam > *)pCustomParamList;

	if( vecCustomParam.size() <= EMISSIVE_INDEX )
	{
		return 0;
	}

	PCEmissiveInfo *pPCEmissiveInfo = (PCEmissiveInfo *)vecCustomParam[ EMISSIVE_INDEX ].pUserStruct;
	if( pPCEmissiveInfo != NULL ) {
		return pPCEmissiveInfo->nArrayCount | 0x80000000;
	}

	for( int i = 0; i < (int)vecCustomParam.size(); i++) {
		EtParameterHandle hParamHandle = vecCustomParam[ i ].hParamHandle;
		if( pMaterial->IsParameterName( hParamHandle, "g_EmissiveColor" ) ) {
			return vecCustomParam[ i ].nVariableCount | 0x80000000;
		}
	}

	return 0;
}

int VariableCountDirLight( void *pDummy, void *pDummy2 )
{
	int nCount = GetEtOptionController()->GetMaxDirLightCount();	// 파라메터의 타입이 Array일때를 구별하기 위해 최상위 비트를 1로 세팅해준다
	nCount |= 0x80000000;
	return nCount;
}

int VariableCountPointLight( void *pDummy, void *pDummy2 )
{
	int nCount = GetEtOptionController()->GetMaxPointLightCount();
	nCount |= 0x80000000;
	return nCount;
}

int VariableCountSpotLight( void *pDummy, void *pDummy2 )
{
	int nCount = GetEtOptionController()->GetMaxSpotLightCount();
	nCount |= 0x80000000;
	return nCount;
}

void *GetProjectionMat( void *pDummy, void *pDummy2 )
{
	return CEtCamera::GetActiveCamera()->GetProjMat();
}

void *GetViewMat( void *pDummy, void *pDummy2 )
{
	return CEtCamera::GetActiveCamera()->GetViewMat();
}

void *GetViewRotProjMat( void *pDummy, void *pDummy2 )
{
	static EtMatrix ViewRotProjMat;
	EtCameraHandle hCamera;

	hCamera = CEtCamera::GetActiveCamera();
	ViewRotProjMat = *hCamera->GetViewMat();
	ViewRotProjMat._41 = 0.0f;
	ViewRotProjMat._42 = 0.0f;
	ViewRotProjMat._43 = 0.0f;
	return EtMatrixMultiply( &ViewRotProjMat, &ViewRotProjMat, hCamera->GetProjMat() );
}

void *GetPrevViewRotProjMat( void *pDummy, void *pDummy2 )
{
	static EtMatrix ViewRotProjMat;
	EtCameraHandle hCamera;

	hCamera = CEtCamera::GetActiveCamera();
	ViewRotProjMat = *hCamera->GetPrevViewMat();
	ViewRotProjMat._41 = 0.0f;
	ViewRotProjMat._42 = 0.0f;
	ViewRotProjMat._43 = 0.0f;
	return EtMatrixMultiply( &ViewRotProjMat, &ViewRotProjMat, hCamera->GetProjMat() );
}

void *GetViewPosition( void *pDummy, void *pDummy2 )
{
	static EtVector3 ViewPosition;

	ViewPosition = *CEtCamera::GetActiveCamera()->GetPosition();

	return &ViewPosition;
}

void *GetFogFactor( void *pDummy, void *pDummy2 )
{
	static EtVector4 FogVector;

	float fFogNear = CEtCamera::GetActiveCamera()->GetFogNear();
	float fFogFar = CEtCamera::GetActiveCamera()->GetFogFar();
	float fCamFar = CEtCamera::GetActiveCamera()->GetCameraFar();

	FogVector.x = 1.0f / ( fFogFar - fFogNear );
	FogVector.y = -1.0f / ( fCamFar - fFogFar );
	FogVector.z = fFogFar;

	return &FogVector;
}

void *GetFogColor( void *pDummy, void *pDummy2 )
{
	return CEtCamera::GetActiveCamera()->GetFogColor();
}

void *GetFogSkyBoxTex( void *pDummy, void *pDummy2 )
{
	EtTextureHandle hTexture = CEtSkyBoxRTT::GetInstance().GetRTT();
	if( hTexture )
	{
		return hTexture->GetTexturePtr();
	}
	return NULL;
}

void *GetScreenSizeScale( void *pDummy, void *pDummy2 )
{
	static EtVector4 vResult(0, 0, 0, 0);
	vResult.x = 0.5f + 0.5f / GetEtDevice()->Width();
	vResult.y = 0.5f - 0.5f / GetEtDevice()->Height();
	return &vResult;
}

static EtVector4 g_ReturnVector4[ 5 ];
int g_nReturnValue;
void *GetLightAmbient( void *pDummy, void *pDummy2 )
{
	return CEtLight::GetGlobalAmbient();
}

void *GetDirLightCount( void *pDummy, void *pDummy2 )
{
	g_nReturnValue = min( CEtLight::GetDirLightCount(), GetEtOptionController()->GetMaxDirLightCount() );
	return &g_nReturnValue;
}

void *GetDirLightDirection( void *pDummy, void *pDummy2 )
{
	int i, nLightCount;
	SLightInfo *pSLightInfo;

	nLightCount = min( 5, CEtLight::GetDirLightCount() );
	for( i = 0; i < nLightCount; i++ )
	{
		pSLightInfo = CEtLight::GetDirLightInfo( i );
		EtVec3TransformNormal( ( EtVector3 * )( g_ReturnVector4 + i ), &pSLightInfo->Direction, 
			CEtCamera::GetActiveCamera()->GetViewMat() );
		EtVec3Normalize( ( EtVector3 * )( g_ReturnVector4 + i ), ( EtVector3 * )( g_ReturnVector4 + i ) );
	}

	return g_ReturnVector4;
}

void *GetDirLightDiffuse( void *pDummy, void *pDummy2 )
{
	int i, nLightCount;
	SLightInfo *pSLightInfo;

	nLightCount = min( 5, CEtLight::GetDirLightCount() );
	for( i = 0; i < nLightCount; i++ )
	{
		pSLightInfo = CEtLight::GetDirLightInfo( i );
		g_ReturnVector4[ i ] = *( EtVector4 * )&pSLightInfo->Diffuse;
		if( i == 0 )
		{
			g_ReturnVector4[ i ] *= CEtLight::GetDirLightAttenuation();
		}
	}

	return g_ReturnVector4;
}

void *GetDirLightSpecular( void *pDummy, void *pDummy2 )
{
	int i, nLightCount;
	SLightInfo *pSLightInfo;

	nLightCount = min( 5, CEtLight::GetDirLightCount() );
	for( i = 0; i < nLightCount; i++ )
	{
		pSLightInfo = CEtLight::GetDirLightInfo( i );
		g_ReturnVector4[ i ] = *( EtVector4 * )&pSLightInfo->Specular;
		if( i == 0 )
		{
			g_ReturnVector4[ i ] *= CEtLight::GetDirLightAttenuation();
		}
	}

	return g_ReturnVector4;
}


void *GetPointLightCount( void *pDummy, void *pDummy2 )
{
	g_nReturnValue = min( CEtLight::GetInfluencePointLightCount(), GetEtOptionController()->GetMaxPointLightCount() );
	return &g_nReturnValue;
}

void *GetPointLightPosition( void *pDummy, void *pDummy2 )
{
	int i, nLightCount;
	SLightInfo *pSLightInfo;

	nLightCount = min( 5, CEtLight::GetInfluencePointLightCount() );
	for( i = 0; i < nLightCount; i++ )
	{
		pSLightInfo = CEtLight::GetInfluencePointLightInfo( i );
		EtVec3TransformCoord( ( EtVector3 * )( g_ReturnVector4 + i ), &pSLightInfo->Position, 
			CEtCamera::GetActiveCamera()->GetViewMat() );
		g_ReturnVector4[ i ].w = 1.0f / pSLightInfo->fRange;
	}

	return g_ReturnVector4;
}

void *GetPointLightDiffuse( void *pDummy, void *pDummy2 )
{
	int i, nLightCount;
	SLightInfo *pSLightInfo;

	nLightCount = min( 5, CEtLight::GetInfluencePointLightCount() );
	for( i = 0; i < nLightCount; i++ )
	{
		pSLightInfo = CEtLight::GetInfluencePointLightInfo( i );
		g_ReturnVector4[ i ] = *( EtVector4 * )&pSLightInfo->Diffuse;
	}

	return g_ReturnVector4;
}

void *GetPointLightSpecular( void *pDummy, void *pDummy2 )
{
	int i, nLightCount;
	SLightInfo *pSLightInfo;

	nLightCount = min( 5, CEtLight::GetInfluencePointLightCount() );
	for( i = 0; i < nLightCount; i++ )
	{
		pSLightInfo = CEtLight::GetInfluencePointLightInfo( i );
		g_ReturnVector4[ i ] = *( EtVector4 * )&pSLightInfo->Specular;
	}

	return g_ReturnVector4;
}


void *GetSpotLightCount( void *pDummy, void *pDummy2 )
{
	g_nReturnValue = min( CEtLight::GetInfluenceSpotLightCount(), GetEtOptionController()->GetMaxSpotLightCount() );

	return &g_nReturnValue;
}

void *GetSpotLightDirection( void *pDummy, void *pDummy2 )
{
	int i, nLightCount;
	SLightInfo *pSLightInfo;

	nLightCount = min( 5, CEtLight::GetInfluenceSpotLightCount() );
	for( i = 0; i < nLightCount; i++ )
	{
		pSLightInfo = CEtLight::GetInfluenceSpotLightInfo( i );
		EtVec3TransformNormal( ( EtVector3 * )( g_ReturnVector4 + i ), &pSLightInfo->Direction, 
			CEtCamera::GetActiveCamera()->GetViewMat() );
		EtVec3Normalize( ( EtVector3 * )( g_ReturnVector4 + i ), ( EtVector3 * )( g_ReturnVector4 + i ) );
		g_ReturnVector4[ i ].w = pSLightInfo->fFalloff;
	}

	return g_ReturnVector4;
}

void *GetSpotLightPosition( void *pDummy, void *pDummy2 )
{
	int i, nLightCount;
	SLightInfo *pSLightInfo;

	nLightCount = min( 5, CEtLight::GetInfluenceSpotLightCount() );
	for( i = 0; i < nLightCount; i++ )
	{
		pSLightInfo = CEtLight::GetInfluenceSpotLightInfo( i );
		EtVec3TransformCoord( ( EtVector3 * )( g_ReturnVector4 + i ), &pSLightInfo->Position, 
			CEtCamera::GetActiveCamera()->GetViewMat() );
		g_ReturnVector4[ i ].w = pSLightInfo->fRange;	// 스폿 라이트는 감쇄 안하기 때문에.. range값을 직접 넘긴다.
	}

	return g_ReturnVector4;
}

void *GetSpotLightDiffuse( void *pDummy, void *pDummy2 )
{
	int i, nLightCount;
	SLightInfo *pSLightInfo;

	nLightCount = min( 5, CEtLight::GetInfluenceSpotLightCount() );
	for( i = 0; i < nLightCount; i++ )
	{
		pSLightInfo = CEtLight::GetInfluenceSpotLightInfo( i );
		g_ReturnVector4[ i ] = *( EtVector4 * )&pSLightInfo->Diffuse;
		if( pSLightInfo->fTheta <= pSLightInfo->fPhi )	// 0이나 -값이 나오지 않도록..
		{
			g_ReturnVector4[ i ].w = 0.0000001f;
		}
		else
		{
			g_ReturnVector4[ i ].w = pSLightInfo->fTheta - pSLightInfo->fPhi;
		}
	}

	return g_ReturnVector4;
}

void *GetSpotLightSpecular( void *pDummy, void *pDummy2 )
{
	int i, nLightCount;
	SLightInfo *pSLightInfo;

	nLightCount = min( 5, CEtLight::GetInfluenceSpotLightCount() );
	for( i = 0; i < nLightCount; i++ )
	{
		pSLightInfo = CEtLight::GetInfluenceSpotLightInfo( i );
		g_ReturnVector4[ i ] = *( EtVector4 * )&pSLightInfo->Specular;
		g_ReturnVector4[ i ].w = pSLightInfo->fPhi;
	}

	return g_ReturnVector4;
}


void *GetShadowMap( void *pDummy, void *pDummy2 )
{
	return GetEtShadowMap()->GetShadowMapTexture();
}

void *GetShadowDensity( void *pDummy, void *pDummy2 )
{
	float fMaterialAmbient = 0.682f;		// 지형이랑 프랍에 지는 그림자랑 색깔 맞춘다.
	g_ReturnVector4[0] = EtVector4( CEtLight::GetGlobalAmbient()->r * fMaterialAmbient,
												CEtLight::GetGlobalAmbient()->g * fMaterialAmbient,
												CEtLight::GetGlobalAmbient()->b * fMaterialAmbient,
												1.0f );
	return &g_ReturnVector4[0];
}

void *GetShadowFetchDistance( void *pDummy, void *pDummy2 )
{
	static float s_fFetchDist = 0.8f;
	return &s_fFetchDist;
};

void *GetElapsedTime( void *pDummy, void *pDummy2 )
{
	return &g_fTotalElapsedTime;
}

void *GetEnvTexture( void *pDummy, void *pDummy2 )
{
	return CEtTexture::GetEnvTexture();
}

void *GetBackBuffer( void *pDummy, void *pDummy2 )
{
	return GetEtBackBufferMng()->GetBackBuffer()->GetTexturePtr();
}

void *GetWorldMat( void *pWorldMat, void *pPrevWorldMat )
{
	return pWorldMat;
}

void *GetInvViewMat( void *pDummy, void *pDummy2 )
{
	return CEtCamera::GetActiveCamera()->GetInvViewMat();
}

void *GetWorldViewMat( void *pWorldMat, void *pPrevWorldMat )
{
	static EtMatrix WorldViewMat;

	return EtMatrixMultiply( &WorldViewMat, ( EtMatrix * )pWorldMat, CEtCamera::GetActiveCamera()->GetViewMat() );
}

void *GetViewProjMat( void *pWorldMat, void *pPrevWorldMat )
{
	static EtMatrix ViewProjMat;
	EtCameraHandle hCamera;

	hCamera = CEtCamera::GetActiveCamera();
	memcpy( &ViewProjMat, hCamera->GetViewProjMat(), sizeof( EtMatrix ) );

	return &ViewProjMat;
}

void *GetWorldViewProjMat( void *pWorldMat, void *pPrevWorldMat )
{
	static EtMatrix WorldViewProjMat;
	EtCameraHandle hCamera;

	hCamera = CEtCamera::GetActiveCamera();
	EtMatrixMultiply( &WorldViewProjMat, ( EtMatrix * )pWorldMat, hCamera->GetViewMat() );
	return EtMatrixMultiply( &WorldViewProjMat, &WorldViewProjMat, hCamera->GetProjMat() );
}

void *GetPrevWorldViewProjMat( void *pWorldMat, void *pPrevWorldMat )
{
	static EtMatrix WorldViewProjMat;
	EtCameraHandle hCamera;

	hCamera = CEtCamera::GetActiveCamera();
	EtMatrixMultiply( &WorldViewProjMat, ( EtMatrix * )pPrevWorldMat, hCamera->GetPrevViewMat() );
	return EtMatrixMultiply( &WorldViewProjMat, &WorldViewProjMat, hCamera->GetProjMat() );
}

void *GetInvWorldViewPrevWorldViewProjMat( void *pWorldMat, void *pPrevWorldMat )
{
	static EtMatrix InvWorldViewMat;
	EtCameraHandle hCamera;

	hCamera = CEtCamera::GetActiveCamera();
	EtMatrixMultiply( &InvWorldViewMat, ( EtMatrix * )pWorldMat, hCamera->GetViewMat() );
	EtMatrixInverse( &InvWorldViewMat, NULL, &InvWorldViewMat );
	EtMatrixMultiply( &InvWorldViewMat, &InvWorldViewMat, ( EtMatrix * )pPrevWorldMat );
	EtMatrixMultiply( &InvWorldViewMat, &InvWorldViewMat, hCamera->GetPrevViewMat() );
	return EtMatrixMultiply( &InvWorldViewMat, &InvWorldViewMat, hCamera->GetProjMat() );
}

void *GetModelViewMat( void *pWorldMat, void *pPrevWorldMat )
{
	static EtMatrix ModelViewMat;
	EtCameraHandle hCamera;

	hCamera = CEtCamera::GetActiveCamera();
	return EtMatrixLookAtLH( &ModelViewMat, hCamera->GetPosition(), ( EtVector3 * )&( ( EtMatrix * )pWorldMat )->_41, &EtVector3( 0.0f, 1.0f, 0.0f ) );
}

void *GetInvViewModelViewMat( void *pWorldMat, void *pPrevWorldMat )
{
	static EtMatrix InvViewModelViewMat;
	EtCameraHandle hCamera;

	hCamera = CEtCamera::GetActiveCamera();
	EtMatrixLookAtLH( &InvViewModelViewMat, hCamera->GetPosition(), ( EtVector3 * )&( ( EtMatrix * )pWorldMat )->_41, &EtVector3( 0.0f, 1.0f, 0.0f ) );
	return EtMatrixMultiply( &InvViewModelViewMat, hCamera->GetInvViewMat(), &InvViewModelViewMat );
}

void *GetWorldLightViewProjMat( void *pWorldMat, void *pPrevWorldMat )
{
	static EtMatrix WorldLightViewProjMat;

	EtMatrixMultiply( &WorldLightViewProjMat, ( EtMatrix * )pWorldMat, GetEtShadowMap()->GetLightViewMat() );
	return EtMatrixMultiply( &WorldLightViewProjMat, &WorldLightViewProjMat, GetEtShadowMap()->GetLightProjMat() );
}

void *GetInvViewLightViewProjMat( void *pWorldMat, void *pPrevWorldMat )
{
	static EtMatrix InvViewLightViewProjMat;

	EtMatrixMultiply( &InvViewLightViewProjMat, CEtCamera::GetActiveCamera()->GetInvViewMat(), GetEtShadowMap()->GetLightViewMat() );
	return EtMatrixMultiply( &InvViewLightViewProjMat, &InvViewLightViewProjMat, GetEtShadowMap()->GetLightProjMat() );
}

void *GetWorldLightViewProjDepth( void *pWorldMat, void *pPrevWorldMat )
{
	EtMatrix WorldLightViewProjMat;
	static EtVector4 WorldLightViewProjDepth;

	EtMatrixMultiply( &WorldLightViewProjMat, ( EtMatrix * )pWorldMat, GetEtShadowMap()->GetLightViewMat() );
	EtMatrixMultiply( &WorldLightViewProjMat, &WorldLightViewProjMat, GetEtShadowMap()->GetLightProjDepthMat() );
	WorldLightViewProjDepth = EtVector4( WorldLightViewProjMat._13, WorldLightViewProjMat._23, WorldLightViewProjMat._33, WorldLightViewProjMat._43);
	return &WorldLightViewProjDepth;
}

void *GetInvViewLightViewProjDepth( void *pWorldMat, void *pPrevWorldMat )
{
	EtMatrix InvViewLightViewProjMat;
	static EtVector4 InvViewLightViewProjDepth;

	EtMatrixMultiply( &InvViewLightViewProjMat, CEtCamera::GetActiveCamera()->GetInvViewMat(), GetEtShadowMap()->GetLightViewMat() );
	EtMatrixMultiply( &InvViewLightViewProjMat, &InvViewLightViewProjMat, GetEtShadowMap()->GetLightProjDepthMat() );
	InvViewLightViewProjDepth = EtVector4( InvViewLightViewProjMat._13, InvViewLightViewProjMat._23, InvViewLightViewProjMat._33, InvViewLightViewProjMat._43);
	return &InvViewLightViewProjDepth;
}

void *ComputeEmissive( void *pMaterialPtr, void *pCustomParamList )
{
	static EtColor EmissiveResult[5];

	if( ( !pCustomParamList ) || ( !pMaterialPtr ) )
	{
		memset( EmissiveResult, 0, sizeof(EtColor)*5 );
		return EmissiveResult;
	}

	CEtMaterial *pMaterial = (CEtMaterial*)pMaterialPtr;
	std::vector< SCustomParam > &vecCustomParam = *(std::vector< SCustomParam > *)pCustomParamList;
	
	if( vecCustomParam.size() <= EMISSIVE_INDEX )
	{
		memset( EmissiveResult, 0, sizeof(EtColor)*5 );
		return EmissiveResult;
	}

	PCEmissiveInfo *pPCEmissiveInfo = (PCEmissiveInfo *)vecCustomParam[ EMISSIVE_INDEX ].pUserStruct;
	if( pPCEmissiveInfo == NULL  ) {
		
		vecCustomParam[ EMISSIVE_INDEX ].pUserStruct = pPCEmissiveInfo = (PCEmissiveInfo *)::malloc( sizeof(PCEmissiveInfo) );
		pMaterial->AddUserAllocMem( pPCEmissiveInfo );
	
		pPCEmissiveInfo->nArrayCount = 0;
		for( int i = 0; i < (int)vecCustomParam.size(); i++) {
			EtParameterHandle hParamHandle = vecCustomParam[ i ].hParamHandle;

			if( vecCustomParam[ i ].nVariableCount == 1 ) {
				if( pMaterial->IsParameterName( hParamHandle, "g_EmissivePower" ) ) {
					pPCEmissiveInfo->Param[ 0 ].fPower = vecCustomParam[ i ].fFloat;				
				}			
				else if( pMaterial->IsParameterName( hParamHandle, "g_EmissivePowerRange" ) ) {				
					pPCEmissiveInfo->Param[ 0 ].fRange = vecCustomParam[ i ].fFloat;				
				}
				else if( pMaterial->IsParameterName( hParamHandle, "g_EmissiveAniSpeed" ) ) {
					pPCEmissiveInfo->Param[ 0 ].fAniSpeed = vecCustomParam[ i ].fFloat;
				}		
				else if( pMaterial->IsParameterName( hParamHandle, "g_EmissiveColor" ) ) {		
					memcpy(&pPCEmissiveInfo->Param[ 0 ].Color, vecCustomParam[ i ].fFloat4, sizeof(EtColor));
					pPCEmissiveInfo->nArrayCount = 1;
				}
			}
			else {	
				if( pMaterial->IsParameterName( hParamHandle, "g_EmissiveColor" ) ) {
					pPCEmissiveInfo->nArrayCount = vecCustomParam[ i ].nVariableCount;
					memcpy(pPCEmissiveInfo->Param, vecCustomParam[ i ].pPointer, sizeof(EmissiveParam) * pPCEmissiveInfo->nArrayCount );
				}
			}
		}
	}
	
	for( int i = 0; i < pPCEmissiveInfo->nArrayCount; i++) {
		EmissiveResult[ i ] = pPCEmissiveInfo->Param[ i ].Color *
			(pPCEmissiveInfo->Param[ i ].fPower + pPCEmissiveInfo->Param[ i ].fRange * 
					cosf( g_fTotalElapsedTime * pPCEmissiveInfo->Param[ i ].fAniSpeed ) );
	}	

	return EmissiveResult;
}

void ComputeCoefficient( void *pMaterialPtr, void *pCustomParamList )
{
	const float fMaxRadiusSq = 400.0f;

	CEtMaterial *pMaterial = (CEtMaterial*)pMaterialPtr;
	std::vector< SCustomParam > &vecCustomParam = *(std::vector< SCustomParam > *)pCustomParamList;

	CoefficientInfo *pCoefficientInfo = (CoefficientInfo *)vecCustomParam[ COEFFICIENT_INDEX ].pUserStruct;
	if( pCoefficientInfo == NULL ) {
		vecCustomParam[ COEFFICIENT_INDEX ].pUserStruct = pCoefficientInfo = (CoefficientInfo *)::malloc( sizeof(CoefficientInfo) );
		pMaterial->AddUserAllocMem( pCoefficientInfo );
		
		EtColor SkinColor(0.5f, 0.5f, 0.5f, 1.0f);
		for( int i = 0; i < (int)vecCustomParam.size(); i++) {
			EtParameterHandle hParamHandle = vecCustomParam[ i ].hParamHandle;
			if( vecCustomParam[ i ].nVariableCount == 1 && vecCustomParam[ i ].Type == EPT_VECTOR ) {
				if( pMaterial->IsParameterName( hParamHandle, "g_SkinColor" ) ) {
					SkinColor = *(EtColor*)vecCustomParam[ i ].fFloat4;
					break;
				}
			}
		}
		/*
			원의 방정식 (x-a)^2 + (y-b)^2 = r^2 에서 
			컬러 채널별 SkinColor의 강도에 따라
			반지름 r 을 1 ~ 20 사이의 값을 선택한다. 
			반지름이 1인 경우 최대 강도이며 반지름이 클수록
			f(x) = x 의 그래프에 수렴하게 된다.
			그후 f(0) = 0, f(1) = 1 을 만족하는 적당한 a,b 를 
			2차방정식의 근의 공식을 이용해 구한후
			최종적으로 쉐이더에 넘겨질 방정식의 계수들을 구한다.
		*/
		for( int i = 0; i < 3; i++) {
			float *pColors = SkinColor;
			pCoefficientInfo->fC[ i ] = powf( fMaxRadiusSq, ((0.5f - fabsf( pColors[ i ] - 0.5f )) * 2.0f ));

			if( pColors[ i ] > 0.5f ) {
				pCoefficientInfo->fB[ i ] = -1.0f;
				pCoefficientInfo->fD[ i ] =  0.5f * ( sqrtf( 2.0f * pCoefficientInfo->fC[ i ] - 1.0f ) + 1 );
				pCoefficientInfo->fA[ i ] = 1.0f - pCoefficientInfo->fD[ i ];
			}
			else {
				pCoefficientInfo->fB[ i ] = 1.0f;
				pCoefficientInfo->fD[ i ] =  -0.5f * ( sqrtf( 2.0f * pCoefficientInfo->fC[ i ] - 1.0f ) - 1 );
				pCoefficientInfo->fA[ i ] = 1.0f - pCoefficientInfo->fD[ i ];
			}
		}
		pCoefficientInfo->fA[3] = SkinColor.a;			// fA 에 Intensity 값을 넣어준다...
		pCoefficientInfo->fB[3] = pCoefficientInfo->fC[3] = pCoefficientInfo->fD[3] = 0.0f;
	}
}

void *GetCoefficientA( void *pMaterialPtr, void *pCustomParamList )
{
	ComputeCoefficient( pMaterialPtr, pCustomParamList );

	std::vector< SCustomParam > &vecCustomParam = *(std::vector< SCustomParam > *)pCustomParamList;
	CoefficientInfo *pCoefficientInfo = (CoefficientInfo *)vecCustomParam[ COEFFICIENT_INDEX ].pUserStruct;
	
	ASSERT( pCoefficientInfo != NULL );
	
	return pCoefficientInfo->fA;
}

void *GetCoefficientB( void *pMaterialPtr, void *pCustomParamList )
{
	ComputeCoefficient( pMaterialPtr, pCustomParamList );

	std::vector< SCustomParam > &vecCustomParam = *(std::vector< SCustomParam > *)pCustomParamList;
	CoefficientInfo *pCoefficientInfo = (CoefficientInfo *)vecCustomParam[ COEFFICIENT_INDEX ].pUserStruct;

	ASSERT( pCoefficientInfo != NULL );

	return pCoefficientInfo->fB;
}

void *GetCoefficientC( void *pMaterialPtr, void *pCustomParamList )
{
	ComputeCoefficient( pMaterialPtr, pCustomParamList );

	std::vector< SCustomParam > &vecCustomParam = *(std::vector< SCustomParam > *)pCustomParamList;
	CoefficientInfo *pCoefficientInfo = (CoefficientInfo *)vecCustomParam[ COEFFICIENT_INDEX ].pUserStruct;

	ASSERT( pCoefficientInfo != NULL );

	return pCoefficientInfo->fC;
}

void *GetCoefficientD( void *pMaterialPtr, void *pCustomParamList )
{
	ComputeCoefficient( pMaterialPtr, pCustomParamList );

	std::vector< SCustomParam > &vecCustomParam = *(std::vector< SCustomParam > *)pCustomParamList;
	CoefficientInfo *pCoefficientInfo = (CoefficientInfo *)vecCustomParam[ COEFFICIENT_INDEX ].pUserStruct;

	ASSERT( pCoefficientInfo != NULL );

	return pCoefficientInfo->fD;
}

void *GetAniTime( void *pMaterialPtr, void *pCustomParamList )
{
	static FLOAT fResult = 0.0f;
	CEtMaterial *pMaterial = (CEtMaterial*)pMaterialPtr;
	std::vector< SCustomParam > &vecCustomParam = *(std::vector< SCustomParam > *)pCustomParamList;

	for( int i = 0; i < (int)vecCustomParam.size(); i++) {
		EtParameterHandle hParamHandle = vecCustomParam[ i ].hParamHandle;
		if( vecCustomParam[ i ].nVariableCount == 1 && vecCustomParam[ i ].Type == EPT_FLOAT ) {
			if( pMaterial->IsParameterName( hParamHandle, "g_AniTime" ) ) {
				fResult = fmodf( g_fTotalElapsedTime, vecCustomParam[ i ].fFloat ) / vecCustomParam[ i ].fFloat;
				break;
			}
		}
	}

	return &fResult;
}

// 알파테스트시엔 예전처럼 0.5를 리턴하고 알파 블랜딩시에는 (255 / 256) 을 리턴해 완전 불투명한 부분만 뎊스를 그린다.
void *GetDepthAlphaRef( void *pDummy, void *pDummy2 )	
{
	/*static */const float fAlphaMax = 255.5f;
	/*static */const float fAlphaDivisor = 256.0f;
	static FLOAT fResult = 0.f;
	fResult = (GetEtDevice()->GetAlphaRef()+1.5f) / fAlphaDivisor;
	return &fResult;
}

