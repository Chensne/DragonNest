#pragma once

#include "DnActor.h"
class CDnSkill;


// 데이터는 몬스터 테이블의 데이터를 끌어와 사용한다.
// 실제적으로 뭔가 하고 있지는 않음...
// 프랍쪽에서 시그널 처리를 하고 있기 때문에 Process 돌 필요도 없음...
class CDnPropActor : public CDnActor, public TBoostMemoryPool< CDnPropActor >
{
public:
#ifdef _GAMESERVER
	CDnPropActor( CMultiRoom* pRoom, int nClassID );
#else
	CDnPropActor( int nClassID, bool bProcess = true );
#endif
	virtual ~CDnPropActor(void);

	//vector<DnSkillHandle> m_vlhSkillList;

protected:
	bool Initialize( void );
	//void OnDispatchMessage( DWORD dwActorProtocol, BYTE *pPacket );

	//// 프랍이 사용하는 스킬을 따로 만든다... MP가 다는 것도 아니고 직업 제한 같은 것도 없으며
	//// 일정 시그널에 발동만 되면 끝.
	//// hit 되었을 때 적절히 상태효과가 add 되기 위해서 MASkillUser::m_hProcessSkill 을 적당히 셋팅해 준다.(서버측)
	//bool AddPropSkill( int iSkillID, int iSkillLevelID );
	//DnSkillHandle UsePropSkill( int iSkillID, LOCAL_TIME LocalTime, float fDelta );
};
