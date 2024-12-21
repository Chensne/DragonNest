#include "StdAfx.h"
#include "DnDungeonHistory.h"
#include "DnGameTask.h"
#include "DnWorld.h"
#include "DnWorldGrid.h"
#include "DnWorldSector.h"
#include "DnWorldActProp.h"


CDnDungeonHistory::CDnDungeonHistory( CDnGameTask *pTask )
{
	m_pGameTask = pTask;
	m_nRandomSeed = 0;
}

CDnDungeonHistory::~CDnDungeonHistory()
{
	SAFE_DELETE_VEC( m_dwMapLiveUnitList );
	for( DWORD i=0; i<m_VecLiveSectorList.size(); i++ ) {
		SAFE_DELETE_VEC( m_VecLiveSectorList[i].dwMapLivePropList );
	}
	SAFE_DELETE_VEC( m_VecLiveSectorList );
}

bool CDnDungeonHistory::Backup()
{
	SAFE_DELETE_VEC( m_dwMapLiveUnitList );
	for( DWORD i=0; i<m_VecLiveSectorList.size(); i++ ) {
		SAFE_DELETE_VEC( m_VecLiveSectorList[i].dwMapLivePropList );
	}
	SAFE_DELETE_VEC( m_VecLiveSectorList );

	// 일단 유닛
	CMultiRoom *pRoom = m_pGameTask->GetRoom();
	int nRoomID = pRoom->GetRoomID();
	DWORD dwCount = (DWORD)CDnActor::s_pVecProcessList[nRoomID].size();
	for( DWORD i=0; i<dwCount; i++ ) {
		DnActorHandle hActor = CDnActor::s_pVecProcessList[nRoomID][i]->GetMySmartPtr();
		if( !hActor ) continue;
		if( hActor->IsDie() ) continue;

		m_dwMapLiveUnitList.insert( make_pair(hActor->GetUniqueID(), hActor->GetUniqueID()) );
	}
	// 그담 푸랍
	for( DWORD j=0; j<CDnWorld::GetInstance(pRoom).GetGrid()->GetActiveSectorCount(); j++ ) {
		CDnWorldSector *pSector = (CDnWorldSector *)CDnWorld::GetInstance(pRoom).GetGrid()->GetActiveSector(j);
		dwCount = pSector->GetPropCount();

		SectorStruct Struct;
		Struct.Index = pSector->GetIndex();

		for( DWORD i=0; i<dwCount; i++ ) {
			DnPropHandle hProp = ((CDnWorldProp *)pSector->GetPropFromIndex(i))->GetMySmartPtr();
			if( !hProp ) continue;

			DWORD dwActionIndex = -1;
			switch( hProp->GetPropType() ) {
				case PTE_Static: continue;
				case PTE_Light: continue;
				case PTE_Trap: continue;
				case PTE_Action:
				case PTE_Broken:
				case PTE_Operation:
				case PTE_Chest:
					dwActionIndex = ((CDnWorldActProp*)hProp.GetPointer())->GetElementIndex( ((CDnWorldActProp*)hProp.GetPointer())->GetCurrentAction() );
					break;
			}

			Struct.dwMapLivePropList.insert( make_pair(hProp->GetCreateUniqueID(), dwActionIndex) );
		}
		m_VecLiveSectorList.push_back( Struct );
	}
	return true;
}

bool CDnDungeonHistory::Recovery()
{
	CMultiRoom *pRoom = m_pGameTask->GetRoom();
	int nRoomID = pRoom->GetRoomID();
	DWORD dwCount = (DWORD)CDnActor::s_pVecProcessList[nRoomID].size();
	std::map<DWORD,DWORD>::iterator it;

	// 유닛 살리구
	for( DWORD i=0; i<dwCount; i++ ) {
		DnActorHandle hActor = CDnActor::s_pVecProcessList[nRoomID][i]->GetMySmartPtr();
		if( !hActor ) continue;
		it = m_dwMapLiveUnitList.find( hActor->GetUniqueID() );
		if( it != m_dwMapLiveUnitList.end() ) continue;

		hActor->SetDestroy();
	}

	// 프랍 살리구.
	for( DWORD j=0; j<CDnWorld::GetInstance(pRoom).GetGrid()->GetActiveSectorCount(); j++ ) {
		CDnWorldSector *pSector = (CDnWorldSector *)CDnWorld::GetInstance(pRoom).GetGrid()->GetActiveSector(j);
		dwCount = pSector->GetPropCount();

		SectorStruct *pStruct = NULL;
		for( DWORD k=0; k<m_VecLiveSectorList.size(); k++ ) {
			if( m_VecLiveSectorList[k].Index == pSector->GetIndex() ) {
				pStruct = &m_VecLiveSectorList[k];
				break;
			}
		}
		if( pStruct == NULL ) continue;

		for( DWORD i=0; i<dwCount; i++ ) {
			DnPropHandle hProp = ((CDnWorldProp *)pSector->GetPropFromIndex(i))->GetMySmartPtr();
			if( !hProp ) continue;
			switch( hProp->GetPropType() ) {
				case PTE_Static: continue;
				case PTE_Trap: continue;
				case PTE_Light: continue;
				case PTE_Action:
				case PTE_Broken:
				case PTE_Operation:
				case PTE_Chest:

					break;
			}

			it = pStruct->dwMapLivePropList.find( hProp->GetCreateUniqueID() );
			if( it != pStruct->dwMapLivePropList.end() ) continue;

			SAFE_RELEASE_SPTR( hProp );
			i--;
		}
	}

	return true;
}
