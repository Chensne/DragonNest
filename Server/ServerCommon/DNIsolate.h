
#pragma once
#include "DNServerDef.h"
#include "Util.h"

class CDNUserSession;
class CDNUserSendManager;
class CDNIsolate : public TBoostMemoryPool<CDNIsolate>
{
public:
	CDNIsolate(CDNUserSendManager * pManager);
	~CDNIsolate();

	bool AddIsolateItem(const TIsolateItem * pItem);
	bool DelIsolateItem(const WCHAR * pDelitem);
	bool DelIsolateItem(INT64 biCharacterDBID);
	
	int GetIsolateCount() { return (int)m_IsolateList.size(); }
	bool IsIsolateItem(const WCHAR * pName);
	INT64 GetIsolateCharDBID(const WCHAR * pName);
	bool GetIsolateChrName(INT64 biIsolateDBID, WCHAR * pName);

	//for send
	void GetIsolateList(SCIsolateList * pPacket, int &nSize);

private:
	CDNUserSendManager * m_pSendManager;
	std::map <std::wstring, TIsolateItem*> m_IsolateList;
};