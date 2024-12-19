#include "stdafx.h"
#include "DnAggroResetBlow.h"
#include "DnMonsterActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnAggroResetBlow::CDnAggroResetBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_148;
	SetValue( szValue );
	m_fValue = (float)atof( szValue );

	// 0 : ��׷� ����(�ٽ� ���� ��׷� ������ ���� �ȵ�)
	// 1 : ��׷� ����(�ٽ� ���� ��׷� ������ ���� ��)
	// 1�� ��� �������� ��׷� ���� ���·� ��� �ϰ�, ����ȿ�� ���� �Ǹ� ���� ���·� ��� �ǹǷ�
	// ��׷� ���� ���� �ʴ´�.
	// 0�� ���� �׳� ��׷� ���� ��Ű�� ��.
	m_iType = (int)m_fValue;
}

CDnAggroResetBlow::~CDnAggroResetBlow(void)
{

}

void CDnAggroResetBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
#ifdef _GAMESERVER
	if(m_hActor->IsMonsterActor() )
	{
		if (m_iType == 0)
			static_cast<CDnMonsterActor*>( m_hActor.GetPointer() )->ResetAggro( m_ParentSkillInfo.hSkillUser );
	}
	else
	{
		OutputDebug( "CDnAggroResetBlow::OnBegin - ���� ���Ϳ��Ը� ��ȿ�մϴ�. ����� �߸� ������.\n" );
		SetState( STATE_BLOW::STATE_END );
	}
#else
	if( m_hActor->IsMonsterActor() == false )
	{
		SetState( STATE_BLOW::STATE_END );
	}
#endif

	OutputDebug( "CDnAggroResetBlow::OnBegin\n");
}


void CDnAggroResetBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
}


void CDnAggroResetBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
#if defined(_GAMESERVER)
	if(m_hActor && m_hActor->IsMonsterActor() )
	{
		//Type : 0�� ��� �������� ��׷� ���� �����ش�...
		if (m_iType == 0)
			static_cast<CDnMonsterActor*>( m_hActor.GetPointer() )->ResetAggro(m_ParentSkillInfo.hSkillUser);
	}
#endif // _GAMESERVER

	OutputDebug( "CDnAggroResetBlow::OnEnd\n");
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnAggroResetBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}

void CDnAggroResetBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
