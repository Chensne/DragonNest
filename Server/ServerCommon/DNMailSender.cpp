
#include "Stdafx.h"
#include "DNMailSender.h"
#include "DNGameDataManager.h"
#include "DNDBConnection.h"
#include "DNUserSession.h"
#include "EtUIXML.h"
#include "DNDBConnectionManager.h"

#if defined(PRE_MAILRENEWAL)
void CDNMailSender::MakeMailData(TMailTableData* pData, std::wstring& wstrTitle, std::wstring& wstrFrom, std::wstring& wstrText, std::vector<TItem> &VecList, std::vector<TItem> &VecCashList, int* pItemSNList, bool &bNewFlag, CDNUserSession* pSession/*=NULL*/)
#else	// #if defined(PRE_MAILRENEWAL)
void CDNMailSender::MakeMailData(TMailTableData* pData, std::wstring& wstrTitle, std::wstring& wstrFrom, std::wstring& wstrText, TItem* pList, int* pItemSNList, bool &bNewFlag, CDNUserSession* pSession/*=NULL*/)
#endif	// #if defined(PRE_MAILRENEWAL)
{
	if (!pData) return;

	switch( pData->Code )
	{
		case DBDNWorldDef::MailTypeCode::Normal:
		case DBDNWorldDef::MailTypeCode::Mission:
		case DBDNWorldDef::MailTypeCode::EventMail:
		case DBDNWorldDef::MailTypeCode::VIP:
		case DBDNWorldDef::MailTypeCode::MasterSystem:
		case DBDNWorldDef::MailTypeCode::NpcReputaion:
		case DBDNWorldDef::MailTypeCode::SpecialBox_Account:
		case DBDNWorldDef::MailTypeCode::SpecialBox_Character:
		case DBDNWorldDef::MailTypeCode::AppellationBookReward:
		{
			wstrTitle = boost::io::str( boost::wformat(L"%d") % pData->nTitleUIStringIndex );
			wstrText = boost::io::str( boost::wformat(L"%d") % pData->nTextUIStringIndex );
			bNewFlag = true;
			break;
		}
		case DBDNWorldDef::MailTypeCode::GuildWarReward:
			{
#if defined(PRE_ADD_MULTILANGUAGE)
				wstrTitle = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pData->nTitleUIStringIndex ? pData->nTitleUIStringIndex : 1976, pSession ? pSession->m_eSelectedLanguage : MultiLanguage::eDefaultLanguage );
				// ���⼭ ��������Ʈ�� �������Ʈ �Է�
				if( pSession )
				{					
					wstrText = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pData->nTextUIStringIndex ? pData->nTextUIStringIndex : 1976, pSession ? pSession->m_eSelectedLanguage : MultiLanguage::eDefaultLanguage ),
						pSession->GetGuildWarRewardFestivalPoint(), pSession->GetGuildWarRewardGuildPoint());
					bNewFlag = false;				
				}
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
				wstrTitle = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pData->nTitleUIStringIndex ? pData->nTitleUIStringIndex : 1976 );
				// ���⼭ ��������Ʈ�� �������Ʈ �Է�
				if( pSession )
				{					
					wstrText = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pData->nTextUIStringIndex ? pData->nTextUIStringIndex : 1976 ),
						pSession->GetGuildWarRewardFestivalPoint(), pSession->GetGuildWarRewardGuildPoint());
					bNewFlag = false;				
				}
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
			}
			break;
		default:
		{
#if defined(PRE_ADD_MULTILANGUAGE)
			wstrTitle = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pData->nTitleUIStringIndex ? pData->nTitleUIStringIndex : 1976, pSession ? pSession->m_eSelectedLanguage : MultiLanguage::eDefaultLanguage );
			wstrText = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pData->nTextUIStringIndex ? pData->nTextUIStringIndex : 1976, pSession ? pSession->m_eSelectedLanguage : MultiLanguage::eDefaultLanguage );
#else	//#if defined(PRE_ADD_MULTILANGUAGE)
			wstrTitle = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pData->nTitleUIStringIndex ? pData->nTitleUIStringIndex : 1976 );
			wstrText = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pData->nTextUIStringIndex ? pData->nTextUIStringIndex : 1976 );
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
			bNewFlag = false;
			break;
		}
	}

