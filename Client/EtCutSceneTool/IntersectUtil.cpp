#include "stdafx.h"
#include "EternityEngine.h"
#include "intersectUtil.h"

const float EPSILON = 0.000001f;



CIntersectUtil::CIntersectUtil(void)
{
}

CIntersectUtil::~CIntersectUtil(void)
{
}



// :*: 교차 판정의 원리 및 순서 :*:
// 1. 우선, 광선의 시작 점 뒤쪽에 있는지 판정해서 뒤쪽에 있으면 100% 충돌 실패이므로 그대로 리턴한다.
// 광선의 방향 벡터와 광선의 시작점에서 구의 중심으로 향한 벡터의 내적이 음수, 즉, 두 직선이 이루는
// 각이 둔각일 때이고, 광선의 시작점이 구의 외부일 경우가 되겠다.
//
// 2. 그 다음에, 광선 시작점에서 구의 중심까지의 거리와 광선과 구의 중심까지 그은 선분과  피타고라스의 법칙을 사용하여 
// 두 직선의 거리를 구한다. 그 거리가 반지름 보다 크면 충돌실패.
bool CIntersectUtil::RaySphereIntersect( const IN EtVector3& vRayStartPos, const IN EtVector3& vRayDir /* 반드시 정규화 된 벡터 */, 
                                         const IN float fRadius, const IN EtVector3& vSpherePos,
                                         OUT EtVector3* pvNearIntersectPos, OUT EtVector3* pvFarIntersectPos )
{
	EtVector3 vRayStartToCenter;
	float fDotResult = 0.0f;
	float fLengthSqA = 0.0f;              // 광선 시작점에서 구의 중심까지의 거리 제곱
	float fLengthSqB = 0.0f;              // 구의 중심에서 구의 접점까지의 거리 제곱
	float fLengthSqC = 0.0f;              // 광선 시작점에서 구와의 접점까지의 거리 제곱
	float fRadiusSq = fRadius * fRadius;

	vRayStartToCenter = vSpherePos - vRayStartPos;
	fDotResult = EtVec3Dot( &vRayDir, &vRayStartToCenter );

	fLengthSqA = EtVec3LengthSq( &vRayStartToCenter );

	if( fDotResult < 0.0f && fLengthSqA > fRadiusSq )
		return false;

	fLengthSqC = fDotResult * fDotResult;
	fLengthSqB = fLengthSqA - fLengthSqC;

	if( fRadiusSq < fLengthSqB )
		return false;

	if( NULL != pvNearIntersectPos || NULL != pvFarIntersectPos )
	{
		fLengthSqC = fRadiusSq - fLengthSqB;
	    
		if( pvNearIntersectPos )
			*pvNearIntersectPos = vRayStartPos + (fDotResult - sqrt(fLengthSqC)) * vRayDir;
	    
		if( pvFarIntersectPos )
			*pvFarIntersectPos = vRayStartPos + (fDotResult + sqrt(fLengthSqC)) * vRayDir;
	}

	return true;
}



// 직선과 평면의 교차 판정




//int RayTriIntersect(CVector  orig, CVector dir,CVector  vert0, CVector vert1, CVector vert2,
//					Scalar *t, Scalar *u, Scalar *v)
//{
//	CVector edge1, edge2, tvec, pvec, qvec;
//	Scalar det,inv_det;
//
//	/* find vectors for two edges sharing vert0 */
//	SUB(edge1, vert1, vert0);
//	SUB(edge2, vert2, vert0);
//
//	/* begin calculating determinant - also used to calculate U parameter */
//	CROSS(pvec, dir, edge2);
//
//	/* if determinant is near zero, ray lies in plane of triangle */
//	det = DOT(edge1, pvec);
//
//#ifdef TEST_CULL           /* define TEST_CULL if culling is desired */
//	if (det < EPSILON)
//		return 0;
//
//	/* calculate distance from vert0 to ray origin */
//	SUB(tvec, orig, vert0);
//
//	/* calculate U parameter and test bounds */
//	*u = DOT(tvec, pvec);
//	if (*u < 0.0 || *u > det)
//		return 0;
//
//	/* prepare to test V parameter */
//	CROSS(qvec, tvec, edge1);
//
//	/* calculate V parameter and test bounds */
//	*v = DOT(dir, qvec);
//	if (*v < 0.0 || *u + *v > det)
//		return 0;
//
//	/* calculate t, scale parameters, ray intersects triangle */
//	*t = DOT(edge2, qvec);
//	inv_det = 1.0 / det;
//	*t *= inv_det;
//	*u *= inv_det;
//	*v *= inv_det;
//#else                    /* the non-culling branch */
//	if (det > -EPSILON && det < EPSILON)
//		return 0;
//	inv_det = 1.0 / det;
//
//	/* calculate distance from vert0 to ray origin */
//	SUB(tvec, orig, vert0);
//
//	/* calculate U parameter and test bounds */
//	*u = DOT(tvec, pvec) * inv_det;
//	if (*u < 0.0 || *u > 1.0)
//		return 0;
//
//	/* prepare to test V parameter */
//	CROSS(qvec, tvec, edge1);
//
//	/* calculate V parameter and test bounds */
//	*v = DOT(dir, qvec) * inv_det;
//	if (*v < 0.0 || *u + *v > 1.0)
//		return 0;
//
//	/* calculate t, ray intersects triangle */
//	*t = DOT(edge2, qvec) * inv_det;
//#endif
//	return 1;



