
#pragma once

#include "DNServerDef.h"

#if defined (PRE_ADD_BESTFRIEND)

class CDNUserSession;
class CDNUserSendManager;

class CDNBestFriend : public TBoostMemoryPool<CDNBestFriend>
{
public:
	CDNBestFriend(CDNUserSession* pSession);
	~CDNBestFriend();

	void LoadData(const TAGetBestFriend* pData);
	bool IsRegistered();
	void CalcRegistTime();
	void CheckReward();
	void CheckStatus(bool bCheat=false);
	void RemoveReward();
	void Close();
	void Reset();
	void ExecuteReward(int nItemID);
	void ChangeName(const WCHAR* pwszName);
	void ChangeMemo(bool bMine, const WCHAR* pwszName);
	void ChangeLevel(BYTE cLevel);
	void CheckAndSendData();

	TBestFriendInfo& GetInfo() {return m_Info;}

	void SetRegistSerial(INT64 biSerial) {m_biRegistSerial = biSerial;}
	INT64 GetRegistSerial() {return m_biRegistSerial;}

	void SendGetBestFriendInfo();

private:
	INT64 m_biRegistSerial;
	TBestFriendInfo m_Info;

	CDNUserSession * m_pUserSession;
};
#endif  // #if defined (PRE_ADD_BESTFRIEND)
