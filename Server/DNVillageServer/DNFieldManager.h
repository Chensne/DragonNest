#pragma once

#include "DNField.h"

typedef vector<CDNField*> TVecFields;

class CDNFieldData;
class CDNFieldManager
{
private:
	TVecFields m_pFields;
	volatile bool m_bIsInit;
	std::map<int,bool> m_mChannelShow;
	CSyncLock m_ChannleShowLock;

public:
	CDNFieldManager(void);
	virtual ~CDNFieldManager(void);

	void DoUpdate(DWORD CurTick);

	bool Init();
	void Final();

	bool AddField(int nChannelID, int nChannAtt, int nChannelMaxUser, int nMapIdx, const WCHAR * szMapName);
	bool DelField(int nMapIndex);
	bool bIsIncludePvPLobby();

	void FinalUser( CDNBaseObject* pObj );

	CDNField * GetFieldByIdx(int idx);

	CDNField* GetField(int nChannelID);
	CDNField * GetFieldByMapIdx(int nMapIdx);
	CDNNpcObject* GetNpcObjectByNpcObjID(int nChannelID, UINT nNpcObjID);

	void SendFieldNotice(int nMapIndex, int cLen, const WCHAR *pwszChatMsg, int nShowSec);
	void SendChannelNotice(int nChannelID, int nLen, const WCHAR * pszMsg, int nShowSec);

	void SendChannelChat(int nChannelID, int cLen, const WCHAR *pwszCharacterName, const WCHAR *pwszChatMsg, int nRet = ERROR_NONE);
	void SendDebugChat(int nChannelID, const WCHAR* szMsg);

	void UpdateChannelShowInfo( bool bForce=true );
#if defined(PRE_ADD_DWC)
	bool bIsIncludeDWCVillage();
#endif
};

extern CDNFieldManager* g_pFieldManager;
