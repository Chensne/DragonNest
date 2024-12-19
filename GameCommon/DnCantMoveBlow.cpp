#include "StdAfx.h"
#include "DnCantMoveBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnCantMoveBlow::CDnCantMoveBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )/*, m_iOriginalCalcPositionFlag( 0 )*/,
																				m_bCantMoveXZ( false )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_070;

	if( NULL != szValue )
	{
		SetValue( szValue );
		m_fValue = (float)atof( szValue );
	}

	if( 1.0f == m_fValue )
		m_bCantMoveXZ = true;
}

CDnCantMoveBlow::~CDnCantMoveBlow(void)
{

}



void CDnCantMoveBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_bCantMoveXZ )
	{
		if( 0 == m_hActor->GetCantXZMoveSEReferenceCount() )
		{
			m_hActor->SetMovable( false );
			m_hActor->SetStateEffect( m_hActor->GetStateEffect() | CDnActorState::Cant_Move );

			//상대방 캐릭터가 이동중 이 상태효과로 멈출때 Local이 아닌 다른 클라이언트는
			//계속 이동 정보가 설정 되어 있어서 MAWalkMovement의 ProcessCommand함수에서 캐릭터의 이동 처리가 될 수 있음.
			//그래서 이 상태효과가 최초로 적용 되는 시점에 Move정보 리셋을 위해 CmdStop호출함.
			//#36970 이동중이 아닌경우
			EtVector3 *pMovePos = m_hActor->GetMovePos();
			if (pMovePos && EtVec3LengthSq( pMovePos ) > 0.0f)
				m_hActor->CmdStop("Stand");
		}

		m_hActor->AddedCantXZMoveSE();

		OutputDebug( "CDnCantMoveBlow::OnBegin XZ Ref: [%d --> %d]\n", m_hActor->GetCantXZMoveSEReferenceCount()-1, m_hActor->GetCantXZMoveSEReferenceCount() );
	}
	else
	{
		if( 0 == m_hActor->GetCantMoveSEReferenceCount() )
		{
			m_hActor->SetMovable( false );
			m_hActor->SetStateEffect( m_hActor->GetStateEffect() | CDnActorState::Cant_Move );

			//상대방 캐릭터가 이동중 이 상태효과로 멈출때 Local이 아닌 다른 클라이언트는
			//계속 이동 정보가 설정 되어 있어서 MAWalkMovement의 ProcessCommand함수에서 캐릭터의 이동 처리가 될 수 있음.
			//그래서 이 상태효과가 최초로 적용 되는 시점에 Move정보 리셋을 위해 CmdStop호출함.
			//#36970 이동중이 아닌경우
			EtVector3 *pMovePos = m_hActor->GetMovePos();
			if (pMovePos && EtVec3LengthSq( pMovePos ) > 0.0f)
				m_hActor->CmdStop("Stand");
		}

		m_hActor->AddedCantMoveSE();

		OutputDebug( "CDnCantMoveBlow::OnBegin Ref: [%d --> %d]\n", m_hActor->GetCantMoveSEReferenceCount()-1, m_hActor->GetCantMoveSEReferenceCount() );
	}
}


void CDnCantMoveBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
 	if( m_bCantMoveXZ )
	{
		m_hActor->RemovedCantXZMoveSE();

		if( 0 == m_hActor->GetCantXZMoveSEReferenceCount() )
		{
			// CanMove 시그널쪽에서 계속 업데이트 해주므로 여기서는 그대로 둔다.
			//m_hActor->SetMovable( true );
			if( m_hActor->GetCantMoveSEReferenceCount() == 0 ) // 둘다 체크해야합니다.
				m_hActor->SetStateEffect( m_hActor->GetStateEffect() & ~CDnActorState::Cant_Move );
		}

		OutputDebug( "CDnCantMoveBlow::OnEnd XZ Ref: [%d --> %d]\n", m_hActor->GetCantMoveSEReferenceCount()+1, m_hActor->GetCantMoveSEReferenceCount() );
	}
	else
	{
		m_hActor->RemovedCantMoveSE();

		if( 0 == m_hActor->GetCantMoveSEReferenceCount() )
		{
			// CanMove 시그널쪽에서 계속 업데이트 해주므로 여기서는 그대로 둔다.
			//m_hActor->SetMovable( true );
			if( m_hActor->GetCantXZMoveSEReferenceCount() == 0 ) // 둘다 체크해야합니다.
				m_hActor->SetStateEffect( m_hActor->GetStateEffect() & ~CDnActorState::Cant_Move );
		}

		OutputDebug( "CDnCantMoveBlow::OnEnd Ref: [%d --> %d]\n", m_hActor->GetCantMoveSEReferenceCount()+1, m_hActor->GetCantMoveSEReferenceCount() );
	}
}

void CDnCantMoveBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process(LocalTime, fDelta);

	// [2010/11/26 semozz]
	// 서버와의 동기화기 필요한 StateBlow는 서버에서 패킷 받아서 제거 하도록
	// 클라이언트에서는 STATE_END로 전환하지 않는다.
	// [2010/12/13 semozz]
	// IsPermanent가 설정 되어 있지 않고
	if( !IsPermanent() &&
		m_StateBlow.fDurationTime <= 0.0f )
	{
#if defined(_GAMESERVER)
		//서버에서 StateBlow제거 패킷을 보낸다
		//서버에서는 패킷만 보낸다. 지우는건 StateBlow의 Process상에서
		m_hActor->SendRemoveStateEffectFromID(GetBlowID());
		m_StateBlow.fDurationTime = 0.0f;
		SetState( STATE_BLOW::STATE_END );

		// [2010/12/22 semozz]
		// 이번 프레임에서 삭제를 위해 삭제 리스트에 등록한다.
		// 여기서 등록 안 하면 다음번 프레임에서 빠진다.
		m_hActor->RemoveStateBlowFromID(GetBlowID());
#else
		m_StateBlow.fDurationTime = 0.01f;
		SetState( STATE_BLOW::STATE_DURATION );
#endif	
	}
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnCantMoveBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}

void CDnCantMoveBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW