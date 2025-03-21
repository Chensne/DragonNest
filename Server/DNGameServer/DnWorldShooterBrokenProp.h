#pragma once
#include "DnWorldBrokenProp.h"
#include "DnSkillPropComponent.h"
#include "MAActorProp.h"


// 무언가 쏘면서 hit 되고 내구도 다 되면 Broken 되는 프랍.
class CDnWorldShooterBrokenProp : public CDnWorldBrokenProp, 
								  public MAActorProp,
								  public CDnDamageBase,
								  public TBoostMemoryPool<CDnWorldShooterBrokenProp>
{
private:
	CDnSkillPropComponent m_SkillComponent;
	string m_strLastActionName;

public:
	CDnWorldShooterBrokenProp( CMultiRoom* pRoom );
	virtual ~CDnWorldShooterBrokenProp( void );

	bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );
	bool InitializeTable( int nTableID );
	void ReleasePostCustomParam( void ) 
	{ 
		if( m_pData && m_bPostCreateCustomParam )
		{
			ShooterBrokenStruct* pShooterBrokenStruct = static_cast<ShooterBrokenStruct*>( m_pData );
			SAFE_DELETE( pShooterBrokenStruct );
			m_pData = NULL;		// 반드시 NULL 로 해주어야 이 클래스를 상속받은 클래스의 소멸자에서 다시 지워지지 않습니다.
		}
	};

	void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );
	void OnSyncComplete( CDNUserSession* pBreakIntoGameSession/*=NULL*/ );
	void OnChangeAction( const char *szPrevAction );
	
	// from CDnDamageBase
	virtual DnActorHandle GetActorHandle( void ) { return m_hMonster; };
	virtual DWORD GetDamageObjectUniqueID() { return GetUniqueID(); }
};
