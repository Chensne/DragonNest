#include "StdAfx.h"
#include "DnDropItem.h"
#include "DnWorld.h"
#include "DnWorldGrid.h"
#include "DnWorldSector.h"
#include "DnTableDB.h"
#include "PerfCheck.h"

#include "GameSendPacket.h"
#include "TaskManager.h"
#include "Task.h"
#include "DNGameRoom.h"
#include "DNUserSession.h"

#include "NavigationMesh.h"
#include "DNGameDataManager.h"

#include "DnGameTask.h"

DECL_DN_SMART_PTR_STATIC( CDnDropItem, MAX_SESSION_COUNT, 100 )

float CEtOctreeNode<DnDropItemHandle>::s_fMinRadius = 500.0f;

STATIC_DECL_INIT( CDnDropItem, bool, s_bActive ) = { false, };
STATIC_DECL_INIT( CDnDropItem, DWORD, s_dwUniqueCount ) = { 0, };
STATIC_DECL_INIT( CDnDropItem, std::vector<DnDropItemHandle>, s_hVecPreLoadItemList );

#define DROPITEM_LIFETIME 180.f

CDnDropItem::CDnDropItem( CMultiRoom *pRoom, DWORD dwUniqueID, bool bProcess )
: CDnUnknownRenderObject( pRoom, bProcess )
{
	CDnActionBase::Initialize( this );
	m_nItemID = -1;
	m_nOverlapCount = 1;
	m_nRandomSeed = 0;
	m_cOption = 0;
	m_dwUniqueID = dwUniqueID;

	m_fDistance = 0.f;
	m_fLifeTime = DROPITEM_LIFETIME;
	m_nRotate	= 0;

	m_fDisappearTime = 0.f;
	m_bDisappear = false;
	m_pCurCell = NULL;
	m_dwOwnerUniqueID = -1;
	m_Rank = ITEMRANK_D;

	m_bReversionItem = false;
	m_bReversionLock = false;
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	m_nEnchantID = 0;
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	if( bProcess ) InsertDropItem();
}

CDnDropItem::~CDnDropItem()
{
	RemoveDropItem();
}

int CDnDropItem::GetRoomRand( CMultiRoom* pRoom )
{
	return _roomrand(pRoom);
}

bool CDnDropItem::IsActive( CMultiRoom *pRoom ) 
{ 
	return STATIC_INSTANCE_( s_bActive ); 
}

bool CDnDropItem::InitializeClass( CMultiRoom *pRoom )
{
	if( !CDnWorld::IsActive(pRoom) ) return false;
	/*
	CDnWorldGrid *pGrid = (CDnWorldGrid*)CDnWorld::GetInstance(pRoom).GetGrid();
	float fSize = max( pGrid->GetGridX() * pGrid->GetGridWidth() * 100.f, pGrid->GetGridY() * pGrid->GetGridHeight() * 100.f );
	if( CDnWorld::IsActive(pRoom) ) {
		float fMaxMultiply = 0.f;
		for( DWORD i=0; i<CDnWorld::GetInstance(pRoom).GetGrid()->GetActiveSectorCount(); i++ ) {
			float fTemp = CDnWorld::GetInstance(pRoom).GetGrid()->GetActiveSector(i)->GetHeightMultiply();
			if( fTemp > fMaxMultiply ) fMaxMultiply = fTemp;
		}
		fSize = max( fSize, fMaxMultiply * 65535.f );
	}
	*/

	float fCenter, fSize;
	CDnWorld::GetInstance(pRoom).CalcWorldSize( fCenter, fSize );

	STATIC_INSTANCE_(s_bActive) = true;
	STATIC_INSTANCE_(s_dwUniqueCount) = 0;
	return true;
}

