
#pragma once

#include "MAScannerFilter.h"

class MAScanner : public CSingleton<MAScanner>
{
public:

	MAScanner();
	~MAScanner();

	enum eType
	{
		OpponentTeamScan = 0,			// ����� ��ĵ(NPC ����)
		MonsterSkillSameTeam,			// ���� ��ų ���� ������ ��ĵ
		MonsterSkillSameTeamExpectMe,	// ���� ��ų ���� ���� ������ ������ ��ĵ
		MonsterSkillOpponentTeam,		// ���� ��ų ���� ����� ��ĵ
		Max,
	};

	void Scan( MAScanner::eType Type, DnActorHandle hActor, float fMinRange, float fMaxRange, DNVector(DnActorHandle)& vOutputActor );

private:

	void			_CreateFilter();
	void			_CreateScanner();
	void			_CreateOpponentTeamScanner();
	void			_CreateMonsterSkillSameTeamScanner();
	void			_CreateMonsterSkillSameTeamExpectMeScanner();
	void			_CreateMonsterSkillOpponentTeamScanner();

	IMAScanFilter*	_GetFilter( IMAScanFilter::eFilterType Type );

	std::vector<IMAScanFilter*>	m_vFilterRepository;
	std::vector<IMAScanFilter*> m_vScanner[MAScanner::eType::Max];
};

#define GetMAScanner()	MAScanner::GetInstance()
