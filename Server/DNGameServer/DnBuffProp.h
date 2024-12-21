#pragma once

#include "DnWorldActProp.h"
#include "MAActorProp.h"
#include "DnBuffPropComponent.h"


//A. �ֺ� ������ ������ �ִ� ���
//B. Ư�� Area�� Ư�� Buff�� �ش�.
//C. ȿ�� ��� Ÿ��
//i. �÷��̾� �� ���� (����)
//ii. ����
//iii. �� ��
//D. ȿ�� ǥ��
//i. ����� Prop�� ���� (+������ ����Ʈ)
//ii. ���� �̻� ȿ���� �����ִ� ��İ� �����ϰ� ĳ���Ϳ� �����ش�.
//1. UI
//2. ���� �ٴ� ����Ʈ
//2. ���� ���� ����
//A. �� ����� Ư�� Prop�� Action���� ����
//3. Ư�� �̺�Ʈ ������ ȿ���� �ִ� ������� ����
class CDnBuffProp : public CDnWorldActProp,
					public MAActorProp,	
					public CDnDamageBase,
					public TBoostMemoryPool< CDnBuffProp >
{
private:
	CDnBuffPropComponent m_BuffPropComponent;


protected:
	virtual bool InitializeTable( int nTableID );
	void ReleasePostCustomParam( void ) 
	{ 
		if( m_pData && m_bPostCreateCustomParam )
		{
			BuffStruct* pBuffStruct = static_cast<BuffStruct*>( m_pData );
			SAFE_DELETE( pBuffStruct );
			m_pData = NULL;		// �ݵ�� NULL �� ���־�� �� Ŭ������ ��ӹ��� Ŭ������ �Ҹ��ڿ��� �ٽ� �������� �ʽ��ϴ�.
		}
	};

public:
	CDnBuffProp( CMultiRoom* pRoom );
	virtual ~CDnBuffProp(void);

	// from CDnWorldActProp
	virtual bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );
	virtual void OnChangeAction( const char *szPrevAction );

	// from CDnDamageBase
	virtual DnActorHandle GetActorHandle( void ) { return m_hMonster; };
	void OnMessage( const boost::shared_ptr<IStateMessage>& pMessage );

	void SetSummoner( DnActorHandle hActor ) 
	{
		m_BuffPropComponent.GetSkillComponent().SetSkillUser( hActor ); 
		m_BuffPropComponent.SetTeam( hActor->GetTeam() );
	};
};