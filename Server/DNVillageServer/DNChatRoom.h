#pragma once

class CDNUserSession;
class CDNUserSendManager;

class CDNChatRoom : public TBoostMemoryPool< CDNChatRoom >
{
private:
	UINT 			m_nChatRoomID;

	UINT 			m_nChatRoomMemberArray[CHATROOMMAX];	// 채팅방 참여 캐릭터 AccountDBID로 관리된다
	UINT 			m_nLeaderAccountDBID;					// 방장 캐릭터

	BYTE			m_nChatRoomType;
	USHORT			m_nChatRoomAllow;

	std::wstring	m_wstrChatRoomName;		// 채팅방 이름
	std::wstring	m_wstrChatRoomPW;		// 채팅방 패스워드

	std::wstring	m_wstrChatRoomPRLine1;	// 채팅방 PR 메시지 1
	std::wstring	m_wstrChatRoomPRLine2;	// 채팅방 PR 메시지 2
	std::wstring	m_wstrChatRoomPRLine3;	// 채팅방 PR 메시지 3

	std::vector <UINT> m_VecKickedList;	//영구킥리스트

public:
	CDNChatRoom( UINT nChatRoomID, WCHAR * pwzRoomName, BYTE nRoomType, USHORT nRoomAllow, WCHAR * pwzPassword,
				 WCHAR * pwzPRLine1, WCHAR * pwzPRLine2, WCHAR * pwzPRLine3 );
	virtual ~CDNChatRoom();

	UINT GetChatRoomID()			{	return m_nChatRoomID;					}
	UINT GetLeaderAID()				{	return m_nLeaderAccountDBID;			}
	const WCHAR * GetChatRoomName()	{	return m_wstrChatRoomName.c_str();		}
	BYTE GetChatRoomType()			{	return m_nChatRoomType;					}

	const WCHAR * GetChatRoomPRLine1()	{	return m_wstrChatRoomPRLine1.c_str();		}
	const WCHAR * GetChatRoomPRLine2()	{	return m_wstrChatRoomPRLine2.c_str();		}
	const WCHAR * GetChatRoomPRLine3()	{	return m_wstrChatRoomPRLine3.c_str();		}

	UINT GetMemberAIDFromIndex( int nIndex );

	bool UsePassword()				{	return (m_wstrChatRoomPW.length() > 0);	}

	int GetUserCount();

	bool IsLeader( UINT nUserAID );
	int LeaveUser( UINT nUserAID, int nReason );

	int EnterRoom( UINT nUserAID, WCHAR * pwzPassword, bool bLeader );

	void SendChatMessageToAll( int cLen, const WCHAR *pwszCharacterName, const WCHAR *pwszChatMsg );
	void SendProfileToAll( int nSessionID, TProfile & profile );

	void ChangeRoomOption( WCHAR * pwzRoomName, BYTE nRoomType, USHORT nRoomAllow, WCHAR * pwzPassword,
						   WCHAR * pwzPRLine1, WCHAR * pwzPRLine2, WCHAR * pwzPRLine3 );

protected:
	int _CanEnterRoom( UINT nUserAID, WCHAR * pwzPassword, bool bLeader );

	int _FindUserFromAID( UINT nUserAID );
	int _FindEmptyIndex();
};