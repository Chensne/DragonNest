#pragma once
#include "DnMonsterActor.h"
#include "MAStaticMovement.h"
#include "MASingleBody.h"
#include "MASingleDamage.h"
#include "DnSkill.h"


// �õ巡��׽�Ʈ�������� ���̴� ������ ���� ���� ����.
// 235792 �� ���ͷ� �߰��Ǵ� �ű� ���� ����.
// AI �� ������� �ʴ´�.
class CDnCannonMonsterActor : public TDnMonsterActor< MASingleBody, MASingleDamage >
{
private:
	// �� ���� ���͸� �����ϰ� �ִ� ������ �ڵ�.
	DnActorHandle m_hMasterPlayerActor;
	EtVector3 m_vNowShootDir;				// ���� ���� ���� ��ǥ���� �߻�ü ����. �׼������� ���� ����� ���� �ǹ�.
	map<int, const ProjectileStruct*> m_mapCannonProjectileSignalBySkillID;
	EtVector3 m_vCannonLookDirection;
	bool m_bHaveMaster; // Rotha ���� ������ �����Ͱ� �ִ°�>?

	// ���� ������ �ٶ󺸰� �ִ� ȸ�� ���� ���Ե� EtMatrixEx
	MatrixEx m_matExCannonRotated;
	MatrixEx m_matExOriginal;

	// ���� ��ų�� ���������� ���� �ִ�.
	// Ŭ���̾�Ʈ������ ��Ÿ�� ǥ�ø��� ���� �ʿ��� �����͸� ����Ѵ�.
	// Ŭ���̾�Ʈ������ ������ UseSkill �̳� ��ų �ý����� ���ư��� �ʽ��ϴ�.
	// ���� ��/�� ���� ��� �� ���� ����.	

public:
	CDnCannonMonsterActor( int nClassID, bool bProcess = true );
	virtual ~CDnCannonMonsterActor();

	virtual MAMovementBase* CreateMovement()
	{
		MAMovementBase* pMovement = new MAStaticMovement();
		return pMovement;
	}

	virtual bool Initialize( void );
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );
	virtual void OnDie( DnActorHandle hHitter );
	virtual void SetDestroy();
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void ProcessCannonLook(bool bLocal);
	virtual void Look( EtVector2& vVec, bool bForceRotate=true);
	
	virtual void OnHitSuccess( LOCAL_TIME LocalTime, DnActorHandle hActor );
	// from CDnMonsterActor
	virtual void OnSkillUse( int iSkillID, int iLevel );
	void SetMasterPlayerActor( DnActorHandle hMasterPlayerActor );
	void ClearMasterPlayerActor( void );
	bool GetCannonGroundHitPosition( /*IN*/ DnCameraHandle hCamera,  /*OUT*/ EtVector3& vCannonGroundHitPos );
	void OnMasterPlayerActorDie( void );
	DnActorHandle GetMasterPlayerActor( void ) { return m_hMasterPlayerActor; };

	const EtVector3& GetNowShootDir( void ) { return m_vNowShootDir; };

	void SetCannonLookDirection(EtVector3 *Direction) { m_vCannonLookDirection = *Direction; }
	EtVector3 GetCannonLookDricetion(){return m_vCannonLookDirection;}
	bool IsHaveMaster(){return m_bHaveMaster;}

	virtual void OnCreateProjectileBegin( /*IN OUT*/ MatrixEx& LocalCross );
	virtual void OnCreateProjectileEnd( void ); 
	virtual void OnComboCount( int nValue, int nDelay );
	
};
