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

	// 스킬을 갖고 있지 않다면 상태효과를 바로 끝낸다.
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
	_ASSERT( m_hActor->IsMonsterActor() && "죽으면서 특정 스킬 사용하는 상태효과는 몬스터만 가능합니다." );
	if( false == m_hActor->IsMonsterActor() )
		SetState( STATE_BLOW::STATE_END );

	// hp 가 0 인지 아닌지 판단해서 죽은 경우라면 게임서버에서
	// CDnActor::Die() 호출될 때 hp 1 로 복구 시켜주고 함수 처리를 유예시킨다.
	// 사용된 스킬이 종료되면 그때 죽인다..
	// 죽은 시점부터 무적 상태효과 적용..

	OutputDebug( "CDnUsingSkillWhenDieBlow::OnBegin\n");
}

void CDnUsingSkillWhenDieBlow::OnDie( void )
{
	// 서버 클라 동일하게 죽은 시점부터 무적 상태효과 적용..
	m_hActor->SetHP( 1 );
	m_bDead = true;

	// 게임서버에서 hp 를 1로 지정하고, 지정된 스킬을 실행.
	// 몬스터 ai 는 정지시킨다.
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
			// 죽으면서 스킬이 사용되었는지 체크한다.	
			// 스킬 사용은 Stand 액션일 때 사용하도록 기준을 정한다.
			DnSkillHandle hSkill = m_hActor->GetProcessSkill();
			if( hSkill &&
				(hSkill->GetClassID() == (int)m_fValue) )
			{
				m_bUsedDieSkill = true;
			}
#ifdef _GAMESERVER
			else
			{
				// 공중에서 스킬을 사용하게 되는 경우는 일단 구현해놓고 보도록 하자.
				// 아마도 땅에 착지할 떄 까지 스킬 사용을 유보해야할 듯.
				if( strstr( m_hActor->GetCurrentAction(), "Stand") )
				{
					m_hActor->UseSkill( (int)m_fValue, false );


					//#44884 몬스터의 DieSkill 사용시 서버/클라이언트 동기화 문제 발생
					//클라이언트에서 체크가 되지 않는 경우가 발생함.
					//서버에서 스킬 사용하고 클라이언트로 동기화 패킷 보냄..
					//기존 상태효과 ShowStateEffect 함수를 이용 하도록..

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
			// HP 를 0 으로 만들고 죽는 액션 나오면서 객체 삭제.
			// HP 가 0 이 되면, ProcessDie 에서 destroy 가 셋팅된다.
			if( false == m_hActor->IsProcessSkill() )
			{
				m_hActor->SetActionQueue( "Die" );
				m_hActor->SetHP( 0 );

#if defined(PRE_FIX_44884)
				//최종적으로 여기에서 OnDie를 호출 한다..
				//클라이언트 CDnActor::OnDie가 protected로 선언 되어 있어서
				//이 상태효과는 몬스터 액터만 동작 하는 설정이라 몬스터 액터로 캐스팅 해서 OnDie호출 한다.
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