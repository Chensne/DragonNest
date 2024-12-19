
#pragma once

#include <vector>
#include "DNServerDef.h"

class CDNUserSession;
class CDNUserSendManager;

class CDNFriend : public TBoostMemoryPool<CDNFriend>
{
public:
	CDNFriend(CDNUserSendManager * pManager);
	~CDNFriend();

	int GetFriendCount() { return (int)m_FriendList.size(); }
	bool AddGroup(UINT nGroupDBID, const WCHAR * pGroupName);
	bool DelGroup(UINT nGroupDBID);

	bool AddFriend(UINT nFriendAccountDBID, INT64 biFriendCharacterDBID, const WCHAR * pName, UINT nGroupID, bool bMobileAuthentication);
	bool DelFriend(INT64 biFriendCharacterDBID);
	INT64 DelFriend(const WCHAR * pwszFriendCharacterName);

	bool UpdateFriend(INT64 biFriendCharacterDBID, UINT nGroupDBID, const WCHAR * pMemo);
	bool UpdateGroup(UINT nGroupDBID, const WCHAR * pGroupName);

	void SendFriendList();
	void SendFriendLocation();

	bool HasFriend(INT64 biFriendCharacterDBID);
	bool HasFriend(const WCHAR * pName);

	TFriendGroup * GetGroup(UINT nGroupDBID);
	TFriendGroup * GetGroup(const WCHAR * pName);

	TFriend * GetFriend(INT64 biFriendCharDBID);
	TFriend * GetFriend(const WCHAR * pName);

private:
	CDNUserSendManager * m_pUserSendManager;

	std::vector <TFriendGroup*> m_FriendGroupList;
	std::vector <TFriend*> m_FriendList;
	CSyncLock m_Sync;

	TFriendGroup * GetGroupAsync(UINT nGroupDBID);
};