#include "Stdafx.h"
#include "DNGuildWare.h"
#include "DNGuildVillage.h"

#include "DNUserSession.h"

#include "DNGameDataManager.h"
#include "DNDBConnection.h"
#include "DNDBConnectionManager.h"
#include "DNMasterConnection.h"
#include "DNAuthManager.h"

#include "Timeset.h"

extern TVillageConfig g_Config;
extern CDNDBConnectionManager* g_pDBConnectionManager;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////						PUBLIC FUNCTION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDNGuildWare::CDNGuildWare()
{
	Reset();
}

CDNGuildWare::~CDNGuildWare()
{
	Reset();
}

void CDNGuildWare::Open(CDNGuildVillage* pGuildVillage, const TAGetGuildWareInfo* pPacket)
{
	// DB객체 얻어오기
	if(!m_pDBCon)
		m_pDBCon = g_pDBConnectionManager->GetDBConnection( m_cDBThreadID );
	
	// 상위 객체 관리
	m_GuildOwner = pGuildVillage;

	// 창고 아이템 리스트 저장(시리얼별로 저장한다)
	m_MapWareItem.clear();
	
	INT64 nSerial = 0;
	for(int i=0; i<pPacket->cItemListCount; i++)
	{
		nSerial = pPacket->ItemList[i].Item.nSerial;
		m_MapWareItem [nSerial] = pPacket->ItemList[i];
	}

	// 슬롯버젼 초기화
	UpdateVersionDate();

	// 최신 웨어슬롯 리스트를 마스터를 통해 각 빌리지서버에 요청한다.
	g_pMasterConnection->SendGuildWareInfo(m_GuildOwner->GetUID());
}

void CDNGuildWare::ReportWareSlot()
{
	// 창고 리스트를 DB에 저장한다.
	if(m_pDBCon)
	{
		TGuildWareSlot WareSlotList[GUILD_WAREHOUSE_MAX];
		BYTE cSlotListCount = GetSlotList(WareSlotList);
		m_pDBCon->QuerySetGuildWareInfo(m_cDBThreadID, m_GuildOwner->GetUID().nDBID, g_Config.nWorldSetID, cSlotListCount, WareSlotList);
	}
	else
		_DANGER_POINT();
}

// FROM USER
// 길드아이템
bool CDNGuildWare::OnRecvMoveGuildItem(CDNUserSession* pUserSession, CSMoveGuildItem* pPacket)
{
	int nRet = ERROR_NONE;

	switch(pPacket->cMoveType)
	{
	case MoveType_GuildWare:
		nRet = ItemInWare(pUserSession, pPacket); 
		break;

	case MoveType_InvenToGuildWare:
		nRet = ItemInvenToWare(pUserSession, pPacket);
		break;

	case MoveType_GuildWareToInven:
		nRet = ItemWareToInven(pUserSession, pPacket);
		break;

	default:
		_DANGER_POINT();
		break;
	}

	// 에러처리
	if(nRet != ERROR_NONE)
	{
		pUserSession->SendMoveGuildItem(pPacket->cMoveType, 0, 0, NULL, NULL, nRet);
		return false;
	}
	
	return true;
}

// 길드코인 
bool CDNGuildWare::OnRecvMoveGuildCoin(CDNUserSession* pUserSession, CSMoveGuildCoin* pMove)
{
	int nRet = ERROR_NONE;

	switch(pMove->cMoveType)
	{
	case MoveType_InvenToGuildWare:
		nRet = CoinInvenToWare(pUserSession, pMove);
		break;

	case MoveType_GuildWareToInven:
		nRet = CoinWareToInven(pUserSession, pMove);
		break;

	default:
		_DANGER_POINT();
		break;
	}

	if(nRet != ERROR_NONE)
	{
		pUserSession->SendMoveGuildCoin(pMove->cMoveType, 0, 0, 0, 0, nRet);
		return false;
	}

	return true;
}

// 길드창고 히스토리
void CDNGuildWare::OnRecvGetWareHistory(CDNUserSession* pUserSession, CSGetGuildWareHistory* pPacket)
{
	// DB에 히스토리 목록을 얻어온다
	if(m_pDBCon)
		m_pDBCon->QueryGetGuildWareHistory(m_cDBThreadID, pUserSession->GetAccountDBID(), pUserSession->GetCharacterDBID(), m_GuildOwner->GetUID().nDBID, g_Config.nWorldSetID, pPacket->nIndex);
}

//FROM MASTER
bool CDNGuildWare::OnRecvGuildWareInfo(const MAGuildWareInfo *pPacket)
{
	MAGuildWareInfoResult result;
	memset(&result, 0, sizeof(result));

	result.nFromManagedID = pPacket->nManagedID;
	SaveWareResultPacket(result);

	g_pMasterConnection->SendGuildWareInfoResult(result);
	return true;
}