void CDnDropItem::ProcessClass( CMultiRoom *pRoom, LOCAL_TIME LocalTime, float fDelta )
{
	for( DWORD i=0; i<STATIC_INSTANCE_(s_pVecProcessList).size(); i++ ) {
		STATIC_INSTANCE_(s_pVecProcessList)[i]->SyncClassTime( LocalTime );
		STATIC_INSTANCE_(s_pVecProcessList)[i]->Process( LocalTime, fDelta );

		if( STATIC_INSTANCE_(s_pVecProcessList)[i]->IsDestroy() ) {
			STATIC_INSTANCE_(s_pVecProcessList)[i]->Release();
			i--;
		}
	}
}

void CDnDropItem::ReleaseClass( CMultiRoom *pRoom )
{
	STATIC_INSTANCE_(s_bActive) = false;
	SAFE_RELEASE_SPTRVEC( STATIC_INSTANCE_(s_hVecPreLoadItemList) );
	DeleteAllObject( pRoom );

	STATIC_INSTANCE_(s_pVecProcessList).clear();
}
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
DnDropItemHandle CDnDropItem::DropItem( CMultiRoom *pRoom, EtVector3 &vPos, DWORD dwUniqueID, int nItemID, int nSeed, char cOption, int nOverlapCount, int nRotate, UINT nOwnerUniqueID, int nEnchantID )
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
DnDropItemHandle CDnDropItem::DropItem( CMultiRoom *pRoom, EtVector3 &vPos, DWORD dwUniqueID, int nItemID, int nSeed, char cOption, int nOverlapCount, int nRotate, UINT nOwnerUniqueID )
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
{
	CDnDropItem *pDropItem = new CDnDropItem(pRoom,dwUniqueID,true);

	CTask *pTask = CTaskManager::GetInstance(pRoom).GetTask( "ItemTask" );
	pDropItem->InitializeRoom( pTask->GetRoom() );

	if( cOption == -1 ) {
		TItemData *pItemData = g_pDataManager->GetItemData(nItemID);
		if( pItemData ) {
			if ((pItemData->nType == ITEMTYPE_WEAPON) || (pItemData->nType == ITEMTYPE_PARTS)) {
				if( pItemData->nTypeParam[0] > 0 ) {
					TPotentialData *pPotential = g_pDataManager->GetPotentialData( pItemData->nTypeParam[0] );
					if( pPotential && pPotential->nTotalProb > 0 ) {
						int nRand = GetRoomRand(pRoom) % pPotential->nTotalProb;
						for (int i = 0; i < (int)pPotential->pVecItemData.size(); i++){
							if (nRand < pPotential->pVecItemData[i]->nPotentailOffset){
								cOption = i + 1;
								break;
							}
						}
					}
				}
			}
		}
	}
	if( cOption == -1 ) cOption = 0;
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	pDropItem->Initialize( vPos, nItemID, nSeed, cOption, nOverlapCount, nRotate, false, nEnchantID );
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	pDropItem->Initialize( vPos, nItemID, nSeed, cOption, nOverlapCount, nRotate );
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	pDropItem->SetOwnerUniqueID( nOwnerUniqueID );

	return pDropItem->GetDropItemHandle();
}

void CDnDropItem::CalcCoinLump( CMultiRoom *pRoom, int nCoin, int nMin, int nMax, std::vector<int> &nVecResult )
{
	int nResult = ( nMax - nMin ) + 1;
	if( nResult <= 0 ) nResult = 1;
	nResult = nMin + GetRoomRand(pRoom)%nResult;

	if( nCoin < nResult ) nResult = nCoin;

	int nRemainder = nCoin;
	int nValue;
	nVecResult.clear();
	for( int i=0; i<nResult; i++ ) {
		if( i == nResult - 1 ) nValue = nRemainder;
		else {
			nValue = ( GetRoomRand(pRoom) % nRemainder ) + 1;
			if( nValue > nRemainder ) nValue = nRemainder;
		}
		nRemainder -= nValue;

		nVecResult.push_back( nValue );
		if( nRemainder == 0 ) break;
	}
}

