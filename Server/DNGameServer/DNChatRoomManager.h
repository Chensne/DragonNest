#pragma once

class CDNChatRoom;
class CDNChatRoomManager
{
private:
	std::vector <CDNChatRoom*>		m_pVecChatRoomList;
	std::map <UINT, CDNChatRoom*>	m_pMapChatRoomList;

	UINT		m_nLastRoomID;

public:
	CDNChatRoomManager();
	virtual ~CDNChatRoomManager();

	CDNChatRoom * GetChatRoom(UINT ChatRoomID);

	int CreateChatRoom( UINT uLeaderAID, WCHAR * pwzRoomName, BYTE nRoomType, USHORT uEnterAllow, WCHAR * pwzPassword,
						WCHAR * PRLine1, WCHAR * PRLine2, WCHAR * PRLine3, CDNGameRoom* pGameRoom );
	void DestroyChatRoom( UINT nChatRoomID );

	void SendChatRoomInfo( CDNUserSession* pBreakIntoGameSession );


private:
	UINT _GetNewRoomID();
};
