#pragma once
#include "DnWorldBrokenProp.h"
#include "MAActorProp.h"
#include "DnBuffPropComponent.h"


// �μ����� ���� ����. �⺻���� �Ӽ��� BrokenProp �� ������.
class CDnWorldBrokenBuffProp : public CDnWorldBrokenProp,
							   public MAActorProp,
							   public TBoostMemoryPool< CDnWorldBrokenBuffProp >


{
private:
	// ���� ó�� ���� FSM �ϳ� �� ����. ���ū �����ʿ��� ���� FSM�� ó�����ְ� ����.
	TDnFiniteStateMachine<DnPropHandle>* m_pBuffFSM;
	CDnBuffPropComponent m_BuffPropComponent;

protected:
	void ReleasePostCustomParam( void )
	{ 
		if( m_pData && m_bPostCreateCustomParam )
		{
			BuffBrokenStruct* pBuffBrokenStruct = static_cast<BuffBrokenStruct*>( m_pData );
			SAFE_DELETE( pBuffBrokenStruct ); 
			m_pData = NULL;		// �ݵ�� NULL �� ���־�� �� Ŭ������ ��ӹ��� Ŭ������ �Ҹ��ڿ��� �ٽ� �������� �ʽ��ϴ�.
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

	// ��ȯ�ڿ� ���� ��ȯ�ɶ��� �� �Լ��� ȣ��ȴ�.
	void SetSummoner( DnActorHandle hActor ) 
	{ 
		m_BuffPropComponent.GetSkillComponent().SetSkillUser( hActor ); 
		m_BuffPropComponent.SetTeam( hActor->GetTeam() );
	};
};