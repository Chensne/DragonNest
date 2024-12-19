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
	// TODO: ��Ÿ�� �ߵ��� ���¿��� ��ų�������� �Ѵٴ��� �ϸ� ���� �� ������.. �׽�Ʈ �ʿ���.
	SAFE_RELEASE_SPTR( m_hParentSkillForCoolTime );
#endif
}

void CDnCooltimeParryBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );

#ifdef _GAMESERVER
	// ���� process ���� ���κп� ����ȿ�� process �� ���� ������ ���⼭ �������ָ� �и��ñ׳��� ������ �����ϰ� enable �� ó���� ��.
	m_bEnableLastLoop = m_bEnable;
	m_fAdditionalSignalProbLastLoop = m_fAdditionalSignalProb;
	m_bEnable = false;
	m_fAdditionalSignalProb = 0.0f;
#else
	if( m_pLocalPlayerActor && m_hParentSkillForCoolTime )
	{
		// ��Ÿ�� ����.
		m_hParentSkillForCoolTime->Process( LocalTime, fDelta ); 
	}
#endif
}

void CDnCooltimeParryBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
#ifdef _GAMESERVER
	// ��Ÿ���� �θ� ��ų�� ������ �״�� Ȱ���Ѵ�.
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
	//���� �ߵ��� fDamage�� 0�̶� ���̶�� �� �ߵ� ���� �ϵ���..
	//�Ʊ��� ��� ���� ó�� fDamage ��0�̸� �� ���� �ʵ��� ��.
	if (IsCanBlock(hHitter, m_hActor, HitParam) == false)
		return false;
#else
	// hit percent �� 0% �� ��� �ߵ����� �ʴ� ������ ��. (#21175)
	if( 0.0f == HitParam.fDamage )
		return false;
#endif // PRE_FIX_BLOCK_CONDITION

#if defined(PRE_ADD_49166)
	// �ǰ��ڰ� ����� (Freezing / FrameStop) ����ȿ���� ����Ǿ� �ִ� ���� �� �ߵ� ���� �ʵ���..
	if (IsInVaildBlockCondition(m_hActor) == true)
		return false;
#endif // PRE_ADD_49166

	// ���������� ���� �ð����� ��Ÿ�Ӹ�ŭ �ð��� �ȵȴٸ� ����� �� ����.
	if( GetTickCount() - m_dwLastParryingTime < m_dwCoolTime )
		return false;

	// �÷��̾��� ��쿣,
	if( m_hActor->IsPlayerActor() )
	{
		// ���⸦ ��� ���� ���� ��쿣 �ߵ����� ����. #26772
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
		// HitParam�� ������ ���� �����Ѵ�.
		HitParam.szActionName = m_strParringActionName;
		OutputDebug( "CDnCooltimeParryingBlow::OnDefenseAttack\n" );

		// ���������� �и��� ���� �ð� ����.
		m_dwLastParryingTime = GetTickCount();

		// �ɾ���� �ֵ����� �˷���. /////////////////////////////////////////////
		boost::shared_ptr<IDnObserverNotifyEvent> pEvent( IDnObserverNotifyEvent::Create( EVENT_BUBBLE_COOLTIME_PARRING_SUCCESS ) );
		pEvent->SetSkillID( m_ParentSkillInfo.iSkillID );
		Notify( pEvent );
		//////////////////////////////////////////////////////////////////////////

		// Ŭ��� ��Ŷ���� �˷��ش�. ����μ� Ŭ�󿡼� �� ������ ���� �� ����� ����.
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
	// �������� ��Ÿ�� �и��� �������� ��� ���� ��Ŷ���� �����ͼ� ȣ��ȴ�.
	// ���� Ŭ���̾�Ʈ�϶��� ��Ÿ�� �����ָ� �ȴ�.
	if( m_pLocalPlayerActor )
	{
		// ��Ÿ�Ӹ� ���������. ȭ�� ǥ�ÿ�
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

	//�ʿ��� �� ����
	float fValue[2];

	//////////////////////////////////////////////////////////////////////////
	//ù��° ��
	fValue[0] = (float)atof( szOrigValue );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//�ι�°
	fValue[1] = (float)atof( szAddValue );
	//////////////////////////////////////////////////////////////////////////

	//�� ���� ���Ѵ�.
	float fResultValue = fValue[0] + fValue[1];

	sprintf_s(szBuff, "%f", fResultValue);

	szNewValue = szBuff;
}

void CDnCooltimeParryBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//�ʿ��� �� ����
	float fValue[2];

	//////////////////////////////////////////////////////////////////////////
	//ù��° ��
	fValue[0] = (float)atof( szOrigValue );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//�ι�°
	fValue[1] = (float)atof( szAddValue );
	//////////////////////////////////////////////////////////////////////////

	//�� ���
	float fResultValue = fValue[0] - fValue[1];

	sprintf_s(szBuff, "%f", fResultValue);

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
