#pragma once

#include "DNGuildSystem.h"
#include "TimeSet.h"

class CDNDBConnection;
class CDNGuildVillage;


typedef std::map <INT64, TItemInfo> TMapGuildWareItem;		// [시리얼][아이템정보]
typedef std::vector <TGuildWareHistory> TVecGuildWareHistory;	// 길드창고 히스토리


class CDNGuildWare 
{
public:
	CDNGuildWare();
	~CDNGuildWare();

	// OPEN
	void Open(CDNGuildVillage* pGuildVillage, const TAGetGuildWareInfo* pPacket);
	void ReportWareSlot();

	// RECV FROM USER
	bool OnRecvMoveGuildItem(CDNUserSession* pUserSession, CSMoveGuildItem* pPacket);
	bool OnRecvMoveGuildCoin(CDNUserSession* pUserSession, CSMoveGuildCoin* pPacket);
	void OnRecvGetWareHistory(CDNUserSession* pUserSession, CSGetGuildWareHistory* pPacket);

	// FROM DB Server
	int	OnMoveItemInGuildWare(CDNUserSession* pUserSession, TAMoveItemInGuildWare* pResult);
	int	OnMoveInvenToGuildWare(CDNUserSession* pUserSession, TAMoveInvenToGuildWare* pResult);
	int	OnMoveGuildWareToInven(CDNUserSession* pUserSession, TAMoveGuildWareToInven* pResult);
	int	OnModGuildCoin(CDNUserSession* pUserSession, TAGuildWareHouseCoin* pResult);
#ifdef PRE_ADD_JOINGUILD_SUPPORT
	int	OnAddGuildCoin(CDNUserSession* pUserSession, INT64 nAddCoin);
	int OnAddInGuildWare(CDNUserSession * pUserSession, DBPacket::TTakeAttachInfo * pItem);
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT

	// FROM MASTER
	bool OnRecvGuildWareInfo(const MAGuildWareInfo *pPacket);
	bool OnRecvGuildWareInfoResult(MAGuildWareInfoResult *pPacket);

	// WARE INFORMATION
	BYTE GetWareList(TItemInfo* pItemInfo);
	BYTE GetSlotList(TGuildWareSlot* pSlot);
	void UpdateWareSlot(BYTE cCount, TGuildWareSlot* List);
	size_t GetWareItemCount();

	// VERSION
	__time64_t GetVersionDate();
	void UpdateVersionDate();
	void SetVersionDate(__time64_t tVersion);

	// COIN
	INT64 GetWarehouseCoin();
	void SetWarehouseCoin(INT64 biTotalCoin);

	// SIZE
	void ChangeWareSize(CDNUserBase* pUserObject, short wSize);

	// WAREHOUSE
	const TItem* GetWareItem(INT64 biItemSerial);
	const TItemInfo* GetWareItemInfo(INT64 biItemSerial);

	int	AddWareItemInfo(TItemInfo& AddItem);
	int	RemoveWareItem(INT64 nSerial);
	void UpdateWareInfo(TItemInfo& ItemInfo);

#ifdef PRE_ADD_JOINGUILD_SUPPORT
	void BroadCastRefreshGuildItem(char cType, INT64 biSrcSerial, INT64 biDestSerial, const TItemInfo *pSrcInfo, const TItemInfo *pDestInfo, CDNUserSession* pSession, bool bNoUpdate = false);
#else		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
	void BroadCastRefreshGuildItem(char cType, INT64 biSrcSerial, INT64 biDestSerial, const TItemInfo *pSrcInfo, const TItemInfo *pDestInfo, CDNUserSession* pSession);
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
	void BroadCastRefreshGuildCoin(INT64 biTotalCoin, CDNUserSession* pSession);

private:
	void Reset();

	// ACTION
	int	ItemInWare(CDNUserSession* pUserSession, CSMoveGuildItem* pPacket);
	int	ItemInvenToWare(CDNUserSession* pUserSession, CSMoveGuildItem* pPacket);
	int	ItemWareToInven(CDNUserSession* pUserSession, CSMoveGuildItem* pPacket);

	int	CoinInvenToWare(CDNUserSession* pUserSession, CSMoveGuildCoin* pPacket);
	int	CoinWareToInven(CDNUserSession* pUserSession, CSMoveGuildCoin* pPacket);

	// SLOT
	void UpdateSlotList(__time64_t tVersion, BYTE cCount, TGuildWareSlot* List);
	void SaveWareResultPacket(MAGuildWareInfoResult& pResult);

	INT64 MakeItemSerial();

private:
	TMapGuildWareItem m_MapWareItem;

	CTimeSet m_TimeSet;			
	__time64_t m_VersionDate;				// 창고 버젼

	// DB
	CDNDBConnection* m_pDBCon;
	BYTE m_cDBThreadID;

	CDNGuildVillage* m_GuildOwner;			// 창고 주인

	CSyncLock m_WareLock;					// 락 객체
};