void CDnDropItem::CalcDropItemList( CMultiRoom *pRoom, TDUNGEONDIFFICULTY Difficulty, int nItemDropGroupTableID, DNVector(CDnItem::DropItemStruct) &VecList, bool bIncreaseUniqueID )
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEMDROPGROUP );
	std::vector<int> nVecList;
	pSox->GetItemIDListFromField( "_DropGroupID", nItemDropGroupTableID, nVecList );
	if( nVecList.empty() ) 
		return;

	if( Difficulty == Dungeon::Difficulty::Max ) 
	{
		CDnGameTask *pTask = (CDnGameTask *)CTaskManager::GetInstance(pRoom).GetTask( "GameTask" );
		if( !pTask ) 
			Difficulty = Dungeon::Difficulty::Easy;
		else 
			Difficulty = pTask->GetStageDifficulty();
	}

	for( DWORD i=0; i<nVecList.size(); i++ ) {
		int nItemID = nVecList[i];
		if( pSox->GetFieldFromLablePtr( nItemID, "_Difficulty" )->GetInteger() != Difficulty ) continue;
		INT64 nProb = (INT64)( pSox->GetFieldFromLablePtr( nItemID, "_Expectation" )->GetFloat() * 1000000000 );
		while( nProb > 0 ) {
			if( GetRoomRand(pRoom)%1000000000 < nProb ) {
				int nItemTableID = pSox->GetFieldFromLablePtr( nItemID, "_DropID" )->GetInteger();
 				if( nItemTableID < 1 ) continue;
				CDnDropItem::CalcDropItemList( pRoom, nItemTableID, VecList, bIncreaseUniqueID );
			}
			nProb -= 1000000000;
		}
	}
}

