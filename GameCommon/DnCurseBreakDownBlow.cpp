#include "StdAfx.h"
#include "DnCurseBreakDownBlow.h"
#include "DnMonsterActor.h"
#include "EtActionSignal.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#define CURSE_BREKDOWN_ACTION_NAME "Hit_Curse"

CDnCurseBreakDownBlow::CDnCurseBreakDownBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor ),
																								  m_pActionElement( NULL ),
																								  m_fFrame( 0.0f )
#ifdef _GAMESERVER
																								,m_iOriAttackMMin( 0 ),
																								 m_iOriAttackMMax( 0 )
#endif
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_245;

	SetValue( szValue );
	m_fValue = (float)atof(szValue);

	m_pActionElement = m_hActor->GetElement( CURSE_BREKDOWN_ACTION_NAME );
	_ASSERT( m_pActionElement && "저주 파괴 액션 정보를 찾을 수 없음." );

	m_ActionDurationTime = 0;
	if (m_pActionElement)
	{
		m_ActionDurationTime = m_pActionElement->dwLength;
		m_vlpSignalList = m_pActionElement->pVecSignalList;
	}
}

CDnCurseBreakDownBlow::~CDnCurseBreakDownBlow(void)
{

}

bool CDnCurseBreakDownBlow::CanBegin( void )
{
	// 화상 상태효과가 걸려있는 상태가 아니라면 시작될 수 없다.
	bool bCanBegin = m_hActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_244 );

	if( false == bCanBegin )
		OutputDebug( "CDnCurseBreakDownBlow::CanBegin - 저주 상태효과가 걸려있지 않으므로 저주 폭발 상태효과 발동 실패.\n" );

	// 폭발 액션 없을 땐 발동시키지 않는다.
	if( NULL == m_pActionElement )
	{
		bCanBegin = false;
		OutputDebug( "CDnCurseBreakDownBlow::CanBegin - 저주 폭발 액션이 없으므로 저주 폭발 상태효과 발동 실패.\n" );
	}

	return bCanBegin;
}

void CDnCurseBreakDownBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	// 저주는 해제 되지 않는다..
// #ifdef _GAMESERVER
// 	m_hActor->CmdRemoveStateEffect( STATE_BLOW::BLOW_244 );
// #endif

	// 이 상태효과가 끼어있는 스킬의 시전자의 마법 공격력으로 상태효과가 적용된 액터의 마법공격력을 지속시간 동안 대체시킴.
#ifdef _GAMESERVER
	_ASSERT( m_ParentSkillInfo.hSkillUser );
	if( !m_ParentSkillInfo.hSkillUser )
	{
		g_Log.Log( LogType::_ERROR, L"CDnCurseBreakDownBlow::OnBegin - 저주 파괴 상태효과 액터가 유효하지 않아서 발동 실패..\n" );
		SetState( STATE_BLOW::STATE_END );
		return;
	}

	m_iOriAttackMMin = m_hActor->GetAttackMMin();
	m_iOriAttackMMax = m_hActor->GetAttackMMax();

	m_hActor->SetAttackMMin( m_ParentSkillInfo.hSkillUser->GetAttackMMin() );
	m_hActor->SetAttackMMax( m_ParentSkillInfo.hSkillUser->GetAttackMMax() );

	// 몬스터라면 클라에서 데미지 표시를 위해 셋팅.
	if( m_hActor && CDnActorState::Reserved6 < m_hActor->GetActorType() )
		static_cast<CDnMonsterActor*>(m_hActor.GetPointer())->SlaveOf( m_ParentSkillInfo.hSkillUser->GetUniqueID() );
#endif

	// 지속시간은 폭발 액션의 지속시간으로 처리한다.
	float fFPS = 60.0f;
	//m_StateBlow.fDurationTime = (float)m_pActionElement->dwLength / fFPS;
	m_StateBlow.fDurationTime = m_ActionDurationTime != 0 ? (float)m_ActionDurationTime / fFPS : 0.0f;

	// 바로 종료가 될 수 있으므로 정상 상태일때만 추가
	if (STATE_BLOW::STATE_BEGIN == GetBlowState())
		m_hActor->AddStateBlowSignal(this->GetMySmartPtr());

	OutputDebug( "%s\n", __FUNCTION__ );
}


void CDnCurseBreakDownBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );

}


void CDnCurseBreakDownBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
#ifdef _GAMESERVER
	m_hActor->SetAttackMMin( m_iOriAttackMMin );
	m_hActor->SetAttackMMax( m_iOriAttackMMax );

	// 몬스터라면 클라에서 데미지 표시를 위해 셋팅한 것 원상 복구.
	//if( CDnActorState::Reserved6 < m_hActor->GetActorType() )
	if( m_hActor && m_hActor->IsMonsterActor() )
	{
		DWORD dwInvalidMasterActorID = UINT_MAX;
		static_cast<CDnMonsterActor*>(m_hActor.GetPointer())->SlaveOf( dwInvalidMasterActorID );
	}
#endif

	m_hActor->RemoveStateBlowSignal(this->GetMySmartPtr());

	OutputDebug( "%s\n", __FUNCTION__);
}


void CDnCurseBreakDownBlow::SignalProcess( LOCAL_TIME LocalTime, float fDelta )
{
	// 실제 액션을 실행하지 않고 시그널들만 실행시키도록 한다.
	float fFPS = 60.0f;
	float fPrevFrame = m_fFrame;
	m_fFrame += (fDelta * fFPS);

	if (m_ActionDurationTime < (DWORD)m_fFrame)
	{
		m_fFrame = 1.0f;
		fPrevFrame = -0.1f;

		// 액션을 다 실행했다면 더 이상 시그널 처리를 하지 않고 상태효과가 끝난다. 
		// 액션 길이 만큼 지속시간을 설정했으므로 곧 상태효과 종료됨.
		return;
	}

	if( 0.0f == fPrevFrame )
		fPrevFrame = -(m_fFrame * 0.1f);

	CEtActionSignal *pSignal = NULL;

#if defined(PRE_FIX_55378)
	m_hActor->SetChargerAction(CURSE_BREKDOWN_ACTION_NAME);
#endif // PRE_FIX_55378

	for( DWORD i=0; i < m_vlpSignalList.size(); i++ )
	{
		pSignal = m_vlpSignalList[i];
		if( pSignal->CheckSignal( fPrevFrame, m_fFrame ) == true ) 
		{
			LOCAL_TIME StartTime = LocalTime - (LOCAL_TIME)( 1000.f / fFPS * ( m_fFrame - pSignal->GetStartFrame() ) );
			LOCAL_TIME EndTime = LocalTime + (LOCAL_TIME)( 1000.f / fFPS * ( pSignal->GetEndFrame() - m_fFrame ) );

			m_hActor->OnSignal( (SignalTypeEnum)pSignal->GetSignalIndex(), pSignal->GetData(), LocalTime, 
				StartTime, EndTime, pSignal->GetSignalListArrayIndex() );
		}
	}

#if defined(PRE_FIX_55378)
	m_hActor->SetChargerAction(NULL);
#endif // PRE_FIX_55378
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnCurseBreakDownBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

void CDnCurseBreakDownBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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