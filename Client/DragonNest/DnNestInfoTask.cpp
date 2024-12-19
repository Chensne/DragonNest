#include "StdAfx.h"
#include "DnNestInfoTask.h"
#include "DnTableDB.h"
#include "DnWorld.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnCommunityDlg.h"
#include "DnNestDlg.h"
#include "DnItemTask.h"
#include "DnCommonTask.h"
#include "TaskManager.h"

CDnNestInfoTask::CDnNestInfoTask()
: CTaskListener( false )
, m_nExpandTryCount( 0 )
, m_bInitPCBangNestInfo( false )
{
}

CDnNestInfoTask::~CDnNestInfoTask()
{
	Finalize();
}

void CDnNestInfoTask::OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize )
{
	switch( nMainCmd ) {
		case SC_CHAR: OnRecvCharMessage( nSubCmd, pData, nSize ); break;
	}
}

void CDnNestInfoTask::OnRecvCharMessage( int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd ) {
		case eChar::SC_NESTCLEAR: OnRecvCharNestClear( (SCNestClear*)pData ); break;
		case eChar::SC_UPDATENESTCLEAR: OnRecvCharUpdateNestClear( (SCUpdateNestClear*)pData ); break;
	}
}

void CDnNestInfoTask::RefreshNestDungeonList()
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMAP );
	if( !pSox ) return;

	std::vector<int> nVecList;
	pSox->GetItemIDListFromField( "_MapSubType", CDnWorld::MapSubTypeNest, nVecList );
	pSox->GetItemIDListFromField( "_MapSubType", CDnWorld::MapSubTypeEvent, nVecList, false );
	pSox->GetItemIDListFromField( "_MapSubType", CDnWorld::MapSubTypeChaosField, nVecList, false );
	pSox->GetItemIDListFromField( "_MapSubType", CDnWorld::MapSubTypeDarkLair, nVecList, false );
#if defined( PRE_ADD_SUBTYPE_TREASURESTAGE )
	pSox->GetItemIDListFromField( "_MapSubType", CDnWorld::MapSubTypeTreasureStage, nVecList, false );
#endif	// #if defined( PRE_ADD_SUBTYPE_TREASURESTAGE )
#ifdef PRE_ADD_DRAGON_FELLOWSHIP
	pSox->GetItemIDListFromField( "_MapSubType", CDnWorld::MapSubTypeFellowship, nVecList, false );
#endif // PRE_ADD_DRAGON_FELLOWSHIP

	for( DWORD i=0; i<nVecList.size(); i++ ) {
		if( pSox->GetFieldFromLablePtr( nVecList[i], "_MapType" )->GetInteger() != CDnWorld::MapTypeDungeon ) continue;
		if( pSox->GetFieldFromLablePtr( nVecList[i], "_IncludeBuild" )->GetInteger() != 1 ) continue;
		int nEnterTableID = pSox->GetFieldFromLablePtr( nVecList[i], "_EnterConditionTableID" )->GetInteger();
		if( nEnterTableID < 1 ) continue;

		int nMapNameID = pSox->GetFieldFromLablePtr( nVecList[i], "_MapNameID" )->GetInteger();
		NestDungeonInfoStruct *pStruct = new NestDungeonInfoStruct;
		pStruct->nMapTableID = nVecList[i];
		pStruct->szMapName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nMapNameID );
		pStruct->nMaxTryCount = pSox->GetFieldFromLablePtr( nVecList[i], "_MaxClearCount" )->GetInteger();
		pStruct->SubType = (CDnWorld::MapSubTypeEnum)pSox->GetFieldFromLablePtr( nVecList[i], "_MapSubType" )->GetInteger();
		pStruct->nTryCount = 0;
		pStruct->bExpandable = pSox->GetFieldFromLablePtr( nVecList[i], "_Expandable" )->GetInteger() ? true : false;
		pStruct->nEnterTableID = nEnterTableID;
		pStruct->bPCBangAdd = false;
		pStruct->nPCBangClearCount = 0;

		// 카오스필드 랜덤맵이라 _EnterConditionTableID 다 셋팅되어있어 구분하지 못하므로 중복 체크 함.
		bool bExistChaosField = false;