// 직선과 폴리곤의 교차판정
bool CIntersectUtil::RayTriIntersect( const IN EtVector3& vRayStart, const IN EtVector3& vRayDir, /* 정규화된 벡터 */
									  const IN EtVector3 vVert0, const IN EtVector3 vVert1, const IN EtVector3 vVert2,
									  float *t, float *u, float *v )
{
	EtVector3 vEdge[ 2 ];
	EtVector3 vTvec, vPvec, vQvec;
	float fDet = 0.0f;
	float fInv_det = 0.0f;

	// vVert0 을 공유하는 두 개의 변을 구한다.
	vEdge[ 0 ] = vVert1 - vVert0;
	vEdge[ 1 ] = vVert2 - vVert0;

	// 행렬식을 계산하기 시작한다. 이 값은 U 값을 계산할 때도 쓰인다.
	EtVec3Cross( &vPvec, &vRayDir, &vEdge[ 1 ] );

	// 행렬식이 제로에 가까워지면 Ray가 평면과 평행하다는 사실~!! 그럼 절대로 충돌이 일어날 수가 없지.
	fDet = EtVec3Dot( &vEdge[ 0 ], &vPvec );

#ifdef TEST_CULL			// 후면 컬링된 것 까지 전부 계산?
	if( det < EPSILON )
		return false;

	// vVert0 부터 직선의 원점까지의 거리
	vTvec = vRayStart - vVert0;

	// U 값을 계산하고 범위안에 있는지 체크한다.
	*u = EtVec3Dot( &vTvec, &vPvec );
	if( *u < 0.0f || *u > fDet )
		return false;

	// V 값 테스트를 위한 준비
	EtVec3Cross( &vQvec, &vTvec, &vEdge[ 0 ] );

	// V 값이 범위안에 있는지 체크한다.
	*v = EtVec3Dot( &vRayDir, &vQvec );
	if( *v < 0.0f || *u + *v > det )
		return false;

	// T 값을 계산하고 확대 파라메터를 만들어서 적용해준다.
	*t = EtVec3Dot( &vRayDir, &vQvec );
	fInv_det = 1.0f / fDet;
	*t *= fInv_det;
	*u *= fInv_det;
	*v *= fInv_det;
#else				 // 컬링을 안 쓸경우.
	if( fDet > -EPSILON && fDet < EPSILON )
		return false;

	fInv_det = 1.0f / fDet;
	
	// vVert0 부터 RayStart 까지의 거리
	vTvec = vRayStart - vVert0;

	// U 값 계산
	*u = EtVec3Dot( &vTvec, &vPvec ) * fInv_det;
	if( *u < 0.0f || *u > 1.0f )
		return false;

	// V 값 계산
	EtVec3Cross( &vQvec, &vTvec, &vEdge[ 0 ] );

	*v = EtVec3Dot( &vRayDir, &vQvec ) * fInv_det;
	if( *v < 0.0f || *u + *v > 1.0f )
		return 0;

	*t = EtVec3Dot( &vEdge[ 1 ], &vQvec ) * fInv_det;
#endif

	return true;
}