void CDnDropItem::CalcDropItemList( CMultiRoom *pRoom, int nItemDropTableID, DNVector(CDnItem::DropItemStruct) &VecList, bool bIncreaseUniqueID )
{
	DNTableFileFormat* pSox = pSox = GetDNTable( CDnTableDB::TITEMDROP );
	if( !pSox->IsExistItem( nItemDropTableID ) ) return;
	CDnItem::DropItemStruct Struct;

	// 돈 계산
	int nMin = pSox->GetFieldFromLablePtr( nItemDropTableID, "_GoldMin" )->GetInteger();
	int nMax = pSox->GetFieldFromLablePtr( nItemDropTableID, "_GoldMax" )->GetInteger();
	int nProb = pSox->GetFieldFromLablePtr( nItemDropTableID, "_GoldProb" )->GetInteger();
	if( GetRoomRand(pRoom)%1000000000 < nProb ) {
		int nLumpMin = pSox->GetFieldFromLablePtr( nItemDropTableID, "_GoldLumpMin" )->GetInteger();
		int nLumpMax = pSox->GetFieldFromLablePtr( nItemDropTableID, "_GoldLumpMax" )->GetInteger();
		int nAmount = ( nMax - nMin ) + 1;
		if( nAmount <= 0 ) nAmount = 1;
		int nCoin = nMin + GetRoomRand(pRoom)%nAmount;
		std::vector<int> nVecLumpCoin;
		CalcCoinLump( pRoom, nCoin, nLumpMin, nLumpMax, nVecLumpCoin );
		
		for( DWORD i=0; i<nVecLumpCoin.size(); i++ ) {
			Struct.nItemID = 0;
			Struct.nSeed = abs( GetRoomRand(pRoom) );
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
			Struct.nEnchantID = 0;
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
			Struct.nCount = nVecLumpCoin[i];
			Struct.dwUniqueID = ( bIncreaseUniqueID ) ? STATIC_INSTANCE_(CDnDropItem::s_dwUniqueCount)++ : 0;

			VecList.push_back( Struct );
		}
	}

	int nDepth = ITEMDROP_DEPTH;
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	CalcDropItems( pRoom, nItemDropTableID, VecList, nDepth, bIncreaseUniqueID, nItemDropTableID );
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	CalcDropItems( pRoom, nItemDropTableID, VecList, nDepth, bIncreaseUniqueID );
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	/*
	// 아이템 계산
	int nCompoundItemCount = pSox->GetFieldFromLablePtr( nItemDropTableID, "_ItemMaxCompound" )->GetInteger();
	std::vector<int> nVecItemIndex;

	// 테이블값이 들어있는 인덱스값들을 먼저 얻고
	char szLabel[64];
	for( DWORD i=0; i<20; i++ ) {
		sprintf_s( szLabel, "_Item%dIndex", i + 1 );
		if( pSox->GetFieldFromLablePtr( nItemDropTableID, szLabel )->GetInteger() == 0 ) continue;
		nVecItemIndex.push_back( (int)i + 1 );
	}

	// 계산
	int nDropItemID;
	std::vector<int> nVecRandomArray;
	int nValue;
	while( !nVecItemIndex.empty() ) {
		nValue = _rand(pRoom)%(int)nVecItemIndex.size();
		nVecRandomArray.push_back( nVecItemIndex[nValue] );
		nVecItemIndex.erase( nVecItemIndex.begin() + nValue );
	}
	nVecItemIndex = nVecRandomArray;
	for( DWORD i=0; i<nVecItemIndex.size(); i++ ) {
		sprintf_s( szLabel, "_Item%dIndex", nVecItemIndex[i] );
		nDropItemID = pSox->GetFieldFromLablePtr( nItemDropTableID, szLabel )->GetInteger();

		sprintf_s( szLabel, "_Item%dProb", nVecItemIndex[i] );
		nProb = (int)( pSox->GetFieldFromLablePtr( nItemDropTableID, szLabel )->GetFloat() * 1000000000.0 );
		if( _rand(pRoom)%1000000000 >= nProb ) continue;

		sprintf_s( szLabel, "_Item%dCountMin", nVecItemIndex[i] );
		nMin = pSox->GetFieldFromLablePtr( nItemDropTableID, szLabel )->GetInteger();

		sprintf_s( szLabel, "_Item%dCountMax", nVecItemIndex[i] );
		nMax = pSox->GetFieldFromLablePtr( nItemDropTableID, szLabel )->GetInteger();

		if( nMin == 0 && nMax == 0 ) continue;
		if( nMin > nMax ) continue;

		int nAmount = ( nMax - nMin ) + 1;
		if( nAmount <= 0 ) nAmount = 1;

		Struct.nItemID = nDropItemID;
		Struct.nSeed = abs( _rand(pRoom) );
		Struct.nCount = nMin + _rand(pRoom)%nAmount;

		Struct.dwUniqueID = STATIC_INSTANCE_(CDnDropItem::s_dwUniqueCount)++;

		VecList.push_back( Struct );

		nCompoundItemCount--;
		if( nCompoundItemCount <= 0 ) break;
	}
	*/
}

