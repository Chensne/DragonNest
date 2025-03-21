#pragma once

class CSyncLock;
class CDNChatRoom;

class CDNChatRoomManager
{
private:
	std::vector <CDNChatRoom*>		m_pVecChatRoomList;
	std::map <UINT, CDNChatRoom*>	m_pMapChatRoomList;

	CSyncLock	m_Sync;

	UINT		m_nLastRoomID;

public:
	CDNChatRoomManager();
	virtual ~CDNChatRoomManager();

	CDNChatRoom * GetChatRoom(UINT ChatRoomID);

	int CreateChatRoom( UINT uLeaderAID, WCHAR * pwzRoomName, BYTE nRoomType, USHORT uEnterAllow, WCHAR * pwzPassword,
						WCHAR * PRLine1, WCHAR * PRLine2, WCHAR * PRLine3 );
	void DestroyChatRoom( UINT nChatRoomID );

private:
	UINT _GetNewRoomID();
};

extern CDNChatRoomManager * g_pChatRoomManager;