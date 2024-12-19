#pragma once



// �浹 ��ƿ�� ��Ƴ��� Ŭ����. 
class CIntersectUtil
{
public:
    CIntersectUtil(void);
    ~CIntersectUtil(void);

    // �����ͷ� ������ �����ٰ� NULL�� �������ָ� ���� �־����� �ʵ��� �Ѵ�.
    static bool RaySphereIntersect( const IN EtVector3& vRayStartPos, const IN EtVector3& vRayDir, 
									const IN float fRadius, const IN EtVector3& vSpherePos,
									OUT EtVector3* pvNearIntersectPos, OUT EtVector3* pvFarIntersectPos );

	// ������ ����� ���� ����

	// ������ �������� ���� ����
	static bool RayTriIntersect( const IN EtVector3& vRayStart, const IN EtVector3& vRayDir, 
								 const IN EtVector3 vVert0, const IN EtVector3 vVert1, const IN EtVector3 vVert2,
								 float *t, float *u, float *v );
};
