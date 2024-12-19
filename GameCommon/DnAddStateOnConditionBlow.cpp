#include "StdAfx.h"
#include "DnAddStateOnConditionBlow.h"
#if defined( _GAMESERVER )
#include "DnPlayerActor.h"
#endif // #if defined( _GAMESERVER )

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )



CDnAddStateOnConditionBlow::CDnAddStateOnConditionBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
#if !defined(_GAMESERVER)
,m_IntervalChecker( hActor, GetMySmartPtr() )
#endif // _GAMESERVER
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_172;
	SetValue( szValue );
	
	SetInfo(szValue);

#ifdef _GAMESERVER
	AddCallBackType( SB_ONTARGETHIT );

	m_bCalcProb = false;
	m_bOnDamaged = false;
#else
	//기본적으로 이펙트 표시 하지 않도록..
	UseTableDefinedGraphicEffect( false );
	m_bGraphicEffectShow = false;
	m_bTrigger = false;
#endif
}

CDnAddStateOnConditionBlow::~CDnAddStateOnConditionBlow(void)
{

}



void CDnAddStateOnConditionBlow::OnBegin(LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);
}



void CDnAddStateOnConditionBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process( LocalTime, fDelta );

#if defined(_GAMESERVER)
	m_bCalcProb = m_bOnDamaged = false;
#else
	if (m_bTrigger)
	{
		if (m_bGraphicEffectShow)
		{
			_AttachGraphicEffect();
			if( m_hEtcObjectEffect )
			{
				CEtActionBase::ActionElementStruct* pStruct = m_hEtcObjectEffect->GetElement( "Idle" );
				if( pStruct )
					m_IntervalChecker.OnBegin( LocalTime, pStruct->dwLength + 1000);
			}
		}
		else
		{
			if (m_hEtcObjectEffect)
			{
				_DetachGraphicEffect();
				m_IntervalChecker.OnEnd(LocalTime, fDelta);
			}
		}

		m_bTrigger = false;
	}

	m_IntervalChecker.Process( LocalTime, fDelta );
#endif // _GAMESERVER
}


void CDnAddStateOnConditionBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

#if !defined(_GAMESERVER)
	_DetachGraphicEffect();
#endif // _GAMESERVER
}

void CDnAddStateOnConditionBlow::SetInfo(const char* szValue)
{
	std::string str = szValue;//"[확률;HitStateIndex;HitAttackAttributeIndex;DestStateIndex;상태효과지속시간][상태효과설정값]";
	std::vector<std::string> tokens;
	std::string delimiters = "[]";

	//1. 구분
	TokenizeA(str, tokens, delimiters);

	if (tokens.size() != 2)
	{
		OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szValue);
	}
	else
	{
		//2. 기본 정보
		std::string defaultInfo = tokens[0];
		std::vector<std::string> infoTokens;
		delimiters = ";";

		TokenizeA(defaultInfo, infoTokens, delimiters);
		if (5 == infoTokens.size())
		{
			m_fRate = m_fValue = (float)atof(infoTokens[0].c_str());
			m_nHitStateBlowIndex = atoi(infoTokens[1].c_str());
			m_nHitAttackAttribute = atoi(infoTokens[2].c_str());
			m_nDestStateBlowIndex = atoi(infoTokens[3].c_str());
			m_nStateDurationTime = atoi(infoTokens[4].c_str());
		}
		else
			OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szValue);

		//3. 상태효과 설정값
		m_strStateAttribute = tokens[1];
	}
}


#ifdef _GAMESERVER
bool CDnAddStateOnConditionBlow::CalcProb()
{
	bool bExecuteable = false;

	//스킬 쿨타임 적용해야함..
	//패시브 스킬일때만??
	DnSkillHandle hSkill;
	const CDnSkill::SkillInfo* pSkillInfo = GetParentSkillInfo();
	if (pSkillInfo)
	{
		hSkill = m_hActor->FindSkill(pSkillInfo->iSkillID);

		if (!hSkill)
			return bExecuteable;

		if (CDnSkill::UsingResult::Success != hSkill->CanExecute())
		{
			OutputDebug("%s CanExecute 실패\n", __FUNCTION__);
			return bExecuteable;
		}
	}
	else
		return bExecuteable;

	//확률 계산..
	bExecuteable = rand() % 10000 <= (m_fRate * 10000.0f);
	if (!bExecuteable)
	{
		OutputDebug("%s 확률 걸러짐\n", __FUNCTION__);
		return bExecuteable;
	}

	// 쿨타임 적용 요청 [2011/03/15 semozz]
	//클라이언트에 패스브 스킬 쿨타임 적용을 위해 패킷 전송..
	bool isPassiveSkill = CDnSkill::Passive == hSkill->GetSkillType();
	if (isPassiveSkill)
	{
		BYTE pBuffer[128];
		CPacketCompressStream Stream( pBuffer, 128 );

		DWORD dwUniqueID = m_hActor ? m_hActor->GetUniqueID() : -1;
		int nSkillID = hSkill->GetClassID();

		Stream.Write( &dwUniqueID, sizeof(dwUniqueID) );
		Stream.Write( &nSkillID, sizeof(nSkillID));

		m_hActor->Send(eActor::SC_PASSIVESKILL_COOLTIME, &Stream);
	}

	if (hSkill)
		hSkill->OnBeginCoolTime();

	return bExecuteable;
}