#if defined(PRE_ADD_MULTILANGUAGE)
	wstrFrom = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pData->nSenderUIStringIndex ? pData->nSenderUIStringIndex : 5121, pSession ? pSession->m_eSelectedLanguage : MultiLanguage::eDefaultLanguage );
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	wstrFrom = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pData->nSenderUIStringIndex ? pData->nSenderUIStringIndex : 5121 );
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

	for( int i=0; i<MAILATTACHITEMMAX; i++ ) 
	{
#if defined(PRE_MAILRENEWAL)
		if (pData->IsCash){
			if( pData->ItemSNArr[i] <= 0 ) continue;

			pItemSNList[i] = pData->ItemSNArr[i];

			TCashCommodityData CashData;
			TCashPackageData PackageData;

			memset(&PackageData, 0, sizeof(TCashPackageData));
			bool bPackage = g_pDataManager->GetCashPackageData(pData->ItemSNArr[i], PackageData);
			if (bPackage){
				for (int j = 0; j < (int)PackageData.nVecCommoditySN.size(); j++){
					memset(&CashData, 0, sizeof(TCashCommodityData));
					bool bRet = g_pDataManager->GetCashCommodityData(PackageData.nVecCommoditySN[j], CashData);
					if (!bRet) continue;

					TItem AddItem = {0,};
					if (CDNUserItem::MakeCashItemStruct(CashData.nSN, CashData.nItemID[0], AddItem, -1, 0) == ERROR_NONE){
						VecCashList.push_back(AddItem);
					}
				}
			}
			else{
				memset(&CashData, 0, sizeof(TCashCommodityData));
				bool bRet = g_pDataManager->GetCashCommodityData(pData->ItemSNArr[i], CashData);
				if (!bRet) continue;

				TItem AddItem = {0,};
				if (CDNUserItem::MakeCashItemStruct(CashData.nSN, CashData.nItemID[0], AddItem, -1, 0) == ERROR_NONE){
					VecCashList.push_back(AddItem);
				}
			}			
		}
		else{
			if (pData->ItemIDArr[i] > 0 && pData->ItemCountArr[i] > 0)
			{
				TItem AddItem = {0,};
				if (CDNUserItem::MakeItemStruct(pData->ItemIDArr[i], AddItem) == true){
					VecList.push_back(AddItem);
				}
			}
		}
#else	// #if defined(PRE_MAILRENEWAL)
		if (pData->IsCash){
			if( pData->ItemSNArr[i] <= 0 ) continue;
			pItemSNList[i] = pData->ItemSNArr[i];
		}
		else{
			if( pData->ItemIDArr[i] > 0 && pData->ItemCountArr[i] > 0 )
			{
				if( CDNUserItem::MakeItemStruct( pData->ItemIDArr[i], pList[i] ) == false )
					pList[i].nItemID = 0;
				else
				{
					pList[i].wCount = pData->ItemCountArr[i];
				}
			}
		}
#endif	// #if defined(PRE_MAILRENEWAL)
	}
}

