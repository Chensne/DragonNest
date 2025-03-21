#pragma once
#include "DnWorldBrokenProp.h"
#include "MAActorProp.h"
#include "DnBuffPropComponent.h"


// 부서지는 버프 프랍. 기본적인 속성은 BrokenProp 을 따른다.
class CDnWorldBrokenBuffProp : public CDnWorldBrokenProp,
							   public MAActorProp,
							   public TBoostMemoryPool< CDnWorldBrokenBuffProp >


{
private:
	// 버프 처리 관련 FSM 하나 더 돈다. 브로큰 프랍쪽에서 원래 FSM은 처리해주고 있음.
	TDnFiniteStateMachine<DnPropHandle>* m_pBuffFSM;
	CDnBuffPropComponent m_BuffPropComponent;

protected:
	void ReleasePostCustomParam( void )
	{ 
		if( m_pData && m_bPostCreateCustomParam )
		{
			BuffBrokenStruct* pBuffBrokenStruct = static_cast<BuffBrokenStruct*>( m_pData );
			SAFE_DELETE( pBuffBrokenStruct ); 
			m_pData = NULL;		// 반드시 NULL 로 해주어야 이 클래스를 상속받은 클래스의 소멸자에서 다시 지워지지 않습니다.
		}
	};

public:
	CDnWorldBrokenBuffProp( CMultiRoom* pGameRoom );
	virtual ~CDnWorldBrokenBuffProp(void);

	virtual bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );
	virtual bool InitializeTable( int nTableID );

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	void OnMessage( const boost::shared_ptr<IStateMessage>& pMessage );
	void OnChangeAction( const char *szPrevAction );

	// 소환자에 의해 소환될때는 이 함수가 호출된다.
	void SetSummoner( DnActorHandle hActor ) 
	{ 
		m_BuffPropComponent.GetSkillComponent().SetSkillUser( hActor ); 
		m_BuffPropComponent.SetTeam( hActor->GetTeam() );
	};
};