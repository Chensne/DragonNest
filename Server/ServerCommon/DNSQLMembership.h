#pragma once

/* -----------------------------------------
	Login, DB, Cash 가 같이 쓰는 파일임
----------------------------------------- */

#include "SQLConnection.h"
#if defined(_LOGINSERVER)
#include "DNUserConnection.h"
#endif

class CDNSQLMembership: public CSQLConnection
{
public:
	CDNSQLMembership(void);
	~CDNSQLMembership(void);

	int QueryGetDatabaseVersion( TDatabaseVersion* pA );

#if defined(_LOGINSERVER)
	int QueryLogin(CDNUserConnection *pUserCon, WCHAR *pPassword, BYTE cStep, OUT Login::TQueryLoginOutput &LoginOutput);
#endif	// _LOGINSERVER
	int QueryGetCharacterCount(WCHAR *pAccountName, std::map<int,int>& mWorldCharCount);
	int QueryLogout(UINT nAccountDBID, UINT nSessionID, const BYTE * pMachineID = NULL);
	int QueryModLastConnectDate(UINT nAccountDBID);
	int QueryAddAccountKey(UINT nAccountDBID, UINT nUserNo);

	int QueryAddRestraint( DBDNWorldDef::RestraintTargetCode::eCode TargetCode, UINT uiAccountDBID, INT64 biCharacterDBID, int iReasonID, DBDNWorldDef::RestraintTypeCode::eCode TypeCode, WCHAR* pwszMemo, WCHAR* pwszRestraintMsg, TIMESTAMP_STRUCT StartDate, TIMESTAMP_STRUCT EndDate );
#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
	int QueryAddBlockedIP(int nWorldID, UINT nAccountDBID, const char * pszIP);
	int QueryGetSimpleConfigValue(TASimpleConfig * pSimpleConfig);
#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
#ifdef PRE_ADD_SEETMAINTENANCEFLAG
	int QuerySetMaintenanceFlag(int nFlag);
#endif		//#ifdef PRE_ADD_SEETMAINTENANCEFLAG
	int QueryGetListRestraintForAccount(UINT nAccountDBID, OUT std::vector<TRestraintForAccountAndCharacter> &VecRestraint);
	int QueryGetListRestraintForCharacter(INT64 biCharacterDBID, OUT std::vector<TRestraintForAccountAndCharacter> &VecRestraint);
	int QueryGetGameOption(UINT nAccountDBID, OUT TGameOptions &Options);
	int QueryModGameOption(UINT nAccountDBID, TGameOptions &Options);
	int QueryAddCharacter(UINT nAccountDBID, WCHAR *pCharName, int nWorldID, int nDefaultMaxCharacterCountPerAccount, OUT INT64 &biCharacterDBID, OUT TIMESTAMP_STRUCT &CreateDate);
	int QueryRollbackAddCharacter(INT64 biCharacterDBID);
	int QueryDelCharacter(INT64 biCharacterDBID, WCHAR *pPrivateIp, WCHAR *pPublicIp, bool bFirstVillage, TIMESTAMP_STRUCT& DeleteDate );
	int QueryReviveCharacter( INT64 biCharacterDBID, WCHAR* wszPrivateIP, WCHAR* wszPublicIP );
	int QueryGetKeySettingOption( UINT uiAccountDBID, TKeySetting* pKeySetting );
	int QueryModKeySettingOption( TQModKeySettingOption* pQ, TAModKeySettingOption* pA );
	int QueryGetPadSettingOption( UINT uiAccountDBID, TPadSetting* pPadSetting );
	int QueryModPadSettingOption( TQModPadSettingOption* pQ, TAModPadSettingOption* pA );
	int QueryCheckPCRoomIP(char *pIp, bool &bPCbang, char &cPCbangGrade);	// 5.PC방 IP 여부 확인 - P_CheckPCRoomIP

	// 국가 코드 얻어오기
	int QueryGetNationalityCode(UINT nAccountDBID, OUT BYTE &cRegion);

