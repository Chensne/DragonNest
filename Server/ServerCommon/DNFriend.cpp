
#include "stdafx.h"
#include "DNFriend.h"
#include "DNUserSendManager.h"
#include "DNWorldUserState.h"
#include "Log.h"

CDNFriend::CDNFriend(CDNUserSendManager * pManager)
{
	m_pUserSendManager = pManager;
}

CDNFriend::~CDNFriend()
{
	ScopeLock <CSyncLock> lock(m_Sync);

	std::vector <TFriend*>::iterator ii;
	for (ii = m_FriendList.begin(); ii != m_FriendList.end(); ii++)
		SAFE_DELETE((*ii));
}

bool CDNFriend::AddGroup(UINT nGroupDBID, const WCHAR * pGroupName)
{
	ScopeLock <CSyncLock> lock(m_Sync);

	std::vector <TFriendGroup*>::iterator ii;
	for (ii = m_FriendGroupList.begin(); ii != m_FriendGroupList.end(); ii++)
		if ((*ii)->nGroupDBID == nGroupDBID)
			return false;

	TFriendGroup * pGroup = new TFriendGroup;
	pGroup->nGroupDBID = nGroupDBID;
	_wcscpy(pGroup->wszGroupName, _countof(pGroup->wszGroupName), pGroupName, (int)wcslen(pGroupName));

	m_FriendGroupList.push_back(pGroup);
	return true;
}

bool CDNFriend::DelGroup(UINT nGroupDBID)
{
	ScopeLock <CSyncLock> lock(m_Sync);

	std::vector <TFriendGroup*>::iterator ii;
	for (ii = m_FriendGroupList.begin(); ii != m_FriendGroupList.end(); ii++)
	{
		if ((*ii)->nGroupDBID == nGroupDBID)
		{
			TFriendGroup * pGroup = (*ii);
			m_FriendGroupList.erase(ii);

			std::vector <TFriend*>::iterator iter;
			for (iter = m_FriendList.begin(); iter != m_FriendList.end(); iter++)
				if ((*iter)->nGroupDBID == nGroupDBID)
				{
					(*iter)->nGroupDBID = 0;
					memset((*iter)->wszGroupName, 0, sizeof((*iter)->wszGroupName));
				}

			SAFE_DELETE(pGroup);
			return true;
		}
	}
	return false;
}

bool CDNFriend::AddFriend(UINT nFriendAccountDBID, INT64 biFriendCharacterDBID, const WCHAR * pName, UINT nGroupID, bool bMobileAuthentication)
{
	if (HasFriend(biFriendCharacterDBID) == true)
		return false;		//Already Have

	ScopeLock <CSyncLock> lock(m_Sync);

	TFriend * pFriend = new TFriend;
	memset(pFriend, 0, sizeof(TFriend));

	pFriend->nFriendAccountDBID = nFriendAccountDBID;
	pFriend->biFriendCharacterDBID = biFriendCharacterDBID;
	_wcscpy(pFriend->wszCharacterName, _countof(pFriend->wszCharacterName), pName, (int)wcslen(pName));
#ifdef PRE_ADD_DOORS
	pFriend->bMobileAuthentication = bMobileAuthentication;
#endif		//#ifdef PRE_ADD_DOORS

	if (nGroupID > 0)
	{
		TFriendGroup * pGroup = GetGroupAsync(nGroupID);
		if (pGroup != NULL)
		{
			_wcscpy(pFriend->wszGroupName, _countof(pFriend->wszGroupName), pGroup->wszGroupName, (int)wcslen( pGroup->wszGroupName));
			pFriend->nGroupDBID = pGroup->nGroupDBID;
		}	else
		{
			//그룹이 존재해야하는ㄷ 소유한 그룹이 없는 거삼
			SAFE_DELETE(pFriend);
			_DANGER_POINT();
			return false;
		}
	}
	m_FriendList.push_back(pFriend);
	return true;
}


bool CDNFriend::DelFriend(INT64 biFriendCharacterDBID)
{
	ScopeLock <CSyncLock> lock(m_Sync);

	std::vector <TFriend*>::iterator ii;
	for (ii = m_FriendList.begin(); ii != m_FriendList.end(); ii++)
	{
		if ((*ii)->biFriendCharacterDBID == biFriendCharacterDBID)
		{
			TFriend * pFriend = (*ii);
			m_FriendList.erase(ii);
			SAFE_DELETE(pFriend);
			return true;
		}
	}
	return false;
}

INT64 CDNFriend::DelFriend(const WCHAR * pwszFriendCharacterName)
{
	ScopeLock <CSyncLock> lock(m_Sync);

	INT64 biCharacterDBID = 0;
	std::vector <TFriend*>::iterator ii;
	for (ii = m_FriendList.begin(); ii != m_FriendList.end(); ii++)
	{
		if (!wcscmp((*ii)->wszCharacterName, pwszFriendCharacterName))
		{
			TFriend * pFriend = (*ii);
			m_FriendList.erase(ii);

			if (pFriend != NULL)
			{
				biCharacterDBID = pFriend->biFriendCharacterDBID;
				SAFE_DELETE(pFriend);
			}
			return biCharacterDBID;
		}
	}
	return biCharacterDBID;
}

bool CDNFriend::UpdateFriend(INT64 biFriendCharacterDBID, UINT nGroupDBID, const WCHAR * pMemo)
{
	ScopeLock <CSyncLock> lock(m_Sync);

	std::vector <TFriend*>::iterator ii;
	for (ii = m_FriendList.begin(); ii != m_FriendList.end(); ii++)
	{
		if ((*ii)->biFriendCharacterDBID == biFriendCharacterDBID)
		{
			TFriendGroup * pGroup = NULL;
			if (nGroupDBID > 0)
				pGroup = GetGroupAsync(nGroupDBID);
			
			if (pGroup != NULL)
			{
				_wcscpy((*ii)->wszGroupName, _countof((*ii)->wszGroupName), pGroup->wszGroupName, (int)wcslen(pGroup->wszGroupName));
				(*ii)->nGroupDBID = nGroupDBID;
			}
			else
			{
				memset((*ii)->wszGroupName, 0, sizeof((*ii)->wszGroupName));
				(*ii)->nGroupDBID = 0;
			}
			if (pMemo != NULL && pMemo[0] != '\0' && pMemo[0] != 0)
				_wcscpy((*ii)->wszFriendMemo, _countof((*ii)->wszFriendMemo), pMemo, (int)wcslen(pMemo));
			return true;
		}
	}
	return false;
}

bool CDNFriend::UpdateGroup(UINT nGroupDBID, const WCHAR * pGroupName)
{
	ScopeLock <CSyncLock> lock(m_Sync);

	std::vector <TFriendGroup*>::iterator ii;
	for (ii = m_FriendGroupList.begin(); ii != m_FriendGroupList.end(); ii++)
	{
		if ((*ii)->nGroupDBID == nGroupDBID)
		{
			_wcscpy((*ii)->wszGroupName, _countof((*ii)->wszGroupName), pGroupName, (int)wcslen(pGroupName));
			return true;
		}
	}
	return false;
}

void CDNFriend::SendFriendList()
{
	{
		SCFriendGroupList packet;
		memset(&packet, 0, sizeof(packet));

		int nSize = 0;
		{
			ScopeLock <CSyncLock> lock(m_Sync);

			std::vector <TFriendGroup*>::iterator ii;
			for (ii = m_FriendGroupList.begin(); ii != m_FriendGroupList.end(); ii++)
			{
				packet.nGroupDBID[packet.cGroupCount] = (*ii)->nGroupDBID;
				if( (*ii)->wszGroupName[0] > 0 )
				{
					size_t len = wcslen( (*ii)->wszGroupName );
					if( len > FRIEND_GROUP_NAMELENMAX )
					{
						_DANGER_POINT();
						return;
					}

					packet.cGroupNameLen[packet.cGroupCount] = (BYTE)wcslen((*ii)->wszGroupName);
					wmemcpy_s( packet.wszBuf + nSize, _countof(packet.wszBuf)-nSize, (*ii)->wszGroupName, packet.cGroupNameLen[packet.cGroupCount]);
				}
				else
				{
					packet.cGroupNameLen[packet.cGroupCount] = 0;
				}

				nSize += packet.cGroupNameLen[packet.cGroupCount];
				packet.cGroupCount++;
				if (packet.cGroupCount >= FRIEND_GROUP_MAX)
					break;
			}
		}

		m_pUserSendManager->SendFriendGroupList(&packet, nSize);
	}

	{
		SCFriendList packet;
		memset(&packet, 0, sizeof(packet));

		{
			ScopeLock <CSyncLock> lock(m_Sync);

			std::vector <TFriend*>::iterator ii;
			for (ii = m_FriendList.begin(); ii != m_FriendList.end(); ii++)
			{
				//깨지는 건데 좀 너무 이쁘게 깨지고 정확한 다른 곳에서 터지라고 일단 넣어봄
				if ((*ii) == NULL) continue;

				sWorldUserState State;
				if (g_pWorldUserState->GetUserState((*ii)->wszCharacterName, (*ii)->biFriendCharacterDBID, &State))
				{
					packet.Info[packet.cCount].Location.cServerLocation = State.nLocationState;
					packet.Info[packet.cCount].Location.nChannelID = State.nLocationState == _LOCATION_VILLAGE ? State.nChannelID : -1;
					packet.Info[packet.cCount].Location.nMapIdx = State.nMapIdx;
				}
				else
					packet.Info[packet.cCount].Location.cServerLocation = _LOCATION_NONE;

				packet.Info[packet.cCount].biFriendCharacterDBID = (*ii)->biFriendCharacterDBID;
				packet.Info[packet.cCount].nGroupDBID = (*ii)->nGroupDBID;

				_wcscpy(packet.Info[packet.cCount].wszFriendName, _countof(packet.Info[packet.cCount].wszFriendName), (*ii)->wszCharacterName, (int)wcslen((*ii)->wszCharacterName));
				packet.cCount++;
				if (packet.cCount >= FRIEND_MAXCOUNT)
					break;
			}
		}

		m_pUserSendManager->SendFriendList(&packet);
	}
}