bool CDNMailSender::Process( CDNUserSession* pSession, int iMailID, BYTE cPayMethodCode/* = 0*/, INT64 biFKey/* = 0*/ )
{
	if( !pSession )
		return false;

#if defined( PRE_ADD_DWC )
	if( AccountLevel_DWC == pSession->GetAccountLevel() )
		return false;
#endif // #if defined( PRE_ADD_DWC )

	TMailTableData* pData = g_pDataManager->GetMailTableData( iMailID );
	if( pData == NULL )
		return false;

	CDNDBConnection* pDBCon = pSession->GetDBConnection();
	if( pDBCon == NULL || pDBCon->GetActive() == false )
		return false;

	std::wstring wstrTitle, wstrFrom, wstrText;
	bool bNewFlag = true;

	int nItemSNArray[MAILATTACHITEMMAX] = { 0, };
#if defined(PRE_MAILRENEWAL)
	std::vector<TItem> VecMailItemList, VecMailCashItemList;
	VecMailItemList.clear();
	VecMailCashItemList.clear();
	MakeMailData(pData, wstrTitle, wstrFrom, wstrText, VecMailItemList, VecMailCashItemList, nItemSNArray, bNewFlag, pSession);
#else	// #if defined(PRE_MAILRENEWAL)
	TItem AttachItemArray[MAILATTACHITEMMAX] = { 0, };
	MakeMailData( pData, wstrTitle, wstrFrom, wstrText, AttachItemArray, nItemSNArray, bNewFlag, pSession );
#endif	// #if defined(PRE_MAILRENEWAL)

	switch( pData->Code )
	{
#if defined(PRE_SPECIALBOX)
	case DBDNWorldDef::MailTypeCode::SpecialBox_Account:
	case DBDNWorldDef::MailTypeCode::SpecialBox_Character:
		{
			std::vector<TSpecialBoxItemInfo> VecItemList;
			VecItemList.clear();
			if (pData->nKeepBoxType > 0){
				TKeepBoxProvideItemData *pProvideItem = g_pDataManager->GetKeepBoxProvideItemData(pData->nKeepBoxType);
				if (pProvideItem){
					for (int i = 0; i < (int)pProvideItem->VecProvideItem.size(); i++){
						TSpecialBoxItemInfo AddItem = {0,};
						if (pProvideItem->VecProvideItem[i].nCashSN > 0){
							AddItem.bCashItem = true;
							AddItem.RewardItem.nItemID = pProvideItem->VecProvideItem[i].nCashSN;
							AddItem.RewardItem.cOption = pProvideItem->VecProvideItem[i].nOption;
						}
						if (pProvideItem->VecProvideItem[i].nItemID > 0){
							CDNUserItem::MakeItemStruct(pProvideItem->VecProvideItem[i].nItemID, AddItem.RewardItem, 0, pProvideItem->VecProvideItem[i].nOption);

							AddItem.bCashItem = false;
							AddItem.RewardItem.wCount = pProvideItem->VecProvideItem[i].nCount;
							AddItem.RewardItem.cSealCount = pProvideItem->VecProvideItem[i].nSealCount;
						}

						VecItemList.push_back(AddItem);
					}
				}
			}

#if defined(PRE_MAILRENEWAL)
			if (!VecMailItemList.empty()){
				for (int i = 0; i < (int)VecMailItemList.size(); i++){
					TSpecialBoxItemInfo AddItem = {0,};
					AddItem.bCashItem = false;
					AddItem.RewardItem = VecMailItemList[i];
					VecItemList.push_back(AddItem);
				}
			}

#else	// #if defined(PRE_MAILRENEWAL)
			if (AttachItemArray[0].nItemID > 0){
				for (int i = 0; i < MAILATTACHITEMMAX; i++){
					if (AttachItemArray[i].nItemID <= 0) continue;

					TSpecialBoxItemInfo AddItem = {0,};
					AddItem.bCashItem = false;
					AddItem.RewardItem = AttachItemArray[i];
					VecItemList.push_back(AddItem);
				}
			}
#endif	// #if defined(PRE_MAILRENEWAL)

			if (nItemSNArray[0] > 0){
				for (int i = 0; i < MAILATTACHITEMMAX; i++){
					if (nItemSNArray[i] <= 0) continue;

					TSpecialBoxItemInfo AddItem = {0,};
					AddItem.bCashItem = true;
					AddItem.RewardItem.nItemID = nItemSNArray[i];
					VecItemList.push_back(AddItem);
				}
			}

			int nTargetType = SpecialBox::TargetTypeCode::AccountSelect;
			if (pData->Code == DBDNWorldDef::MailTypeCode::SpecialBox_Character)
				nTargetType = SpecialBox::TargetTypeCode::CharacterSelect;

			pDBCon->QueryAddEventReward(pSession, pData->nKeepBoxReceive, nTargetType, pData->nKeepBoxClass, pData->nKeepBoxLevelMin, pData->nKeepBoxLevelMax, pData->nTitleUIStringIndex, 
				pData->nSenderUIStringIndex, pData->nTextUIStringIndex, pData->nPresentMoney, DBDNWorldDef::EventRewardType::System, VecItemList);
		}
		break;
#endif	// #if defined(PRE_SPECIALBOX)
	default:
		{
#if defined(PRE_MAILRENEWAL)
			if (!VecMailItemList.empty()){
				TItem AttachItemArray[MAILATTACHITEMMAX] = { 0, };
				for (int i = 0; i < (int)VecMailItemList.size(); i++){
					AttachItemArray[i] = VecMailItemList[i];
				}

				pDBCon->QuerySendSystemMail(pSession->GetDBThreadID(), pSession->GetWorldSetID(), pSession->GetAccountDBID(), const_cast<WCHAR*>(wstrFrom.c_str()), pSession->GetCharacterDBID(), pData->Code, 0, 
					const_cast<WCHAR*>(wstrTitle.c_str()), const_cast<WCHAR*>(wstrText.c_str()), pData->nPresentMoney, AttachItemArray, pSession->GetChannelID(), pSession->GetMapIndex(), bNewFlag);
			}

			if (!VecMailCashItemList.empty()){
				int nTotalSize = (int)VecMailCashItemList.size();
				int nStart = 0, nEnd = std::min<int>(MAILATTACHITEMMAX, nTotalSize);

				while(true){
					TItem AttachItemArray[MAILATTACHITEMMAX] = { 0, };
					int nCount = 0;
					for (int i = nStart; i < nEnd; i++){
						AttachItemArray[nCount] = VecMailCashItemList[i];
						nCount++;
					}

					pDBCon->QuerySendSystemMail(pSession->GetDBThreadID(), pSession->GetWorldSetID(), pSession->GetAccountDBID(), const_cast<WCHAR*>(wstrFrom.c_str()), pSession->GetCharacterDBID(), pData->Code, 0, 
						const_cast<WCHAR*>(wstrTitle.c_str()), const_cast<WCHAR*>(wstrText.c_str()), pData->nPresentMoney, AttachItemArray, pSession->GetChannelID(), pSession->GetMapIndex(), bNewFlag);

					if (nTotalSize <= nEnd)
						break;

					nStart = nEnd;
					nEnd = std::min<int>(nEnd + MAILATTACHITEMMAX, nTotalSize);
				}
			}

#else	// #if defined(PRE_MAILRENEWAL)
			if (pData->IsCash)
			{
				switch (cPayMethodCode)
				{
					case DBDNWorldDef::PayMethodCode::Coin:				// 1: ����
					case DBDNWorldDef::PayMethodCode::Cash:				// 2: ĳ��
					case DBDNWorldDef::PayMethodCode::Petal:			// 3: ������
					case DBDNWorldDef::PayMethodCode::Coupon:			// 4: ����
					case DBDNWorldDef::PayMethodCode::Admin:			// 5: ������ ����
					case DBDNWorldDef::PayMethodCode::Quest:			// 6: ����Ʈ����
					case DBDNWorldDef::PayMethodCode::Mission:			// 7: �̼Ǻ���
					case DBDNWorldDef::PayMethodCode::LevelupEvent:		// 8=ĳ���� ���� �� ����
					case DBDNWorldDef::PayMethodCode::VIP:				// 9 : VIP ����
					case DBDNWorldDef::PayMethodCode::PvP:				// 10 : PVP ���� ����Ʈ
					case DBDNWorldDef::PayMethodCode::Union_Commerical:	// 11 : ���� ���� ����Ʈ
					case DBDNWorldDef::PayMethodCode::Union_Royal:		// 12 : �ռ� ���� ����Ʈ
					case DBDNWorldDef::PayMethodCode::Union_Liberty:	// 13 : ���� ���� ����Ʈ
					case DBDNWorldDef::PayMethodCode::Cash_NexonUSA:	// 14 : �ؽ��Ƹ޸�ī ĳ��
					case DBDNWorldDef::PayMethodCode::Event:			// 15 : �̺�Ʈ
					case DBDNWorldDef::PayMethodCode::GuildWar_Festival:// 16 : ����� ���� ����Ʈ
					case DBDNWorldDef::PayMethodCode::GuildWar_Reward:	// 17 : ����� ����
					case DBDNWorldDef::PayMethodCode::Comeback:			// 19 : �������� ����
					case DBDNWorldDef::PayMethodCode::BeginnerGuild:	// 21 : �ʺ���� ����
					{
						pDBCon->QueryMakeGift(pSession->GetDBThreadID(), pSession->GetWorldSetID(), pSession->GetAccountDBID(), pSession->GetCharacterDBID(), pSession->IsPCBang(), const_cast<WCHAR*>(wstrText.c_str()), 
							cPayMethodCode, static_cast<int>(biFKey), const_cast<char*>(pSession->GetIp()), nItemSNArray, bNewFlag);
					}
					break;			
				}
			}
			else
			{
				pDBCon->QuerySendSystemMail( pSession->GetDBThreadID(), pSession->GetWorldSetID(), pSession->GetAccountDBID(), const_cast<WCHAR*>(wstrFrom.c_str()), pSession->GetCharacterDBID(), pData->Code, 0, 
					const_cast<WCHAR*>(wstrTitle.c_str()), const_cast<WCHAR*>(wstrText.c_str()), pData->nPresentMoney, AttachItemArray, pSession->GetChannelID(), pSession->GetMapIndex(), bNewFlag );
			}
#endif	// #if defined(PRE_MAILRENEWAL)
		}
	}

	return true;
}

