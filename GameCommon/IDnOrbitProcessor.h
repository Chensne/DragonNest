#pragma once
#include "SignalHeader.h"
#include "EtMatrixEx.h"


struct S_PROJECTILE_PROPERTY;		// CDnProjectile::S_ORBIT_PROPERTY 에 정의되어있음.


// OrbitType:Homing, VelocityType:Constant 타입의 호밍 발사체를 처리하는 클래스
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
	
	// 궤적 계산에 필요한 값들을 위한 공통 인터페이스
	virtual void Initialize( const MatrixEx& Cross, const MatrixEx& OffsetCross ) {};
	virtual void SetTargetActor( DnActorHandle hActor ) {};
	virtual void SetTargetPosition( EtVector3 vTarget ) {};
	virtual void SetValidTimePointer( int* piValidTime ) {};

	// 게임서버용 룸 객체 셋팅
#ifdef _GAMESERVER
	void SetRoom( CMultiRoom* pRoom ) { m_pRoom = pRoom; };
#endif

	virtual void ProcessOrbit( /*IN OUT*/ MatrixEx& Cross, EtVector3& vPrevPos, LOCAL_TIME LocalTime, float fDelta ) = 0;

	// 디폴트로 CDnProjectile::ProcessDestroyOrbit() 에서 fallgravity 로 설정된 발사체의 궤적 처리. 클라이언트에서만 사용한다.
	virtual void ProcessFallGravityOrbit( MatrixEx& Cross, EtVector3& vPrevPos, LOCAL_TIME LocalTime, float fDelta, float& fGravity, float fFallGravityAccell );

	// 궤적 처리 객체를 생성하는 팩토리
	static IDnOrbitProcessor* Create( const MatrixEx& Cross, const MatrixEx& OffsetCross, const S_PROJECTILE_PROPERTY* pOrbitProperty );

protected:
	int m_nTargetPartsIndex;
	int m_nTargetPartsBoneIndex;	//선택된 Parts에서 실제 Bone정보를 가진 Index..(MonsterParts를 Vector로 가지고, 다시 _Info가 _PartsInfo를 vector로 가짐)
public:
	//파츠몬스터일경우 선택된 파츠 인덱스를 설정
	virtual void SetTargetPartsIndex(int nPartsIndex, int nBoneIndex) { m_nTargetPartsIndex = nPartsIndex; m_nTargetPartsBoneIndex = nBoneIndex; };

	EtVector3 FindTargetPosition(DnActorHandle hActor, int nPartsIndex, int nBoneIndex);
};