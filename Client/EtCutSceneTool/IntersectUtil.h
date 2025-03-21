#pragma once



// 충돌 유틸을 모아놓은 클래스. 
class CIntersectUtil
{
public:
    CIntersectUtil(void);
    ~CIntersectUtil(void);

    // 포인터로 나가는 값에다가 NULL을 셋팅해주면 값을 넣어주지 않도록 한다.
    static bool RaySphereIntersect( const IN EtVector3& vRayStartPos, const IN EtVector3& vRayDir, 
									const IN float fRadius, const IN EtVector3& vSpherePos,
									OUT EtVector3* pvNearIntersectPos, OUT EtVector3* pvFarIntersectPos );

	// 직선과 평면의 교차 판정

	// 직선과 폴리곤의 교차 판정
	static bool RayTriIntersect( const IN EtVector3& vRayStart, const IN EtVector3& vRayDir, 
								 const IN EtVector3 vVert0, const IN EtVector3 vVert1, const IN EtVector3 vVert2,
								 float *t, float *u, float *v );
};
