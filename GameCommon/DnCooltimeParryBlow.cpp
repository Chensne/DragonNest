#include "StdAfx.h"
#include "DnCooltimeParryBlow.h"
#ifdef _GAMESERVER
#include "DnStateBlow.h"
#include "DnPlayerActor.h"
#else
#include "DnLocalPlayerActor.h"
#include "DnInterface.h"
#include "DnMainDlg.h"
#include "TaskManager.h"
#include "DnGameTask.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCooltimeParryBlow::CDnCooltimeParryBlow( DnActorHandle hActor, const char *szValue )
											: CDnBlow(hActor)
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_153;
	AddCallBackType(SB_ONDEFENSEATTACK);
	SetValue(szValue);
	//m_nValue = atoi( szValue );
	m_fValue = (float)atof(szValue);

#ifdef _GAMESERVER
	m_strParringActionName.assign( "Skill_Parrying" );
	m_bEnable = false;
	m_bEnableLastLoop = false;
	m_fAdditionalSignalProb = 0.0f;

	m_dwCoolTime = 0;
	m_dwLastParryingTime = 0;

	m_fAdditionalSignalProbLastLoop = 0.0f;
#else
	m_pLocalPlayerActor = NULL;
#endif

}

CDnCooltimeParryBlow::~CDnCooltimeParryBlow(void)
{
#ifndef _GAMESERVER
	// TODO: 쿨타임 발동된 상태에서 스킬레벨업을 한다던가 하면 뻑날 것 같은데.. 테스트 필요함.
	SAFE_RELEASE_SPTR( m_hParentSkillForCoolTime );
#endif
}

void CDnCooltimeParryBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );

#ifdef _GAMESERVER
	// 액터 process 거의 끝부분에 상태효과 process 를 돌기 때문에 여기서 리셋해주면 패링시그널의 구간과 동일하게 enable 이 처리될 것.
	m_bEnableLastLoop = m_bEnable;
	m_fAdditionalSignalProbLastLoop = m_fAdditionalSignalProb;
	m_bEnable = false;
	m_fAdditionalSignalProb = 0.0f;
#else
	if( m_pLocalPlayerActor && m_hParentSkillForCoolTime )
	{
		// 쿨타임 진행.
		m_hParentSkillForCoolTime->Process( LocalTime, fDelta ); 
	}
#endif
}

void CDnCooltimeParryBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
#ifdef _GAMESERVER
	// 쿨타임을 부모 스킬의 정보를 그대로 활용한다.
	if( m_ParentSkillInfo.hSkillUser )
	{
		DnSkillHandle hParentSkill = m_ParentSkillInfo.hSkillUser->FindSkill( m_ParentSkillInfo.iSkillID );
		m_dwCoolTime = DWORD(hParentSkill->GetDelayTime() * 1000.0f);
	}
#else
	m_pLocalPlayerActor = dynamic_cast<CDnLocalPlayerActor*>( m_hActor.GetPointer() );
	if( !m_hParentSkillForCoolTime && m_pLocalPlayerActor )
	{
		m_hParentSkillForCoolTime = CDnSkill::CreateSkill( m_hActor, m_ParentSkillInfo.iSkillID, 
#if defined(PRE_FIX_NEXTSKILLINFO)
															m_ParentSkillInfo.nSkillLevel);
#else
														   m_ParentSkillInfo.iSkillLevelID - m_ParentSkillInfo.iSkillLevelIDOffset + 1 );
#endif // PRE_FIX_NEXTSKILLINFO

#if defined( PRE_FIX_82147 )
		if( m_hParentSkillForCoolTime && m_hActor->IsPlayerActor() )
		{
			int iSkillLevelDataType = CDnSkill::PVE;

			CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
			if( pGameTask && pGameTask->GetGameTaskType() == GameTaskType::PvP )
				iSkillLevelDataType = CDnSkill::PVP;

			m_hParentSkillForCoolTime->SelectLevelDataType( iSkillLevelDataType );
		}
#endif

	}
#endif

	OutputDebug( "CDnCooltimeParryBlow::OnBegin\n" );
}

void CDnCooltimeParryBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
#ifndef _GAMESERVER
	//if( m_pLocalPlayerActor )
	//	m_hParentSkillForCoolTime->OnEnd( 0, 0.0f );
#endif

	OutputDebug( "CDnCooltimeParryBlow::OnEnd\n" );
}