namespace DropItemNameSpace {
	struct DropTempStruct {
		bool bIsGroup;
		int nItemID;
		int nCount;
		int nOffset;
	};
}
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
void CDnDropItem::CalcDropItems( CMultiRoom *pRoom, int nDropItemTableID, DNVector(CDnItem::DropItemStruct) &VecResult, int &nDepth, bool bIncreaseUniqueID, int nBaseDropItemTableID )
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
void CDnDropItem::CalcDropItems( CMultiRoom *pRoom, int nDropItemTableID, DNVector(CDnItem::DropItemStruct) &VecResult, int &nDepth, bool bIncreaseUniqueID )
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
{
	nDepth -= 1;
	if( nDepth < 0 ) return;
	using namespace DropItemNameSpace;
 
	// 테이블값이 들어있는 인덱스값들을 먼저 얻고
	DNTableFileFormat* pSox = pSox = GetDNTable( CDnTableDB::TITEMDROP );
	if( !pSox->IsExistItem( nDropItemTableID ) ) 
	{
		return;
	}

	DNVector(DropTempStruct) VecList;

	char szLabel[64];
	int nOffset = 0;
	DropTempStruct Struct;
	for( DWORD i=0; i<20; i++ ) {
		sprintf_s( szLabel, "_IsGroup%d", i + 1 );
		bool bGroup = ( pSox->GetFieldFromLablePtr( nDropItemTableID, szLabel )->GetInteger() == 1 ) ? true : false;

		sprintf_s( szLabel, "_Item%dIndex", i + 1 );
		int nIndex = pSox->GetFieldFromLablePtr( nDropItemTableID, szLabel )->GetInteger();
		if( nIndex < 1 ) continue;

		sprintf_s( szLabel, "_Item%dProb", i + 1 );
		int nProb = pSox->GetFieldFromLablePtr( nDropItemTableID, szLabel )->GetInteger();
		if( nProb <= 0 ) continue;

		sprintf_s( szLabel, "_Item%dInfo", i + 1 );
		int nInfo = pSox->GetFieldFromLablePtr( nDropItemTableID, szLabel )->GetInteger();
		if( !bGroup ) {
			if( nInfo < 1 ) continue;
			if( !g_pDataManager->GetItemData( nIndex ) ) continue;
		}

		nOffset += nProb;

		Struct.bIsGroup = bGroup;
		Struct.nItemID = nIndex;
		Struct.nCount = nInfo;
		Struct.nOffset = nOffset;
		VecList.push_back( Struct );
	}
	int nSeed = GetRoomRand(pRoom)%1000000000;

	int nPrevOffset = 0;
	for( DWORD i=0; i<VecList.size(); i++ ) {
		if( nSeed >= nPrevOffset && nSeed < VecList[i].nOffset ) {
			if( VecList[i].bIsGroup ) 
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
				CalcDropItems( pRoom, VecList[i].nItemID, VecResult, nDepth, bIncreaseUniqueID, nBaseDropItemTableID );
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
				CalcDropItems( pRoom, VecList[i].nItemID, VecResult, nDepth, bIncreaseUniqueID );
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
			else {
				CDnItem::DropItemStruct Result;
				Result.nItemID = VecList[i].nItemID;
				Result.nSeed = abs( GetRoomRand(pRoom) );
				Result.nCount = VecList[i].nCount;
				Result.cOption = 0;
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
				Result.nEnchantID = 0;
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
				TItemData *pItemData = g_pDataManager->GetItemData(Result.nItemID);
				if( pItemData ) {
					if ((pItemData->nType == ITEMTYPE_WEAPON) || (pItemData->nType == ITEMTYPE_PARTS)) {
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
						Result.nEnchantID = CDnDropItem::GetDropItemEnchantID(pRoom, nBaseDropItemTableID);
						if(Result.nEnchantID < 0)
						{
							g_Log.Log(LogType::_ERROR, L"[CalcDropItems] Get DropItemEnchantID Error(TableID[%d])", nBaseDropItemTableID);
							Result.nEnchantID = 0;
						}
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
						if( pItemData->nTypeParam[0] > 0 ) {
							TPotentialData *pPotential = g_pDataManager->GetPotentialData( pItemData->nTypeParam[0] );
							if( pPotential && pPotential->nTotalProb > 0 ) {
								int nRand = Result.nSeed % pPotential->nTotalProb;
								for (int j = 0; j < (int)pPotential->pVecItemData.size(); j++){
									if (nRand < pPotential->pVecItemData[j]->nPotentailOffset){
										Result.cOption = j + 1;
										break;
									}
								}
							}
						}
					}
				}

				Result.dwUniqueID = ( bIncreaseUniqueID ) ? STATIC_INSTANCE_(CDnDropItem::s_dwUniqueCount)++ : 0;

				VecResult.push_back( Result );

			}
			break;
		}
		nPrevOffset = VecList[i].nOffset;
	}
}
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
bool CDnDropItem::PreInitializeItem( CMultiRoom *pRoom, int nItemID, int nEnchantID )
{
	for( DWORD i=0; i<STATIC_INSTANCE_(s_hVecPreLoadItemList).size(); i++ ) {
		if( STATIC_INSTANCE_(s_hVecPreLoadItemList)[i]->GetItemID() == nItemID ) return true;
	}

	CDnDropItem *pDropItem = new CDnDropItem( pRoom, -1, false );
	if( pDropItem->Initialize( EtVector3( 0.f, 0.f, 0.f ), nItemID, 0, 0, 1, 0, true, nEnchantID ) == false ) {
		SAFE_DELETE( pDropItem );
		return false;
	}

	STATIC_INSTANCE_(s_hVecPreLoadItemList).push_back( pDropItem->GetMySmartPtr() );

	return true;
}
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
bool CDnDropItem::PreInitializeItem( CMultiRoom *pRoom, int nItemID )
{
	for( DWORD i=0; i<STATIC_INSTANCE_(s_hVecPreLoadItemList).size(); i++ ) {
		if( STATIC_INSTANCE_(s_hVecPreLoadItemList)[i]->GetItemID() == nItemID ) return true;
	}

	CDnDropItem *pDropItem = new CDnDropItem( pRoom, -1, false );
	if( pDropItem->Initialize( EtVector3( 0.f, 0.f, 0.f ), nItemID, 0, 0, 1, 0, true ) == false ) {
		SAFE_DELETE( pDropItem );
		return false;
	}
	STATIC_INSTANCE_(s_hVecPreLoadItemList).push_back( pDropItem->GetMySmartPtr() );

	return true;
}
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)

