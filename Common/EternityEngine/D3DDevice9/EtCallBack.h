#pragma once

int VariableCountOne(void *pDummy, void *pDummy2);
int VariableCountEmissive( void *pMaterialPtr, void *pCustomParamList );
int VariableCountDirLight(void *pDummy, void *pDummy2);
int VariableCountPointLight(void *pDummy, void *pDummy2);
int VariableCountSpotLight(void *pDummy, void *pDummy2);

///////////////////////////////////////////////////////////////////////////////////////////////////

void *GetProjectionMat( void *pDummy, void *pDummy2 );
void *GetViewMat( void *pDummy, void *pDummy2 );
void *GetViewRotProjMat( void *pDummy, void *pDummy2 );
void *GetPrevViewRotProjMat( void *pDummy, void *pDummy2 );
void *GetViewPosition( void *pDummy, void *pDummy2 );

void *GetFogFactor( void *pDummy, void *pDummy2 );
void *GetFogColor( void *pDummy, void *pDummy2 );
void *GetFogSkyBoxTex( void *pDummy, void *pDummy2 );
void *GetScreenSizeScale( void *pDummy, void *pDummy2 );

void *GetLightAmbient( void *pDummy, void *pDummy2 );

void *GetDirLightCount( void *pDummy, void *pDummy2 );
void *GetDirLightDirection( void *pDummy, void *pDummy2 );
void *GetDirLightDiffuse( void *pDummy, void *pDummy2 );
void *GetDirLightSpecular( void *pDummy, void *pDummy2 );

void *GetPointLightCount( void *pDummy, void *pDummy2 );
void *GetPointLightPosition( void *pDummy, void *pDummy2 );
void *GetPointLightDiffuse( void *pDummy, void *pDummy2 );
void *GetPointLightSpecular( void *pDummy, void *pDummy2 );

void *GetSpotLightCount( void *pDummy, void *pDummy2 );
void *GetSpotLightDirection( void *pDummy, void *pDummy2 );
void *GetSpotLightPosition( void *pDummy, void *pDummy2 );
void *GetSpotLightDiffuse( void *pDummy, void *pDummy2 );
void *GetSpotLightSpecular( void *pDummy, void *pDummy2 );

void *GetShadowMap( void *pDummy, void *pDummy2 );
void *GetShadowDensity( void *pDummy, void *pDummy2 );
void *GetShadowFetchDistance( void *pDummy, void *pDummy2 );

void *GetElapsedTime( void *pDummy, void *pDummy2 );
void *GetEnvTexture( void *pDummy, void *pDummy2 );
void *GetBackBuffer( void *pDummy, void *pDummy2 );

void *GetWorldMat( void *pWorldMat, void *pPrevWorldMat );
void *GetInvViewMat( void *pDummy, void *pDummy2 );
void *GetWorldViewMat( void *pWorldMat, void *pPrevWorldMat );
void *GetViewProjMat( void *pWorldMat, void *pPrevWorldMat );
void *GetWorldViewProjMat( void *pWorldMat, void *pPrevWorldMat );
void *GetPrevWorldViewProjMat( void *pWorldMat, void *pPrevWorldMat );
void *GetInvWorldViewPrevWorldViewProjMat( void *pWorldMat, void *pPrevWorldMat );
void *GetModelViewMat( void *pWorldMat, void *pPrevWorldMat );
void *GetInvViewModelViewMat( void *pWorldMat, void *pPrevWorldMat );
void *GetWorldLightViewProjMat( void *pWorldMat, void *pPrevWorldMat );
void *GetInvViewLightViewProjMat( void *pWorldMat, void *pPrevWorldMat );
void *GetWorldLightViewProjDepth( void *pWorldMat, void *pPrevWorldMat );
void *GetInvViewLightViewProjDepth( void *pWorldMat, void *pPrevWorldMat );

void *ComputeEmissive( void *pMaterialPtr, void *pCustomParamList );
void *GetCoefficientA( void *pMaterialPtr, void *pCustomParamList );
void *GetCoefficientB( void *pMaterialPtr, void *pCustomParamList );
void *GetCoefficientC( void *pMaterialPtr, void *pCustomParamList );
void *GetCoefficientD( void *pMaterialPtr, void *pCustomParamList );
void *GetAniTime( void *pMaterialPtr, void *pCustomParamList );
void *GetDepthAlphaRef( void *pDummy, void *pDummy2 );
