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
		// 클라에게 상태효과 제거하란 패킷만 보내고 서버에서는 순리대로 blow 를 해제시킨다.
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
	//  #25797 히트 시그널에 bIgnoreParring 이 켜져 있으면 발동하지 않는다.
	if( HitParam.bIgnoreParring )
		return false;

	// 대포모드일때는 발동하지 않음. 
	// 블록 상태효과는 무조건 플레이어만 사용한다.
	_ASSERT( m_hActor->IsPlayerActor() );
	if( m_hActor->IsPlayerActor() )
	{
		CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(m_hActor.GetPointer());
		if( pPlayerActor->IsCannonMode() )
			return false;

		// 무기를 들고 있지 않은 경우엔 발동하지 않음. #26772
		if( false == (pPlayerActor->IsBattleMode() && pPlayerActor->GetWeapon( 0 ) && pPlayerActor->GetWeapon( 1 )) )
			return false;
	}

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

	bool bResult = false;


#if defined(PRE_ADD_49166)
	// 피격자가 디버프 (Freezing / FrameStop) 상태효과가 적용되어 있는 경우는 블럭 발동 되지 않도록..
	if (IsInVaildBlockCondition(m_hActor) == true)
		return false;
#endif // PRE_ADD_49166

	if( m_iCanBlockCount > 0 )
	{
		--m_iCanBlockCount;

		HitParam.szActionName.assign( "Skill_Block" );	
		OutputDebug( "CDnBlockBlow::OnDefenseAttack, CanBlockCount:%d \n", m_iCanBlockCount );

		// 걸어놓은 애들한테 알려줌. /////////////////////////////////////////////
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

void CDnBlockBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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