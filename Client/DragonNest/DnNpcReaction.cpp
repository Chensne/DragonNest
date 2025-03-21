#include "StdAfx.h"
#include "DnNpcReaction.h"
#include "DnTableDB.h"
#include "DnWorldProp.h"
#include "DnNpcActor.h"
#include "ReputationSystemRepository.h"
#include "DnChatOption.h"

#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM

const float NPC_REACTION_CHECK_RANGE = 300.0f;


CDnNpcReaction::CDnNpcReaction(void) : m_bInitialized( false ), m_pReputationRepos(NULL)
{

}

CDnNpcReaction::~CDnNpcReaction(void)
{

}

// 처음 마을에 접속했을 때 한번만 호출.
void CDnNpcReaction::Initialize( CReputationSystemRepository* pReputationRepos )
{
	_ASSERT( pReputationRepos );
	m_pReputationRepos = pReputationRepos;

	// 테이블 읽음
	DNTableFileFormat*  pNpcReactionTable = GetDNTable( CDnTableDB::TNPCREACTION );

	char acBuffer[ 256 ] = { 0 };
	int iItemCount = pNpcReactionTable->GetItemCount();
	for( int iIndex = 0; iIndex < iItemCount; ++iIndex )
	{
		int iItemID = pNpcReactionTable->GetItemID( iIndex );

		S_NPC_REACTION_INFO Info;
		Info.iNpcID = pNpcReactionTable->GetFieldFromLablePtr( iItemID, "_NpcID" )->GetInteger();

		if(Info.iNpcID <= 0 )
			continue;

		for( int i = 0; i < NPC_REACTION_COUNT_MAX; ++i )
		{
			sprintf_s( acBuffer, "_Favor%d", i+1 );
			Info.aiFavorThresholdPercent[ i ] = pNpcReactionTable->GetFieldFromLablePtr( iItemID, acBuffer )->GetInteger();

			// 호감도 기준 비율값이 0 이면 거기서 끝임.
			if( 0 < Info.aiFavorThresholdPercent[ i ] )
			{
				sprintf_s( acBuffer, "_NpcTalk%d", i+1 );
				Info.aiSpeechUIStringID[ i ] = pNpcReactionTable->GetFieldFromLablePtr( iItemID, acBuffer )->GetInteger();

				sprintf_s( acBuffer, "_ActFile%d", i+1 );
				const char* pReactionActionName = pNpcReactionTable->GetFieldFromLablePtr( iItemID, acBuffer )->GetString();
				Info.astrEffectActionName[ i ].assign( pReactionActionName );
			}
		}

		m_mapNpcReaction.insert( make_pair(Info.iNpcID, Info) );
	}

	// 분노, 실망
	m_vlSCNpcEffectActionName.push_back( "Expression_Angry" );
	m_vlSCNpcEffectActionName.push_back( "Expression_Disappoint" );

	m_bInitialized = true;
}


// 마을이나 기타 맵 이동할 때 마다 호출해 줌.
void CDnNpcReaction::Clear( void )
{
	m_setNpcsInRange.clear();
}


// 특정 npc 에게 서버로부터 요청온 이펙트를 붙여준다.
void CDnNpcReaction::AttachNpcEffect( UINT uiNpcID, int iEffectdIndex )
{
	_ASSERT( iEffectdIndex < (int)m_vlSCNpcEffectActionName.size() );
	if( (int)m_vlSCNpcEffectActionName.size() <= iEffectdIndex )
		return;

	CDnNPCActor* pNpcActor = CDnNPCActor::FindNpcActorFromID( uiNpcID );
	if( pNpcActor )
	{
		// 이펙트 셋팅함.
		DnEtcHandle hHandle = CDnInCodeResource::GetInstance().CreatePlayerCommonEffect();
		if( hHandle )
		{
			_ASSERT( false == m_vlSCNpcEffectActionName.at(iEffectdIndex).empty() );
			hHandle->SetActionQueue( m_vlSCNpcEffectActionName.at(iEffectdIndex).c_str() );
			hHandle->SetPosition( *pNpcActor->GetPosition() );
			pNpcActor->TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::InsertSignalHandle( -1, -1, hHandle );
		}
	}
}