void CDnAddStateOnConditionBlow::OnTargetHit( DnActorHandle hTargetActor )
{
	if (!hTargetActor)
		return;

	// 1. HitStateIndex에 해당하는 상태효과가 있는지 확인.
	DNVector(DnBlowHandle) vlAppliedBlows;
	hTargetActor->GatherAppliedStateBlowByBlowIndex((STATE_BLOW::emBLOW_INDEX)m_nHitStateBlowIndex, vlAppliedBlows);
	if (vlAppliedBlows.empty())
		return;

	//////////////////////////////////////////////////////////////////////////
	//#52905 관련
	//추가 상태효과 지속 시간을 -1로 설정 했을 경우, 설정된 상태효과의 남은 시간으로 설정 하도록 한다.
	int nListCount = (int)vlAppliedBlows.size();
	float fDurationTime = 0.0f;
	for (int i = 0; i < nListCount; ++i)
	{
		DnBlowHandle hBlow = vlAppliedBlows[i];
		if (hBlow && hBlow->GetDurationTime() > fDurationTime)
			fDurationTime = hBlow->GetDurationTime();
	}
	//////////////////////////////////////////////////////////////////////////

	// 2. 공격 속성을 확인
	CDnDamageBase::SHitParam* pHitParam = hTargetActor->GetHitParam();
	if (NULL == pHitParam || pHitParam->HasElement != m_nHitAttackAttribute)
		return;
	
	// 3. 확률에 맞으면 적용
	//확률 계산이 되어 있지 않다면 확률 계산으로 스킬 사용 여부를 확인한다.
	if (false == m_bCalcProb)
	{
		m_bOnDamaged = CalcProb();
		m_bCalcProb = true;

		if (m_bOnDamaged)
		{
			//이펙트 표시용 패킷을 전송..
			BYTE pBuffer[128];
			CPacketCompressStream Stream( pBuffer, 128 );

			DWORD dwUniqueID = m_hActor ? m_hActor->GetUniqueID() : -1;
			bool bShowEffect = true;
			Stream.Write( &dwUniqueID, sizeof(dwUniqueID) );
			Stream.Write( &m_StateBlow.emBlowIndex, sizeof(m_StateBlow.emBlowIndex));
			Stream.Write( &bShowEffect, sizeof(bShowEffect));

			m_hActor->Send(eActor::SC_SHOW_STATE_EFFECT, &Stream);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// #52905 관련
	// m_nStateDurationTime 이 -1로 설정 되어 있으면 위에서 저장해놓은 fDurationTime을 적용 하도록 한다.
	// fDurationTime은 1.0 -> 1초.  ===> 1000분의 1초 단위로 변경 해야 한다.
	int nApplyDurationTime = m_nStateDurationTime;
	if (m_nStateDurationTime == -1)
		nApplyDurationTime = (int)(fDurationTime * 1000.0f);
	//////////////////////////////////////////////////////////////////////////
	
	if (m_bOnDamaged)
		hTargetActor->CmdAddStateEffect(&m_ParentSkillInfo, (STATE_BLOW::emBLOW_INDEX)m_nDestStateBlowIndex, nApplyDurationTime, m_strStateAttribute.c_str());
}
#else
bool CDnAddStateOnConditionBlow::OnCustomIntervalProcess( void )
{
	if (m_hEtcObjectEffect)
	{
		_DetachGraphicEffect();
		m_IntervalChecker.OnEnd(0, 0.0f);
	}

	return true;
}

void CDnAddStateOnConditionBlow::ShowGraphicEffect(bool bShow)
{
	m_bGraphicEffectShow = bShow;
	m_bTrigger = true;
}
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnAddStateOnConditionBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	std::string strValue[2];
	std::vector<std::string> tokens[2];
	std::string delimiters = "[]";


	float fRate[2] = {0.0f, };
	int nHitStateBlowIndex[2] = {0, };
	int nHitAttackAttribute[2] = {0, };
	int nDestStateBlowIndex[2] = {0, };
	int nStateDurationTime[2] = {0, };
	std::string szArgment[2];


	strValue[0] = szOrigValue;
	//1. 구분
	TokenizeA(strValue[0], tokens[0], delimiters);

	if (tokens[0].size() != 2)
	{
		OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szOrigValue);
	}
	else
	{
		//2. 기본 정보
		std::string defaultInfo = tokens[0][0];
		std::vector<std::string> infoTokens;
		delimiters = ";";

		TokenizeA(defaultInfo, infoTokens, delimiters);
		if (5 == infoTokens.size())
		{
			fRate[0] = (float)atof(infoTokens[0].c_str());
			nHitStateBlowIndex[0] = atoi(infoTokens[1].c_str());
			nHitAttackAttribute[0] = atoi(infoTokens[2].c_str());
			nDestStateBlowIndex[0] = atoi(infoTokens[3].c_str());
			nStateDurationTime[0] = atoi(infoTokens[4].c_str());
		}
		else
			OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szOrigValue);

		//3. 상태효과 설정값
		szArgment[0] = tokens[0][1];
	}

	//////////////////////////////////////////////////////////////////////////
	strValue[1] = szAddValue;
	//1. 구분
	TokenizeA(strValue[1], tokens[1], delimiters);

	if (tokens[1].size() != 2)
	{
		OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szAddValue);
	}
	else
	{
		//2. 기본 정보
		std::string defaultInfo = tokens[1][0];
		std::vector<std::string> infoTokens;
		delimiters = ";";

		TokenizeA(defaultInfo, infoTokens, delimiters);
		if (5 == infoTokens.size())
		{
			fRate[1] = (float)atof(infoTokens[0].c_str());
			nHitStateBlowIndex[1] = atoi(infoTokens[1].c_str());
			nHitAttackAttribute[1] = atoi(infoTokens[2].c_str());
			nDestStateBlowIndex[1] = atoi(infoTokens[3].c_str());
			nStateDurationTime[1] = atoi(infoTokens[4].c_str());
		}
		else
			OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szAddValue);

		//3. 상태효과 설정값
		szArgment[1] = tokens[1][1];
	}


	float fResultRate = fRate[0] + fRate[1];
	int nResultHitStateBlowIndex = nHitStateBlowIndex[0];
	int nResultHitAttackAttribute = nHitAttackAttribute[0];
	int nResultDestStateBlowIndex = nDestStateBlowIndex[0];
	int nResultStateDurationTime = max(nStateDurationTime[0], nStateDurationTime[1]);

	sprintf_s(szBuff, "[%f;%d;%d;%d;%d][%s]", fResultRate, nResultHitStateBlowIndex, nResultHitAttackAttribute, nResultDestStateBlowIndex, nResultStateDurationTime, szArgment[0].c_str());

	szNewValue = szBuff;
}

void CDnAddStateOnConditionBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	std::string strValue[2];
	std::vector<std::string> tokens[2];
	std::string delimiters = "[]";


	float fRate[2] = {0.0f, };
	int nHitStateBlowIndex[2] = {0, };
	int nHitAttackAttribute[2] = {0, };
	int nDestStateBlowIndex[2] = {0, };
	int nStateDurationTime[2] = {0, };
	std::string szArgment[2];


	strValue[0] = szOrigValue;
	//1. 구분
	TokenizeA(strValue[0], tokens[0], delimiters);

	if (tokens[0].size() != 2)
	{
		OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szOrigValue);
	}
	else
	{
		//2. 기본 정보
		std::string defaultInfo = tokens[0][0];
		std::vector<std::string> infoTokens;
		delimiters = ";";

		TokenizeA(defaultInfo, infoTokens, delimiters);
		if (5 == infoTokens.size())
		{
			fRate[0] = (float)atof(infoTokens[0].c_str());
			nHitStateBlowIndex[0] = atoi(infoTokens[1].c_str());
			nHitAttackAttribute[0] = atoi(infoTokens[2].c_str());
			nDestStateBlowIndex[0] = atoi(infoTokens[3].c_str());
			nStateDurationTime[0] = atoi(infoTokens[4].c_str());
		}
		else
			OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szOrigValue);

		//3. 상태효과 설정값
		szArgment[0] = tokens[0][1];
	}

	//////////////////////////////////////////////////////////////////////////
	strValue[1] = szAddValue;
	//1. 구분
	TokenizeA(strValue[1], tokens[1], delimiters);

	if (tokens[1].size() != 2)
	{
		OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szAddValue);
	}
	else
	{
		//2. 기본 정보
		std::string defaultInfo = tokens[1][0];
		std::vector<std::string> infoTokens;
		delimiters = ";";

		TokenizeA(defaultInfo, infoTokens, delimiters);
		if (5 == infoTokens.size())
		{
			fRate[1] = (float)atof(infoTokens[0].c_str());
			nHitStateBlowIndex[1] = atoi(infoTokens[1].c_str());
			nHitAttackAttribute[1] = atoi(infoTokens[2].c_str());
			nDestStateBlowIndex[1] = atoi(infoTokens[3].c_str());
			nStateDurationTime[1] = atoi(infoTokens[4].c_str());
		}
		else
			OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szAddValue);

		//3. 상태효과 설정값
		szArgment[1] = tokens[1][1];
	}


	float fResultRate = fRate[0] + fRate[1];
	int nResultHitStateBlowIndex = nHitStateBlowIndex[0];
	int nResultHitAttackAttribute = nHitAttackAttribute[0];
	int nResultDestStateBlowIndex = nDestStateBlowIndex[0];
	int nResultStateDurationTime = max(nStateDurationTime[0], nStateDurationTime[1]);

	sprintf_s(szBuff, "[%f;%d;%d;%d;%d][%s]", fResultRate, nResultHitStateBlowIndex, nResultHitAttackAttribute, nResultDestStateBlowIndex, nResultStateDurationTime, szArgment[0].c_str());

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