#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
bool CDnDropItem::Initialize( EtVector3 &vPos, int nItemID, int nRandomSeed, char cOption, int nCount, int nRotate, bool bPreInitialize, int nEnchantID )
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
bool CDnDropItem::Initialize( EtVector3 &vPos, int nItemID, int nRandomSeed, char cOption, int nCount, int nRotate, bool bPreInitialize )
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
{
	TItemData *pData = g_pDataManager->GetItemData( nItemID );
	if( nItemID > 0 && !pData ) return false;

	m_nOverlapCount			= nCount;
	m_nItemID				= nItemID;
	m_nRandomSeed			= nRandomSeed;
	m_cOption				= cOption;
	m_Cross.m_vPosition		= vPos;
	m_nRotate				= nRotate;
	m_Rank = ITEMRANK_D;
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	m_nEnchantID = nEnchantID;
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	if( pData )
		m_Rank = (eItemRank)pData->cRank;

	if( !bPreInitialize ) {
		m_Cross.RotateYaw( (float)nRotate );
		m_fDistance = 50.f + ( 10 * ( m_nRandomSeed % 11 ) );
		int nIndex = GetElementIndex( "Disappear" );
		if( nIndex != -1 ) {
			m_fDisappearTime = GetElement( nIndex )->dwLength / 60.f;
		}
		else m_bDisappear = true;
	}

	return true;
}

