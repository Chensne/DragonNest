#include "StdAfx.h"
#include "DnPileAddEffectBlow.h"
#include "DnTableDB.h"
#include "EtActionSignal.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

/*
해당 효과가 정해진 횟수만큼(스킬테이블 설정) 중첩될 경우 특정 액션을 실행한다.
중첩 될때 이펙트 동작을 변경한다.
특정 액션 실행시 이 상태효과가 적용된 액터의 위치로 스킬 사용자 위치를 잠깐 이동 시킴.
*/
CDnPileAddEffectBlow::CDnPileAddEffectBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_249;

	SetValue( szValue );
	m_fValue = 0.0f;
	
	m_szTargetAction = szValue;
	m_pActionElement = NULL;
	m_fFrame = 0.0f;
	m_ActionDurationTime = 0;

	m_bActivatedBlow = false;
}

CDnPileAddEffectBlow::~CDnPileAddEffectBlow(void)
{

}

void CDnPileAddEffectBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

	OutputDebug( "%s\n", __FUNCTION__ );
}


void CDnPileAddEffectBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process( LocalTime, fDelta );

}


void CDnPileAddEffectBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

	m_hActor->RemoveStateBlowSignal(this->GetMySmartPtr());

	OutputDebug( "%s\n", __FUNCTION__);
}

void CDnPileAddEffectBlow::OnDuplicate( const STATE_BLOW& StateBlowInfo )
{
	// 시간은 넘어온 값으로 대체됨
	m_StateBlow.fDurationTime = StateBlowInfo.fDurationTime;

	//중복될때 이펙트의 동작을 변경 시킨다..
#ifndef _GAMESERVER
	std::string szAction = "Idle_";
	char buffer[65];
	_itoa_s(m_iDuplicateCount, buffer, 65, 10 );
	szAction += buffer;

	if (m_hEtcObjectEffect)
		m_hEtcObjectEffect->SetActionQueue(szAction.c_str());
#endif // _GAMESERVER

	//설정된 중복 카운트가 되면
	DNTableFileFormat* pSkillTable = NULL;
	int iDuplicateCount = 0;
	pSkillTable = GetDNTable( CDnTableDB::TSKILL );
	if (pSkillTable)
		iDuplicateCount = pSkillTable->GetFieldFromLablePtr( m_ParentSkillInfo.iSkillID, "_EffectAmassCount" )->GetInteger();

	if (iDuplicateCount != 0 && iDuplicateCount == m_iDuplicateCount)
	{
		//중첩 카운트 수치에 도달 하면 활성화 설정한다.
		SetActivateBlow(true);

		m_hActor->AddStateBlowSignal(this->GetMySmartPtr());
	}
}

void CDnPileAddEffectBlow::OnSetParentSkillInfo()
{
	if (m_ParentSkillInfo.hSkillUser)
	{
		m_pActionElement = m_ParentSkillInfo.hSkillUser->GetElement( m_szTargetAction.c_str() );
		
		m_ActionDurationTime = 0;

		if (m_pActionElement)
		{
			m_ActionDurationTime = m_pActionElement->dwLength;
			m_vlpSignalList = m_pActionElement->pVecSignalList;
		}
	}
}

void CDnPileAddEffectBlow::SignalProcess( LOCAL_TIME LocalTime, float fDelta )
{
	if (IsEnd() == true)
		return;

	//#ifdef _GAMESERVER
	if( m_ParentSkillInfo.hSkillUser &&
		m_hActor )
	{
		if( m_pActionElement )
		{
			float fFPS = 60.0f;
			float fPrevFrame = m_fFrame - 1.0f;

			if( m_pActionElement->dwLength < (DWORD)m_fFrame )
			{
				m_fFrame = 1.0f;
				fPrevFrame = -0.1f;

				//동작을 다 했으면..상태효과 끝??
				SetState(STATE_BLOW::STATE_END);

				return;
			}

			// 위치만 현재 액터로 바꿔서 시그널돌리고 다시 복구시킴.
			MatrixEx OriginalCross = *(m_ParentSkillInfo.hSkillUser->GetMatEx());
			MatrixEx ChargerActorCross = *(m_hActor->GetMatEx());
			*(m_ParentSkillInfo.hSkillUser->GetMatEx()) = ChargerActorCross;

#ifdef _GAMESERVER
			m_ParentSkillInfo.hSkillUser->PushForceHitElement( m_ParentSkillInfo.eSkillElement );
#endif

			CEtActionSignal *pSignal = NULL;

#if defined(PRE_FIX_55378)
			m_ParentSkillInfo.hSkillUser->SetChargerAction(m_szTargetAction.c_str());
#endif // PRE_FIX_55378

			for( DWORD i=0; i < m_vlpSignalList.size(); i++ )
			{
				pSignal = m_vlpSignalList[i];
				if( pSignal->CheckSignal( fPrevFrame, m_fFrame ) == true ) 
				{
					LOCAL_TIME StartTime = LocalTime - (LOCAL_TIME)( 1000.f / fFPS * ( m_fFrame - pSignal->GetStartFrame() ) );
					LOCAL_TIME EndTime = LocalTime + (LOCAL_TIME)( 1000.f / fFPS * ( pSignal->GetEndFrame() - m_fFrame ) );

					m_ParentSkillInfo.hSkillUser->OnSignal( (SignalTypeEnum)pSignal->GetSignalIndex(), pSignal->GetData(), LocalTime, 
						StartTime, EndTime, pSignal->GetSignalListArrayIndex() );
				}
			}

#if defined(PRE_FIX_55378)
			m_ParentSkillInfo.hSkillUser->SetChargerAction(NULL);
#endif // PRE_FIX_55378

			*(m_ParentSkillInfo.hSkillUser->GetMatEx()) = OriginalCross;

#ifdef _GAMESERVER
			m_ParentSkillInfo.hSkillUser->PopForceHitElement();
#endif

			m_fFrame += (fDelta * fFPS);
		}
	}
}


#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnPileAddEffectBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}

void CDnPileAddEffectBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW