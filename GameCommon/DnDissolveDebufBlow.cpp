#include "StdAfx.h"
#include "DnDissolveDebufBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnDissolveDebufBlow::CDnDissolveDebufBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_069;
	SetValue( szValue );
	//m_nValue = atoi( szValue );
	m_fValue = (float)atof(szValue);
}

CDnDissolveDebufBlow::~CDnDissolveDebufBlow(void)
{
}



bool CDnDissolveDebufBlow::_IsBadStateBlow( DnBlowHandle hBlow )
{
	return hBlow->GetBlowIndex() == STATE_BLOW::BLOW_041 ||
		   hBlow->GetBlowIndex() == STATE_BLOW::BLOW_042 ||
		   hBlow->GetBlowIndex() == STATE_BLOW::BLOW_043 ||
		   hBlow->GetBlowIndex() == STATE_BLOW::BLOW_044 ||
		   hBlow->GetBlowIndex() == STATE_BLOW::BLOW_045 || // 수면
		   hBlow->GetBlowIndex() == STATE_BLOW::BLOW_078 ||	// 스킬 사용 불가 상태효과
		   hBlow->GetBlowIndex() == STATE_BLOW::BLOW_070 ||
		   hBlow->GetBlowIndex() == STATE_BLOW::BLOW_071;
}


void CDnDissolveDebufBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{

#ifdef _GAMESERVER
	// 해제 단위는 상태효과 갯수가 아니라 디버프 스킬 묶음 단위.
	multimap<int, DnBlowHandle> mmapDebuffBlowsBySkill;

	int iDissolveCount = 0;
	int iNumAppliedBlow = m_hActor->GetNumAppliedStateBlow();
	vector<int> vlDebuffSkillID;
	for( int iBlow = 0; iBlow < iNumAppliedBlow; ++iBlow )
	{
		DnBlowHandle hBlow = m_hActor->GetAppliedStateBlow( iBlow );
		if( !hBlow )
			continue;

		CDnSkill::SkillInfo* pSkillInfo = const_cast<CDnSkill::SkillInfo*>(hBlow->GetParentSkillInfo());

		if( pSkillInfo && pSkillInfo->iSkillID )
		{
			// 안좋은 상태효과. 감전, 중독, 화상, 행동불가, 이동불가, 혹은 디버프로 셋팅된 스킬들
			if( (0 == pSkillInfo->iDissolvable) && 
				(CDnSkill::Debuff == pSkillInfo->eDurationType || _IsBadStateBlow(hBlow)) )
			{
				// 만약 디버프이고 지정된 나쁜 상태효과가 아니라면 팀에 대한 고려가 있어야 한다.
				bool bBadBlow = true;
				if( (pSkillInfo->hSkillUser) && (false == _IsBadStateBlow(hBlow)) )
				{
					bBadBlow = false;
					bool bOtherTeam = (m_hActor->GetTeam() != pSkillInfo->hSkillUser->GetTeam());
					if( (CDnSkill::Enemy == pSkillInfo->eTargetType) && bOtherTeam )
					{
						bBadBlow = true;
					}

					// 버블버블처럼 스킬 타겟타입이 All 이고 각 상태효과별로 타겟이 있는 경우.
					if( (CDnSkill::All == pSkillInfo->eTargetType) && bOtherTeam )
					{
						DnSkillHandle hSkill = pSkillInfo->hSkillUser->FindSkill( pSkillInfo->iSkillID );
						if( hSkill )
						{
							DWORD dwNumSE = hSkill->GetStateEffectCount();
							for( DWORD dwSE = 0; dwSE < dwNumSE; ++dwSE )
							{
								const CDnSkill::StateEffectStruct* pSE = hSkill->GetStateEffectFromIndex( dwSE );
								if( pSE && pSE->ApplyType == CDnSkill::StateEffectApplyType::ApplyEnemy )
								{
									if( (STATE_BLOW::emBLOW_INDEX)pSE->nID == hBlow->GetBlowIndex() )
									{
										bBadBlow = true;
										break;
									}
								}
							}
						}
					}
				}

				if( bBadBlow )
				{
					mmapDebuffBlowsBySkill.insert( make_pair(pSkillInfo->iSkillID, hBlow) );

					// 하나의 스킬에 여러개의 상태효과가 있을 수 있다. 유니크한 스킬 아이디만 넣어준다.
					if( vlDebuffSkillID.end() == find( vlDebuffSkillID.begin(), vlDebuffSkillID.end(), pSkillInfo->iSkillID ) )
						vlDebuffSkillID.push_back( pSkillInfo->iSkillID );
				}
			}
		}
	}

	DNVector(int) vlAppliedDebuffBlowIds;
	for( int i = 0; i < (int)vlDebuffSkillID.size(); ++i )
	{
		if( iDissolveCount >= (int)m_fValue )
			break;

		pair<multimap<int, DnBlowHandle>::iterator, multimap<int, DnBlowHandle>::iterator> iter_pair = 
			mmapDebuffBlowsBySkill.equal_range( vlDebuffSkillID.at(i) );

		// 같은 스킬에서 비롯된 디버프 상태효과들을 모아둔다.
		multimap<int, DnBlowHandle>::iterator iter = iter_pair.first;
		for( iter; iter != iter_pair.second; ++iter )
			vlAppliedDebuffBlowIds.push_back( iter->second->GetBlowID() );

		++iDissolveCount;
	}


	// 해제 가능한 나쁜 스킬, 상태효과만 해제
	int iNumBlow = (int)vlAppliedDebuffBlowIds.size();
	for( int iBlow = 0; iBlow < iNumBlow; ++iBlow )
	{
		int iBlowID = vlAppliedDebuffBlowIds.at( iBlow );
		m_hActor->CmdRemoveStateEffectFromID( iBlowID );
	}
#endif
}


void CDnDissolveDebufBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{

}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnDissolveDebufBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//필요한 값 변수
	float fValue[2];

	//////////////////////////////////////////////////////////////////////////
	//첫번째 값
	fValue[0] = (float)atof( szOrigValue );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째
	fValue[1] = (float)atof( szAddValue );
	//////////////////////////////////////////////////////////////////////////

	//두 값을 더한다.
	float fResultValue = fValue[0] + fValue[1];

	sprintf_s(szBuff, "%f", fResultValue);

	szNewValue = szBuff;
}

void CDnDissolveDebufBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//필요한 값 변수
	float fValue[2];

	//////////////////////////////////////////////////////////////////////////
	//첫번째 값
	fValue[0] = (float)atof( szOrigValue );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째
	fValue[1] = (float)atof( szAddValue );
	//////////////////////////////////////////////////////////////////////////

	//값 계산
	float fResultValue = fValue[0] - fValue[1];

	sprintf_s(szBuff, "%f", fResultValue);

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