	// Auth
	int QueryBeginAuth(UINT uiAccountDBID, char& cCertifyingStep, int& nServerID, char& cWorldID, UINT& nSessionID, INT64& biCharacterDBID, LPCWSTR wszAccountName, LPCWSTR wszCharacterName, char cAccountLevel, BYTE bIsAdult, char nAge, BYTE nDailyCreateCount, int nPrmInt1);
	int QueryStoreAuth(UINT uiAccountDBID, INT64 biCertifyingKey, int nCurServerID, char cWorldID, UINT nSessionID, INT64 biCharacterDBID, LPCWSTR wszAccountName, LPCWSTR wszCharacterName, BYTE bIsAdult, char nAge, BYTE nDailyCreateCount, int nPrmInt1, char cLastServerType);
	int QueryCheckAuth(UINT uiAccountDBID, INT64 biCertifyingKey, int nCurServerID, OUT char& cWorldSetID, OUT UINT& nSessionID, OUT INT64& biCharacterDBID, OUT LPWSTR* wszAccountName, OUT LPWSTR* wszCharacterName, OUT char& cAccountLevel, OUT BYTE& bIsAdult, OUT char& nAge, OUT BYTE& nDailyCreateCount, OUT int& nPrmInt1, OUT char& cLastServerType);
	int QueryResetAuth(UINT uiAccountDBID, UINT nSessionID);
	int QueryResetAuthServer(int nServerID);
	int QueryResetAuthByAccountWorld(UINT uiAccountDBID, char cWorldID, UINT nSessionID);
	int QuerySetWorldIDAuth(char cWorldID, UINT nAccountDBID);

	// 2차 인증 관련 쿼리
	int QueryValidataSecondAuthPassphrase( UINT uiAccountDBID, const WCHAR* pszPW, BYTE& cFailCount );
	int QueryModSecondAuthPassphrase( UINT uiAccountDBID, const WCHAR* pszOldPW, const WCHAR* pszNewPW, BYTE& cFailCount );
	int QueryModSecondAuthLockFlag( UINT uiAccountDBID, bool bLock );
	int QueryGetSecondAuthStatus( UINT uiAccountDBID, bool& bSetPW, bool& bLock, __time64_t& tResetDate );
	int QueryInitSecondAuth( UINT uiAccountDBID );
#if defined(PRE_ADD_SHA256)
	int QueryValidataSecondAuthPassphraseByServer( UINT uiAccountDBID, const WCHAR* pszPW, BYTE& cFailCount );
	int QueryModSecondAuthPassphraseByServer(UINT uiAccountDBID, const WCHAR* pszOldPW, const WCHAR* pszNewPW, BYTE& cFailCount );
#endif

#ifdef PRE_ADD_23829
	int QueryCheckLastSecondAuthNotifyDate(UINT nAccountDBID, int nCheckPeriod);		//Period : day
	int QueryModLastSecondAuthNotifyDate(UINT nAccountDBID);
#endif

	int QueryCheckIntroducedAccount(UINT nAccountDBID, OUT bool &bCheckResult);

	// Cash
	int QueryGetProduct(int nProductID, TCashCommodityData *Product);		// 1.유료 상품 단건 조회 - P_GetProduct

#ifdef PRE_ADD_LIMITED_CASHITEM
	int QueryAddPurchaseOrderByCash(INT64 biCharacterDBID, bool bPCBang, int nItemSN, int nPrice, int nLimitCount, int nSelectItemID, BYTE cSelectItemOption, int nLimitedItemMax, int nTotalPrice, char *pIp, 
		bool bGift, WCHAR *pReceiveCharacterName, WCHAR *pMemo, OUT INT64 &biPurchaseOrderID, OUT char* pPurchaseOrderDetailID=NULL, const char *pszRefundAbles=NULL, bool bCheatFlag=false);	// 2.캐쉬 결제 요청 - P_AddPurchaseOrderByCash
#else		//#ifdef PRE_ADD_LIMITED_CASHITEM
	// 단품..
	int QueryAddPurchaseOrderByCash(INT64 biCharacterDBID, bool bPCBang, int nItemSN, int nPrice, int nLimitCount, int nSelectItemID, BYTE cSelectItemOption, int nTotalPrice, char *pIp, 
		bool bGift, WCHAR *pReceiveCharacterName, WCHAR *pMemo, OUT INT64 &biPurchaseOrderID, OUT char* pPurchaseOrderDetailID=NULL, const char *pszRefundAbles=NULL, bool bCheatFlag=false);	// 2.캐쉬 결제 요청 - P_AddPurchaseOrderByCash	
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM
	// 여러개 동시..
	int QueryAddPurchaseOrderByCash(INT64 biCharacterDBID, bool bPCBang, std::vector<string> &VecItemString, int nTotalPrice, char *pIp, 
		bool bGift, WCHAR *pReceiveCharacterName, WCHAR *pMemo, OUT INT64 &biPurchaseOrderID, OUT char* pPurchaseOrderDetailIDs=NULL, bool bCheatFlag=false);	// 2.캐쉬 결제 요청 - P_AddPurchaseOrderByCash