void CDnNpcReaction::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( !CDnActor::s_hLocalActor )
		return;

	// 주변에서 NPC 를 뒤진다. 범위는 일단 임의로 정해본다.
	// 새롭게 들어온 NPC 들에게만 말풍선을 보여주도록 한다.
	// 범위에서 벗어나면 말풍선 제거해준다.
	DNVector( DnActorHandle ) vlhResult;
	CDnActor::ScanActor( *CDnActor::s_hLocalActor->GetPosition(), NPC_REACTION_CHECK_RANGE, vlhResult );

	set<DWORD> setPrevNpcsInRange = m_setNpcsInRange;
	m_setNpcsInRange.clear();

	int iNumActor = (int)vlhResult.size();
	for( int iActor = 0; iActor < iNumActor; ++iActor )
	{
		DnActorHandle hActor = vlhResult.at( iActor );
		if( CDnActorState::Npc == hActor->GetActorType() )		// npc 만.
			m_setNpcsInRange.insert( hActor->GetUniqueID() );
	}

	// 새로 들어온 npc 들을 골라냄.
	DNVector( DWORD ) vlNewNpcs;
	set_difference( m_setNpcsInRange.begin(), m_setNpcsInRange.end(),
					setPrevNpcsInRange.begin(), setPrevNpcsInRange.end(),
					back_inserter(vlNewNpcs) );

	// 기존에 범위에 있다가 벗어난 npc 들을 골라냄.
	DNVector( DWORD ) vlOutNpcs;
	set_difference( setPrevNpcsInRange.begin(), setPrevNpcsInRange.end(),
					m_setNpcsInRange.begin(), m_setNpcsInRange.end(),
					back_inserter(vlOutNpcs) );
	
	// 새로 들어온 npc 들에게 리액션 취하게 처리.
	iNumActor = (int)vlNewNpcs.size();
	for( int iActor = 0; iActor < iNumActor; ++iActor )
	{
		DnActorHandle hActor = CDnActor::FindActorFromUniqueID( vlNewNpcs.at( iActor ) );
		_ASSERT( hActor );
		if( !hActor )
			continue;

		_ASSERT( hActor->IsNpcActor() );
		CDnNPCActor* pNpcActor = static_cast<CDnNPCActor*>( hActor.GetPointer() );
		int iNpcID = pNpcActor->GetNpcData().nNpcID;

		map<int, S_NPC_REACTION_INFO>::iterator iter = m_mapNpcReaction.find( iNpcID );
		if( m_mapNpcReaction.end() != iter )
		{
			const S_NPC_REACTION_INFO& Info = iter->second;
			_ASSERT( Info.iNpcID == iNpcID );

			int iReactionIndex = -1;
			int iCurrentPercent = m_pReputationRepos->GetNpcReputationPercent( Info.iNpcID, IReputationSystem::NpcFavor );
			for( int k = 0; k < NPC_REACTION_COUNT_MAX; ++k )
			{
				int iThresholdPercent = Info.aiFavorThresholdPercent[ k ];
				if( 0 != iThresholdPercent )
				{
					if( iThresholdPercent <= iCurrentPercent )
					{
						iReactionIndex = k;
					}
				}
			}

			// 호감도가 일정 값 이상이어야 리액션을 처리하도록 한다.
			if( -1 < iReactionIndex )
			{
				// 말풍선 보여줌
				hActor->SetChatBalloonText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Info.aiSpeechUIStringID[iReactionIndex] ), GetTickCount(), CHAT_REPUTE );
				hActor->GetChatBalloon().SetAdditionalYOffset( (pNpcActor->GetNPCStateMark() > 0) ? 0.1f : 0.0f );

				// 이펙트 셋팅함.
				DnEtcHandle hHandle = CDnInCodeResource::GetInstance().CreatePlayerCommonEffect();
				if( hHandle )
				{
					_ASSERT( false == Info.astrEffectActionName[ iReactionIndex ].empty() );
					hHandle->SetActionQueue( Info.astrEffectActionName[ iReactionIndex ].c_str() );
					hHandle->SetPosition( *hActor->GetPosition() );
					hActor->TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::InsertSignalHandle( -1, -1, hHandle );
				}

				// TODO: 나중엔 특정 액션을 취하게 할 수도 있다.
			}
		}
		
	}

	// 범위에서 벗어난 npc 들에게 리액션 취하는 것 끝냄.
	// 새로 들어온 npc 들에게 리액션 취하게 처리.
	iNumActor = (int)vlOutNpcs.size();
	for( int iActor = 0; iActor < iNumActor; ++iActor )
	{
		DnActorHandle hActor = CDnActor::FindActorFromUniqueID( vlOutNpcs.at( iActor ) );
		_ASSERT( hActor );
		if( !hActor )
			continue;

		_ASSERT( hActor->IsNpcActor() );
		CDnNPCActor* pNpcActor = static_cast<CDnNPCActor*>( hActor.GetPointer() );
		int iNpcID = pNpcActor->GetNpcData().nNpcID;

		map<int, S_NPC_REACTION_INFO>::iterator iter = m_mapNpcReaction.find( iNpcID );
		if( m_mapNpcReaction.end() != iter )
		{
			// 리액션 하던 것들 종료.
			hActor->StopHeadEffect( CDnHeadEffectRender::HeadEffect_ChatBalloon );

			// 이펙트 제거.
			// 현재 이펙트는 한번만 재생되고 액션 끝쪽에 destroy 시그널로 알아서 파괴되므로 그대로 둔다.

			// TODO: 나중엔 특정 액션을 취하게 할 수도 있다.
		}

	}

}

#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM