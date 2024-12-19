#pragma once
#include "DnWorldBrokenProp.h"
#include "DnSkillPropComponent.h"
#include "MAActorProp.h"

class CDnPropState;

// #24282 복수의 내구도를 가지는 broken prop
class CDnWorldMultiDurabilityProp : public CDnWorldBrokenProp, 
									public MAActorProp,
									public CDnDamageBase,
									public TBoostMemoryPool<CDnWorldMultiDurabilityProp>
{
private:
	CDnSkillPropComponent m_SkillComponent;
	//string m_strLastActionName;

	struct S_STATE_INFO
	{
		const char* pActionNameWhenSubDurabilityZero;
		int iOriginalDurability;
		int iNowDurability;
		CDnPropState* pState;

		S_STATE_INFO( void ) : pActionNameWhenSubDurabilityZero( NULL ), 
							   iOriginalDurability(0 ),
							   iNowDurability( 0 ),
							   pState( NULL )
		{};

		void RestoreDurability( void )
		{
			iNowDurability = iOriginalDurability;
		}
	};
	DNVector( S_STATE_INFO ) m_vlStateInfos;

	int m_iNowStateIndex;
	CDnPropState* m_pActivateState;			// 트리거로 Activate 액션을 실행시켰을 때 이 State 로 들어오게 된다.

protected:
	void _InitializeFSM( void );

public:
	CDnWorldMultiDurabilityProp( CMultiRoom* pRoom );
	virtual ~CDnWorldMultiDurabilityProp( void );

	bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );
	bool InitializeTable( int nTableID );
	void ReleasePostCustomParam( void ) 
	{ 
		if( m_pData && m_bPostCreateCustomParam )
		{
			MultiDurabilityBrokenPropStruct* pMultiDurabilityStruct = static_cast<MultiDurabilityBrokenPropStruct*>( m_pData );
			SAFE_DELETE( pMultiDurabilityStruct );
			m_pData = NULL;		// 반드시 NULL 로 해주어야 이 클래스를 상속받은 클래스의 소멸자에서 다시 지워지지 않습니다.
		}
	};

	virtual void SetActionQueue( const char *szActionName, int nLoopCount /* = 0 */, float fBlendFrame /* = 3.f */, float fStartFrame /* = 0.f */ );

	void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );
	void OnSyncComplete( CDNUserSession* pBreakIntoGameSession/*=NULL*/ );
	void OnChangeAction( const char *szPrevAction );
	
	// from CDnDamageBase
	virtual DnActorHandle GetActorHandle( void ) { return m_hMonster; };
	virtual DWORD GetDamageObjectUniqueID() { return GetUniqueID(); };

	// from CDnWorldBrokenProp
	virtual void CalcDamage( CDnDamageBase *pHitter, CDnDamageBase::SHitParam &HitParam );

	virtual int GetSubDurability( int iIndex );

	virtual void OnFSMStateEntry( const TDnFSMState<DnPropHandle>* pState );
};
