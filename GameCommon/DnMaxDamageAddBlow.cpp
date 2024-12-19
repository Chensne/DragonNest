#include "StdAfx.h"
#include "DnMaxDamageAddBlow.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )


CDnMaxDamageAddBlow::CDnMaxDamageAddBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
#if !defined(_GAMESERVER)
,m_IntervalChecker( hActor, GetMySmartPtr() )
#endif // _GAMESERVER
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_160;
	SetValue( szValue );

	m_fValue = (float)atof(szValue);

	m_fRate = m_fValue;

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

CDnMaxDamageAddBlow::~CDnMaxDamageAddBlow(void)
{

}

void CDnMaxDamageAddBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);
}

void CDnMaxDamageAddBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
	
#if defined(_GAMESERVER)
	//플래그 리셋
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


void CDnMaxDamageAddBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

#if !defined(_GAMESERVER)
	_DetachGraphicEffect();
#endif // _GAMESERVER
}

#if defined(_GAMESERVER)

bool CDnMaxDamageAddBlow::CalcProb()
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

void CDnMaxDamageAddBlow::OnTargetHit( DnActorHandle hTargetActor )
{
	//#30953 1타 2킬 상황 막음. - 액터가 죽었으면 데미지 처리 안되도록..
	if (!m_hActor || m_hActor->IsDie())
		return;

	if (!hTargetActor || hTargetActor->IsDie())
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

	//사용 가능하면 TargetActor를 리스트에 추가 한다.
	if (m_bOnDamaged)
	{
		// [2011/03/09 semozz]
		// 최대물리공격력에서 스킬에 의한 증가분은 빼고 캐릭터와 아이템에 의한 최대물리공격력을 가져와야 한다...
		int nAddDamage = m_hActor->GetAttackPMaxWithoutSkill();

		char buffer[65];
		_itoa_s(nAddDamage, buffer, 65, 10 );
		hTargetActor->CmdAddStateEffect(&m_ParentSkillInfo, STATE_BLOW::BLOW_177, 0, buffer, false, false);
	}

}
#else

bool CDnMaxDamageAddBlow::OnCustomIntervalProcess( void )
{
	if (m_hEtcObjectEffect)
	{
		_DetachGraphicEffect();
		m_IntervalChecker.OnEnd(0, 0.0f);
	}

	return true;
}

void CDnMaxDamageAddBlow::ShowGraphicEffect(bool bShow)
{
	m_bGraphicEffectShow = bShow;
	m_bTrigger = true;
}

#endif // _GAMESERVER

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnMaxDamageAddBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

void CDnMaxDamageAddBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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
