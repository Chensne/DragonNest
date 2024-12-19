#include "StdAfx.h"
#include "DnAddDamageOnCriticalBlow.h"
#include "DnSkill.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )

CDnAddDamageOnCriticalBlow::CDnAddDamageOnCriticalBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
#if !defined(_GAMESERVER)
,m_IntervalChecker( hActor, GetMySmartPtr() )
#endif // _GAMESERVER
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_169;
	SetValue( szValue );
	
	std::string str = szValue;//"확률;추가데미지비율";
	std::vector<std::string> tokens;
	std::string delimiters = ";";

	//1. 상태효과 인덱스 리스트와 적용 레벨 구분
	TokenizeA(str, tokens, delimiters);
	if (2 == tokens.size())
	{
		m_fRate = m_fValue = (float)atof( tokens[0].c_str() );
		m_fAddDamageRate = (float)atof(tokens[1].c_str());
	}
	else
	{
		m_fRate = m_fValue = 0.0f;
		m_fAddDamageRate = 0.0f;

		OutputDebug("%s Invalid Value!!!!\n", __FUNCTION__);
	}

#ifdef _GAMESERVER
	m_bCalcProb = false;
	m_bOnDamaged = false;
	
	AddCallBackType( SB_ONTARGETHIT );
#else
	//기본적으로 이펙트 표시 하지 않도록..
	UseTableDefinedGraphicEffect( false );
	m_bGraphicEffectShow = false;
	m_bTrigger = false;
#endif
}

CDnAddDamageOnCriticalBlow::~CDnAddDamageOnCriticalBlow(void)
{

}

void CDnAddDamageOnCriticalBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

}


void CDnAddDamageOnCriticalBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process(LocalTime, fDelta);

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


void CDnAddDamageOnCriticalBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

#if !defined(_GAMESERVER)
	_DetachGraphicEffect();
#endif // _GAMESERVER
}

#if defined(_GAMESERVER)

bool CDnAddDamageOnCriticalBlow::CalcProb()
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

void CDnAddDamageOnCriticalBlow::OnTargetHit( DnActorHandle hTargetActor )
{
	//#30953 1타 2킬 상황 막음. - 액터가 죽었으면 데미지 처리 안되도록..
	if (!hTargetActor || hTargetActor->IsDie())
		return;

	CDnDamageBase::SHitParam* pHitParam = hTargetActor->GetHitParam();
	if (NULL == pHitParam || (pHitParam->HitType != CDnWeapon::HitTypeEnum::Critical))
		return;
	
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
	
	if (m_bOnDamaged)
	{
		int nAddDamage = (int)(pHitParam->nCalcDamage * m_fAddDamageRate);

		char buffer[65];
		_itoa_s(nAddDamage, buffer, 65, 10 );

		hTargetActor->CmdAddStateEffect(&m_ParentSkillInfo, STATE_BLOW::BLOW_177, 0, buffer, false, false);
	}
}
#else


bool CDnAddDamageOnCriticalBlow::OnCustomIntervalProcess( void )
{
	if (m_hEtcObjectEffect)
	{
		_DetachGraphicEffect();
		m_IntervalChecker.OnEnd(0, 0.0f);
	}

	return true;
}

void CDnAddDamageOnCriticalBlow::ShowGraphicEffect(bool bShow)
{
	m_bGraphicEffectShow = bShow;
	m_bTrigger = true;
}
#endif // _GAMESERVER

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnAddDamageOnCriticalBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//파싱에 필요한 변수 선언
	std::vector<string> vlTokens[2];
	string strArgument[2];

	//필요한 값 변수
	float fRate[2] = { 0.0f, };
	float fAddDamageRate[2] = { 0.0f, };

	//////////////////////////////////////////////////////////////////////////
	//첫번째 문자열 파싱.
	strArgument[0] = szOrigValue;
	TokenizeA( strArgument[0], vlTokens[0], ";" );

	if( vlTokens[0].size() == 2 ) 
	{
		fRate[0] = (float)atof( vlTokens[0][0].c_str() );
		fAddDamageRate[0] = (float)atof( vlTokens[0][1].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째 문자열 파싱
	strArgument[1] = szAddValue;
	TokenizeA( strArgument[1], vlTokens[1], ";" );

	if( vlTokens[1].size() == 2 ) 
	{
		fRate[1] = (float)atof( vlTokens[1][0].c_str() );
		fAddDamageRate[1] = (float)atof( vlTokens[1][1].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////

	//두 값을 더한다.
	float fResultValue = fRate[0] + fRate[1];
	float fResultAddDamageRate = fAddDamageRate[0] + fAddDamageRate[1];

	sprintf_s(szBuff, "%f;%f", fResultValue, fResultAddDamageRate);

	szNewValue = szBuff;
}

void CDnAddDamageOnCriticalBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//파싱에 필요한 변수 선언
	std::vector<string> vlTokens[2];
	string strArgument[2];

	//필요한 값 변수
	float fRate[2] = { 0.0f, };
	float fAddDamageRate[2] = { 0.0f, };

	//////////////////////////////////////////////////////////////////////////
	//첫번째 문자열 파싱.
	strArgument[0] = szOrigValue;
	TokenizeA( strArgument[0], vlTokens[0], ";" );

	if( vlTokens[0].size() == 2 ) 
	{
		fRate[0] = (float)atof( vlTokens[0][0].c_str() );
		fAddDamageRate[0] = (float)atof( vlTokens[0][1].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째 문자열 파싱
	strArgument[1] = szAddValue;
	TokenizeA( strArgument[1], vlTokens[1], ";" );

	if( vlTokens[1].size() == 2 ) 
	{
		fRate[1] = (float)atof( vlTokens[1][0].c_str() );
		fAddDamageRate[1] = (float)atof( vlTokens[1][1].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////

	//두 값을 더한다.
	float fResultValue = fRate[0] - fRate[1];
	float fResultAddDamageRate = fAddDamageRate[0] - fAddDamageRate[1];

	sprintf_s(szBuff, "%f;%f", fResultValue, fResultAddDamageRate);

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW