#include "StdAfx.h"
#include "DnChangeActionSetBlow.h"
#include "IDnSkillProcessor.h"
#include "DnChangeActionStrProcessor.h"
#include "DnApplySEWhenActionSetBlowEnabledProcessor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnChangeActionSetBlow::CDnChangeActionSetBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor ),
																							  m_pChangeActionStrProcessor( NULL ),
																							  m_bEnable( false ),
																							  m_bChangedActionBegun( false )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_129;

	_ASSERT( strlen(szValue) < STATE_BLOW_ARGUMENT_MAX_SIZE );
	SetValue( szValue );

	// ���ڰ��� Ư�� �׼��� ���ϰ� ���� ���� �׼� �̸��� �ٲٵ��� �ϰ� ���� ��..
	// �ش� �׼� �̸��� �޾Ƶд�.
	// �� �׼��� ���ϰ� ���� ���� �׼� ��ü�� CDnActor::SetActionQueue() �Լ����� �Ͼ��.
	if( 0 < strlen(szValue) && (strcmp(szValue, "0") != 0) )
		m_strTriggerAction.assign( szValue );
}

CDnChangeActionSetBlow::~CDnChangeActionSetBlow(void)
{

}

void CDnChangeActionSetBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	DnSkillHandle hParentSkill = m_ParentSkillInfo.hSkillUser->FindSkill( m_ParentSkillInfo.iSkillID );
	m_bEnable = false;
	m_strCancleAction.clear();
	_ASSERT( hParentSkill );
	if( hParentSkill )
	{
		m_pChangeActionStrProcessor = static_cast<CDnChangeActionStrProcessor*>( hParentSkill->GetProcessor( IDnSkillProcessor::CHANGE_ACTIONSTR ) );
		OutputDebug( "CDnChangeActionSetBlow::OnBegin\n" );
	}
	else
	{
		SetState( STATE_BLOW::STATE_END );
		OutputDebug( "CDnChangeActionSetBlow::OnBegin Fail!! Can't find Parent Skill Object!!\n" );
	}
}


void CDnChangeActionSetBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
}


void CDnChangeActionSetBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
#ifdef _GAMESERVER
	// #35037 ����ȿ�� ����� �� �ٷ� �ο��� ����ȿ����� ���� �Ӽ� ������ �������ش�.
	OnNotChangeAction();
#endif // _GAMESERVER
	OutputDebug( "CDnChangeActionSetBlow::OnEnd\n" );
}


CDnChangeActionStrProcessor* CDnChangeActionSetBlow::GetChangeActionStrProcessor( void )
{
	if( m_bEnable )
	{
		// ������ ��� �߻��ؼ� ������� �ǽ� ���� �����͸� �����ϰ� �����غ���.
		CDnChangeActionStrProcessor* pProcessor = NULL;
		if( m_ParentSkillInfo.hSkillUser )
		{
			DnSkillHandle hParentSkill = m_ParentSkillInfo.hSkillUser->FindSkill( m_ParentSkillInfo.iSkillID );
			if( hParentSkill )
				pProcessor = static_cast<CDnChangeActionStrProcessor*>( hParentSkill->GetProcessor( IDnSkillProcessor::CHANGE_ACTIONSTR ) );
		}

		// ���� Processor �� ���µ� ��������� Valid �ϴٸ� �̰� �´°���. �α� �ھƺ�
		if( pProcessor == NULL && m_pChangeActionStrProcessor )
		{
			g_Log.Log(LogType::_ERROR, L"CDnChangeActionSetBlow::GetChangeActionStrProcessor Invalid Call!!" );
		}

		return pProcessor;


		//return m_pChangeActionStrProcessor;
	}
	else
		return NULL;
}

void CDnChangeActionSetBlow::UpdateEnable( const char* pPrevAction, const char* pCurrentAction )
{
	if( strcmp(pPrevAction, pCurrentAction) == 0 )
		return;

	// #40128 �θ�ų�� ��밡�� ���� ������ ���ͼ� üũ.
	if( m_ParentSkillInfo.hSkillUser )
	{
		DnSkillHandle hParentSkill = m_ParentSkillInfo.hSkillUser->FindSkill( m_ParentSkillInfo.iSkillID );
		if( hParentSkill && !hParentSkill->IsSatisfyWeapon() )
		{
			m_bEnable = false;
			return;
		}
	}

	if( m_strTriggerAction.empty() )
	{
		m_bEnable = true;
	}
	else
	if( pCurrentAction && 0 < strlen(pCurrentAction) &&
		pPrevAction && 0 < strlen(pPrevAction) )
	{
		// ���� �ٲ� �׼��� Ʈ���� �׼ǰ� ���ٸ� Ȱ��ȭ ����.
		if( false == m_bEnable && m_strTriggerAction == pCurrentAction )
		{
			m_bEnable = true;
			m_strCancleAction = pPrevAction;
		}
		else
		if( m_strCancleAction == pCurrentAction )
		{
			m_bEnable = false;
		}
	}
}


#ifdef _GAMESERVER
void CDnChangeActionSetBlow::OnChangeAction( void )
{
	if( false == m_bChangedActionBegun )
	{
		// �׼� ����ÿ� ����ȿ�� ���� ����Ÿ���� �ִٸ� �Լ� ȣ������.
		CDnApplySEWhenActionSetBlowEnabledProcessor* pProcessor = NULL;
		DnSkillHandle hParentSkill = m_ParentSkillInfo.hSkillUser->FindSkill( m_ParentSkillInfo.iSkillID );
		if( hParentSkill )
		{
			pProcessor = static_cast<CDnApplySEWhenActionSetBlowEnabledProcessor*>( hParentSkill->GetProcessor( IDnSkillProcessor::APPLY_SE_WHEN_ACTIONSET_ENABLED ) );
			if( pProcessor )
				pProcessor->OnChangeActionBegin();
		}

		m_bChangedActionBegun = true;
	}
}


void CDnChangeActionSetBlow::OnNotChangeAction( void )
{
	if( m_bChangedActionBegun )
	{
		// �׼� ����ÿ� ����ȿ�� ���� ����Ÿ���� �ִٸ� �Լ� ȣ������.
		CDnApplySEWhenActionSetBlowEnabledProcessor* pProcessor = NULL;
		DnSkillHandle hParentSkill = m_ParentSkillInfo.hSkillUser->FindSkill( m_ParentSkillInfo.iSkillID );
		if( hParentSkill )
		{
			pProcessor = static_cast<CDnApplySEWhenActionSetBlowEnabledProcessor*>( hParentSkill->GetProcessor( IDnSkillProcessor::APPLY_SE_WHEN_ACTIONSET_ENABLED ) );
			if( pProcessor )
				pProcessor->OnChangeActionSetBlowEnd();
		}

		m_bChangedActionBegun = false;
	}
}
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnChangeActionSetBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}

void CDnChangeActionSetBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW