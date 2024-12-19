
#include "Stdafx.h"
#include "ReputationSystemEventHandlerForServer.h"
#include "ReputationSystemRepository.h"
#include "DNUserSession.h"
#include "DNDBConnection.h"
#include "NpcReputationSystem.h"
#include "DNGameDataManager.h"
#include "TimeSet.h"

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

CServerReputationSystemEventHandler::CServerReputationSystemEventHandler( CDNUserSession* pSession )
:m_pSession(pSession)
{

}

void CServerReputationSystemEventHandler::OnComplete( IReputationSystem* pReputation, IReputationSystem::eType Type )
{
	switch( Type )
	{
		case IReputationSystem::NpcMalice:
		{
#if defined( PRE_REMOVE_NPCREPUTATION_MALICE )
#else
			CReputationSystemRepository* pRepository = m_pSession->GetReputationSystem();
			_ASSERT( pRepository );

			_ASSERT( dynamic_cast<CNpcReputationSystem*>(pReputation) );
			CNpcReputationSystem* pNpcReputaion = static_cast<CNpcReputationSystem*>(pReputation);

			// 당사자
			pRepository->SetNpcReputation( pNpcReputaion->GetNpcID(), IReputationSystem::NpcFavor, 0 );	// 호의 초기화
			pRepository->SetNpcReputation( pNpcReputaion->GetNpcID(), Type, 0 );							// 악의 초기화

			TReputeTableData* pReputationTable = g_pDataManager->GetReputeTableData( pNpcReputaion->GetNpcID() );
			if (!pReputationTable) break;

			// 친한사람
			for( UINT i=0 ; i<pReputationTable->vFavorNpcID.size() ; ++i )
			{
				REPUTATION_TYPE value = pRepository->GetNpcReputation( pReputationTable->vFavorNpcID[i], IReputationSystem::NpcFavor );
				value = value*pReputationTable->iFavorGroupBomb/100;

				pRepository->AddNpcReputation( pReputationTable->vFavorNpcID[i], IReputationSystem::NpcFavor, -value );	// 호감도 하락
			}

			// 싫어하는사람
			for( UINT i=0 ; i<pReputationTable->vMaliceNpcID.size() ; ++i )
			{
				REPUTATION_TYPE value = pRepository->GetNpcReputation( pReputationTable->vMaliceNpcID[i], IReputationSystem::NpcFavor );
				value = value*pReputationTable->iMaliceGroupBomb/100;

				pRepository->AddNpcReputation( pReputationTable->vMaliceNpcID[i], IReputationSystem::NpcFavor, value );	// 호감도 상승
			}
#endif // #if defined( PRE_REMOVE_NPCREPUTATION_MALICE )

			break;
		}
	}
}

void CServerReputationSystemEventHandler::OnChange( IReputationSystem::eType Type )
{
}

#if defined( PRE_ADD_REPUTATION_EXPOSURE )
void CServerReputationSystemEventHandler::OnCommit( std::map<IReputationSystem*,size_t>& Reputation, bool bClientSend )
{
	if( m_pSession == NULL || m_pSession->GetDBConnection() == NULL || m_pSession->GetReputationSystem() == NULL )
		return;

	std::vector<TNpcReputation> vData;
	m_pSession->GetReputationSystem()->GetCommitData( vData );

	if( vData.empty() )
		return;

	// DB M/W 전송
	CDNDBConnection* pDBCon = m_pSession->GetDBConnection();
	pDBCon->QueryModNpcReputation( m_pSession, vData );

	// 유저 전송
	if( bClientSend )
		m_pSession->SendModReputation( vData );
}
#else
void CServerReputationSystemEventHandler::OnCommit( std::map<IReputationSystem*,int>& Reputation, bool bClientSend )
{
	if( m_pSession == NULL || m_pSession->GetDBConnection() == NULL || m_pSession->GetReputationSystem() == NULL )
		return;

	std::vector<TNpcReputation> vData;
	m_pSession->GetReputationSystem()->GetCommitData( vData );

	if( vData.empty() )
		return;

	// DB M/W 전송
	CDNDBConnection* pDBCon = m_pSession->GetDBConnection();
	pDBCon->QueryModNpcReputation( m_pSession, vData );

	// 유저 전송
	if( bClientSend )
		m_pSession->SendModReputation( vData );
}
#endif // #if defined( PRE_ADD_REPUTATION_EXPOSURE )

void CServerReputationSystemEventHandler::OnConnect( __time64_t tTime, bool bSend )
{
	std::vector<int>			vNpcData;
	std::vector<TNpcReputation>	vNpcReputation;

	CReputationSystemRepository* pReputationSystem = m_pSession->GetReputationSystem();
	pReputationSystem->GetAllNpcData( vNpcData );

	for( UINT i=0 ; i<vNpcData.size() ; ++i )
	{
		vNpcReputation.push_back( TNpcReputation( vNpcData[i],
												  static_cast<int>(pReputationSystem->GetNpcReputation(vNpcData[i],IReputationSystem::NpcFavor)),
												  static_cast<int>(pReputationSystem->GetNpcReputation(vNpcData[i],IReputationSystem::NpcMalice)) ) );
	}		

#if defined( _VILLAGESERVER )
	// 1일당 깍이는 수치 
	if( bSend == false && m_pSession->GetLastServerType() == SERVERTYPE_LOGIN )
	{
		CTimeSet timeSet;
		__time64_t tLocalTime = timeSet.GetTimeT64_LC();

		INT64	GapSec	= tLocalTime-tTime;
		int		GapDay	= static_cast<int>(GapSec/(60*60*24));

		//if( GapDay > 0 )	// 해당기능 임시 비활성화
		if(0)
		{
			for( UINT i=0 ; i<vNpcData.size() ; ++i )
			{
#if defined( _WORK )
				WCHAR wszBuf[MAX_PATH];
				int iBeforeFavor,iBeforeMalice = 0;
				int iAfterFavor,iAfterMalice  = 0;
#endif // #if defined( _WORK )
				TReputeTableData* pReputationTable = g_pDataManager->GetReputeTableData( vNpcData[i] );
				if( pReputationTable == NULL )
					continue;

#if defined( _WORK )
				iBeforeFavor	= static_cast<int>(pReputationSystem->GetNpcReputation(vNpcData[i],IReputationSystem::NpcFavor));
				iBeforeMalice	= static_cast<int>(pReputationSystem->GetNpcReputation(vNpcData[i],IReputationSystem::NpcMalice));
#endif // #if defined( _WORK )

				pReputationSystem->AddNpcReputation( vNpcData[i], IReputationSystem::NpcFavor, pReputationTable->iTakeFavorPerDay*GapDay*-1, false );
				pReputationSystem->AddNpcReputation( vNpcData[i], IReputationSystem::NpcMalice, pReputationTable->iAddMalicePerDay*GapDay, false );

				for( UINT j=0 ; j<vNpcReputation.size() ; ++j )
				{
					if( vNpcReputation[j].iNpcID == vNpcData[i] )
					{
						vNpcReputation[j].iFavorPoint	= static_cast<int>(pReputationSystem->GetNpcReputation(vNpcData[i],IReputationSystem::NpcFavor));
						vNpcReputation[j].iMalicePoint	= static_cast<int>(pReputationSystem->GetNpcReputation(vNpcData[i],IReputationSystem::NpcMalice));
					}
				}

#if defined( _WORK )
				iAfterFavor		= static_cast<int>(pReputationSystem->GetNpcReputation(vNpcData[i],IReputationSystem::NpcFavor));
				iAfterMalice	= static_cast<int>(pReputationSystem->GetNpcReputation(vNpcData[i],IReputationSystem::NpcMalice));
				wsprintf( wszBuf, L"[NPC:%d] %d일 접속지나 Favor:%d->%d Malice:%d->%d", vNpcData[i], GapDay, iBeforeFavor, iAfterFavor, iBeforeMalice, iAfterMalice );
				m_pSession->SendDebugChat( wszBuf );
#endif // #if defined( _WORK )
			}

			pReputationSystem->Commit( false );
		}
	}
#endif // #if defined( _VILLAGESERVER )

	if( bSend )
	{
		m_pSession->SendReputationList( vNpcReputation );
		m_pSession->SendUnionMembershipList(m_pSession->GetItem());
#if defined (_VILLAGESERVER)
		m_pSession->SendUnionPoint();
#endif // #if defined (_VILLAGESERVER)
	}
}

#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