void CDNFriend::SendFriendLocation()
{
	ScopeLock <CSyncLock> lock(m_Sync);

	SCFriendLocationList packet;		//SC_FRIEND / SC_FRIEND_LOCATIONLIST
	memset(&packet, 0, sizeof(packet));

	std::vector <TFriend*>::iterator ii;
	for (ii = m_FriendList.begin(); ii != m_FriendList.end(); ii++)
	{
		sWorldUserState State;
		if (g_pWorldUserState->GetUserState((*ii)->wszCharacterName, (*ii)->biFriendCharacterDBID, &State))
		{
			packet.FriendLocation[packet.cCount].Location.cServerLocation = State.nLocationState;
			packet.FriendLocation[packet.cCount].Location.nChannelID = State.nLocationState == _LOCATION_VILLAGE ? State.nChannelID : -1;
			packet.FriendLocation[packet.cCount].Location.nMapIdx = State.nMapIdx;
		}
		else
			packet.FriendLocation[packet.cCount].Location.cServerLocation = _LOCATION_NONE;

		packet.FriendLocation[packet.cCount].nGroupDBID	= (*ii)->nGroupDBID;
		packet.FriendLocation[packet.cCount].biFriendCharacterDBID = (*ii)->biFriendCharacterDBID;
		packet.cCount++;
		if (packet.cCount >= FRIEND_MAXCOUNT)
		{
			_DANGER_POINT();
			break;
		}
	}

	m_pUserSendManager->SendFriendLocationList(&packet);
}

bool CDNFriend::HasFriend(INT64 biFriendCharacterDBID)
{
	ScopeLock <CSyncLock> lock(m_Sync);

	std::vector <TFriend*>::iterator ii;
	for (ii = m_FriendList.begin(); ii != m_FriendList.end(); ii++)
		if ((*ii)->biFriendCharacterDBID == biFriendCharacterDBID)
			return true;
	return false;
}

bool CDNFriend::HasFriend(const WCHAR * pName)
{
	ScopeLock <CSyncLock> lock(m_Sync);

	std::vector <TFriend*>::iterator ii;
	for (ii = m_FriendList.begin(); ii != m_FriendList.end(); ii++)
		if (!__wcsicmp_l((*ii)->wszCharacterName, pName))
			return true;
	return false;
}

TFriendGroup * CDNFriend::GetGroup(UINT nGroupDBID)
{
	ScopeLock <CSyncLock> lock(m_Sync);

	std::vector <TFriendGroup*>::iterator ii;
	for (ii = m_FriendGroupList.begin(); ii != m_FriendGroupList.end(); ii++)
		if ((*ii)->nGroupDBID == nGroupDBID)
			return (*ii);
	return NULL;
}

TFriendGroup * CDNFriend::GetGroup(const WCHAR * pName)
{
	ScopeLock <CSyncLock> lock(m_Sync);

	std::vector <TFriendGroup*>::iterator ii;
	for (ii = m_FriendGroupList.begin(); ii != m_FriendGroupList.end(); ii++)
		if (!_wcsicmp((*ii)->wszGroupName, pName))
			return (*ii);
	return NULL;
}

TFriend * CDNFriend::GetFriend(INT64 biFriendCharDBID)
{
	ScopeLock <CSyncLock> lock(m_Sync);

	std::vector <TFriend*>::iterator ii;
	for (ii = m_FriendList.begin(); ii != m_FriendList.end(); ii++)
		if ((*ii)->biFriendCharacterDBID == biFriendCharDBID)
			return (*ii);
	return NULL;
}

TFriend * CDNFriend::GetFriend(const WCHAR * pName)
{
	ScopeLock <CSyncLock> lock(m_Sync);

	std::vector <TFriend*>::iterator ii;
	for (ii = m_FriendList.begin(); ii != m_FriendList.end(); ii++)
		if (!__wcsicmp_l((*ii)->wszCharacterName, pName))
			return (*ii);
	return NULL;
}

TFriendGroup * CDNFriend::GetGroupAsync(UINT nGroupDBID)
{
	std::vector <TFriendGroup*>::iterator ii;
	for (ii = m_FriendGroupList.begin(); ii != m_FriendGroupList.end(); ii++)
		if ((*ii)->nGroupDBID == nGroupDBID)
			return (*ii);
	return NULL;
}