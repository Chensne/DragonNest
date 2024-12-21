#include "StdAfx.h"
#include "DnParryBlow.h"
#ifdef _GAMESERVER
#include "DnStateBlow.h"
#include "DnPlayerActor.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnParryBlow::CDnParryBlow(DnActorHandle hActor, const char *szValue)
	: CDnBlow(hActor)
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_031;
	AddCallBackType(SB_ONDEFENSEATTACK);
	SetValue(szValue);
	//m_nValue = atoi( szValue );
	m_fValue = (float)atof(szValue);

#ifdef _GAMESERVER
	m_strParringActionName.assign( "Skill_Parrying" );
	m_bEnable = false;
	m_bEnableLastLoop = false;
	m_fAdditionalSignalProb = 0.0f;
	m_fAdditionalSignalProbLastLoop = 0.0f;
#endif
}

CDnParryBlow::~CDnParryBlow(void)
{
}

void CDnParryBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );

#ifdef _GAMESERVER
	// ���� process ���� ���κп� ����ȿ�� process �� ���� ������ ���⼭ �������ָ� �и��ñ׳��� ������ �����ϰ� enable �� ó���� ��.
	m_bEnableLastLoop = m_bEnable;
	m_fAdditionalSignalProbLastLoop = m_fAdditionalSignalProb;
	m_bEnable = false;
	m_fAdditionalSignalProb = 0.0f;
#endif
}

void CDnParryBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "CDnParryBlow::OnBegin\n" );
}

void CDnParryBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "CDnParryBlow::OnEnd\n" );
}

#ifdef _GAMESERVER
bool CDnParryBlow::OnDefenseAttack( DnActorHandle hHitter, CDnState* pAttackerState, CDnDamageBase::SHitParam &HitParam, bool bHitSuccess )
{
	// #25797 ��Ʈ �ñ׳ο� bIgnoreParring �� ���� ������ �ߵ����� �ʴ´�.
	if( HitParam.bIgnoreParring )
		return false;

	if( !m_bEnableLastLoop )
		return false;

	if( !bHitSuccess ) 
		return false;

	/*
	// hit percent �� 0% �� ��� �ߵ����� �ʴ� ������ ��. (#21175)
	if( 0.0f == HitParam.fDamage )
	{
		// #29204 fDamage�� 0�϶� �и� Ǯ���� ���� ����.
		// ���� ���� ��츸 ��� ���� [2011/02/24 semozz]
		// �ٸ� ���� ���� ���� ��ƾ ó��...
		if (hHitter && m_hActor &&
			(hHitter->GetTeam() == m_hActor->GetTeam()))
		{
			return false;
		}
	}
	*/

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

	// �÷��̾��� ��쿣,
	if( m_hActor->IsPlayerActor() )
	{
		// ��������϶��� �ߵ����� ����. 
		CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(m_hActor.GetPointer());
		if( pPlayerActor->IsCannonMode() )
			return false;

		// ���⸦ ��� ���� ���� ��쿣 �ߵ����� ����. #26772
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
		OutputDebug( "CDnParryingBlow::OnDefenseAttack\n" );

		// �ɾ���� �ֵ����� �˷���. /////////////////////////////////////////////
		boost::shared_ptr<IDnObserverNotifyEvent> pEvent( IDnObserverNotifyEvent::Create( EVENT_BUBBLE_PARRING_SUCCESS ) );
		pEvent->SetSkillID( m_ParentSkillInfo.iSkillID );
		Notify( pEvent );
		//////////////////////////////////////////////////////////////////////////

		return true;
	}
	else
	{
		return false;
	}
}
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnParryBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

void CDnParryBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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