	int QuerySetPurchaseOrderResult(INT64 biPurchaseOrderID, char cOrderStatusCode, char *pOrderKey, int nOrderResult, int nPetalAmount, int nPaidCashAmount, char *pContextKey = NULL);			// 3.캐쉬 결제 응답 기록 - P_SetPurchaseOrderResult
	int QueryRollbackPurchaseOrderByCash(INT64 biPurchaseOrderID, OUT char *pOrderKey, OUT int &nProductID);		// 4.캐쉬 결제 취소 요청 - P_RollbackPurchaseOrderByCash
	int QuerySetRollbackPurchaseOrderResult(INT64 biOrderID, char cStatusCode, int nRollbackResult);				// 5.캐쉬 결제 취소 응답 기록 - P_SetRollbackPurchaseOrderResult

#if defined(PRE_ADD_LIMITED_CASHITEM) && defined(_CASHSERVER)
	int QueryGetLimitedItem();
#endif		//#if defined(PRE_ADD_LIMITED_CASHITEM) && defined(_CASHSERVER)

#if defined(PRE_ADD_CASH_REFUND)
	int QueryGetListRefundableProducts(INT64 biCharacterDBID, TAPaymentItemList* pPaymentItemList, TAPaymentPackageItemList* pPaymentPackageItemList, int nRefundLimitPeriod = 7);	// 6. 결제 인벤토리 목록 요청 - P_GetListRefundableProducts
	int QueryModPurchaseOrderDetail(INT64 biPurchaseOrderDetailID);	// 결재인벤에서 캐쉬인벤으로 옮기기.
	int QueryCashRefund(INT64 biPurchaseOrderDetailID, int nRefundLimitPeriod = 7 );
#endif
	int QueryCheatGiftClear(INT64 biCharacterDBID);

	int QueryIncreaseVIPBasicPoint(INT64 biCharacterDBID, int nBasicPoint, INT64 biPurchaseOrderID, short wVIPPeriod, bool bAutoPay, OUT int &nVIPTotalPoint, OUT __time64_t &tVIPEndDate);	// 6.VIP 기본 포인트 가산 - P_IncreaseVIPBasicPoint
	int QueryGetVIPPoint(INT64 biCharacterDBID, OUT int &nVIPTotalPoint, OUT __time64_t &tVIPEndDate, OUT bool &bAutoPay);		// 7.VIP 기간 및 포인트 조회 - P_GetVIPPoint
	int QueryModVIPAutoPayFlag(INT64 biCharacterDBID, bool bAutoPay);	// 8.VIP 자동 결제 상태 변경 - P_ModVIPAutoPayFlag

	int QueryAddBanOfSale(int nItemSN);	// P_AddBanOfSale
	int QueryGetListBanOfSale(std::vector<int> &VecProhibitList);	// 2.판매 금지 상품 목록 - P_GetListBanOfSale

	int QueryGetPetalBalance(int nAccountDBID, OUT int &nPetalBalance);								// 1.페탈 잔액 조회 - P_GetPetalBalance
	int QueryPurchaseItemByPetal(INT64 biCharacterDBID, bool bPCBang, std::vector<string> &VecItemString, int nTotalPrice, char *pIp, OUT INT64 &biPurchaseOrderID);				// 2.페탈 결제 - P_PurchaseItemByPetal
	int QueryRollbackPurchaseItemByPetal(INT64 biOrderID);											// 3.페탈 결제 취소 - P_RollbackPurchaseItemByPetal
	int QueryAddPetalIncome(INT64 biCharacterDBID, INT64 biPurchaseOrderID, int nPetal, OUT int &nTotalPetal, bool bTrade=false);	// 4. 페탈 충전(페탈 상품권 사용) - P_AddPetalIncome
#if defined( PRE_PVP_GAMBLEROOM )
	int QueryAddPetalIncome(INT64 biCharacterDBID, INT64 biPurchaseOrderID, int nPetal, OUT int &nTotalPetal, BYTE cPetalChangeCode, INT64 nGambleDBID=0);
	int QueryUsePetal(INT64 biCharacterDBID, int nPetal, BYTE cPetalChangeCode, INT64 nGambleDBID=0);
#endif
#if defined(PRE_ADD_CASH_REFUND)	
	int QueryAddPetalIncomeCashRefund(INT64 biCharacterDBID, INT64 biPurchaseOrderDetailID, int nPetal, OUT int &nTotalPetal); // 4. 페탈 충전 - P_AddPetalIncomeForReward 청약철회관련 결재인벤에서 캐쉬인벤으로 옮길때 사용합니다
#endif
	int QueryGetCashBalance(int nAccountDBID, WCHAR *pAccountName, int &nCashBalance);
	int QueryDeductCash(int nAccountDBID, INT64 biPurchaseOrderID, int nDelCash, int &nCashOutgoID, int &nCashBalance);

#if defined( PRE_ADD_NEW_MONEY_SEED )
	int QueryPurchaseItemBySeed(INT64 biCharacterDBID, bool bPCBang, std::vector<string> &VecItemString, int nTotalPrice, char *pIp, OUT INT64 &biPurchaseOrderID);				// 시드 결제 - P_PurchaseItemBySeed
#endif

