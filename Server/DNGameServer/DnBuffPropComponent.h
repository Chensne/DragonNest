#pragma once
#include "IStateMessage.h"
#include "DnSkillPropComponent.h"


class CDnBuffPropComponent
{
private:
	int m_iTargetTeamType;		// 영향을 주는 팀
	int m_iMyTeam;				// 이 버프 프랍이 속한 팀.
	set<int> m_setAffectingActors;
	map<int, vector<int> > m_mapAppliedStateEffect;
	set<int> m_setTargetActorIDs;
	CDnSkillPropComponent m_SkillComponent;
	CMultiRoom*	m_pGameRoom;

public:
	CDnBuffPropComponent(void);
	virtual ~CDnBuffPropComponent(void);

	void Initialize( CMultiRoom* pRoom, int iTargetTeamType,  int iSkillID, int iSkillLevel, const set<int>* pSetTargetActorIDs = NULL );
	void OnMessage( const boost::shared_ptr<IStateMessage>& pMessage );
	void RemoveAffectedStateEffects( void );

	void SetTeam( int iTeam ) { m_iMyTeam = iTeam; };
	CDnSkillPropComponent& GetSkillComponent( void ) { return m_SkillComponent; };
};
