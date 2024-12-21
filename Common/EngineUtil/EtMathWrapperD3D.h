#pragma once
#if defined(_DEBUG) || defined (_RDEBUG)
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif
#include <d3dx9.h>

////////////////////////////////////////////////////////////////////////////////////
// Math Function Definition
////////////////////////////////////////////////////////////////////////////////////
typedef D3DXVECTOR2		EtVector2;
typedef D3DXVECTOR3		EtVector3;
typedef D3DXVECTOR4		EtVector4;
typedef D3DXMATRIX		EtMatrix;
typedef D3DXQUATERNION	EtQuat;
typedef D3DXPLANE		EtPlane;
typedef D3DXCOLOR		EtColor;

inline float EtVec2Length( const EtVector2 *pVector )
{	
	return D3DXVec2Length( pVector );
}
inline float EtVec2LengthSq( const EtVector2 *pVector )
{
	return D3DXVec2LengthSq( pVector );
}
inline float EtVec2Dot( const EtVector2 *pVector1, const EtVector2 *pVector2 )
{
	return D3DXVec2Dot( pVector1, pVector2 );
}
inline float EtVec2CCW( const EtVector2 *pVector1, const EtVector2 *pVector2 )	
{
	return D3DXVec2CCW( pVector1, pVector2 );
}
inline EtVector2 *EtVec2Add( EtVector2 *pOut, const EtVector2 *pVector1, const EtVector2 *pVector2 )
{
	return D3DXVec2Add( pOut, pVector1, pVector2 );
}
inline EtVector2 *EtVec2Subtract( EtVector2 *pOut, const EtVector2 *pVector1, const EtVector2 *pVector2 )
{
	return D3DXVec2Subtract( pOut, pVector1, pVector2 );
}
inline EtVector2 *EtVec2Minimize( EtVector2 *pOut, const EtVector2 *pVector1, const EtVector2 *pVector2 )
{
	return D3DXVec2Minimize( pOut, pVector1, pVector2 );
}
inline EtVector2 *EtVec2Maximize( EtVector2 *pOut, const EtVector2 *pVector1, const EtVector2 *pVector2 )
{
	return D3DXVec2Maximize( pOut, pVector1, pVector2 );
}
inline EtVector2 *EtVec2Scale( EtVector2 *pOut, const EtVector2 *pVector1, float fScale )
{
	return D3DXVec2Scale( pOut, pVector1, fScale );
}
inline EtVector2 *EtVec2Lerp( EtVector2 *pOut, const EtVector2 *pVector1, const EtVector2 *pVector2, float fWeight )
{
	return D3DXVec2Lerp( pOut, pVector1, pVector2, fWeight );
}
inline EtVector2 *EtVec2Normalize( EtVector2 *pOut, const EtVector2 *pVector1 )
{
	return D3DXVec2Normalize( pOut, pVector1 );
}
inline EtVector2 *EtVec2Hermite( EtVector2 *pOut, const EtVector2 *pPosition1, const EtVector2 *pTangent1, 
								const EtVector2 *pPosition2, const EtVector2 *pTangent2, float fWeight )
{
	return D3DXVec2Hermite( pOut, pPosition1, pTangent1, pPosition2, pTangent2, fWeight );
}
inline EtVector2 *EtVec2CatmullRom( EtVector2 *pOut, const EtVector2 *pPosition1, const EtVector2 *pPosition2, 
								   const EtVector2 *pPosition3, const EtVector2 *pPosition4, float fWeight )
{
	return D3DXVec2CatmullRom( pOut, pPosition1, pPosition2, pPosition3, pPosition4, fWeight );
}
inline EtVector2 *EtVec2BaryCentric( EtVector2 *pOut, const EtVector2 *pVector1, const EtVector2 *pVector2, 
									const EtVector2 *pVector3, float fWeight1, float fWeight2 )
{
	return D3DXVec2BaryCentric( pOut, pVector1, pVector2, pVector3, fWeight1, fWeight2 );
}
inline EtVector4 *EtVec2Transform( EtVector4 *pOut, const EtVector2 *pVector1, const EtMatrix *pMatrix )
{
	return D3DXVec2Transform( pOut, pVector1, pMatrix );
}
inline EtVector2 *EtVec2TransformCoord( EtVector2 *pOut, const EtVector2 *pVector1, const EtMatrix *pMatrix )
{
	return D3DXVec2TransformCoord( pOut, pVector1, pMatrix );
}
inline EtVector2 *EtVec2TransformNormal( EtVector2 *pOut, const EtVector2 *pVector1, const EtMatrix *pMatrix )
{
	return D3DXVec2TransformNormal( pOut, pVector1, pMatrix );
}
inline EtVector4 *EtVec2TransformArray( EtVector4 *pOut, int nOutStride, const EtVector2 *pVector1, int nVectorStride,
									   const EtMatrix *pMatrix, int nCount )
{
	return D3DXVec2TransformArray( pOut, nOutStride, pVector1, nVectorStride, pMatrix, nCount );
}
inline EtVector2 *EtVec2TransformCoordArray( EtVector2 *pOut, int nOutStride, const EtVector2 *pVector1, int nVectorStride,
											const EtMatrix *pMatrix, int nCount )
{
	return D3DXVec2TransformCoordArray( pOut, nOutStride, pVector1, nVectorStride, pMatrix, nCount );
}
inline EtVector2 *EtVec2TransformNormalArray( EtVector2 *pOut, int nOutStride, const EtVector2 *pVector1, int nVectorStride,
											 const EtMatrix *pMatrix, int nCount )
{
	return D3DXVec2TransformNormalArray( pOut, nOutStride, pVector1, nVectorStride, pMatrix, nCount );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
inline float EtVec3Length( const EtVector3 *pVector )
{	
	return D3DXVec3Length( pVector );
}
inline float EtVec3LengthSq( const EtVector3 *pVector )
{
	return D3DXVec3LengthSq( pVector );
}
inline float EtVec3Dot( const EtVector3 *pVector1, const EtVector3 *pVector2 )
{
	return D3DXVec3Dot( pVector1, pVector2 );
}
inline EtVector3 *EtVec3Cross( EtVector3 *pOut, const EtVector3 *pVector1, const EtVector3 *pVector2 )
{
	return D3DXVec3Cross( pOut, pVector1, pVector2 );
}
inline EtVector3 *EtVec3Add( EtVector3 *pOut, const EtVector3 *pVector1, const EtVector3 *pVector2 )
{
	return D3DXVec3Add( pOut, pVector1, pVector2 );
}
inline EtVector3 *EtVec3Subtract( EtVector3 *pOut, const EtVector3 *pVector1, const EtVector3 *pVector2 )
{
	return D3DXVec3Subtract( pOut, pVector1, pVector2 );
}
inline EtVector3 *EtVec3Minimize( EtVector3 *pOut, const EtVector3 *pVector1, const EtVector3 *pVector2 )
{
	return D3DXVec3Minimize( pOut, pVector1, pVector2 );
}
inline EtVector3 *EtVec3Maximize( EtVector3 *pOut, const EtVector3 *pVector1, const EtVector3 *pVector2 )
{
	return D3DXVec3Maximize( pOut, pVector1, pVector2 );
}
inline EtVector3 *EtVec3Scale( EtVector3 *pOut, const EtVector3 *pVector1, float fScale )
{
	return D3DXVec3Scale( pOut, pVector1, fScale );
}
inline EtVector3 *EtVec3Lerp( EtVector3 *pOut, const EtVector3 *pVector1, const EtVector3 *pVector2, float fWeight )
{
	return D3DXVec3Lerp( pOut, pVector1, pVector2, fWeight );
}
inline EtVector3 *EtVec3Normalize( EtVector3 *pOut, const EtVector3 *pVector1 )
{
	return D3DXVec3Normalize( pOut, pVector1 );
}
inline EtVector3 *EtVec3Hermite( EtVector3 *pOut, const EtVector3 *pPosition1, const EtVector3 *pTangent1, 
								const EtVector3 *pPosition2, const EtVector3 *pTangent2, float fWeight )
{
	return D3DXVec3Hermite( pOut, pPosition1, pTangent1, pPosition2, pTangent2, fWeight );
}
inline EtVector3 *EtVec3CatmullRom( EtVector3 *pOut, const EtVector3 *pPosition1, const EtVector3 *pPosition2, 
								   const EtVector3 *pPosition3, const EtVector3 *pPosition4, float fWeight )
{
	return D3DXVec3CatmullRom( pOut, pPosition1, pPosition2, pPosition3, pPosition4, fWeight );
}
inline EtVector3 *EtVec3BaryCentric( EtVector3 *pOut, const EtVector3 *pVector1, const EtVector3 *pVector2, 
									const EtVector3 *pVector3, float fWeight1, float fWeight2 )
{
	return D3DXVec3BaryCentric( pOut, pVector1, pVector2, pVector3, fWeight1, fWeight2 );
}
inline EtVector4 *EtVec3Transform( EtVector4 *pOut, const EtVector3 *pVector1, const EtMatrix *pMatrix )
{
	return D3DXVec3Transform( pOut, pVector1, pMatrix );
}
inline EtVector3 *EtVec3TransformCoord( EtVector3 *pOut, const EtVector3 *pVector1, const EtMatrix *pMatrix )
{
	return D3DXVec3TransformCoord( pOut, pVector1, pMatrix );
}
inline EtVector3 *EtVec3TransformNormal( EtVector3 *pOut, const EtVector3 *pVector1, const EtMatrix *pMatrix )
{
	return D3DXVec3TransformNormal( pOut, pVector1, pMatrix );
}
inline EtVector3 *EtVec3TransformNormalNUS( EtVector3 *pOut, const EtVector3 *pVector1, const EtMatrix *pMatrix )
{
	EtMatrix InvTransMat = *pMatrix;
	InvTransMat._41 = InvTransMat._42 = InvTransMat._43 = 0.f;
	D3DXMatrixInverse(&InvTransMat, NULL, &InvTransMat);
	D3DXMatrixTranspose(&InvTransMat, &InvTransMat);
	return D3DXVec3TransformNormal( pOut, pVector1, &InvTransMat );
}
inline EtVector4 *EtVec3TransformArray( EtVector4 *pOut, int nOutStride, const EtVector3 *pVector1, int nVectorStride,
									   const EtMatrix *pMatrix, int nCount )
{
	return D3DXVec3TransformArray( pOut, nOutStride, pVector1, nVectorStride, pMatrix, nCount );
}
inline EtVector3 *EtVec3TransformCoordArray( EtVector3 *pOut, int nOutStride, const EtVector3 *pVector1, int nVectorStride,
											const EtMatrix *pMatrix, int nCount )
{
	return D3DXVec3TransformCoordArray( pOut, nOutStride, pVector1, nVectorStride, pMatrix, nCount );
}
inline EtVector3 *EtVec3TransformNormalArray( EtVector3 *pOut, int nOutStride, const EtVector3 *pVector1, int nVectorStride,
											 const EtMatrix *pMatrix, int nCount )
{
	return D3DXVec3TransformNormalArray( pOut, nOutStride, pVector1, nVectorStride, pMatrix, nCount );
}
inline EtVector3 *EtVec3Project( EtVector3 *pOut, const EtVector3 *pVector, const D3DVIEWPORT9 *pViewport, 
								const EtMatrix *pProjection, const EtMatrix *pView, const EtMatrix *pWorld )
{
	return D3DXVec3Project( pOut, pVector, pViewport, pProjection, pView, pWorld);
}
inline EtVector3 *EtVec3Unproject( EtVector3 *pOut, const EtVector3 *pVector, const D3DVIEWPORT9 *pViewport, 
								  const EtMatrix *pProjection, const EtMatrix *pView, const EtMatrix *pWorld )
{
	return D3DXVec3Unproject( pOut, pVector, pViewport, pProjection, pView, pWorld );
}
inline EtVector3 *EtVec3ProjectArray( EtVector3 *pOut, int nOutStride, const EtVector3 *pVector, int nVectorStride, 
									 const D3DVIEWPORT9 *pViewport, const EtMatrix *pProjection, const EtMatrix *pView, 
									 const EtMatrix *pWorld, int nCount )
{
	return D3DXVec3ProjectArray( pOut, nOutStride, pVector, nVectorStride, pViewport, pProjection, pView, pWorld, nCount );
}
inline EtVector3 *EtVec3UnprojectArray( EtVector3 *pOut, int nOutStride, const EtVector3 *pVector, int nVectorStride, 
									   const D3DVIEWPORT9 *pViewport, const EtMatrix *pProjection, const EtMatrix *pView, 
									   const EtMatrix *pWorld, int nCount )
{
	return D3DXVec3UnprojectArray( pOut, nOutStride, pVector, nVectorStride, pViewport, pProjection, pView, pWorld, nCount );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
inline float EtVec4Length( const EtVector4 *pVector )
{	
	return D3DXVec4Length( pVector );
}
inline float EtVec4LengthSq( const EtVector4 *pVector )
{
	return D3DXVec4LengthSq( pVector );
}
inline float EtVec4Dot( const EtVector4 *pVector1, const EtVector4 *pVector2 )
{
	return D3DXVec4Dot( pVector1, pVector2 );
}
inline EtVector4 *EtVec4Add( EtVector4 *pOut, const EtVector4 *pVector1, const EtVector4 *pVector2 )
{
	return D3DXVec4Add( pOut, pVector1, pVector2 );
}
inline EtVector4 *EtVec4Subtract( EtVector4 *pOut, const EtVector4 *pVector1, const EtVector4 *pVector2 )
{
	return D3DXVec4Subtract( pOut, pVector1, pVector2 );
}
inline EtVector4 *EtVec4Minimize( EtVector4 *pOut, const EtVector4 *pVector1, const EtVector4 *pVector2 )
{
	return D3DXVec4Minimize( pOut, pVector1, pVector2 );
}
inline EtVector4 *EtVec4Maximize( EtVector4 *pOut, const EtVector4 *pVector1, const EtVector4 *pVector2 )
{
	return D3DXVec4Maximize( pOut, pVector1, pVector2 );
}
inline EtVector4 *EtVec4Scale( EtVector4 *pOut, const EtVector4 *pVector1, float fScale )
{
	return D3DXVec4Scale( pOut, pVector1, fScale );
}
inline EtVector4 *EtVec4Lerp( EtVector4 *pOut, const EtVector4 *pVector1, const EtVector4 *pVector2, float fWeight )
{
	return D3DXVec4Lerp( pOut, pVector1, pVector2, fWeight );
}
inline EtVector4 *EtVec4Cross( EtVector4 *pOut, const EtVector4 *pVector1, const EtVector4 *pVector2, const EtVector4 *pVector3 )
{
	return D3DXVec4Cross( pOut, pVector1, pVector2, pVector3 );
}
inline EtVector4 *EtVec4Normalize( EtVector4 *pOut, const EtVector4 *pVector1 )
{
	return D3DXVec4Normalize( pOut, pVector1 );
}
inline EtVector4 *EtVec4Hermite( EtVector4 *pOut, const EtVector4 *pPosition1, const EtVector4 *pTangent1, 
								const EtVector4 *pPosition2, const EtVector4 *pTangent2, float fWeight )
{
	return D3DXVec4Hermite( pOut, pPosition1, pTangent1, pPosition2, pTangent2, fWeight );
}
inline EtVector4 *EtVec4CatmullRom( EtVector4 *pOut, const EtVector4 *pPosition1, const EtVector4 *pPosition2, 
								   const EtVector4 *pPosition3, const EtVector4 *pPosition4, float fWeight )
{
	return D3DXVec4CatmullRom( pOut, pPosition1, pPosition2, pPosition3, pPosition4, fWeight );
}
inline EtVector4 *EtVec4BaryCentric( EtVector4 *pOut, const EtVector4 *pVector1, const EtVector4 *pVector2, 
									const EtVector4 *pVector3, float fWeight1, float fWeight2 )
{
	return D3DXVec4BaryCentric( pOut, pVector1, pVector2, pVector3, fWeight1, fWeight2 );
}
inline EtVector4 *EtVec4Transform( EtVector4 *pOut, const EtVector4 *pVector1, const EtMatrix *pMatrix )
{
	return D3DXVec4Transform( pOut, pVector1, pMatrix );
}
inline EtVector4 *EtVec4TransformArray( EtVector4 *pOut, int nOutStride, const EtVector4 *pVector1, int nVectorStride,
									   const EtMatrix *pMatrix, int nCount )
{
	return D3DXVec4TransformArray( pOut, nOutStride, pVector1, nVectorStride, pMatrix, nCount );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
inline EtMatrix *EtMatrixIdentity( EtMatrix *pOut )
{
	return D3DXMatrixIdentity( pOut );
}
inline BOOL EtMatrixIsIdentity( const EtMatrix *pMatrix )
{
	return D3DXMatrixIsIdentity( pMatrix );
}
inline float EtMatrixDeterminant( const EtMatrix *pMatrix )
{
	return D3DXMatrixDeterminant( pMatrix );
}
inline int EtMatrixDecompose( EtVector3 *pOutScale, EtQuat *pOutRotation, EtVector3 *pOutTranslation, const EtMatrix *pMatrix )
{
	return D3DXMatrixDecompose( pOutScale, pOutRotation, pOutTranslation, pMatrix );
}
inline EtMatrix *EtMatrixTranspose( EtMatrix *pOut, const EtMatrix *pMatrix )
{
	return D3DXMatrixTranspose( pOut, pMatrix );
}
inline EtMatrix *EtMatrixMultiply( EtMatrix *pOut, const EtMatrix *pMatrix1, const EtMatrix *pMatrix2 )
{
	return D3DXMatrixMultiply( pOut, pMatrix1, pMatrix2 );
}
inline EtMatrix *EtMatrixMultiplyTranspose( EtMatrix *pOut, const EtMatrix *pMatrix1, const EtMatrix *pMatrix2 )
{
	return D3DXMatrixMultiplyTranspose( pOut, pMatrix1, pMatrix2 );
}
inline EtMatrix *EtMatrixInverse( EtMatrix *pOut, float *pDeterminant, const EtMatrix *pMatrix )
{
	return D3DXMatrixInverse( pOut, pDeterminant, pMatrix );
}
inline EtMatrix *EtMatrixScaling( EtMatrix *pOut, float fScakeX, float fScakeY, float fScakeZ )
{
	return D3DXMatrixScaling( pOut, fScakeX, fScakeY, fScakeZ );
}
inline EtMatrix *EtMatrixTranslation( EtMatrix *pOut, float fTransX, float fTransY, float fTransZ )
{
	return D3DXMatrixTranslation( pOut, fTransX, fTransY, fTransZ );
}
inline EtMatrix *EtMatrixRotationX( EtMatrix *pOut, float fAngle )
{
	return D3DXMatrixRotationX( pOut, fAngle );
}
inline EtMatrix *EtMatrixRotationY( EtMatrix *pOut, float fAngle )
{
	return D3DXMatrixRotationY( pOut, fAngle );
}
inline EtMatrix *EtMatrixRotationZ( EtMatrix *pOut, float fAngle )
{
	return D3DXMatrixRotationZ( pOut, fAngle );
}
inline EtMatrix *EtMatrixRotationAxis( EtMatrix *pOut, const EtVector3 *pVector, float fAngle )
{
	return D3DXMatrixRotationAxis( pOut, pVector, fAngle );
}
inline EtMatrix *EtMatrixRotationQuaternion( EtMatrix *pOut, const EtQuat * pQuat )
{
	return D3DXMatrixRotationQuaternion( pOut, pQuat );
}
inline EtMatrix *EtMatrixRotationYawPitchRoll( EtMatrix *pOut, float fYaw, float fPitch, float fRoll )
{
	return D3DXMatrixRotationYawPitchRoll( pOut, fYaw, fPitch, fRoll );
}
inline EtMatrix *EtMatrixTransformation( EtMatrix *pOut, const EtVector3 *pScalingCenter, const EtQuat *pScalingRotation, 
										const EtVector3 *pScaling, const EtVector3 *pRotationCenter, const EtQuat *pRotation, const EtVector3 *pTranslation )
{
	return D3DXMatrixTransformation( pOut, pScalingCenter, pScalingRotation, pScaling, pRotationCenter, pRotation, pTranslation );
}
inline EtMatrix *EtMatrixTransformation2D( EtMatrix *pOut, const EtVector2 *pScalingCenter, float ScalingRotation, 
										  const EtVector2 *pScaling, const EtVector2 *pRotationCenter, float Rotation, const EtVector2 *pTranslation )
{
	return D3DXMatrixTransformation2D( pOut, pScalingCenter, ScalingRotation, pScaling, pRotationCenter, Rotation, pTranslation );
}
inline EtMatrix *EtMatrixAffineTransformation( EtMatrix *pOut, float fScaling, const EtVector3 *pRotationCenter, 
											  const EtQuat *pRotation, const EtVector3 *pTranslation )
{
	return D3DXMatrixAffineTransformation( pOut, fScaling, pRotationCenter, pRotation, pTranslation );
}
inline EtMatrix *EtMatrixAffineTransformation2D( EtMatrix *pOut, float fScaling, const EtVector2 *pRotationCenter, 
												float fRotation, const EtVector2 *pTranslation )
{
	return D3DXMatrixAffineTransformation2D( pOut, fScaling, pRotationCenter, fRotation, pTranslation );
}
inline EtMatrix *EtMatrixLookAtRH( EtMatrix *pOut, const EtVector3 *pEye, const EtVector3 *pAt, const EtVector3 *pUp )
{
	return D3DXMatrixLookAtRH( pOut, pEye, pAt, pUp );
}
inline EtMatrix *EtMatrixLookAtLH( EtMatrix *pOut, const EtVector3 *pEye, const EtVector3 *pAt, const EtVector3 *pUp )
{
	return D3DXMatrixLookAtLH( pOut, pEye, pAt, pUp );
}
inline EtMatrix *EtMatrixPerspectiveRH( EtMatrix *pOut, float fWidth, float fHeight, float fNear, float fFar )
{
	return D3DXMatrixPerspectiveRH( pOut, fWidth, fHeight, fNear, fFar );
}
inline EtMatrix *EtMatrixPerspectiveLH( EtMatrix *pOut, float fWidth, float fHeight, float fNear, float fFar )
{
	return D3DXMatrixPerspectiveLH( pOut, fWidth, fHeight, fNear, fFar );
}
inline EtMatrix *EtMatrixPerspectiveFovRH( EtMatrix *pOut, float fFovY, float fAspect, float fNear, float fFar )
{
	return D3DXMatrixPerspectiveFovRH( pOut, fFovY, fAspect, fNear, fFar );
}
inline EtMatrix *EtMatrixPerspectiveFovLH( EtMatrix *pOut, float fFovY, float fAspect, float fNear, float fFar )
{
	return D3DXMatrixPerspectiveFovLH( pOut, fFovY, fAspect, fNear, fFar );
}
inline EtMatrix *EtMatrixPerspectiveOffCenterRH( EtMatrix *pOut, float fMinX, float fMaxX, float fMinY, float fMaxY,
												float fMinZ, float fMaxZ )
{
	return D3DXMatrixPerspectiveOffCenterRH( pOut, fMinX, fMaxX, fMinY, fMaxY, fMinZ, fMaxZ );
}
inline EtMatrix *EtMatrixPerspectiveOffCenterLH( EtMatrix *pOut, float fMinX, float fMaxX, float fMinY, float fMaxY,
												float fMinZ, float fMaxZ )
{
	return D3DXMatrixPerspectiveOffCenterLH( pOut, fMinX, fMaxX, fMinY, fMaxY, fMinZ, fMaxZ );
}
inline EtMatrix *EtMatrixOrthoRH( EtMatrix *pOut, float fWidth, float fHeight, float fNear, float fFar )
{
	return D3DXMatrixOrthoRH( pOut, fWidth, fHeight, fNear, fFar );
}
inline EtMatrix *EtMatrixOrthoLH( EtMatrix *pOut, float fWidth, float fHeight, float fNear, float fFar )
{
	return D3DXMatrixOrthoLH( pOut, fWidth, fHeight, fNear, fFar );
}
inline EtMatrix *EtMatrixOrthoOffCenterRH( EtMatrix *pOut, float fMinX, float fMaxX, float fMinY, float fMaxY,
										  float fMinZ, float fMaxZ )
{
	return D3DXMatrixOrthoOffCenterRH( pOut, fMinX, fMaxX, fMinY, fMaxY, fMinZ, fMaxZ );
}
inline EtMatrix *EtMatrixOrthoOffCenterLH( EtMatrix *pOut, float fMinX, float fMaxX, float fMinY, float fMaxY,
										  float fMinZ, float fMaxZ )
{
	return D3DXMatrixOrthoOffCenterLH( pOut, fMinX, fMaxX, fMinY, fMaxY, fMinZ, fMaxZ );
}
inline EtMatrix *EtMatrixShadow( EtMatrix *pOut, const EtVector4 *pLight, const EtPlane *pPlane )
{
	return D3DXMatrixShadow( pOut, pLight, pPlane );
}
inline EtMatrix *EtMatrixReflect( EtMatrix *pOut, const EtPlane *pPlane )
{
	return D3DXMatrixReflect( pOut, pPlane );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
inline float EtQuaternionLength( const EtQuat *pQuat )
{
	return D3DXQuaternionLength( pQuat );
}
inline float EtQuaternionLengthSq( const EtQuat *pQuat )
{
	return D3DXQuaternionLengthSq( pQuat );
}
inline float EtQuaternionDot( const EtQuat *pQuat1, const EtQuat *pQuat2 )
{
	return D3DXQuaternionDot( pQuat1, pQuat2 );
}
inline EtQuat *EtQuaternionIdentity( EtQuat *pOut )
{
	return D3DXQuaternionIdentity( pOut );
}
inline BOOL EtQuaternionIsIdentity( const EtQuat *pQuat )
{
	return D3DXQuaternionIsIdentity( pQuat );
}
inline EtQuat *EtQuaternionConjugate( EtQuat *pOut, const EtQuat *pQuat )
{
	return D3DXQuaternionConjugate( pOut, pQuat );
}
inline void EtQuaternionToAxisAngle( EtQuat *pOut, EtVector3 *pAxis, float *pAngle )
{
	D3DXQuaternionToAxisAngle( pOut, pAxis, pAngle );
}
inline EtQuat *EtQuaternionRotationMatrix( EtQuat *pOut, const EtMatrix *pMatrix )
{
	return D3DXQuaternionRotationMatrix( pOut, pMatrix );
}
inline EtQuat *EtQuaternionRotationAxis( EtQuat *pOut, EtVector3 *pVector, float fAngle )
{
	return D3DXQuaternionRotationAxis( pOut, pVector, fAngle );
}
inline EtQuat *EtQuaternionRotationYawPitchRoll( EtQuat *pOut, float fYaw, float fPitch, float fRoll )
{
	return D3DXQuaternionRotationYawPitchRoll( pOut, fYaw, fPitch, fRoll );
}
inline EtQuat *EtQuaternionMultiply( EtQuat *pOut, const EtQuat *pQuat1, const EtQuat *pQuat2 )
{
	return D3DXQuaternionMultiply( pOut, pQuat1, pQuat2 );
}
inline EtQuat *EtQuaternionNormalize( EtQuat *pOut, const EtQuat *pQuat )
{
	return D3DXQuaternionNormalize( pOut, pQuat );
}
inline EtQuat *EtQuaternionInverse( EtQuat *pOut, const EtQuat *pQuat )
{
	return D3DXQuaternionInverse( pOut, pQuat );
}
inline EtQuat *EtQuaternionLn( EtQuat *pOut, const EtQuat *pQuat )
{
	return D3DXQuaternionLn( pOut, pQuat );
}
inline EtQuat *EtQuaternionExp( EtQuat *pOut, const EtQuat *pQuat )
{
	return D3DXQuaternionExp( pOut, pQuat );
}
inline EtQuat *EtQuaternionSlerp( EtQuat *pOut, const EtQuat *pQuat1, const EtQuat *pQuat2, float fWeight )
{
	return D3DXQuaternionSlerp( pOut, pQuat1, pQuat2, fWeight );
}
inline EtQuat *EtQuaternionSquad( EtQuat *pOut, const EtQuat *pQuat1, const EtQuat *pQuatA, 
								 const EtQuat *pQuatB, const EtQuat *pQuatC, float fWeight )
{
	return D3DXQuaternionSquad( pOut, pQuat1, pQuatA, pQuatB, pQuatC, fWeight );
}
inline EtQuat *EtQuaternionSquadSetup( EtQuat *pOutA, const EtQuat *pQuat1, const EtQuat *pQuatA, const EtQuat *pQuatB, 
									  const EtQuat *pQuatC, float fWeight )
{
	return D3DXQuaternionSquad( pOutA, pQuat1, pQuatA, pQuatB, pQuatC, fWeight );
}
inline EtQuat *EtQuaternionBaryCentric( EtQuat *pOut, const EtQuat *pQuat1, const EtQuat *pQuat2, 
									   const EtQuat *pQuat3, float fWeight1, float fWeight2 )
{
	return D3DXQuaternionBaryCentric( pOut, pQuat1, pQuat2, pQuat3, fWeight1, fWeight2 );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
inline float EtPlaneDot( const EtPlane *pPlane, const EtVector4 *pVector )
{
	return D3DXPlaneDot( pPlane, pVector );
}
inline float EtPlaneDotCoord( const EtPlane *pPlane, const EtVector3 *pVector )
{
	return D3DXPlaneDotCoord( pPlane, pVector );
}
inline float EtPlaneDotNormal( const EtPlane *pPlane, const EtVector3 *pVector )
{
	return D3DXPlaneDotNormal( pPlane, pVector );
}
inline EtPlane *EtPlaneScale( EtPlane *pOut, const EtPlane *pPlane, float fScale )
{
	return D3DXPlaneScale( pOut, pPlane, fScale );
}
inline EtPlane *EtPlaneNormalize( EtPlane *pOut, const EtPlane *pPlane )
{
	return D3DXPlaneNormalize( pOut, pPlane );
}
inline EtVector3 *EtPlaneIntersectLine( EtVector3 *pOut, const EtPlane *pPlane, const EtVector3 *pVector1, const EtVector3 *pVector2 )
{
	return D3DXPlaneIntersectLine( pOut, pPlane, pVector1, pVector2 );
}
inline EtPlane *EtPlaneFromPointNormal( EtPlane *pOut, const EtVector3 *pPoint, const EtVector3 *pNormal )
{
	return D3DXPlaneFromPointNormal( pOut, pPoint, pNormal );
}
inline EtPlane *EtPlaneFromPoints( EtPlane *pOut, const EtVector3 *pVector1, const EtVector3 *pVector2, const EtVector3 *pVector3 )
{
	return D3DXPlaneFromPoints( pOut, pVector1, pVector2, pVector3 );
}
inline EtPlane *EtPlaneTransform( EtPlane *pOut, const EtPlane *pPlane, const EtMatrix *pMatrix )
{
	return D3DXPlaneTransform( pOut, pPlane, pMatrix );
}
inline EtPlane *EtPlaneTransformArray( EtPlane *pOut, UINT nOutStride, const EtPlane *pPlane, UINT nPStride, 
									  const EtMatrix *pMatrix, UINT nCount )
{
	return D3DXPlaneTransformArray( pOut, nOutStride, pPlane, nPStride, pMatrix, nCount );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
inline EtColor *EtColorNegative( EtColor *pOut, const EtColor *pColor )
{
	return D3DXColorNegative( pOut, pColor );
}
inline EtColor *EtColorAdd( EtColor *pOut, const EtColor *pColor1, const EtColor *pColor2 )
{
	return D3DXColorAdd( pOut, pColor1, pColor2 );
}
inline EtColor *EtColorSubtract( EtColor *pOut, const EtColor *pColor1, const EtColor *pColor2 )
{
	return D3DXColorSubtract( pOut, pColor1, pColor2 );
}
inline EtColor *EtColorScale( EtColor *pOut, const EtColor *pColor, float fScale )
{
	return D3DXColorScale( pOut, pColor, fScale );
}
inline EtColor *EtColorModulate( EtColor *pOut, const EtColor *pColor1, const EtColor *pColor2 )
{
	return D3DXColorModulate( pOut, pColor1, pColor2 );
}
inline EtColor *EtColorLerp( EtColor *pOut, const EtColor *pColor1, const EtColor *pColor2, float fWeight )
{
	return D3DXColorLerp( pOut, pColor1, pColor2, fWeight );
}
inline EtColor *EtColorAdjustSaturation( EtColor *pOut, const EtColor *pColor, float fSaturation )
{
	return D3DXColorAdjustSaturation( pOut, pColor, fSaturation );
}
inline EtColor *EtColorAdjustContrast( EtColor *pOut, const EtColor *pColor, float fContrast )
{
	return D3DXColorAdjustContrast( pOut, pColor, fContrast );
}


