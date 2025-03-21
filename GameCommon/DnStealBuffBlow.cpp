#include "StdAfx.h"
#include "DnStealBuffBlow.h"
#include "DnSkill.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )

CDnStealBuffBlow::CDnStealBuffBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_174;
	SetValue( szValue );

	m_nCount = 0;
	m_nNewDelayTime = 0;
	m_fScanRange = 0.0f;
	
	std::string str = szValue;//"갯수;지속시간(ms(10000 == 1초))";
	std::vector<std::string> tokens;
	std::string delimiters = ";";

	//1. 상태효과 인덱스 리스트와 적용 레벨 구분
	TokenizeA(str, tokens, delimiters);
	if (3 == tokens.size())
	{
		m_nCount = atoi( tokens[0].c_str() );
		m_nNewDelayTime = atoi( tokens[1].c_str() );

		m_fScanRange = (float)atof(tokens[2].c_str());
	}
	else
		OutputDebug("%s Invalid value!!!\n", __FUNCTION__);

}

CDnStealBuffBlow::~CDnStealBuffBlow(void)
{

}

void CDnStealBuffBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

#if defined(_GAMESERVER)
	StealBuffProcess(LocalTime);
#endif // _GAMESERVER
}


void CDnStealBuffBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process(LocalTime, fDelta);
}


void CDnStealBuffBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);
}


#if defined(_GAMESERVER)
void CDnStealBuffBlow::FindSkillBlowList(DnActorHandle hActor, int skillID, DNVector(DnBlowHandle)& vlBlowList)
{
	if (!hActor)
		return;

	int nStateBlowCount = hActor->GetNumAppliedStateBlow();

	for (int i = 0; i < nStateBlowCount; ++i)
	{
		DnBlowHandle hBlow = hActor->GetAppliedStateBlow(i);

		//StealBuff이면 건너뜀.
		if (!hBlow || STATE_BLOW::BLOW_174 == hBlow->GetBlowIndex())
			continue;

		//투명화 상태효과
		if (STATE_BLOW::BLOW_073 == hBlow->GetBlowIndex())
			continue;
		
		CDnSkill::SkillInfo* pSkillInfo = const_cast<CDnSkill::SkillInfo*>(hBlow->GetParentSkillInfo());
		//스킬 정보가 없으면 건너뜀. (스킬 유저가 없을 수 있음..(프랍?에 의해 적용된 상태효과)
		if (!pSkillInfo /*|| !pSkillInfo->hSkillUser*/)
			continue;

		if (pSkillInfo->iSkillID == skillID)
			vlBlowList.push_back(hBlow);
	}
}

void CDnStealBuffBlow::StealBuffProcess()
{
	if (m_nCount <= 0) return;

	if (!m_hActor) return;

	if (!m_ParentSkillInfo.hSkillUser)
		return;

	//제거해야할 Blow 리스트
	DNVector(DnBlowHandle) vlRemoveBlowList;
	typedef std::map<int, int> SKILL_ID_LIST;
	SKILL_ID_LIST skillList;

	int nLoopCount = m_nCount;
	int nStateBlowCount = m_hActor->GetNumAppliedStateBlow();

	//1. 스킬 ID를 수집
	for (int i = 0; i < nStateBlowCount; ++i)
	{
		DnBlowHandle hBlow = m_hActor->GetAppliedStateBlow(i);

		//StealBuff이면 건너뜀.
		if (!hBlow || STATE_BLOW::BLOW_174 == hBlow->GetBlowIndex())
			continue;

		//투명화 상태효과
		if (STATE_BLOW::BLOW_073 == hBlow->GetBlowIndex())
			continue;

		//스킬 정보가 없거나 buff가 아니면 건너뜀.
		CDnSkill::SkillInfo* pSkillInfo = const_cast<CDnSkill::SkillInfo*>(hBlow->GetParentSkillInfo());
		if (!pSkillInfo || CDnSkill::DurationTypeEnum::Buff != pSkillInfo->eDurationType )
			continue;

		//액티브 스킬만..
		if (CDnSkill::Active != pSkillInfo->eSkillType)
			continue;

		skillList.insert(SKILL_ID_LIST::value_type(pSkillInfo->iSkillID, pSkillInfo->iSkillID));	
	}

	//2. 스킬에 부여된 Blow들 수집해서 새로운 캐릭터에 blow부여
	SKILL_ID_LIST::iterator iter = skillList.begin();
	SKILL_ID_LIST::iterator endIter = skillList.end();
	for (; iter != endIter; ++iter)
	{
		if (nLoopCount == 0)
			break;

		//해당 스킬의 blow들을 얻어 온다..
		DNVector(DnBlowHandle) vlSkillBlowList;
		FindSkillBlowList(m_hActor, iter->first, vlSkillBlowList);

		int nBlowCount = (int)vlSkillBlowList.size();
		for (int n = 0; n < nBlowCount; ++n)
		{
			DnBlowHandle hBlow = vlSkillBlowList[n];
			if (!hBlow)
				continue;

			CDnSkill::SkillInfo* pSkillInfo = const_cast<CDnSkill::SkillInfo*>(hBlow->GetParentSkillInfo());
			if (!pSkillInfo || !pSkillInfo->hSkillUser)
				continue;

			//스킬 사용한 유저에게서 스킬 핸들을 얻어 온다. 없으면 건너뜀
			DnSkillHandle hSkill = pSkillInfo->hSkillUser->FindSkill(pSkillInfo->iSkillID);
			if (!hSkill)
				continue;

			//특정 무기를 필요로 하지 않으면
			if (CDnWeapon::EquipTypeEnum_Amount == hSkill->GetNeedEquipType(0) &&
				CDnWeapon::EquipTypeEnum_Amount == hSkill->GetNeedEquipType(1))
			{
				//스킬을 사용한 유저에게 지워지는 상태 효과를 새로운 지속 시간으로 등록한다..
				m_ParentSkillInfo.hSkillUser->CmdAddStateEffect(pSkillInfo, hBlow->GetBlowIndex(), m_nNewDelayTime, hBlow->GetValue(), false, false);
			}
		}

		nLoopCount--;
	}
}