void CDnDropItem::Process( LOCAL_TIME LocalTime, float fDelta )
{
	float fPrevFrame = m_fPrevFrame;
	CDnActionBase::ProcessAction( LocalTime, fDelta );

	if( m_fDistance > 0.f ) 
	{
		float fValue = 100.f * fDelta;
		m_fDistance -= fValue;
		if( m_fDistance <= 0.f ) {
			fValue += m_fDistance;
			m_fDistance = 0.f;
		}

		if( GetGameRoom()->bIsPvPRoom() )
			fValue = 0.f;

		NavigationMesh *pNavMesh = INSTANCE(CDnWorld).GetNavMesh( m_Cross.m_vPosition );
		if( !pNavMesh ) {
			ProcessAttributeMovement( fValue );
		}
		else {
			if( m_pCurCell == NULL ) {
				m_pCurCell = pNavMesh->FindClosestCell( m_Cross.m_vPosition );
				if( ( m_pCurCell == NULL ) || ( !m_pCurCell->IsPointInCellCollumn( m_Cross.m_vPosition ) ) ) {
					m_pCurCell = NULL;
					ProcessAttributeMovement( fValue );
				}
			}
			if( m_pCurCell ) {
				EtVector3 vPrevPos = m_Cross.m_vPosition;
				m_Cross.MoveLocalZAxis( fValue );
				int nSide = -1;
				NavigationCell *pLastCell = NULL;
				m_pCurCell->FindLastCollision( vPrevPos, m_Cross.m_vPosition, &pLastCell, nSide );
				if( nSide != -1 )
				{
					if( pLastCell->Link( nSide ) == NULL )
					{
						EtVector2 vMoveDir2D( m_Cross.m_vPosition.x - vPrevPos.x, m_Cross.m_vPosition.z - vPrevPos.z );
						float fMoveLength = EtVec2Length( &vMoveDir2D );
						vMoveDir2D /= fMoveLength;
						EtVector2 vWallDir2D = pLastCell->Side( nSide )->EndPointB() - pLastCell->Side( nSide )->EndPointA();
						EtVec2Normalize( &vWallDir2D, &vWallDir2D );
						fMoveLength *= EtVec2Dot( &vWallDir2D, &vMoveDir2D );
						m_Cross.m_vPosition.x = vPrevPos.x + fMoveLength * vWallDir2D.x;
						m_Cross.m_vPosition.z = vPrevPos.z + fMoveLength * vWallDir2D.y;

						int nNewSide = -1;
						m_pCurCell->FindLastCollision( vPrevPos, m_Cross.m_vPosition, &pLastCell, nNewSide );
						if( nNewSide != -1 )
						{
							if( pLastCell->Link( nNewSide ) )
							{
								NavigationCell *pNewCell;
								pNewCell = pLastCell->Link( nNewSide );
								if( pNewCell->IsPointInCellCollumn( m_Cross.m_vPosition ) )
								{
									m_pCurCell = pNewCell;
								}
								else
								{
									m_Cross.m_vPosition.x = vPrevPos.x;
									m_Cross.m_vPosition.z = vPrevPos.z;
								}
							}
							else if( !m_pCurCell->IsPointInCellCollumn( m_Cross.m_vPosition ) )
							{
								m_Cross.m_vPosition.x = vPrevPos.x;
								m_Cross.m_vPosition.z = vPrevPos.z;
							}
						}
					}
					else
					{
						m_pCurCell = pLastCell->Link( nSide );
					}
				}

				if( m_pCurCell->GetType() == NavigationCell::CT_PROP )
					m_Cross.m_vPosition.y = m_pCurCell->GetPlane()->SolveForY( m_Cross.m_vPosition.x, m_Cross.m_vPosition.z );
				else
					m_Cross.m_vPosition.y = CDnWorld::GetInstance(GetRoom()).GetHeight( m_Cross.m_vPosition );
			}

		}
	}

	ProcessLifeTime( LocalTime, fDelta );
}

void CDnDropItem::ProcessAttributeMovement( float fDistance )
{
	EtVector3 vPrevPos = m_Cross.m_vPosition;
	m_Cross.MoveLocalZAxis( fDistance );
	if( CDnWorld::GetInstance(GetRoom()).GetAttribute( m_Cross.m_vPosition ) != 0 ) {
		m_Cross.m_vPosition = vPrevPos;
	}
	m_Cross.m_vPosition.y = CDnWorld::GetInstance(GetRoom()).GetHeight( m_Cross.m_vPosition );
}

void CDnDropItem::ProcessLifeTime( LOCAL_TIME LocalTime, float fDelta )
{
	if( GetGameRoom()->GetGameTaskType() == GameTaskType::Farm )
		return;
	if( m_fLifeTime <= 0.f ) return;
	if( m_Rank >= ITEMRANK_B ) return;
	if(m_bReversionLock) return;

	m_fLifeTime -= fDelta;
	if( !m_bDisappear && m_fLifeTime <= m_fDisappearTime ) {
		m_bDisappear = true;
		SetActionQueue( "Disappear", 0, 20.f );
	}
	if( m_fLifeTime <= 0.f ) SetDestroy();
}

void CDnDropItem::SyncClassTime( LOCAL_TIME LocalTime )
{
	CDnActionBase::m_LocalTime = LocalTime;
}

