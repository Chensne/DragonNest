#include "StdAfx.h"
#include "DnRecallSummonBlow.h"

#include "DnMonsterActor.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )


CDnRecallSummonBlow::CDnRecallSummonBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )																				
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_216;
	SetValue( szValue );

	m_fValue = 0.0f;

	m_dwMonsterID = atoi(szValue);
}

CDnRecallSummonBlow::~CDnRecallSummonBlow(void)
{

}

void CDnRecallSummonBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

#if defined(_GAMESERVER)
	if (!m_hActor)
		return;

	EtVector3* pDestPos = m_hActor->GetPosition();

	const std::list<DnMonsterActorHandle>& listSummonMonster = m_hActor->GetSummonedMonsterList();
	RecallSummon(listSummonMonster, m_dwMonsterID, pDestPos);
	

	const std::map<int, std::list<DnMonsterActorHandle> >& mapSummonMonster = m_hActor->GetGroupingSummonedMonsterList();
	std::map<int, std::list<DnMonsterActorHandle> >::const_iterator mapIter;
	for (mapIter = mapSummonMonster.begin(); mapIter != mapSummonMonster.end(); ++mapIter)
	{
		RecallSummon(mapIter->second, m_dwMonsterID, pDestPos);
	}

#endif // _GAMESERVER
}

#if defined(_GAMESERVER)
void CDnRecallSummonBlow::RecallSummon(const std::list<DnMonsterActorHandle>& listSummon, DWORD dwActorClassID, EtVector3* pDestPos)
{
	std::list<DnMonsterActorHandle>::const_iterator listIter;
	for (listIter = listSummon.begin(); listIter != listSummon.end(); ++listIter)
	{
		DnMonsterActorHandle hMonster = (*listIter);
		if (!hMonster)
			continue;

		//주인이 같아야함..
		if (hMonster->GetSummonerPlayerActor() != m_hActor)
			continue;

		//몬스트 클래스 ID가 다르면 스킵
		if (hMonster->GetMonsterClassID() != dwActorClassID)
			continue;
		
		/*
		//이 조건 검사 기능 막음
		//몬스터가 이동 불능 상태이면 스킵
		if (!hMonster->IsMovable() || hMonster->GetCantMoveSEReferenceCount() > 0)
			continue;
		*/

		//몬스터 어그로 초기화.
		hMonster->ResetAggro();

		hMonster->SetPosition(*pDestPos);
		hMonster->CmdWarp(*pDestPos, EtVec3toVec2(*hMonster->GetLookDir()));
	}
}
#endif // _GAMESERVER

void CDnRecallSummonBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process( LocalTime, fDelta );
	
}


void CDnRecallSummonBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnRecallSummonBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}

void CDnRecallSummonBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
