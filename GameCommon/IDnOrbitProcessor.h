#pragma once
#include "SignalHeader.h"
#include "EtMatrixEx.h"


struct S_PROJECTILE_PROPERTY;		// CDnProjectile::S_ORBIT_PROPERTY �� ���ǵǾ�����.


// OrbitType:Homing, VelocityType:Constant Ÿ���� ȣ�� �߻�ü�� ó���ϴ� Ŭ����
class IDnOrbitProcessor
{
protected:
#ifdef _GAMESERVER
	CMultiRoom* m_pRoom;
#endif


public:
#ifdef _GAMESERVER
	IDnOrbitProcessor( void ) : m_pRoom( NULL ) 
	{
		m_nTargetPartsBoneIndex = m_nTargetPartsIndex = -1;
	};
#else
	IDnOrbitProcessor( void ) 
	{
		m_nTargetPartsBoneIndex = m_nTargetPartsIndex = -1;
	};
#endif
	virtual ~IDnOrbitProcessor( void ) {};
	
	// ���� ��꿡 �ʿ��� ������ ���� ���� �������̽�
	virtual void Initialize( const MatrixEx& Cross, const MatrixEx& OffsetCross ) {};
	virtual void SetTargetActor( DnActorHandle hActor ) {};
	virtual void SetTargetPosition( EtVector3 vTarget ) {};
	virtual void SetValidTimePointer( int* piValidTime ) {};

	// ���Ӽ����� �� ��ü ����
#ifdef _GAMESERVER
	void SetRoom( CMultiRoom* pRoom ) { m_pRoom = pRoom; };
#endif

	virtual void ProcessOrbit( /*IN OUT*/ MatrixEx& Cross, EtVector3& vPrevPos, LOCAL_TIME LocalTime, float fDelta ) = 0;

	// ����Ʈ�� CDnProjectile::ProcessDestroyOrbit() ���� fallgravity �� ������ �߻�ü�� ���� ó��. Ŭ���̾�Ʈ������ ����Ѵ�.
	virtual void ProcessFallGravityOrbit( MatrixEx& Cross, EtVector3& vPrevPos, LOCAL_TIME LocalTime, float fDelta, float& fGravity, float fFallGravityAccell );

	// ���� ó�� ��ü�� �����ϴ� ���丮
	static IDnOrbitProcessor* Create( const MatrixEx& Cross, const MatrixEx& OffsetCross, const S_PROJECTILE_PROPERTY* pOrbitProperty );

protected:
	int m_nTargetPartsIndex;
	int m_nTargetPartsBoneIndex;	//���õ� Parts���� ���� Bone������ ���� Index..(MonsterParts�� Vector�� ������, �ٽ� _Info�� _PartsInfo�� vector�� ����)
public:
	//���������ϰ�� ���õ� ���� �ε����� ����
	virtual void SetTargetPartsIndex(int nPartsIndex, int nBoneIndex) { m_nTargetPartsIndex = nPartsIndex; m_nTargetPartsBoneIndex = nBoneIndex; };

	EtVector3 FindTargetPosition(DnActorHandle hActor, int nPartsIndex, int nBoneIndex);
};