	int QueryMakeGiftByCoupon(INT64 biCharacterDBID, bool bPCBang, const char *pszItemSNs, const char *pszSelectItemIDs, const char *pszSelectItemOptions, WCHAR *pCoupon, const char *pOrderKey, char *pIp, OUT INT64 &biPurchaseOrderID);	// 3.쿠폰으로 아이템 결제 - P_MakeGiftByCoupon
	int QueryMakeGiftByQuest(INT64 biCharacterDBID, bool bPCBang, int nItemSN, const char *pszSelectItemIDs, const char *pszSelectItemOptions, int nQuestID, WCHAR *pMemo, int nLifeSpan, char *pIp, bool bNewFlag, OUT INT64 &biPurchaseOrderID);	// 4.퀘스트 보상으로 선물 지급 - P_MakeGiftByQuest
	int QueryMakeGiftByMission(INT64 biCharacterDBID, bool bPCBang, int nItemSN, const char *pszSelectItemIDs, const char *pszSelectItemOptions, int nMissionID, WCHAR *pMemo, int nLifeSpan, char *pIp, bool bNewFlag, OUT INT64 &biPurchaseOrderID);	// 5.미션 보상으로 선물 지급 - P_MakeGiftByMission
#ifdef PRE_ADD_LIMITED_CASHITEM
	int QueryMakeGift(INT64 biCharacterDBID, bool bPCBang, int nItemSN, const char *pszSelectItemIDs, const char *pszSelectItemOptions, const char * pszLimitedQuantityMax, WCHAR *pMemo, int nLifeSpan, int nPayMethodCode, int iOrderKey, char *pIp, bool bNewFlag, OUT INT64 &biPurchaseOrderID);	// 7.선물 지급(퀘스트, 미션 보상용 SP를 대체할 수 있는 범용 SP) - P_MakeGift
#else		//#ifdef PRE_ADD_LIMITED_CASHITEM
	int QueryMakeGift(INT64 biCharacterDBID, bool bPCBang, int nItemSN, const char *pszSelectItemIDs, const char *pszSelectItemOptions, WCHAR *pMemo, int nLifeSpan, int nPayMethodCode, int iOrderKey, char *pIp, bool bNewFlag, OUT INT64 &biPurchaseOrderID);	// 7.선물 지급(퀘스트, 미션 보상용 SP를 대체할 수 있는 범용 SP) - P_MakeGift
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM
	int QueryAddGuildWarMakeGiftQueue(INT64 biCharacterDBID, int nItemSN, const char *pszSelectItemIDs, WCHAR *pMemo, int nLifeSpan, int nPayMethodCode, int nMatchTypeCode, char *pIp, bool bNewFlag); // 길드전 본선 보상
	int QueryAddGiftSendSchedule(const char *pszCharacterDBIDs, int nItemSN, const char *pszSelectItemIDs, WCHAR *pMemo, int nLifeSpan, int nPayMethodCode, char *pIp, bool bNewFlag); // 길드전 본선 보상

	int QueryModGiveFailFlag(INT64 biPurchaseOrderID, int nProductSN, int nItemID, bool bItemGiveFail);
	int QueryModGiveFailFlag2(INT64 biPurchaseOrderDetailID, bool bItemGiveFail);

	int QueryGetListGiveFailItem(INT64 biCharacterDBID, TAGetListGiveFailItem *pA);