#ifdef PRE_ADD_DRAGON_FELLOWSHIP
		int nMapSubType = pSox->GetFieldFromLablePtr( nVecList[i], "_MapSubType" )->GetInteger();
		if( nMapSubType == CDnWorld::MapSubTypeChaosField || nMapSubType == CDnWorld::MapSubTypeFellowship )
#else // PRE_ADD_DRAGON_FELLOWSHIP
		if( pSox->GetFieldFromLablePtr( nVecList[i], "_MapSubType" )->GetInteger() == CDnWorld::MapSubTypeChaosField )	
#endif // PRE_ADD_DRAGON_FELLOWSHIP
		{
			for( int i=0; i<static_cast<int>( m_pVecNestList.size() ); i++ )
			{
				if( m_pVecNestList[i] && m_pVecNestList[i]->nEnterTableID == pStruct->nEnterTableID )
				{
					bExistChaosField = true;
					SAFE_DELETE( pStruct );
				}
			}
		}

		if( !bExistChaosField )
			m_pVecNestList.push_back( pStruct );
	}

	RefreshExpandTryCount();
}

void CDnNestInfoTask::SetPCBangNestCountInfo()
{
	if( m_bInitPCBangNestInfo )
		return;

	for( DWORD i=0; i<m_pVecNestList.size(); i++ ) 
	{
		int nPCBangAddCount = CheckPCBangAddNestCount( m_pVecNestList[i]->nMapTableID );
		if( nPCBangAddCount > 0 )
		{
			m_pVecNestList[i]->bPCBangAdd = true;
			m_pVecNestList[i]->nPCBangAddCount = nPCBangAddCount;
		}
		else
		{
			m_pVecNestList[i]->bPCBangAdd = false;
			m_pVecNestList[i]->nPCBangAddCount = 0;
		}
	}

	m_bInitPCBangNestInfo = true;
}

int CDnNestInfoTask::CheckPCBangAddNestCount( int nMapTableID )
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPCCAFE );
	if( pSox == NULL )
		return 0;

	CDnCommonTask* pCommonTask = (CDnCommonTask *)CTaskManager::GetInstance().GetTask( "CommonTask" );
	if( pCommonTask == NULL )
		return 0;

	int nPCBangAddCount = 0;
	for( int i=0; i<pSox->GetItemCount(); ++i )
	{
		int nTableID = pSox->GetItemID( i );
		int nType = pSox->GetFieldFromLablePtr( nTableID, "_Type" )->GetInteger();
		
		if( nType == PCBang::Type::NestClearCount )
		{
			switch( pCommonTask->GetPCBangGrade() )
			{
				case PCBang::Grade::Normal:
					{
						int nPCBangMapID = pSox->GetFieldFromLablePtr( nTableID, "_PCParam1" )->GetInteger();
						if( nMapTableID == nPCBangMapID )
						{
							nPCBangAddCount = pSox->GetFieldFromLablePtr( nTableID, "_PCParam2" )->GetInteger();
						}
					}
					break;
				case PCBang::Grade::Premium:
					{
						int nPCBangMapID = pSox->GetFieldFromLablePtr( nTableID, "_PremiumPCParam1" )->GetInteger();
						if( nMapTableID == nPCBangMapID )
						{
							nPCBangAddCount = pSox->GetFieldFromLablePtr( nTableID, "_PremiumPCParam2" )->GetInteger();
						}
					}
					break;
				case PCBang::Grade::Gold:
					{
						int nPCBangMapID = pSox->GetFieldFromLablePtr( nTableID, "_GoldPCParam1" )->GetInteger();
						if( nMapTableID == nPCBangMapID )
						{
							nPCBangAddCount = pSox->GetFieldFromLablePtr( nTableID, "_GoldPCParam2" )->GetInteger();
						}
					}
					break;
				case PCBang::Grade::Silver:
					{
						int nPCBangMapID = pSox->GetFieldFromLablePtr( nTableID, "_SilverPCParam1" )->GetInteger();
						if( nMapTableID == nPCBangMapID )
						{
							nPCBangAddCount = pSox->GetFieldFromLablePtr( nTableID, "_SilverPCParam2" )->GetInteger();
						}
					}
					break;
				case PCBang::Grade::Red:
					{
						int nPCBangMapID = pSox->GetFieldFromLablePtr( nTableID, "_RedPCParam1" )->GetInteger();
						if( nMapTableID == nPCBangMapID )
						{
							nPCBangAddCount = pSox->GetFieldFromLablePtr( nTableID, "_RedPCParam2" )->GetInteger();
						}
					}
					break;
			}

			if( nPCBangAddCount > 0 )
				break;
		}
	}

	return nPCBangAddCount;
}