void CDnStealBuffBlow::StealBuffProcess(LOCAL_TIME LocalTime)
{
	DNVector(DnActorHandle) VecList;

	m_hActor->ScanActor( m_hActor->GetRoom(), *m_hActor->GetPosition(), m_fScanRange, VecList );

	for( DWORD i=0; i<VecList.size(); i++ ) {
		if( !VecList[i] || !VecList[i]->IsHittable( m_hActor, LocalTime, NULL ) ) 
			continue;

		if (VecList[i]->GetTeam() == m_hActor->GetTeam())
			continue;

		StealBuffFromActor(VecList[i]);
	}
}

void CDnStealBuffBlow::StealBuffFromActor( DnActorHandle hTargetActor )
{
	if (m_nCount <= 0) return;
	if (!m_hActor) return;

	//면역 처리...
	if (!hTargetActor || hTargetActor->IsImmuned(m_StateBlow.emBlowIndex)) return;

	//제거해야할 Blow 리스트
	DNVector(DnBlowHandle) vlRemoveBlowList;
	typedef std::map<int, int> SKILL_ID_LIST;
	SKILL_ID_LIST skillList;

	int nLoopCount = m_nCount;
	int nStateBlowCount = hTargetActor->GetNumAppliedStateBlow();

	//1. 스킬 ID를 수집
	for (int i = 0; i < nStateBlowCount; ++i)
	{
		DnBlowHandle hBlow = hTargetActor->GetAppliedStateBlow(i);

		//StealBuff이면 건너뜀.
		if (!hBlow || STATE_BLOW::BLOW_174 == hBlow->GetBlowIndex())
			continue;

		//투명화 상태효과
		if (STATE_BLOW::BLOW_073 == hBlow->GetBlowIndex())
			continue;

		//스킬 정보가 없거나 buff가 아니면 건너뜀.
		CDnSkill::SkillInfo* pSkillInfo = const_cast<CDnSkill::SkillInfo*>(hBlow->GetParentSkillInfo());
		if (!pSkillInfo || CDnSkill::DurationTypeEnum::Buff != pSkillInfo->eDurationType )
			continue;

		//액티브 스킬만..
		if (CDnSkill::Active != pSkillInfo->eSkillType)
			continue;

		skillList.insert(SKILL_ID_LIST::value_type(pSkillInfo->iSkillID, pSkillInfo->iSkillID));	
	}

	//2. 스킬에 부여된 Blow들 수집해서 새로운 캐릭터에 blow부여
	SKILL_ID_LIST::iterator iter = skillList.begin();
	SKILL_ID_LIST::iterator endIter = skillList.end();
	for (; iter != endIter; ++iter)
	{
		if (nLoopCount == 0)
			break;

		//해당 스킬의 blow들을 얻어 온다..
		DNVector(DnBlowHandle) vlSkillBlowList;
		FindSkillBlowList(hTargetActor, iter->first, vlSkillBlowList);

		int nBlowCount = (int)vlSkillBlowList.size();
		for (int n = 0; n < nBlowCount; ++n)
		{
			DnBlowHandle hBlow = vlSkillBlowList[n];
			if (!hBlow)
				continue;

			// [2011/03/30 semozz]
			// 스킬 유저 액터 확인은 안 하는걸로..(프랍?에 의해서 추가된 상태효과는 스킬 유저 액터가 없음.)
			CDnSkill::SkillInfo* pSkillInfo = const_cast<CDnSkill::SkillInfo*>(hBlow->GetParentSkillInfo());
			if (!pSkillInfo /*|| !pSkillInfo->hSkillUser*/)
				continue;

			//스킬 유저 정보가 설정 되어 있을경우
			if (pSkillInfo->hSkillUser)
			{
				//스킬 유저 정보가 있다면 스킬 검사..
				DnSkillHandle hSkill = pSkillInfo->hSkillUser->FindSkill(pSkillInfo->iSkillID);
				//스킬이 없으면 건너뜀..
				if (!hSkill)
					continue;

				//검색된 스킬이 있으면 특정 무기를 필요로 하는 스킬이면 건너 뜀
				if (CDnWeapon::EquipTypeEnum_Amount != hSkill->GetNeedEquipType(0) ||
					CDnWeapon::EquipTypeEnum_Amount != hSkill->GetNeedEquipType(1))
						continue;
			}
 			
			//자신에게 적용할 상태효과를 예약한다..
			m_hActor->ReserveStealMagicBuff(hTargetActor, pSkillInfo, m_nNewDelayTime, hBlow->GetBlowIndex(), hBlow->GetValue());
		}

		nLoopCount--;
	}
}