bool CDNGuildWare::OnRecvGuildWareInfoResult(MAGuildWareInfoResult *pPacket)
{
	UpdateSlotList(pPacket->VersionDate, pPacket->cSlotListCount, pPacket->WareSlotList);
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////						PRIVATE FUNCTION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CDNGuildWare::Reset()
{
	m_MapWareItem.clear();
	m_pDBCon = NULL;
	m_GuildOwner = NULL;
	m_VersionDate = 0;
	m_cDBThreadID = 0;
}

BYTE CDNGuildWare::GetSlotList(TGuildWareSlot* pSlot)
{
	BYTE cListCount = 0;

	// 초기화
	memset(pSlot, 0x00, sizeof(TGuildWareSlot)*GUILD_WAREHOUSE_MAX);

	ScopeLock<CSyncLock> AutoLock(m_WareLock);

	TMapGuildWareItem::const_iterator con_iter = m_MapWareItem.begin();

	for(; con_iter != m_MapWareItem.end(); con_iter++)
	{
		pSlot[cListCount].cSlotIndex = con_iter->second.cSlotIndex;
		pSlot[cListCount].nSerial = con_iter->second.Item.nSerial;
		
		cListCount++;
		if(cListCount >= GUILD_WAREHOUSE_MAX)
			break;
	}

	return cListCount;
}

void CDNGuildWare::UpdateWareSlot(BYTE cCount, TGuildWareSlot* List)
{
	if (cCount >  GUILD_WAREHOUSE_MAX)
		return;

	TMapGuildWareItem::iterator iter = m_MapWareItem.begin();

	for(; iter != m_MapWareItem.end(); iter++)
	{
		for(BYTE i=0; i<cCount; i++)
		{
			if(iter->second.Item.nSerial == List[i].nSerial)
			{
				iter->second.cSlotIndex = List[i].cSlotIndex;
			}
		}
	}
}

size_t CDNGuildWare::GetWareItemCount()
{
	return m_MapWareItem.size();
}

__time64_t CDNGuildWare::GetVersionDate()
{
	return m_VersionDate;
}

void CDNGuildWare::UpdateVersionDate()
{
	m_TimeSet.Reset();
	m_VersionDate = m_TimeSet.GetTimeT64_LC();
}

void CDNGuildWare::SetVersionDate(__time64_t tVersion)
{
	m_VersionDate = tVersion;
}

BYTE CDNGuildWare::GetWareList(TItemInfo* pItemInfo)
{
	BYTE cListCount = 0;
	memset(pItemInfo, 0x00, sizeof(TItemInfo)*GUILD_WAREHOUSE_MAX);

	ScopeLock<CSyncLock> AutoLock(m_WareLock);

	TMapGuildWareItem::const_iterator con_iter = m_MapWareItem.begin();

	for(; con_iter != m_MapWareItem.end(); con_iter++)
	{
		if(cListCount >= GUILD_WAREHOUSE_MAX)
		{
			if (m_GuildOwner)
				g_Log.Log(LogType::_ERROR, L"[World:%d][DBID:%d] GetWareList - OverFlow\r\n", m_GuildOwner->GetUID().nWorldID, m_GuildOwner->GetUID().nDBID);
			break;
		}

		pItemInfo[cListCount] = con_iter->second;
		cListCount++;
	}

	return cListCount;
}

const TItem* CDNGuildWare::GetWareItem(INT64 biItemSerial)
{
	if(biItemSerial <= 0) 
		return NULL;

	if(m_MapWareItem.empty()) 
		return NULL;

	ScopeLock<CSyncLock> AutoLock(m_WareLock);

	TMapGuildWareItem::iterator iter = m_MapWareItem.find(biItemSerial);
	if(iter == m_MapWareItem.end())
		return NULL;

	return &(iter->second.Item);
}

const TItemInfo* CDNGuildWare::GetWareItemInfo(INT64 biItemSerial)
{
	if(biItemSerial <= 0) 
		return NULL;

	if(m_MapWareItem.empty()) 
		return NULL;

	ScopeLock<CSyncLock> AutoLock(m_WareLock);

	TMapGuildWareItem::iterator iter = m_MapWareItem.find(biItemSerial);
	if(iter == m_MapWareItem.end())
		return NULL;

	return &(iter->second);
}

int CDNGuildWare::AddWareItemInfo(TItemInfo& AddItem)
{
	ScopeLock<CSyncLock> AutoLock(m_WareLock);
	INT64 nSerial = AddItem.Item.nSerial;
	m_MapWareItem [nSerial] = AddItem;

	if (m_MapWareItem.size() > GUILD_WAREHOUSE_MAX)
	{
		if (m_GuildOwner)
			g_Log.Log(LogType::_ERROR, L"[World:%d][DBID:%d] AddWareItemInfo - OverFlow\r\n", m_GuildOwner->GetUID().nWorldID, m_GuildOwner->GetUID().nDBID);
	}

	return ERROR_NONE;
}

void CDNGuildWare::UpdateWareInfo(TItemInfo& ItemInfo)
{
	INT64 nUpdateSerial = ItemInfo.Item.nSerial;
	const TItemInfo* WareInfo = GetWareItemInfo(nUpdateSerial);

	if(WareInfo)
	{
		if(ItemInfo.Item.wCount <= 0)
		{
			// 정보 삭제
			RemoveWareItem(nUpdateSerial);
		}
		else
		{
			// 업데이트
			ScopeLock<CSyncLock> AutoLock(m_WareLock);
			*const_cast<TItemInfo*>(WareInfo) = ItemInfo;
		}
	}
	else
		AddWareItemInfo(ItemInfo);
}

int CDNGuildWare::RemoveWareItem(INT64 nSerial)
{
	ScopeLock<CSyncLock> AutoLock(m_WareLock);

	m_MapWareItem.erase(nSerial);

	return ERROR_NONE;
}

#ifdef PRE_ADD_JOINGUILD_SUPPORT
void CDNGuildWare::BroadCastRefreshGuildItem(char cType, INT64 biSrcSerial, INT64 biDestSerial, const TItemInfo *pSrcInfo, const TItemInfo *pDestInfo, CDNUserSession* pSession, bool bNoUpdate)
{
	UpdateVersionDate(); // 창고버젼을 업데이트 한다.

	// 자신을 제외한 다른 길드원 사람들에게 알려주면서 개인 창곱버젼을 업데이트한다.
	m_GuildOwner->SendRefreshGuildItem(cType, biSrcSerial, biDestSerial, pSrcInfo, pDestInfo, pSession, bNoUpdate);

	g_pMasterConnection->SendRefreshGuildItem(m_GuildOwner->GetUID(), cType, biSrcSerial, biDestSerial, pSrcInfo, pDestInfo, GetVersionDate());
}
#else		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
void CDNGuildWare::BroadCastRefreshGuildItem(char cType, INT64 biSrcSerial, INT64 biDestSerial, const TItemInfo *pSrcInfo, const TItemInfo *pDestInfo, CDNUserSession* pSession)
{
	UpdateVersionDate(); // 창고버젼을 업데이트 한다.

	// 자신을 제외한 다른 길드원 사람들에게 알려주면서 개인 창곱버젼을 업데이트한다.
	m_GuildOwner->SendRefreshGuildItem(cType, biSrcSerial, biDestSerial, pSrcInfo, pDestInfo, pSession);

	g_pMasterConnection->SendRefreshGuildItem(m_GuildOwner->GetUID(), cType, biSrcSerial, biDestSerial, pSrcInfo, pDestInfo, GetVersionDate());
}
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT

void CDNGuildWare::BroadCastRefreshGuildCoin(INT64 biTotalCoin, CDNUserSession* pSession)
{
	m_GuildOwner->SendRefreshGuildCoin(biTotalCoin, pSession);

	g_pMasterConnection->SendRefreshGuildCoin(m_GuildOwner->GetUID(), biTotalCoin);
}

// 창고안에서만 아이템 이동
int CDNGuildWare::ItemInWare(CDNUserSession* pUserSession, CSMoveGuildItem* pPacket)
{
	if(!m_pDBCon) return ERROR_DB;

	if(!pUserSession)
		return ERROR_GENERIC_INVALIDREQUEST;

	CDNUserItem* pItem = pUserSession->GetItem();
	if(!pItem)
		return ERROR_ITEM_FAIL;

	if(pPacket->wCount <= 0)
		return ERROR_ITEM_NOTFOUND;

	if(pPacket->cDestIndex == pPacket->cSrcIndex)
		return ERROR_ITEM_INDEX_UNMATCH;

	int nGstorage = m_GuildOwner->GetInfo()->wGuildWareSize;

	if( (pPacket->cSrcIndex >= nGstorage) ) 
		return ERROR_ITEM_INDEX_UNMATCH;

	if( (pPacket->cDestIndex >= nGstorage) )
		return ERROR_ITEM_INDEX_UNMATCH;

	const TItemInfo* pSrcInfo = GetWareItemInfo(pPacket->biSrcItemSerial);
	if(!pSrcInfo) 
		return ERROR_ITEM_NOTFOUND;

	const TItem* pSrcItem = &pSrcInfo->Item;
	if(!pSrcItem)
		return ERROR_ITEM_NOTFOUND;

	// 가지고있는것보다많은값이오면에러!
	if(pPacket->wCount > pSrcItem->wCount)
		return ERROR_ITEM_OVERFLOW;

	// 시리얼코드조사
	if(pSrcItem->nSerial != pPacket->biSrcItemSerial) 
		return ERROR_ITEM_NOTFOUND;

	// 오버랩가능갯수확인
	int nSrcWareOverlapCount = g_pDataManager->GetItemOverlapCount(pSrcItem->nItemID);
	if(nSrcWareOverlapCount <= 0) return ERROR_ITEM_FAIL;

	// 길드정보를한번더확인!
	const TGuildUID GuildUID = pUserSession->GetGuildUID();
	if(!GuildUID.IsSet())
		return ERROR_GUILD_YOUR_NOT_BELONGANYGUILD;

	if(GuildUID != m_GuildOwner->GetUID())
		return ERROR_GUILD_YOUR_NOT_BELONGANYGUILD;

	// 길드원 확인
	TGuildMember* pGuildMember = m_GuildOwner->GetMemberInfo(pUserSession->GetCharacterDBID());
	if(!pGuildMember)
		return ERROR_GUILD_YOUR_NOT_BELONGANYGUILD;

	// 아이템 보관 권한이 있는지 확인
	if(!m_GuildOwner->CheckGuildInfoAuth(static_cast<eGuildRoleType>(pGuildMember->btGuildRole), static_cast<eGuildAuthType>(GUILDAUTH_TYPE_TAKEITEM)))
		return ERROR_GUILD_HAS_NO_AUTHORITY;

	const TItemInfo* pDestInfo = GetWareItemInfo(pPacket->biDestItemSerial);
	const TItem* pDestItem = NULL;
	if(pDestInfo)
		pDestItem = &pDestInfo->Item;

	if(pDestItem)
	{
		if( CDNUserItem::bIsDifferentItem( pSrcItem, pDestItem) )
			return pItem->NothingMoveItemInGuildWare(this, pPacket);

		// 소스아이템이 목적아이템과 Merge 가능한지 확인
		int nDestWareOverlapCount = g_pDataManager->GetItemOverlapCount(pDestItem->nItemID);
		if(nDestWareOverlapCount <= 1)
			return pItem->NothingMoveItemInGuildWare(this, pPacket);
		if(pDestItem->wCount == nDestWareOverlapCount)
			return pItem->NothingMoveItemInGuildWare(this, pPacket);

		//DB에 전송할 패킷
		TQMoveItemInGuildWare Move;
		memset(&Move, 0, sizeof(TQMoveItemInGuildWare));

		Move.nCharacterDBID		= pUserSession->GetCharacterDBID();
		Move.nGuildDBID			= GuildUID.nDBID;

		Move.SrcItem.cSlotIndex = pPacket->cSrcIndex;
		Move.SrcItem.nItemID	= pSrcItem->nItemID;
		Move.SrcItem.biSerial	= pSrcItem->nSerial;

		Move.DestItem.cSlotIndex = pPacket->cDestIndex;
		Move.DestItem.nItemID	= pDestItem->nItemID;
		Move.DestItem.biSerial	= pDestItem->nSerial;

		Move.nItemID			= pSrcItem->nItemID;
		Move.biItemSerial		= pSrcItem->nSerial;
		Move.wItemCount			= pSrcItem->wCount;
		Move.nMapIndex			= pUserSession->GetMapIndex();

		// 창고는 split이 없음. merge만 신경쓰면 됨

		// MAX값을 넘긴 경우
		if(pDestItem->wCount + pPacket->wCount > nDestWareOverlapCount)
		{
			int nGapCount = nDestWareOverlapCount - pDestItem->wCount;
			// 소스아이템을 강제로 분할하여 보내준다.
			Move.wSplitItemCount = nGapCount;
			Move.biNewItemSerial = MakeItemSerial();

			Move.SrcItem.wCount = pSrcItem->wCount - nGapCount;
			Move.DestItem.wCount = pDestItem->wCount + nGapCount;
		}
		else{
			if(pSrcItem->wCount == pPacket->wCount){	// 같은경우
				Move.SrcItem.wCount = 0;
				Move.SrcItem.biNewSerial = 0;
			}
			else{
				Move.SrcItem.wCount = pSrcItem->wCount - pPacket->wCount;
			}

			Move.DestItem.wCount = pDestItem->wCount + pPacket->wCount;
		}

		Move.biMergeTargetItemSerial = pDestItem->nSerial;
		Move.wMergeItemCount = pDestItem->wCount;

		m_pDBCon->QueryMoveItemInGuildWare(m_cDBThreadID, pUserSession->GetAccountDBID(), g_Config.nWorldSetID, &Move);
	}
	else
	{
		TItemInfo SrcInfo ={0,};
		SrcInfo.cSlotIndex = pPacket->cSrcIndex;
		SrcInfo.Item.nSerial = pPacket->biSrcItemSerial;

		TItemInfo DestInfo;
		DestInfo.cSlotIndex = pPacket->cDestIndex;
		DestInfo.Item = *pSrcItem;

		RemoveWareItem(pPacket->biSrcItemSerial);
		AddWareItemInfo(DestInfo);

		pUserSession->SendMoveGuildItem(MoveType_GuildWare, pPacket->biSrcItemSerial, pPacket->biDestItemSerial, &SrcInfo, &DestInfo, ERROR_NONE);
		BroadCastRefreshGuildItem(MoveType_GuildWare, pPacket->biSrcItemSerial, pPacket->biDestItemSerial, &SrcInfo, &DestInfo, pUserSession);
	}
	
	return ERROR_NONE;
}

// 인벤 -> 창고로 아이템이동
int CDNGuildWare::ItemInvenToWare(CDNUserSession* pUserSession, CSMoveGuildItem* pPacket)
{
	if(!m_pDBCon) return ERROR_DB;

	if(!pUserSession)
		return ERROR_GENERIC_INVALIDREQUEST;

	CDNUserItem* pItem = pUserSession->GetItem();
	if(!pItem)
		return ERROR_ITEM_FAIL;

	if(pPacket->wCount <= 0)
		return ERROR_ITEM_NOTFOUND;

	int nGstorage = m_GuildOwner->GetInfo()->wGuildWareSize;

	if((pPacket->cDestIndex >= nGstorage)) 
		return ERROR_ITEM_INDEX_UNMATCH;

	if (pItem->CheckRangeInventoryIndex(pPacket->cSrcIndex) == false)
		return ERROR_ITEM_INDEX_UNMATCH;

	// 인벤아이템 조사
	const TItem* pInven = pItem->GetInventory(pPacket->cSrcIndex);
	if(!pInven) 
		return ERROR_ITEM_NOTFOUND;

	// overflow, serial, soulbound 체크
	int nRet = pItem->IsTradeEnableItem(ITEMPOSITION_INVEN, pPacket->cSrcIndex, pPacket->biSrcItemSerial, pPacket->wCount);
	if(nRet != ERROR_NONE)
		return nRet;

	// 오버랩 가능갯수 확인
	int nInvenOverlapCount = g_pDataManager->GetItemOverlapCount(pInven->nItemID);
	if(nInvenOverlapCount <= 0) 
		return ERROR_ITEM_FAIL;

	// 길드정보를 한번더 확인!
	const TGuildUID GuildUID = pUserSession->GetGuildUID();
	if(!GuildUID.IsSet())
		return ERROR_GUILD_YOUR_NOT_BELONGANYGUILD;

	if(GuildUID != m_GuildOwner->GetUID())
		return ERROR_GUILD_YOUR_NOT_BELONGANYGUILD;

	// 길드원 확인
	TGuildMember* pGuildMember = m_GuildOwner->GetMemberInfo(pUserSession->GetCharacterDBID());
	if(!pGuildMember)
		return ERROR_GUILD_YOUR_NOT_BELONGANYGUILD;

	// 아이템 보관 권한이 있는지 확인
	if(!m_GuildOwner->CheckGuildInfoAuth(static_cast<eGuildRoleType>(pGuildMember->btGuildRole), static_cast<eGuildAuthType>(GUILDAUTH_TYPE_STOREITEM)))
		return ERROR_GUILD_HAS_NO_AUTHORITY;

	// DB에 전송할 패킷
	TQMoveInvenToGuildWare Move;
	memset(&Move, 0, sizeof(Move));

	Move.nCharacterDBID		= pUserSession->GetCharacterDBID();
	Move.nGuildDBID			= GuildUID.nDBID;

	Move.InvenItem.cSlotIndex = pPacket->cSrcIndex;
	Move.InvenItem.nItemID	= pInven->nItemID;
	Move.InvenItem.biSerial	= pInven->nSerial;

	Move.GuildWareItem.cSlotIndex = pPacket->cDestIndex;

	Move.nItemID			= pInven->nItemID;
	Move.biItemSerial		= pInven->nSerial;
	Move.wItemCount			= pInven->wCount;
	Move.wMovingItemCount	= pPacket->wCount;
	Move.nMapIndex			= pUserSession->GetMapIndex();

	// 인벤아이템의 Split가 되어 나온건지 확인
	if(pInven->wCount > pPacket->wCount)
		Move.biNewItemSerial = MakeItemSerial();
	else
		Move.biNewItemSerial = 0;

	if(pInven->wCount == pPacket->wCount){	// 같은경우
		Move.InvenItem.wCount = 0;
		Move.InvenItem.biNewSerial = 0;
	}
	else{
		Move.InvenItem.wCount = pInven->wCount - pPacket->wCount;
	}

	// 창고아이템
	const TItem* pWare = GetWareItem(pPacket->biDestItemSerial);
	// 창고아이템이 있다면 Merge 시도!
	if(pWare)
	{
		// 인벤아이템이 창고아이템과 Merge 가능한지 확인
		if( CDNUserItem::bIsDifferentItem( pInven, pWare ) )
			return pItem->NothingMoveInvenToGuildWare(this, pPacket);

		// 인벤아이템이 창고아이템과 Merge 가능한지 확인
		int nWareOverlapCount = g_pDataManager->GetItemOverlapCount(pWare->nItemID);
		if(pWare->wCount == nWareOverlapCount)
			return pItem->NothingMoveInvenToGuildWare(this, pPacket);

		Move.GuildWareItem.nItemID = pWare->nItemID;
		Move.GuildWareItem.biSerial	= pWare->nSerial;
		Move.GuildWareItem.wCount = pWare->wCount;

		// MAX값을 넘긴 경우
		if(pWare->wCount + pPacket->wCount > nWareOverlapCount)
		{
			// 인벤아이템을 강제로 분할하여 보내준다.
			int nGapCount = nWareOverlapCount - pWare->wCount;
			Move.wMovingItemCount = nGapCount;
			Move.biNewItemSerial = MakeItemSerial();

			Move.InvenItem.wCount = pInven->wCount - nGapCount;
			Move.GuildWareItem.wCount = pWare->wCount + nGapCount;
		}
		else{
			Move.GuildWareItem.wCount = pWare->wCount + pPacket->wCount;
		}

		Move.biMergeTargetItemSerial = pWare->nSerial;
		Move.wMergeItemCount = pWare->wCount;
	}
	else{
		Move.GuildWareItem.nItemID = pInven->nItemID;
		Move.GuildWareItem.biNewSerial = (Move.biNewItemSerial > 0) ? Move.biNewItemSerial : pInven->nSerial;
		Move.GuildWareItem.wCount = pPacket->wCount;
	}

	m_pDBCon->QueryMoveInvenToGuildWare(m_cDBThreadID, pUserSession->GetAccountDBID(), g_Config.nWorldSetID, &Move);
	return ERROR_NONE;
}

// 창고 -> 인벤로 아이템이동
int CDNGuildWare::ItemWareToInven(CDNUserSession* pUserSession, CSMoveGuildItem* pPacket)
{
	if(!m_pDBCon) return ERROR_DB;
	if(!pUserSession) return ERROR_GENERIC_INVALIDREQUEST;

	CDNUserItem* pItem = pUserSession->GetItem();
	if(!pItem) return ERROR_ITEM_FAIL;

	if(pPacket->wCount <= 0)
		return ERROR_ITEM_NOTFOUND;

	int nGstorage = m_GuildOwner->GetInfo()->wGuildWareSize;

	if((pPacket->cSrcIndex >= nGstorage))
		return ERROR_ITEM_INDEX_UNMATCH;
	
	if (pItem->CheckRangeInventoryIndex(pPacket->cDestIndex) == false)
		return ERROR_ITEM_INDEX_UNMATCH;

	// 창고아이템 조사
	const TItem* pWare = GetWareItem(pPacket->biSrcItemSerial);
	if(!pWare)
		return ERROR_ITEM_NOTFOUND;

	// 가지고 있는 것보다 많은 값이 오면 에러!
	if(pPacket->wCount > pWare->wCount)
		return ERROR_ITEM_OVERFLOW;

	// 시리얼코드 조사
	if(pWare->nSerial != pPacket->biSrcItemSerial) 
		return ERROR_ITEM_NOTFOUND;

	// 오버랩가능갯수확인
	int nInvenOverlapCount = g_pDataManager->GetItemOverlapCount(pWare->nItemID);
	if(nInvenOverlapCount <= 0) 
		return ERROR_ITEM_FAIL;

	// 길드정보를한번더확인!
	const TGuildUID GuildUID = pUserSession->GetGuildUID();
	if(!GuildUID.IsSet())
		return ERROR_GUILD_YOUR_NOT_BELONGANYGUILD;

	if(GuildUID != m_GuildOwner->GetUID())
		return ERROR_GUILD_YOUR_NOT_BELONGANYGUILD;

	// 길드원 확인(아이템꺼내기)
	TGuildMember* pGuildMember = m_GuildOwner->GetMemberInfo(pUserSession->GetCharacterDBID());
	if(!pGuildMember)
		return ERROR_GUILD_YOUR_NOT_BELONGANYGUILD;

	// 아이템 꺼내기 권한이 있는지 확인
	if(!m_GuildOwner->CheckGuildInfoAuth(static_cast<eGuildRoleType>(pGuildMember->btGuildRole), static_cast<eGuildAuthType>(GUILDAUTH_TYPE_TAKEITEM)))
		return ERROR_GUILD_HAS_NO_AUTHORITY;

#if defined( PRE_ADD_CHANGEGUILDROLE )
	// 최대 이용할 수 있는 카운트를 꺼내온다(길드장과 길드부장이 아닌 사람만 체크한다)
	if(pGuildMember->btGuildRole != GUILDROLE_TYPE_MASTER &&
		pGuildMember->btGuildRole != GUILDROLE_TYPE_SUBMASTER)
#else
	// 최대 이용할 수 있는 카운트를 꺼내온다(길드장이 아닌 사람만 체크한다)
	if(pGuildMember->btGuildRole != GUILDROLE_TYPE_MASTER)
#endif
	{
		int nMaxItemCount = m_GuildOwner->GetInfo()->nRoleMaxItem[pGuildMember->btGuildRole];

		// 최대값 확인
		if(nMaxItemCount == -1)
			return ERROR_MAX_TAKE_GUILDITEM;

		// 임시 꺼내기 카운트를 만들어 본다.
		USHORT uTakeCount = pGuildMember->TakeWareItemCount + 1;

		// 이용횟수 허용 확인
		if(uTakeCount > nMaxItemCount)
			return ERROR_MAX_TAKE_GUILDITEM;
	}

	// DB에 전송할 패킷구성
	TQMoveGuildWareToInven Move;
	memset(&Move, 0, sizeof(Move));

	Move.nCharacterDBID		= pUserSession->GetCharacterDBID();
	Move.nGuildDBID			= GuildUID.nDBID;

	Move.GuildWareItem.cSlotIndex = pPacket->cSrcIndex;
	Move.GuildWareItem.nItemID = pWare->nItemID;
	Move.GuildWareItem.biSerial	= pWare->nSerial;

	Move.InvenItem.cSlotIndex = pPacket->cDestIndex;

	Move.nItemID			= pWare->nItemID;
	Move.biItemSerial		= pWare->nSerial;
	Move.wItemCount			= pWare->wCount;
	Move.wMovingItemCount	= pPacket->wCount;
	Move.nMapIndex			= pUserSession->GetMapIndex();

#if defined( PRE_ADD_CHANGEGUILDROLE )
	if( pGuildMember->btGuildRole != GUILDROLE_TYPE_MASTER &&
		pGuildMember->btGuildRole != GUILDROLE_TYPE_SUBMASTER )
#else
	if(pGuildMember->btGuildRole != GUILDROLE_TYPE_MASTER)
#endif
		Move.nMaxDailyTakeItemCount = m_GuildOwner->GetInfo()->nRoleMaxItem[pGuildMember->btGuildRole];
	else
	{
		Move.nMaxDailyTakeItemCount = -1;
	}
	// 창고아이템이 분할되었는지 확인
	if(pWare->wCount > pPacket->wCount)
		Move.biNewItemSerial = MakeItemSerial();
	else
		Move.biNewItemSerial = 0;

	if(pWare->wCount == pPacket->wCount){	// 같은경우
		Move.GuildWareItem.wCount = 0;
		Move.GuildWareItem.biNewSerial = 0;
	}
	else{
		Move.GuildWareItem.wCount = pWare->wCount - pPacket->wCount;
	}

	// 인벤아이템
	const TItem* pInven = pItem->GetInventory(pPacket->cDestIndex);
	if(pInven)
	{
		// 창고아이템이 인벤아이템과 Merge가능한지 확인
		if( CDNUserItem::bIsDifferentItem( pWare, pInven ) == true )
			return pItem->NothingMoveGuildWareToInven(this, pPacket, pGuildMember->TakeWareItemCount);

		// MAX값 검사
		int nInvenOverlapCount = g_pDataManager->GetItemOverlapCount(pInven->nItemID);
		if(pInven->wCount == nInvenOverlapCount)
			return pItem->NothingMoveGuildWareToInven(this, pPacket, pGuildMember->TakeWareItemCount);

		Move.InvenItem.nItemID	= pInven->nItemID;
		Move.InvenItem.biSerial	= pInven->nSerial;
		Move.InvenItem.wCount	= pInven->wCount;
		
		if(pInven->wCount + pPacket->wCount > nInvenOverlapCount)
		{
			// 창고아이템을 강제로 분할하여 보내준다.
			int nGapCount = nInvenOverlapCount - pInven->wCount;
			Move.wMovingItemCount = nGapCount;
			Move.biNewItemSerial = MakeItemSerial();

			Move.GuildWareItem.wCount = pWare->wCount - nGapCount;
			Move.InvenItem.wCount = pInven->wCount + nGapCount;
		}
		else{
			Move.InvenItem.wCount = pInven->wCount + pPacket->wCount;
		}

		Move.biMergeTargetItemSerial = pInven->nSerial;
		Move.wMergeItemCount = pInven->wCount;
	}
	else{
		Move.InvenItem.nItemID = pWare->nItemID;
		Move.InvenItem.biNewSerial = (Move.biNewItemSerial > 0) ? Move.biNewItemSerial : pWare->nSerial;
		Move.InvenItem.wCount = pPacket->wCount;
	}

	m_pDBCon->QueryMoveGuildWareToInven(m_cDBThreadID, pUserSession->GetAccountDBID(), g_Config.nWorldSetID, &Move);
	return ERROR_NONE;
}

// FROM DB Server
// 길드창고간 아이템 이동
int CDNGuildWare::OnMoveItemInGuildWare(CDNUserSession* pUserSession, TAMoveItemInGuildWare* pResult)
{
	CDNUserItem* pItem = pUserSession->GetItem();
	if(!pItem)
		return ERROR_ITEM_FAIL;

	return pItem->InGuildWare(this, pResult);
}

// 인벤 -> 길드창고로 아이템 이동결과
int CDNGuildWare::OnMoveInvenToGuildWare(CDNUserSession* pUserSession, TAMoveInvenToGuildWare* pResult)
{
	CDNUserItem* pItem = pUserSession->GetItem();
	if(!pItem)
		return ERROR_ITEM_FAIL;

	return pItem->FromInvenToGuildWare(this, pResult);
}

// 길드창고 -> 인벤로 아이템 이동
int CDNGuildWare::OnMoveGuildWareToInven(CDNUserSession* pUserSession, TAMoveGuildWareToInven* pResult)
{
	CDNUserItem* pItem = pUserSession->GetItem();
	if(!pItem)
		return ERROR_ITEM_FAIL;

	TGuildMember* pGuildMember = m_GuildOwner->GetMemberInfo(pUserSession->GetCharacterDBID());
	if(pGuildMember)
	{
		pGuildMember->TakeWareItemCount = pResult->nDailyTakeItemCount;
		pGuildMember->LastWareUseDate = pResult->tUseDate;
	}

	return pItem->FromGuildWareToInven(this, pResult);
}

// 인벤 -> 창고 코인이동
int CDNGuildWare::CoinInvenToWare(CDNUserSession* pUserSession, CSMoveGuildCoin* pPacket)
{
	TGuildMember* pGuildMember = m_GuildOwner->GetMemberInfo(pUserSession->GetCharacterDBID());
	if (!pGuildMember)
		return ERROR_GUILD_YOUR_NOT_BELONGANYGUILD;

	// 코인값 범위체크
	if(pPacket->nMoveCoin <= 0) 
		return ERROR_ITEM_INSUFFICIENCY_MONEY;

	if(pPacket->nMoveCoin > GUILDWARE_COINMAX)
		return ERROR_ITEM_INVENTOWARE02;

	// 인벤의 돈 검사
	if(!pUserSession->CheckEnoughCoin(pPacket->nMoveCoin))
		return ERROR_ITEM_INVENTOWARE01;

	// 창고에 넣을 자리가 없다.
	if(pPacket->nMoveCoin + GetWarehouseCoin() > GUILDWARE_COINMAX)
		return ERROR_ITEM_INVENTOWARE02;

	TQGuildWareHouseCoin Packet;

	Packet.cMoveType	= pPacket->cMoveType;
	Packet.bInOutFlag	= false; // 입금
	Packet.nMoveCoin	= pPacket->nMoveCoin;
	Packet.nGuildDBID   = pUserSession->GetGuildUID().nDBID;
	Packet.nCharacterDBID = pUserSession->GetCharacterDBID();
	Packet.nMapIndex	= pUserSession->GetMapIndex();
	_strcpy(Packet.szIP, _countof(Packet.szIP), pUserSession->GetIp(), (int)strlen(pUserSession->GetIp()));

	if(m_pDBCon)
		m_pDBCon->QueryGuildWarehouseCoin(m_cDBThreadID, pUserSession->GetAccountDBID(), g_Config.nWorldSetID, &Packet);

	return ERROR_NONE;
}

// 창고 -> 인벤 코인이동
int CDNGuildWare::CoinWareToInven(CDNUserSession* pUserSession, CSMoveGuildCoin* pPacket)
{

	// 코인값 범위체크
	if((pPacket->nMoveCoin <= 0) ||(pPacket->nMoveCoin > GUILDWARE_COINMAX)) 
		return ERROR_ITEM_INVENTOWARE01;

	if(pPacket->nMoveCoin > GetWarehouseCoin())			// 창고에돈이없다
		return ERROR_ITEM_WARETOINVEN01;	

	if(!pUserSession->CheckMaxCoin(pPacket->nMoveCoin))	// 인벤에넣을자리가없다
		return ERROR_ITEM_WARETOINVEN02;


	// 길드원 확인(출금)
	TGuildMember* pGuildMember = m_GuildOwner->GetMemberInfo(pUserSession->GetCharacterDBID());

	if(pGuildMember)
	{
		// 출금 권한이 있는지 확인
		if(!m_GuildOwner->CheckGuildInfoAuth(static_cast<eGuildRoleType>(pGuildMember->btGuildRole), static_cast<eGuildAuthType>(GUILDAUTH_TYPE_WITHDRAW)))
			return ERROR_GUILD_HAS_NO_AUTHORITY;

#if defined( PRE_ADD_CHANGEGUILDROLE )
		// 최대이용할수있는카운트를꺼내온다(길드장과 길드부장이아닌사람만체크한다)
		if( pGuildMember->btGuildRole != GUILDROLE_TYPE_MASTER &&
			pGuildMember->btGuildRole != GUILDROLE_TYPE_SUBMASTER )
#else
		// 최대이용할수있는카운트를꺼내온다(길드장이아닌사람만체크한다)
		if(pGuildMember->btGuildRole != GUILDROLE_TYPE_MASTER)
#endif
		{
			// 최대 출금액
			int nMaxCoinCount = m_GuildOwner->GetInfo()->nRoleMaxCoin[pGuildMember->btGuildRole];

			// 임시 출금액을 산정한다.
			INT64 nTotalCoin = pGuildMember->WithdrawCoin + pPacket->nMoveCoin;

			// 출금액 허용 확인
			if(nTotalCoin > nMaxCoinCount)
				return ERROR_MAX_WITHDRAW_GUILDCOIN;
		}
	}
	else
		return ERROR_GUILD_YOUR_NOT_BELONGANYGUILD;

	TQGuildWareHouseCoin Packet;

	Packet.cMoveType	= pPacket->cMoveType;
	Packet.bInOutFlag	= true; // 출금
	Packet.nMoveCoin	= (int)pPacket->nMoveCoin;
	Packet.nGuildDBID   = pUserSession->GetGuildUID().nDBID;
	Packet.nCharacterDBID = pUserSession->GetCharacterDBID();
	Packet.nMapIndex	= pUserSession->GetMapIndex();
	_strcpy(Packet.szIP, _countof(Packet.szIP), pUserSession->GetIp(), (int)strlen(pUserSession->GetIp()));

	if(m_pDBCon)
		m_pDBCon->QueryGuildWarehouseCoin(m_cDBThreadID, pUserSession->GetAccountDBID(), g_Config.nWorldSetID, &Packet);

	return ERROR_NONE;
}

// 길드 코인
int CDNGuildWare::OnModGuildCoin(CDNUserSession* pUserSession, TAGuildWareHouseCoin* pResult)
{
	if(!pUserSession)
		return ERROR_GENERIC_INVALIDREQUEST;

	INT64 nWithdrawCoin = 0;
	// 입금결과
	if(pResult->cMoveType == MoveType_InvenToGuildWare)
	{
		pUserSession->DelCoin(pResult->nMoveCoin, DBDNWorldDef::CoinChangeCode::DoNotDBSave, pUserSession->GetGuildUID().nDBID);
	}
	else if(pResult->cMoveType == MoveType_GuildWareToInven)	// 출금
	{
		pUserSession->AddCoin(pResult->nMoveCoin, DBDNWorldDef::CoinChangeCode::DoNotDBSave, pUserSession->GetGuildUID().nDBID);
		nWithdrawCoin = m_GuildOwner->UpdateMemberCoin(pUserSession->GetCharacterDBID(), pResult->nMoveCoin, pResult->tUseDate);
	}
	else
		return ERROR_GENERIC_INVALIDREQUEST;

	SetWarehouseCoin(pResult->biTotalCoin);

	pUserSession->SendMoveGuildCoin(pResult->cMoveType, pResult->nMoveCoin, pUserSession->GetCoin(), GetWarehouseCoin(), nWithdrawCoin, ERROR_NONE);
	BroadCastRefreshGuildCoin(GetWarehouseCoin(), pUserSession);
	
	return ERROR_NONE;
}

#ifdef PRE_ADD_JOINGUILD_SUPPORT
int	CDNGuildWare::OnAddGuildCoin(CDNUserSession* pUserSession, INT64 nAddCoin)
{
	if(!pUserSession)
		return ERROR_GENERIC_INVALIDREQUEST;

	SetWarehouseCoin(GetWarehouseCoin() + nAddCoin);

	BroadCastRefreshGuildCoin(GetWarehouseCoin(), pUserSession);
	pUserSession->SendRefreshGuildCoin(GetWarehouseCoin());
	return ERROR_NONE;
}

int CDNGuildWare::OnAddInGuildWare(CDNUserSession * pUserSession, DBPacket::TTakeAttachInfo * pItem)
{
	TItemInfo DestInfo;
	for (int i = 0; i < MAILATTACHITEMMAX; i++)
	{
		if (pItem->TakeItem[i].Item.nItemID <= 0) continue;
		memset(&DestInfo, 0, sizeof(TItemInfo));
		DestInfo = pItem->TakeItem[i];

		AddWareItemInfo(DestInfo);
		BroadCastRefreshGuildItem(MoveType_MailToGuildWare, 0, DestInfo.Item.nSerial, NULL, &DestInfo, pUserSession, true);
	}
	return ERROR_NONE;
}
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT

void CDNGuildWare::SetWarehouseCoin(INT64 biTotalCoin)
{
	if(biTotalCoin < 0 || biTotalCoin > GUILDWARE_COINMAX)
		return;

	m_GuildOwner->SetInfo()->ui64GuildMoney = biTotalCoin;
}

void CDNGuildWare::ChangeWareSize(CDNUserBase* pUserObject, short wSize)
{
	if(m_pDBCon)
		m_pDBCon->QueryChangeGuildWareSize(m_cDBThreadID, pUserObject->GetAccountDBID(), pUserObject->GetCharacterDBID(), m_GuildOwner->GetUID().nDBID, g_Config.nWorldSetID, wSize);
}

INT64 CDNGuildWare::GetWarehouseCoin()
{
	if(m_GuildOwner->GetInfo()->ui64GuildMoney <=0)
		m_GuildOwner->SetInfo()->ui64GuildMoney = 0;

	if(m_GuildOwner->GetInfo()->ui64GuildMoney >= GUILDWARE_COINMAX) 
		m_GuildOwner->SetInfo()->ui64GuildMoney = GUILDWARE_COINMAX;

	return m_GuildOwner->GetInfo()->ui64GuildMoney;
}

void CDNGuildWare::UpdateSlotList(__time64_t tVersion, BYTE cCount, TGuildWareSlot* List)
{
	// 최신버젼인지 확인한다.
	if(m_VersionDate > tVersion)
		return;

	// 슬롯리스트 저장
	UpdateWareSlot(cCount, List);

	// 버전 업데이트
	m_VersionDate = tVersion;
}

void CDNGuildWare::SaveWareResultPacket(MAGuildWareInfoResult& result)
{
	result.GuildUID = m_GuildOwner->GetUID();

	// 슬롯리스트 복사
	result.cSlotListCount = GetSlotList(result.WareSlotList);

	// 버젼
	result.VersionDate = m_VersionDate;
}

INT64 CDNGuildWare::MakeItemSerial()
{
	return MakeSerial( static_cast<short>(g_pAuthManager->GetServerID()));
}