bool CDNMailSender::Process( INT64 biReceiveCharacterDBID, BYTE cWorldSetID, int iMailID, WCHAR* pwszSender/*=NULL*/, BYTE cPayMethodCode/* = 0*/, INT64 biFKey/* = 0*/ )
{
	TMailTableData* pData = g_pDataManager->GetMailTableData( iMailID );
	if( pData == NULL )
		return false;

	BYTE cThreadID = 0;
	CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
	if( pDBCon == NULL || pDBCon->GetActive() == false )
		return false;

	std::wstring wstrTitle,wstrFrom,wstrText;
	bool bNewFlag = true;
	int nItemSNArray[MAILATTACHITEMMAX] = { 0, };

#if defined(PRE_MAILRENEWAL)
	std::vector<TItem> VecMailItemList, VecMailCashItemList;
	VecMailItemList.clear();
	VecMailCashItemList.clear();
	MakeMailData(pData, wstrTitle, wstrFrom, wstrText, VecMailItemList, VecMailCashItemList, nItemSNArray, bNewFlag, NULL);

	if (pwszSender)
		wstrFrom = pwszSender;

	if (!VecMailItemList.empty()){
		TItem AttachItemArray[MAILATTACHITEMMAX] = { 0, };
		for (int i = 0; i < (int)VecMailItemList.size(); i++){
			AttachItemArray[i] = VecMailItemList[i];
		}

		pDBCon->QuerySendSystemMail(cThreadID, cWorldSetID, 0, const_cast<WCHAR*>(wstrFrom.c_str()), biReceiveCharacterDBID, pData->Code, 0, const_cast<WCHAR*>(wstrTitle.c_str()),
			const_cast<WCHAR*>(wstrText.c_str()), pData->nPresentMoney, AttachItemArray, 0, 0, bNewFlag);
	}

	if (!VecMailCashItemList.empty()){
		int nTotalSize = (int)VecMailCashItemList.size();
		int nStart = 0, nEnd = std::min<int>(MAILATTACHITEMMAX, nTotalSize);

		while(true){
			TItem AttachItemArray[MAILATTACHITEMMAX] = { 0, };
			int nCount = 0;
			for (int i = nStart; i < nEnd; i++){
				AttachItemArray[nCount] = VecMailCashItemList[i];
			}

			pDBCon->QuerySendSystemMail(cThreadID, cWorldSetID, 0, const_cast<WCHAR*>(wstrFrom.c_str()), biReceiveCharacterDBID, pData->Code, 0, const_cast<WCHAR*>(wstrTitle.c_str()),
				const_cast<WCHAR*>(wstrText.c_str()), pData->nPresentMoney, AttachItemArray, 0, 0, bNewFlag);

			if (nTotalSize <= nEnd)
				break;

			nStart = nEnd;
			nEnd = std::min<int>(nEnd + MAILATTACHITEMMAX, nTotalSize);
		}
	}

#else	// #if defined(PRE_MAILRENEWAL)
	TItem AttachItemArray[MAILATTACHITEMMAX] = { 0, };
	MakeMailData( pData, wstrTitle, wstrFrom, wstrText, AttachItemArray, nItemSNArray, bNewFlag, NULL );

	if (pData->IsCash){
		switch (cPayMethodCode)
		{
		case DBDNWorldDef::PayMethodCode::Coin:				// 1: ����
		case DBDNWorldDef::PayMethodCode::Cash:				// 2: ĳ��
		case DBDNWorldDef::PayMethodCode::Petal:			// 3: ������
		case DBDNWorldDef::PayMethodCode::Coupon:			// 4: ����
		case DBDNWorldDef::PayMethodCode::Admin:			// 5: ������ ����
		case DBDNWorldDef::PayMethodCode::Quest:			// 6: ����Ʈ����
		case DBDNWorldDef::PayMethodCode::Mission:			// 7: �̼Ǻ���
		case DBDNWorldDef::PayMethodCode::LevelupEvent:		// 8=ĳ���� ���� �� ����
		case DBDNWorldDef::PayMethodCode::VIP:				// 9 : VIP ����
		case DBDNWorldDef::PayMethodCode::PvP:				// 10 : PVP ���� ����Ʈ
		case DBDNWorldDef::PayMethodCode::Union_Commerical:	// 11 : ���� ���� ����Ʈ
		case DBDNWorldDef::PayMethodCode::Union_Royal:		// 12 : �ռ� ���� ����Ʈ
		case DBDNWorldDef::PayMethodCode::Cash_NexonUSA:	// 14 : �ؽ��Ƹ޸�ī ĳ��
		case DBDNWorldDef::PayMethodCode::Event:			// 15 : �̺�Ʈ
		case DBDNWorldDef::PayMethodCode::GuildWar_Festival:// 16 : ����� ���� ����Ʈ
		case DBDNWorldDef::PayMethodCode::GuildWar_Reward:	// 17 : ����� ����
			{
				pDBCon->QueryMakeGift(cThreadID, cWorldSetID, 0, biReceiveCharacterDBID, 0, const_cast<WCHAR*>(wstrText.c_str()), cPayMethodCode, static_cast<int>(biFKey), NULL, nItemSNArray, bNewFlag);
			}
			break;			
		}
	}
	else{
		if( pwszSender )
			wstrFrom = pwszSender;

		pDBCon->QuerySendSystemMail( cThreadID, cWorldSetID, 0, const_cast<WCHAR*>(wstrFrom.c_str()), biReceiveCharacterDBID, pData->Code, 0, 
			const_cast<WCHAR*>(wstrTitle.c_str()), const_cast<WCHAR*>(wstrText.c_str()), pData->nPresentMoney, 
			AttachItemArray, 0, 0, bNewFlag );
	}
#endif	// #if defined(PRE_MAILRENEWAL)

	return true;
}