#ifdef _GAMESERVER
bool CDnCooltimeParryBlow::OnDefenseAttack( DnActorHandle hHitter, CDnState* pAttackerState, CDnDamageBase::SHitParam &HitParam, bool bHitSuccess )
{
	if( !m_bEnableLastLoop )
		return false;

	if( !bHitSuccess ) 
		return false;

#if defined(PRE_FIX_BLOCK_CONDITION)
	//블럭의 발동이 fDamage가 0이라도 적이라면 블럭 발동 가능 하도록..
	//아군인 경우 기존 처럼 fDamage 가0이면 블럭 되지 않도록 함.
	if (IsCanBlock(hHitter, m_hActor, HitParam) == false)
		return false;
#else
	// hit percent 가 0% 일 경우 발동하지 않는 것으로 됨. (#21175)
	if( 0.0f == HitParam.fDamage )
		return false;
#endif // PRE_FIX_BLOCK_CONDITION

#if defined(PRE_ADD_49166)
	// 피격자가 디버프 (Freezing / FrameStop) 상태효과가 적용되어 있는 경우는 블럭 발동 되지 않도록..
	if (IsInVaildBlockCondition(m_hActor) == true)
		return false;
#endif // PRE_ADD_49166

	// 마지막으로 사용된 시간에서 쿨타임만큼 시간이 안된다면 사용할 수 없다.
	if( GetTickCount() - m_dwLastParryingTime < m_dwCoolTime )
		return false;

	// 플레이어인 경우엔,
	if( m_hActor->IsPlayerActor() )
	{
		// 무기를 들고 있지 않은 경우엔 발동하지 않음. #26772
		CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(m_hActor.GetPointer());
		if( false == (pPlayerActor->IsBattleMode() && pPlayerActor->GetWeapon( 0 ) && pPlayerActor->GetWeapon( 1 )) )
			return false;
		if( pPlayerActor->IsTransformMode() )
			return false;
	}

	float fAddProb = m_fAdditionalSignalProbLastLoop;
	if( m_hActor->GetStateBlow()->IsApplied( STATE_BLOW::BLOW_056 ) )
	{
		DNVector(DnBlowHandle) vlhParryingProbBlow;
		m_hActor->GetStateBlow()->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_056, vlhParryingProbBlow );
		int iNumBlow = (int)vlhParryingProbBlow.size();
		for( int i = 0; i < iNumBlow; ++i )
			fAddProb += vlhParryingProbBlow.at( i )->GetFloatValue();
	}

	int iProb = int((m_fValue+fAddProb) * 10000.0f);
	int iRandValue = (_rand(GetRoom())%10000);
	if( iRandValue <= iProb )
	{
		// HitParam에 적절한 값을 셋팅한다.
		HitParam.szActionName = m_strParringActionName;
		OutputDebug( "CDnCooltimeParryingBlow::OnDefenseAttack\n" );

		// 마지막으로 패링이 사용된 시간 갱신.
		m_dwLastParryingTime = GetTickCount();

		// 걸어놓은 애들한테 알려줌. /////////////////////////////////////////////
		boost::shared_ptr<IDnObserverNotifyEvent> pEvent( IDnObserverNotifyEvent::Create( EVENT_BUBBLE_COOLTIME_PARRING_SUCCESS ) );
		pEvent->SetSkillID( m_ParentSkillInfo.iSkillID );
		Notify( pEvent );
		//////////////////////////////////////////////////////////////////////////

		// 클라로 패킷으로 알려준다. 현재로선 클라에서 이 시점을 따로 알 방법이 없음.
		static_cast<CDnPlayerActor*>(m_hActor.GetPointer())->RequestCooltimeParrySuccess( m_ParentSkillInfo.iSkillID );

		return true;
	}
	else
	{
		return false;
	}
}
#else
void CDnCooltimeParryBlow::OnSuccess( void )
{
	// 서버에서 쿨타임 패링이 성공했을 경우 따로 패킷으로 보내와서 호출된다.
	// 로컬 클라이언트일때만 쿨타임 보여주면 된다.
	if( m_pLocalPlayerActor )
	{
		// 쿨타임만 진행시켜줌. 화면 표시용
		m_hParentSkillForCoolTime->OnBeginCoolTime();

		if( m_hParentSkillForCoolTime->HasCoolTime() )
		{
			CDnMainDlg *pMainDlg = (CDnMainDlg*)GetInterface().GetMainBarDialog();
			if( pMainDlg ) 
			{
				pMainDlg->AddPassiveSkill( m_hParentSkillForCoolTime );
			}
		}
	}
}
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnCooltimeParryBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

void CDnCooltimeParryBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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
