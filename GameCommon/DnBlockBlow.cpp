#include "StdAfx.h"
#include "DnBlockBlow.h"
#if defined( _GAMESERVER )
#include "DnPlayerActor.h"
#endif // #if defined( _GAMESERVER )

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif



CDnBlockBlow::CDnBlockBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_030;
	SetValue( szValue );
	m_fValue = (float)atof( szValue );

	m_iCanBlockCount = (int)m_fValue;

	AddCallBackType( SB_ONDEFENSEATTACK );
}

CDnBlockBlow::~CDnBlockBlow(void)
{

}



void CDnBlockBlow::OnBegin(LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "CDnBlockBlow::OnBegin, Value:%d \n", (int)m_fValue );
}



void CDnBlockBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );

#ifdef _GAMESERVER
	if( m_iCanBlockCount <= 0 )
	{
		// Ŭ�󿡰� ����ȿ�� �����϶� ��Ŷ�� ������ ���������� ������� blow �� ������Ų��.
		m_hActor->CmdRemoveStateEffect( m_StateBlow.emBlowIndex, false );
		SetState( STATE_BLOW::STATE_END );
	}
#endif
}


void CDnBlockBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "CDnBlockBlow::OnEnd\n");
}


#ifdef _GAMESERVER
bool CDnBlockBlow::OnDefenseAttack( DnActorHandle hHitter, CDnState* pAttackerState, CDnDamageBase::SHitParam &HitParam, bool bHitSuccess )
{
	//  #25797 ��Ʈ �ñ׳ο� bIgnoreParring �� ���� ������ �ߵ����� �ʴ´�.
	if( HitParam.bIgnoreParring )
		return false;

	// ��������϶��� �ߵ����� ����. 
	// ��� ����ȿ���� ������ �÷��̾ ����Ѵ�.
	_ASSERT( m_hActor->IsPlayerActor() );
	if( m_hActor->IsPlayerActor() )
	{
		CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(m_hActor.GetPointer());
		if( pPlayerActor->IsCannonMode() )
			return false;

		// ���⸦ ��� ���� ���� ��쿣 �ߵ����� ����. #26772
		if( false == (pPlayerActor->IsBattleMode() && pPlayerActor->GetWeapon( 0 ) && pPlayerActor->GetWeapon( 1 )) )
			return false;
	}

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

	bool bResult = false;


#if defined(PRE_ADD_49166)
	// �ǰ��ڰ� ����� (Freezing / FrameStop) ����ȿ���� ����Ǿ� �ִ� ���� �� �ߵ� ���� �ʵ���..
	if (IsInVaildBlockCondition(m_hActor) == true)
		return false;
#endif // PRE_ADD_49166

	if( m_iCanBlockCount > 0 )
	{
		--m_iCanBlockCount;

		HitParam.szActionName.assign( "Skill_Block" );	
		OutputDebug( "CDnBlockBlow::OnDefenseAttack, CanBlockCount:%d \n", m_iCanBlockCount );

		// �ɾ���� �ֵ����� �˷���. /////////////////////////////////////////////
		boost::shared_ptr<IDnObserverNotifyEvent> pEvent( IDnObserverNotifyEvent::Create( EVENT_BUBBLE_BLOCK_SUCCESS ) );
		pEvent->SetSkillID( m_ParentSkillInfo.iSkillID );
		Notify( pEvent );
		//////////////////////////////////////////////////////////////////////////

		bResult = true;
	}

	return bResult;
}
#endif


#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnBlockBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

void CDnBlockBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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