#endif // _GAMESERVER

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnStealBuffBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//파싱에 필요한 변수 선언
	std::vector<string> vlTokens[2];
	string strArgument[2];

	//필요한 값 변수
	int nCount[2] = {0, };
	int nNewDelayTime[2] = {0, };
	float fScanRange[2] = {0.0f, };

	//////////////////////////////////////////////////////////////////////////
	//첫번째 문자열 파싱.
	strArgument[0] = szOrigValue;
	TokenizeA( strArgument[0], vlTokens[0], ";" );

	if( vlTokens[0].size() == 3 ) 
	{
		nCount[0] = atoi( vlTokens[0][0].c_str() );
		nNewDelayTime[0] = atoi( vlTokens[0][1].c_str() );
		fScanRange[0] = (float)atof( vlTokens[0][2].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째 문자열 파싱
	strArgument[1] = szAddValue;
	TokenizeA( strArgument[1], vlTokens[1], ";" );

	if( vlTokens[1].size() == 3 ) 
	{
		nCount[1] = atoi( vlTokens[1][0].c_str() );
		nNewDelayTime[1] = atoi( vlTokens[1][1].c_str() );
		fScanRange[1] = (float)atof( vlTokens[1][2].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////

	//두 값을 더한다.
	int nResultCount = nCount[0] + nCount[1];
	int nResultDelayTime = max(nNewDelayTime[0], nNewDelayTime[1]);
	float fResultScanRange = max(fScanRange[0], fScanRange[1]);

	sprintf_s(szBuff, "%d;%d;%f", nResultCount, nResultDelayTime, fResultScanRange);

	szNewValue = szBuff;
}

void CDnStealBuffBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//파싱에 필요한 변수 선언
	std::vector<string> vlTokens[2];
	string strArgument[2];

	//필요한 값 변수
	int nCount[2] = {0, };
	int nNewDelayTime[2] = {0, };
	float fScanRange[2] = {0.0f, };

	//////////////////////////////////////////////////////////////////////////
	//첫번째 문자열 파싱.
	strArgument[0] = szOrigValue;
	TokenizeA( strArgument[0], vlTokens[0], ";" );

	if( vlTokens[0].size() == 3 ) 
	{
		nCount[0] = atoi( vlTokens[0][0].c_str() );
		nNewDelayTime[0] = atoi( vlTokens[0][1].c_str() );
		fScanRange[0] = (float)atof( vlTokens[0][2].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째 문자열 파싱
	strArgument[1] = szAddValue;
	TokenizeA( strArgument[1], vlTokens[1], ";" );

	if( vlTokens[1].size() == 3 ) 
	{
		nCount[1] = atoi( vlTokens[1][0].c_str() );
		nNewDelayTime[1] = atoi( vlTokens[1][1].c_str() );
		fScanRange[1] = (float)atof( vlTokens[1][2].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////

	//두 값을 더한다.
	int nResultCount = nCount[0] - nCount[1];
	int nResultDelayTime = min(nNewDelayTime[0], nNewDelayTime[1]);
	float fResultScanRange = min(fScanRange[0], fScanRange[1]);

	sprintf_s(szBuff, "%d;%d;%f", nResultCount, nResultDelayTime, fResultScanRange);

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
