#pragma once

class CDNMailSender
{
public:
	static bool Process( CDNUserSession* pSession, int iMailID, BYTE cPayMethodCode = 0, INT64 biFKey = 0 );
	static bool Process( INT64 biReceiveCharacterDBID, BYTE cWorldID, int iMailID, WCHAR* pwszName=NULL, BYTE cPayMethodCode = 0, INT64 biFKey = 0 );
#ifdef PRE_ADD_JOINGUILD_SUPPORT
	static bool Process(UINT nReceiveAccountDBID, INT64 biReceiveCharacterDBID, UINT nAchivementAccountDBID, INT64 biAchievementCharacterDBID, WCHAR* pwszName, BYTE cWorldSetID, int iMailID);
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
	static void ModifyGiftData(TGiftInfo *pGiftList, int nCount, char cSelectedLang=0);

private:
#if defined(PRE_MAILRENEWAL)
	static void MakeMailData(TMailTableData* pData, std::wstring& wstrTitle, std::wstring& wstrFrom, std::wstring& wstrText, std::vector<TItem> &VecList, std::vector<TItem> &VecCashList, int* pItemSNList, bool &bNewFlag, CDNUserSession* pSession=NULL);
#else	// #if defined(PRE_MAILRENEWAL)
	static void MakeMailData( TMailTableData* pData, std::wstring& wstrTitle, std::wstring& wstrFrom, std::wstring& wstrText, TItem* pList, int* pItemSNList, bool &bNewFlag, CDNUserSession* pSession=NULL );
#endif	// #if defined(PRE_MAILRENEWAL)
};