#ifdef PRE_ADD_JOINGUILD_SUPPORT
bool CDNMailSender::Process(UINT nReceiveAccountDBID, INT64 biReceiveCharacterDBID, UINT nAchivementAccountDBID, INT64 biAchievementCharacterDBID, WCHAR* pwszName, BYTE cWorldSetID, int iMailID)
{
	if( !pwszName )
		return false;

	TMailTableData* pData = g_pDataManager->GetMailTableData( iMailID );
	if( pData == NULL )
		return false;

	BYTE cThreadID = 0;
	CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
	if( pDBCon == NULL || pDBCon->GetActive() == false )
		return false;

	std::wstring wstrTitle, wstrFrom, wstrText;
	int nItemSNArray[MAILATTACHITEMMAX] = { 0, };
	bool bNewFlag = true;

#if defined(PRE_MAILRENEWAL)
	std::vector<TItem> VecMailItemList, VecMailCashItemList;
	VecMailItemList.clear();
	VecMailCashItemList.clear();
	MakeMailData(pData, wstrTitle, wstrFrom, wstrText, VecMailItemList, VecMailCashItemList, nItemSNArray, bNewFlag, NULL);
#else	// #if defined(PRE_MAILRENEWAL)
	TItem AttachItemArray[MAILATTACHITEMMAX] = { 0, };
	MakeMailData( pData, wstrTitle, wstrFrom, wstrText, AttachItemArray, nItemSNArray, bNewFlag, NULL );
#endif	// #if defined(PRE_MAILRENEWAL)

	if (pData->Code != DBDNWorldDef::MailTypeCode::GuildMaxLevelReward || (pData->Code == DBDNWorldDef::MailTypeCode::GuildMaxLevelReward && pData->IsCash))
	{
		_DANGER_POINT_MSG(L"���â��� ���°ǵ� ĳ������ �ִٴ�! �ȵ�!");
		return false;
	}

#if defined(PRE_MAILRENEWAL)
	if (!VecMailItemList.empty()){
		TItem AttachItemArray[MAILATTACHITEMMAX] = { 0, };
		for (int i = 0; i < (int)VecMailItemList.size(); i++){
			AttachItemArray[i] = VecMailItemList[i];
		}

		pDBCon->QuerySendGuildMail(cThreadID, cWorldSetID, const_cast<WCHAR*>(wstrFrom.c_str()), nReceiveAccountDBID, biReceiveCharacterDBID, nAchivementAccountDBID, biAchievementCharacterDBID, pData->Code, 0, 
			const_cast<WCHAR*>(wstrTitle.c_str()), const_cast<WCHAR*>(wstrText.c_str()), pData->nPresentMoney, AttachItemArray, 0, 0, bNewFlag);
	}
#else	// #if defined(PRE_MAILRENEWAL)
	pDBCon->QuerySendGuildMail( cThreadID, cWorldSetID, const_cast<WCHAR*>(wstrFrom.c_str()), nReceiveAccountDBID, biReceiveCharacterDBID, nAchivementAccountDBID, biAchievementCharacterDBID, pData->Code, 0, 
		const_cast<WCHAR*>(wstrTitle.c_str()), const_cast<WCHAR*>(wstrText.c_str()), pData->nPresentMoney, AttachItemArray, 0, 0, bNewFlag );
#endif	// #if defined(PRE_MAILRENEWAL)

	return true;
}
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT

void CDNMailSender::ModifyGiftData(TGiftInfo *pGiftList, int nCount, char cSelectedLang/*=0*/)
{
	if (nCount <= 0) return;

	for (int i = 0; i < nCount; i++){
		const WCHAR *pSenderName = NULL;
		switch (pGiftList[i].cPayMethodCode)
		{
		case DBDNWorldDef::PayMethodCode::Coin:
		case DBDNWorldDef::PayMethodCode::Cash:
		case DBDNWorldDef::PayMethodCode::Petal:
			break;

		case DBDNWorldDef::PayMethodCode::Coupon:
#if defined(PRE_ADD_MULTILANGUAGE)
			pSenderName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4685, cSelectedLang );	// UISTRING : ����
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
			pSenderName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4685 );	// UISTRING : ����
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
			break;

		case DBDNWorldDef::PayMethodCode::Admin:
#if defined(_US)
			if (pSenderName)
				wsprintf(pGiftList[i].wszSenderName, L"[%s]", pSenderName);
#else	// #if defined(_US)

#if defined(PRE_ADD_MULTILANGUAGE)
			pSenderName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4686, cSelectedLang );	// UISTRING : ���
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
			pSenderName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4686 );	// UISTRING : ���
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

#endif	// #if defined(_US)
			break;

		case DBDNWorldDef::PayMethodCode::Quest:
#if defined(PRE_ADD_MULTILANGUAGE)
			pSenderName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4687, cSelectedLang );	// UISTRING : ����Ʈ
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
			pSenderName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4687 );	// UISTRING : ����Ʈ
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
			break;

		case DBDNWorldDef::PayMethodCode::Mission:
#if defined(PRE_ADD_MULTILANGUAGE)
			pSenderName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4688, cSelectedLang );	// UISTRING : �̼�
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
			pSenderName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4688 );	// UISTRING : �̼�
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
			break;

		case DBDNWorldDef::PayMethodCode::LevelupEvent:
#if defined(PRE_ADD_MULTILANGUAGE)
			pSenderName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4686, cSelectedLang );	// UISTRING : ���
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
			pSenderName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4686 );	// UISTRING : ���
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
			break;

		case DBDNWorldDef::PayMethodCode::VIP:
#if defined(PRE_ADD_MULTILANGUAGE)
			pSenderName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2020000, cSelectedLang );	// UISTRING : VIP
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
			pSenderName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2020000 );	// UISTRING : VIP
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
			break;

		case DBDNWorldDef::PayMethodCode::Event:
#if defined(PRE_ADD_MULTILANGUAGE)
			pSenderName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3203, cSelectedLang ); // UISTRING : �̺�Ʈ
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
			pSenderName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3203 ); // UISTRING : �̺�Ʈ
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
			break;

		case DBDNWorldDef::PayMethodCode::GuildWar_Reward :
#if defined(PRE_ADD_MULTILANGUAGE)
			pSenderName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000015061, cSelectedLang ); // UISTRING : ������ �����
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
			pSenderName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000015061 ); // UISTRING : ������ �����
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
			break;

		default:
#if defined(PRE_ADD_MULTILANGUAGE)
			pSenderName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4686, cSelectedLang );	// UISTRING : ���
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
			pSenderName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4686 );	// UISTRING : ���
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
			break;
		}

#if defined(PRE_ADD_GIFT_RETURN)
		if (pGiftList[i].bGiftReturn)
		{
#if defined(PRE_ADD_MULTILANGUAGE)
			pSenderName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4597, cSelectedLang );		// UISTRING: �ݼ۵� ����
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
			pSenderName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4597 );		// UISTRING: �ݼ۵� ����
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
		}
#endif	// #if defined(PRE_ADD_GIFT_RETURN)

		if (pSenderName){
			wsprintf(pGiftList[i].wszSenderName, L"[%s]", pSenderName);
		}
	}
}