	int QueryGetCountNotReceivedGift(INT64 biCharacterDBID, OUT int &nGiftCount);	// 1.받지 않은 선물 개수 조회 - P_GetCountNotReceivedGift
	int QueryGetListGiftBox(INT64 biCharacterDBID, OUT char &cCount, OUT TGiftInfo *GiftBoxList);	// 2.받은 선물 목록 - P_GetListGiftBox
	int QueryGetListItemOfGift(INT64 biPurchaseOrderID, OUT int &nItemSN, OUT int &nItemPrice, OUT std::vector<DBPacket::TItemIDOption> &VecItemList);	// 3.받을 선물 목록 조회 - P_GetListItemOfGift
	int QueryModGiftReceiveFlag(INT64 biPurchaseOrderID, OUT INT64 &biReplySenderCharacterDBID, OUT WCHAR *pReplyReceiverCharName, OUT INT64 &biGiftSenderCharacterDBID, OUT int &nPaidCashAmount);	// 4.받은 선물을 캐쉬 인벤토리로 이동 표시 - P_ModGiftReceiveFlag
#if defined(PRE_ADD_GIFT_RETURN)
	int QueryModGiftRejectFlag(INT64 biPurchaseOrderID, TAGiftReturn* pGiftReturn); // 5.선물 반송 - P_ModGiftRejectFlag
#endif // #if defined(PRE_ADD_GIFT_RETURN)
	int QueryModCharacterName(TQChangeCharacterName* pQ, TAChangeCharacterName* pA);
	int QueryIncreaseCharacterMaxCount(UINT nAccountDBID, char cAddCount, char cLimitCount, char &cCharMaxCount);	// P_IncreaseCharacterMaxCount
	int QueryGetCharacterMaxCount(UINT nAccountDBID, char &cCharMaxCount);	// P_GetCharacterMaxCount
	int QueryModCharacterSlotCount(INT64 biCharacterDBID, char cAddCount, char &cCharMaxCount);	// P_ModCharacterSlotCount
	int QueryGetCharacterSlotCount(UINT nAccountDBID, int nWorldID, INT64 biCharacterDBID, char &cCharMaxCount);	// P_GetCharacterSlotCount

#if defined(PRE_ADD_PETALTRADE)
	int QueryPurchaseTradeItemByPetal(INT64 biCharacterDBID, int nPetalPrice);
#endif

#if defined(PRE_SPECIALBOX)
	int QueryAddEventReward(TQAddEventReward *pQ, TAAddEventReward *pA);	// 이벤트 보상 입력 - P_AddEventReward
	int QueryGetCountEventReward(int nAccountDBID, INT64 biCharacterDBID, int &nEventTotalCount);		// 이벤트 보상 개수 조회 - P_GetCountEventReward
	int QueryGetListEventReward(TQGetListEventReward *pQ, TAGetListEventReward *pA);		// 이벤트 보상 목록 조회 - P_GetListEventReward
	int QueryGetListEventRewardItem(TQGetListEventRewardItem *pQ, TAGetListEventRewardItem *pA);		// 이벤트 보상 아이템 목록 조회 - P_GetListEventRewardItem
	int QueryAddEventRewardReceiver(int nAccountDBID, INT64 biCharacterDBID, int nEventRewardID);		// 이벤트 보상을 받은 캐릭터 목록 저장 - P_AddEventRewardReceiver
#endif	// #if defined(PRE_SPECIALBOX)

#ifdef PRE_ADD_DOORS
	int QueryGetAuthenticationFlag(UINT nAccountDBID, bool &bFlag);
	int QueryGetDoorsAuthentication(UINT nAccountDBID, char * pszAuthKey);
	int QueryCancelDoorsAuthentication(UINT nAccountDBID);
#endif		//#ifdef PRE_ADD_DOORS

	int QueryModCharacterSortCode(UINT nAccountDBID, BYTE cCharacterSortCode);
	int QueryGetCharacterSortCode(UINT nAccountDBID, BYTE &cCharacterSortCode);
#if defined(PRE_ADD_GAMEQUIT_REWARD)
	int QueryModNewbieRewardFlag(UINT nAccountDBID, bool bRewardFlag);
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
#if defined(PRE_ADD_HEARTBEAT_TO_SQL)
	int QueryHeartbeat();
#endif		//#if defined(PRE_ADD_HEARTBEAT_TO_SQL)
#if defined(PRE_ADD_WORLD_EVENT)
	int QueryEventList(TQEventList * pQ, TAEventList * pA);
#endif //#if defined(PRE_ADD_WORLD_EVENT)
#if defined( PRE_ADD_NEWCOMEBACK )
	int QueryModComebackFlag(UINT nAccountDBID, bool bRewardComeback);
#endif
#if defined( PRE_WORLDCOMBINE_PVP )
	int QueryUpdateWorldPvPRoom(TQUpdateWorldPvPRoom* pQ, TAUpdateWorldPvPRoom *pA);
#endif

	int QueryGetAccountID(const char* uname);
	int QueryCheckLogin(CSCheckLoginTW *pLogin);
};
