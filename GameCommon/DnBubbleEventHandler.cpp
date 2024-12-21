#include "StdAfx.h"
#include "DnBubbleEventHandler.h"
#include "DnObserverEventMessage.h"
#include "DnBubbleSystem.h"
#include "DnPlayerActor.h"
#include "DnBlow.h"
#include "DnTableDB.h"

namespace BubbleSystem
{

IDnBubbleEventHandler* IDnBubbleEventHandler::Create( int iEventHandlerType, DnActorHandle hActor, const char* pArgument )
{
	IDnBubbleEventHandler* pEventHandler = NULL;

	switch( iEventHandlerType )
	{
		case GETTING_BUBBLE:
			pEventHandler = new CDnGettingBubbleHandler;
			break;

		case REMOVE_BUBBLE:
			pEventHandler = new CDnRemoveBubbleHandler;
			break;

		case UPDATE_BUBBLE_STATE_EFFECT:
			pEventHandler = new CDnUpdateStateEffectHandler;
			break;

		case ENCHANT_SKILL_ONCE:
			pEventHandler = new CDnEnchantSkillOnce;
			break;

		default:
			return NULL;
	}

	pEventHandler->Initialize( hActor, pArgument );

	return pEventHandler;
}

// CDnGettingBubbleHandler
void CDnGettingBubbleHandler::Initialize( DnActorHandle hActor, const char* pArgument )
{
	if( !hActor || NULL == pArgument )
		return;

	m_hActor = hActor;

	std::vector<string> vlTokens;
	string strArgument( pArgument );
	TokenizeA( strArgument, vlTokens, ";" );

	_ASSERT( 2 == (int)vlTokens.size() );

	m_iBubbleTypeID = atoi( vlTokens.at(0).c_str() );
	m_fDurationTime = (float)(atoi( vlTokens.at(1).c_str() )) / 1000.0f;

	// 버블 최대 갯수를 넘어간다면 더 이상 버블을 쌓지 않도록 한다.
	m_iAllowedMaxBubbleCount = 0;
	DNTableFileFormat* pBubbleDefineTable = GetDNTable( CDnTableDB::TSKILLBUBBLEDEFINE );
	if( pBubbleDefineTable->IsExistItem( m_iBubbleTypeID ) )
	{
		m_iAllowedMaxBubbleCount = pBubbleDefineTable->GetFieldFromLablePtr( m_iBubbleTypeID, "_BubbleLimit" )->GetInteger();
	}
}

void CDnGettingBubbleHandler::ProcessEvent( CDnBubbleSystem* pBubbleSystem, ::IDnObserverNotifyEvent* pEvent )
{
	// 최대 버블 갯수가 넘어가면 추가하지 않는다.
	int iNowBubbleCount = pBubbleSystem->GetBubbleCountByTypeID( m_iBubbleTypeID );
	if( iNowBubbleCount < m_iAllowedMaxBubbleCount )
	{
		CDnBubbleSystem::S_CREATE_BUBBLE Info;
		Info.iBubbleTypeID = m_iBubbleTypeID;
		Info.iIconIndex = m_iIconIndex;
		Info.fDurationTime = m_fDurationTime;
		pBubbleSystem->AddBubble( Info );

		int iAddedBubbleCount = pBubbleSystem->GetBubbleCountByTypeID( m_iBubbleTypeID );

		// 클라이언트로 버블 추가 패킷 보내줌.
		if( m_hActor )
		{
			// 아이콘 인덱스가 셋팅된 후에 패킷을 만들어둔다.
			char acBuffer[ 32 ] = { 0 };
			CPacketCompressStream Stream( acBuffer, 32 );
			Stream.Write( &m_iBubbleTypeID, sizeof(m_iBubbleTypeID) );
			Stream.Write( &m_iIconIndex, sizeof(m_iIconIndex) );
			Stream.Write( &m_fDurationTime, sizeof(m_fDurationTime) );
			Stream.Write( &iAddedBubbleCount, sizeof(iNowBubbleCount) );

			m_hActor->Send( eActor::SC_ADDBUBBLE, &Stream );
		}
	}
	else
	{
		// 최대 버블 갯수가 넘어갔으므로 기존에 있는 버블들의 지속시간만 갱신해준다.
		if( 0 < pBubbleSystem->GetBubbleCountByTypeID( m_iBubbleTypeID ) )
		{
			// 지속시간 갱신.
			pBubbleSystem->SetDurationTime( m_iBubbleTypeID, m_fDurationTime );
			
			char acBuffer[ 32 ] = { 0 };
			CPacketCompressStream Stream( acBuffer, 32 );
			Stream.Write( &m_iBubbleTypeID, sizeof(int) );
			Stream.Write( &m_iIconIndex, sizeof(m_iIconIndex) );
			Stream.Write( &m_fDurationTime, sizeof(float) );
			Stream.Write( &iNowBubbleCount, sizeof(int) );

			m_hActor->Send( eActor::SC_REFRESH_BUBBLE_DURATIONTIME, &Stream );
		}
	}
}
//////////////////////////////////////////////////////////////////////////


// CDnRemoveBubbleHandler
void CDnRemoveBubbleHandler::Initialize( DnActorHandle hActor, const char* pArgument )
{
	if( !hActor || NULL == pArgument )
		return;

	m_hActor = hActor;

	std::vector<string> vlTokens;
	string strArgument( pArgument );
	TokenizeA( strArgument, vlTokens, ";" );

	_ASSERT( 2 == (int)vlTokens.size() );

	m_iBubbleTypeID = atoi( vlTokens.at(0).c_str() );
	m_iRemoveCount = atoi( vlTokens.at(1).c_str() );

	SecureZeroMemory( &m_PacketBuffer, sizeof(m_PacketBuffer) );
	m_pPacketStream.reset( new CPacketCompressStream( &m_PacketBuffer, sizeof(m_PacketBuffer) ) );
	m_pPacketStream->Write( &m_iBubbleTypeID, sizeof(m_iBubbleTypeID) );
	m_pPacketStream->Write( &m_iRemoveCount, sizeof(m_iRemoveCount) );
}

void CDnRemoveBubbleHandler::ProcessEvent( CDnBubbleSystem* pBubbleSystem, ::IDnObserverNotifyEvent* pEvent )
{
	pBubbleSystem->RemoveBubbleByTypeID( m_iBubbleTypeID, m_iRemoveCount );

	// 클라이언트로 버블 제거 패킷 보내줌.
	if( m_hActor )
	{
		m_hActor->Send( eActor::SC_REMOVEBUBBLE, m_pPacketStream.get() );
	}
}
//////////////////////////////////////////////////////////////////////////

// CDnUpdateStateEffectHandler
void CDnUpdateStateEffectHandler::Initialize( DnActorHandle hActor, const char* pArgument )
{
	if( !hActor || NULL == pArgument )
		return;

	m_hActor = hActor;

	std::vector<string> vlTokens;
	string strArgument( pArgument );
	TokenizeA( strArgument, vlTokens, ";" );

	_ASSERT( 2 == (int)vlTokens.size() );

	m_iBubbleTypeID = atoi( vlTokens.at(0).c_str() );
	m_iSkillID = atoi( vlTokens.at(1).c_str() );
}

void CDnUpdateStateEffectHandler::ProcessEvent( CDnBubbleSystem* pBubbleSystem, ::IDnObserverNotifyEvent* /*pEvent*/ )
{
	// 기존에 존재하는 버블 상태효과가 있다면 삭제
	pBubbleSystem->RemoveBubbleStateBlow( m_iBubbleTypeID );

	// 버블 갯수에 따라 레벨링을 해준다.
	int iLevel = pBubbleSystem->GetBubbleCountByTypeID( m_iBubbleTypeID );
	m_hSkill = CDnSkill::CreateSkill( m_hActor, m_iSkillID, iLevel );

	// 스킬 생성 실패. 데이터가 없거나 잘못된 경우. pvp/pve 모두 데이터가 있어야 한다.
	if( !m_hSkill )
		return;

	int iNumStateEffect = m_hSkill->GetStateEffectCount();
	for( int iSE = 0; iSE < iNumStateEffect; ++iSE )
	{
		const CDnSkill::StateEffectStruct* pSE = m_hSkill->GetStateEffectFromIndex( iSE );
		if( CDnSkill::ApplySelf == pSE->ApplyType )
		{
			// 상태효과 추가 패킷. 여기서 추가된 상태효과의 삭제는 CDnBubbleSystem 쪽에서
			// 처리한다. 모양새가 안좋긴하지만 일단은..
			int iBlowID = m_hActor->CmdAddStateEffect( NULL, (STATE_BLOW::emBLOW_INDEX)pSE->nID, -1, pSE->szValue.c_str() );
			_ASSERT( -1 != iBlowID );
			if( -1 != iBlowID )
			{
				pBubbleSystem->AddBubbleStateBlow( m_iBubbleTypeID, iBlowID );
			}
		}
	}

	SAFE_RELEASE_SPTR( m_hSkill );
}
//////////////////////////////////////////////////////////////////////////

// CDnEnchantSkillOnce
void CDnEnchantSkillOnce::Initialize( DnActorHandle hActor, const char* pArgument )
{
	if( !hActor || NULL == pArgument )
		return;

	m_hActor = hActor;
	_ASSERT( m_hActor->IsPlayerActor() );

	std::vector<string> vlTokens;
	string strArgument( pArgument );
	TokenizeA( strArgument, vlTokens, ";" );

	_ASSERT( 2 == (int)vlTokens.size() );

	m_iTargetSkillID = atoi( vlTokens.at(0).c_str() );
	m_iEnchantSkillID = atoi( vlTokens.at(1).c_str() );
}

void CDnEnchantSkillOnce::ProcessEvent( CDnBubbleSystem* pBubbleSystem, ::IDnObserverNotifyEvent* /*pEvent*/ )
{
	if( m_hActor->IsPlayerActor() )
	{
		CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(m_hActor.GetPointer());
		pPlayerActor->ApplyEnchantSkillOnceFromBubble( m_iTargetSkillID, m_iEnchantSkillID );

		// 클라이언트의 스킬은 강화되지 않고 서버만 한번만 강화 처리됨.
		// 목적이 원래의 스킬 상태효과를 변경시키는 것이므로 상태효과들이 서버에서 모두 패킷으로 전송되고 있기 때문에
		// 게임서버에서만 스킬 사용할 때 한번 바꾸는 걸 처리한다.
		// 현재로서는 클라이언트에서 스킬 사용전에 미리 강화시킬 방법이 없으므로 이렇게 한다.
	}
} 
//////////////////////////////////////////////////////////////////////////

}