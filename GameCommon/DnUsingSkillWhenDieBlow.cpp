#include "StdAfx.h"
#include "DnUsingSkillWhenDieBlow.h"
#include "DnMonsterActor.h"
#include "DnInvincibleBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnUsingSkillWhenDieBlow::CDnUsingSkillWhenDieBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor ),
																								  m_bUsedDieSkill( false ),
																								  m_bDead( false ) 
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_137;
	SetValue( szValue );

	m_fValue = (float)atof( szValue );
}

CDnUsingSkillWhenDieBlow::~CDnUsingSkillWhenDieBlow( void )
{

}

#ifdef _GAMESERVER
bool CDnUsingSkillWhenDieBlow::CanBegin( void )
{
	bool bResult = true;

	// ��ų�� ���� ���� �ʴٸ� ����ȿ���� �ٷ� ������.
	DnSkillHandle hSkill = m_hActor->FindSkill( (int)m_fValue );
	if( !hSkill )
	{
		OutputDebug( "CDnUsingSkillWhenDieBlow::CanBegin - Don't have skill\n" );
		bResult = false;
	}

	return bResult;
}
#endif

void CDnUsingSkillWhenDieBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	_ASSERT( m_hActor->IsMonsterActor() && "�����鼭 Ư�� ��ų ����ϴ� ����ȿ���� ���͸� �����մϴ�." );
	if( false == m_hActor->IsMonsterActor() )
		SetState( STATE_BLOW::STATE_END );

	// hp �� 0 ���� �ƴ��� �Ǵ��ؼ� ���� ����� ���Ӽ�������
	// CDnActor::Die() ȣ��� �� hp 1 �� ���� �����ְ� �Լ� ó���� ������Ų��.
	// ���� ��ų�� ����Ǹ� �׶� ���δ�..
	// ���� �������� ���� ����ȿ�� ����..

	OutputDebug( "CDnUsingSkillWhenDieBlow::OnBegin\n");
}

void CDnUsingSkillWhenDieBlow::OnDie( void )
{
	// ���� Ŭ�� �����ϰ� ���� �������� ���� ����ȿ�� ����..
	m_hActor->SetHP( 1 );
	m_bDead = true;

	// ���Ӽ������� hp �� 1�� �����ϰ�, ������ ��ų�� ����.
	// ���� ai �� ������Ų��.
#ifdef _GAMESERVER
	CDnMonsterActor* pMonsterActor = static_cast<CDnMonsterActor*>(m_hActor.GetPointer());
	pMonsterActor->SetAIState( MAAiReceiver::Disable );

	m_hActor->CmdAddStateEffect( NULL, STATE_BLOW::BLOW_069, 0, "10", false , false);
	m_hActor->CmdAddStateEffect( NULL, STATE_BLOW::BLOW_099, -1, "", false, false );
#endif
}

void CDnUsingSkillWhenDieBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );

#ifdef _GAMESERVER
	if( m_bDead )
	{
#endif
		if( false == m_bUsedDieSkill )
		{
			// �����鼭 ��ų�� ���Ǿ����� üũ�Ѵ�.	
			// ��ų ����� Stand �׼��� �� ����ϵ��� ������ ���Ѵ�.
			DnSkillHandle hSkill = m_hActor->GetProcessSkill();
			if( hSkill &&
				(hSkill->GetClassID() == (int)m_fValue) )
			{
				m_bUsedDieSkill = true;
			}
#ifdef _GAMESERVER
			else
			{
				// ���߿��� ��ų�� ����ϰ� �Ǵ� ���� �ϴ� �����س��� ������ ����.
				// �Ƹ��� ���� ������ �� ���� ��ų ����� �����ؾ��� ��.
				if( strstr( m_hActor->GetCurrentAction(), "Stand") )
				{
					m_hActor->UseSkill( (int)m_fValue, false );


					//#44884 ������ DieSkill ���� ����/Ŭ���̾�Ʈ ����ȭ ���� �߻�
					//Ŭ���̾�Ʈ���� üũ�� ���� �ʴ� ��찡 �߻���.
					//�������� ��ų ����ϰ� Ŭ���̾�Ʈ�� ����ȭ ��Ŷ ����..
					//���� ����ȿ�� ShowStateEffect �Լ��� �̿� �ϵ���..

					BYTE pBuffer[128];
					CPacketCompressStream Stream( pBuffer, 128 );

					DWORD dwUniqueID = m_hActor ? m_hActor->GetUniqueID() : -1;
					STATE_BLOW::emBLOW_INDEX blowIndex = STATE_BLOW::BLOW_137;
					bool bShowEffect = true;
					Stream.Write( &dwUniqueID, sizeof(dwUniqueID) );
					Stream.Write( &blowIndex, sizeof(blowIndex));
					Stream.Write( &bShowEffect, sizeof(bShowEffect));

					m_hActor->Send(eActor::SC_SHOW_STATE_EFFECT, &Stream);
				}
				else if( !m_hActor->IsAir() )
				{
					m_hActor->CmdAction("Stand");
				}
			}
#endif
		}

		if( m_bUsedDieSkill )
		{
			// HP �� 0 ���� ����� �״� �׼� �����鼭 ��ü ����.
			// HP �� 0 �� �Ǹ�, ProcessDie ���� destroy �� ���õȴ�.
			if( false == m_hActor->IsProcessSkill() )
			{
				m_hActor->SetActionQueue( "Die" );
				m_hActor->SetHP( 0 );

#if defined(PRE_FIX_44884)
				//���������� ���⿡�� OnDie�� ȣ�� �Ѵ�..
				//Ŭ���̾�Ʈ CDnActor::OnDie�� protected�� ���� �Ǿ� �־
				//�� ����ȿ���� ���� ���͸� ���� �ϴ� �����̶� ���� ���ͷ� ĳ���� �ؼ� OnDieȣ�� �Ѵ�.
				CDnMonsterActor* pMonsterActor = static_cast<CDnMonsterActor*>(m_hActor.GetPointer());
				if (pMonsterActor)
					pMonsterActor->OnDie(m_FinalHitterActor);

				SetState(STATE_BLOW::STATE_END);
#endif // PRE_FIX_44884

#ifndef _GAMESERVER
				m_hActor->SetDieDelta( 3.0f );
#endif
			}
		}
#ifdef _GAMESERVER
	}
#endif
}

void CDnUsingSkillWhenDieBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "CDnUsingSkillWhenDieBlow::OnEnd\n");
}


#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnUsingSkillWhenDieBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}

void CDnUsingSkillWhenDieBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW