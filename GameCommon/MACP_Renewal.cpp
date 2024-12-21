#include "stdafx.h"
#include "MACP_Renewal.h"
#include "DnSkill.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#if defined( PRE_ADD_CP_RENEWAL )

MACP_Renewal::MACP_Renewal()
: MACP()
{
	memset( m_StandardWeight, 0, sizeof(m_StandardWeight) );
	m_StandardWeight[MACP::CPTypeEnum::KillMonsterScore] = 1.0f;
	m_StandardWeight[MACP::CPTypeEnum::GetItemScore] = 0.5f;
	m_StandardWeight[MACP::CPTypeEnum::GenocideScore] = 0.5f;
	m_StandardWeight[MACP::CPTypeEnum::UseSkillScore] = 0.5f;
	m_StandardWeight[MACP::CPTypeEnum::PartyComboScore] = 0.12f;
	m_StandardWeight[MACP::CPTypeEnum::RebirthPlayerScore] = 0.12f;
	m_StandardWeight[MACP::CPTypeEnum::ComboScore] = 0.12f;
	m_StandardWeight[MACP::CPTypeEnum::PropBreakScore] = 0.12f;
	
	ResetCP();
}

MACP_Renewal::~MACP_Renewal()
{

}

void MACP_Renewal::ResetCP()
{
	m_nStandardCP = 0;
	memset( m_AccumulationCP, 0, sizeof(m_AccumulationCP) );

	MACP::ResetCP();
}

bool MACP_Renewal::InitializeCPScore( const int nMapIndex, const int nClassID, const int nDungeonClearID )
{
	if( false == MACP::InitializeCPScore( nMapIndex, nClassID, nDungeonClearID ) )
		return false;

	DNTableFileFormat * pClearSox = GetDNTable( CDnTableDB::TDUNGEONCLEAR );
	if( NULL == pClearSox )
		return false;

	int nMonsterCount = pClearSox->GetFieldFromLablePtr( nDungeonClearID, "_MonsterVCount" )->GetInteger();
	m_nStandardCP = pClearSox->GetFieldFromLablePtr( nDungeonClearID, "_StageBaseKillScore" )->GetInteger();

	if( 0 != nMonsterCount )
		m_Data.pScore[CPTypeEnum::KillMonsterScore] = m_nStandardCP / nMonsterCount;

	return true;
}

bool MACP_Renewal::VerifyStandardCP( const MACP::CPTypeEnum eCPType, int & nCP )
{
	if( 0 > eCPType || MACP::CPTypeEnum::CPType_Max <= eCPType )
		return false;

	const int nMaxValue = (const int)(m_nStandardCP * m_StandardWeight[eCPType]);
	if( nMaxValue <= nCP )
	{
		nCP = nMaxValue;
		return false;
	}

	return true;
}

bool MACP_Renewal::UpdateCP( const int nScore, const MACP::CPTypeEnum eCPType, int * pResult )
{
	if( nScore == 0 )
		return false;

	int nCP = m_AccumulationCP[eCPType] + nScore;
	VerifyStandardCP( eCPType, nCP );

	nCP = nCP - m_AccumulationCP[eCPType];
	m_Data.nAccumulationCP += nCP;

	if( NULL != pResult )
		*pResult = nScore;
	OnEventCP( eCPType, nCP );

	return true;
}

void MACP_Renewal::OnEventCP( CPTypeEnum Type, int nResult )
{
	if( MACP::MaxComboCount == Type
		|| MACP::KillBossCount == Type )
		m_AccumulationCP[Type] = nResult;
	else
		m_AccumulationCP[Type] += nResult;
	MACP::OnEventCP( Type, nResult );
}

bool MACP_Renewal::UpdateKillMonster( int *pResult )
{
	return UpdateCP( m_Data.pScore[CPTypeEnum::KillMonsterScore], MACP::CPTypeEnum::KillMonsterScore, pResult );;
}

bool MACP_Renewal::UpdateGetItem( int* pResult )
{
	return UpdateCP( m_Data.pScore[CPTypeEnum::GetItemScore], MACP::CPTypeEnum::GetItemScore, pResult );
}

bool MACP_Renewal::UpdateGenocide( int *pResult )
{
	return UpdateCP( m_Data.pScore[CPTypeEnum::GenocideScore], MACP::CPTypeEnum::GenocideScore, pResult );
}

bool MACP_Renewal::UpdateUseSkill( DnSkillHandle hSkill, int *pResult )
{
	if( !hSkill ) return false;

	return UpdateCP( hSkill->GetCPScore(), MACP::CPTypeEnum::UseSkillScore, pResult );
}

bool MACP_Renewal::UpdatePartyCombo( int nCombo, int *pResult )
{
	return UpdateCP( nCombo * m_Data.pScore[CPTypeEnum::PartyComboScore], MACP::CPTypeEnum::PartyComboScore, pResult );
}

bool MACP_Renewal::UpdateRebirthPlayer( int *pResult )
{
	return UpdateCP( m_Data.pScore[CPTypeEnum::RebirthPlayerScore], MACP::CPTypeEnum::RebirthPlayerScore, pResult );
}

bool MACP_Renewal::UpdateCombo( int nCombo, int *pResult )
{
	return UpdateCP( nCombo * m_Data.pScore[CPTypeEnum::ComboScore], MACP::CPTypeEnum::ComboScore, pResult );
}

bool MACP_Renewal::UpdatePropBreak( int* pResult )
{
	return UpdateCP( m_Data.pScore[CPTypeEnum::PropBreakScore], MACP::CPTypeEnum::PropBreakScore, pResult );
}

int MACP_Renewal::GetStandardCP()
{
	return m_nStandardCP;
}

int MACP_Renewal::GetClearTime_Revision( const Dungeon::Difficulty::eCode eDifficulty )
{
	switch( eDifficulty )
	{
	case Dungeon::Difficulty::Easy:
		return 60;

	case Dungeon::Difficulty::Normal:
		return 80;

	case Dungeon::Difficulty::Hard:
		return 100;

	case Dungeon::Difficulty::Master:
		return 100;

	case Dungeon::Difficulty::Abyss:
		return 120;
	}

	return 0;
}

#endif	// #if defined( PRE_ADD_CP_RENEWAL )