void CDnDropItem::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{
	switch( Type ) {
		case STE_Destroy:
			SetDestroy();
			break;
	}
}

void CDnDropItem::GetBoundingSphere( SSphere &Sphere, bool bActorSize/* = false*/ )
{
	Sphere.Center = m_Cross.m_vPosition;
	Sphere.fRadius = 30.f;
}

void CDnDropItem::InsertDropItem()
{
	if( GetRoom() )
		static_cast<CDNGameRoom*>(GetRoom())->AddDropItem( m_dwUniqueID, GetMySmartPtr() );
}

void CDnDropItem::RemoveDropItem()
{
	if( GetRoom() )
		static_cast<CDNGameRoom*>(GetRoom())->EraseDropItem( GetUniqueID() );
}

void CDnDropItem::Send( DWORD dwProtocol, DnActorHandle hActor, CMemoryStream *pStream, GlobalEnum::ClientSessionTypeEnum SessionType )
{
	DWORD dwUniqueID = GetUniqueID();

	BYTE *pBuffer = ( pStream ) ? (BYTE*)pStream->GetBuffer() : NULL;
	int nSize = ( pStream ) ? pStream->Tell() : 0;

	if( m_pRoom )
	{
		for( DWORD i=0; i<m_pRoom->GetUserCount(); i++ ) {
			CDNUserSession *pStruct = m_pRoom->GetUserData(i);
			if( pStruct->GetSessionID() == hActor->GetUniqueID() ) {
				SendGameDropItemMsg( pStruct, dwUniqueID, (USHORT)dwProtocol, pBuffer, nSize );
				break;
			}
		}
	}
}

void CDnDropItem::Send( DWORD dwProtocol, CMemoryStream *pStream, GlobalEnum::ClientSessionTypeEnum SessionType, bool bImmediate )
{
	DWORD dwUniqueID = GetUniqueID();

	BYTE *pBuffer = ( pStream ) ? (BYTE*)pStream->GetBuffer() : NULL;
	int nSize = ( pStream ) ? pStream->Tell() : 0;

	if( m_pRoom )
	{
		for( DWORD i=0; i<m_pRoom->GetUserCount(); i++ ) {
			CDNUserSession *pStruct = m_pRoom->GetUserData(i);
			SendGameDropItemMsg( pStruct, dwUniqueID, (USHORT)dwProtocol, pBuffer, nSize );
		}
	}
}

void CDnDropItem::SetReversionItem(bool bSet)
{
	m_bReversionItem = bSet;
}

void CDnDropItem::LockReversionItem(bool bLock)
{
	m_bReversionLock = bLock;
}

void CDnDropItem::MakeItemStruct( TItem &ItemInfo )
{
	ItemInfo.nItemID = m_nItemID;
	ItemInfo.nRandomSeed = m_nRandomSeed;
	ItemInfo.cOption = m_cOption;
	ItemInfo.wCount = m_nOverlapCount;
}
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
int CDnDropItem::GetDropItemEnchantID( CMultiRoom *pRoom, int nItemDropTableID)
{
	if(!pRoom) 
		return 0;

	if(nItemDropTableID <= 0)
		return 0;

	TItemDropEnchantData* pItemDropEnchantData = g_pDataManager->GetDropItemEnchant(nItemDropTableID);
	if(!pItemDropEnchantData) return 0;

	int nTotalProb = 0;
	std::vector<int> nVecOffset;

	for(int i = 0; i < pItemDropEnchantData->nEnchantCount;i++)
	{
		nVecOffset.push_back(nTotalProb + pItemDropEnchantData->nEnchantProb[i]);
		nTotalProb = nTotalProb + pItemDropEnchantData->nEnchantProb[i];
	}
	int nSeed = GetRoomRand(pRoom)%nTotalProb;

	for(int i=0; i <pItemDropEnchantData->nEnchantCount; i++)
	{
		if(nSeed < nVecOffset[i])
		{
			return pItemDropEnchantData->nEnchantOption[i];
		}
	}
	return 0;
}
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)