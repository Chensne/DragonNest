#pragma once

#include "DnWorldActProp.h"
#include "MAActorProp.h"
#include "DnBuffPropComponent.h"


//A. 주변 영역에 영향을 주는 기능
//B. 특정 Area에 특정 Buff를 준다.
//C. 효과 대상 타겟
//i. 플레이어 및 동료 (몬스터)
//ii. 몬스터
//iii. 둘 다
//D. 효과 표현
//i. 연결된 Prop을 제작 (+프랍의 이펙트)
//ii. 상태 이상 효과를 보여주는 방식과 동일하게 캐릭터에 보여준다.
//1. UI
//2. 몸에 붙는 이펙트
//2. 개발 방향 결정
//A. 본 기능은 특정 Prop의 Action으로 개발
//3. 특정 이벤트 영역에 효과를 주는 방법으로 개발
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
			m_pData = NULL;		// 반드시 NULL 로 해주어야 이 클래스를 상속받은 클래스의 소멸자에서 다시 지워지지 않습니다.
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