void CDnNestInfoTask::RefreshExpandTryCount()
{
	m_nExpandTryCount = 0;
	if( CDnItemTask::IsActive() ) {
		std::vector<CDnItem *> pVecList;
		CDnItemTask::GetInstance().FindItemFromItemType( ITEMTYPE_EXPAND, ST_INVENTORY_CASH, pVecList );
		for( DWORD i=0; i<pVecList.size(); i++ ) {
			CDnItem *pItem = pVecList[i];
			m_nExpandTryCount += pItem->GetTypeParam(0);
		}
	}
}

bool CDnNestInfoTask::Initialize()
{
	RefreshNestDungeonList();
	return true;
}

void CDnNestInfoTask::Finalize()
{
	SAFE_DELETE_PVEC( m_pVecNestList );
}

void CDnNestInfoTask::Refresh()
{
	CDnCommunityDlg *pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
	if( !pCommunityDlg ) return;

	CDnNestDlg *pNestDlg = pCommunityDlg->GetNestDialog();
	if( !pNestDlg ) return;

	if( pNestDlg->IsShow() ) {
		pNestDlg->RefreshStageList();
	}

}

DWORD CDnNestInfoTask::GetNestDungeonCount() 
{ 
	return (DWORD)m_pVecNestList.size(); 
}

CDnNestInfoTask::NestDungeonInfoStruct *CDnNestInfoTask::GetNestDungeonInfo( DWORD dwIndex )
{
	if( dwIndex < 0 || dwIndex >= (DWORD)m_pVecNestList.size() ) return NULL;

	return m_pVecNestList[dwIndex];
}

CDnNestInfoTask::NestDungeonInfoStruct *CDnNestInfoTask::FindNestDungeonInfoFromMapIndex( int nMapIndex )
{
	for( DWORD i=0; i<m_pVecNestList.size(); i++ ) {
		if( m_pVecNestList[i]->nMapTableID == nMapIndex ) {
			return m_pVecNestList[i];
		}
	}
	return NULL;
}

void CDnNestInfoTask::OnRecvCharNestClear( SCNestClear *pPacket )
{
	ResetAllTryCount();
	RefreshExpandTryCount();

	for( int i=0; i<pPacket->cCount; i++ ) {
		TNestClearData *pClearData = &pPacket->NestClear[i];
		NestDungeonInfoStruct *pStruct = FindNestDungeonInfoFromMapIndex( pClearData->nMapIndex );
		if( !pStruct ) {
			// 서버에서 받은 정보중에 클라에서 못읽었던 맵이 있을경우 보여줄건지 체크해봐야함 ( 있던 맵 삭제시 보이게할건지 )
			// 일단 안보이고 무시되도록.
			continue;
		}
		pStruct->nTryCount = pClearData->cClearCount;
		pStruct->nPCBangClearCount = pClearData->cPCBangClearCount;
	}
}

void CDnNestInfoTask::OnRecvCharUpdateNestClear( SCUpdateNestClear *pPacket )
{
	NestDungeonInfoStruct *pStruct = FindNestDungeonInfoFromMapIndex( pPacket->Update.nMapIndex );
	if( !pStruct ) return;

	pStruct->nTryCount = pPacket->Update.cClearCount;
	pStruct->nPCBangClearCount = pPacket->Update.cPCBangClearCount;
	Refresh();
	
}

void CDnNestInfoTask::ResetAllTryCount()
{
	for( DWORD i=0; i<m_pVecNestList.size(); i++ ) {
		m_pVecNestList[i]->nTryCount = 0;
	}
}
