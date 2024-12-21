#pragma once

class CDNUserSession;
class CDNUserSendManager;
class CDNGameRoom;

class CDNChatRoom : public TBoostMemoryPool< CDNChatRoom >
{
private:
	UINT 			m_nChatRoomID;

	UINT 			m_nChatRoomMemberArray[CHATROOMMAX];	// ä�ù� ���� ĳ���� AccountDBID�� �����ȴ�
	UINT 			m_nLeaderSessionID;					// ���� ĳ����

	BYTE			m_nChatRoomType;
	USHORT			m_nChatRoomAllow;

	std::wstring	m_wstrChatRoomName;		// ä�ù� �̸�
	std::wstring	m_wstrChatRoomPW;		// ä�ù� �н�����

	std::wstring	m_wstrChatRoomPRLine1;	// ä�ù� PR �޽��� 1
	std::wstring	m_wstrChatRoomPRLine2;	// ä�ù� PR �޽��� 2
	std::wstring	m_wstrChatRoomPRLine3;	// ä�ù� PR �޽��� 3

	std::vector <UINT> m_VecKickedList;	//����ű����Ʈ

	CDNGameRoom*	m_pGameRoom;

public:
	CDNChatRoom( UINT nChatRoomID, WCHAR * pwzRoomName, BYTE nRoomType, USHORT nRoomAllow, WCHAR * pwzPassword,
				 WCHAR * pwzPRLine1, WCHAR * pwzPRLine2, WCHAR * pwzPRLine3 );
	virtual ~CDNChatRoom();

	UINT GetChatRoomID()			{	return m_nChatRoomID;					}
	UINT GetLeaderID()				{	return m_nLeaderSessionID;				}
	const WCHAR * GetChatRoomName()	{	return m_wstrChatRoomName.c_str();		}
	BYTE GetChatRoomType()			{	return m_nChatRoomType;					}

	const WCHAR * GetChatRoomPRLine1()	{	return m_wstrChatRoomPRLine1.c_str();		}
	const WCHAR * GetChatRoomPRLine2()	{	return m_wstrChatRoomPRLine2.c_str();		}
	const WCHAR * GetChatRoomPRLine3()	{	return m_wstrChatRoomPRLine3.c_str();		}

	UINT* GetMemberArray() {return m_nChatRoomMemberArray;}

	UINT GetMemberAIDFromIndex( int nIndex );

	bool UsePassword()				{	return(m_wstrChatRoomPW.length() > 0);	}

	int GetUserCount();

	bool IsLeader( UINT nUserAID );
	int LeaveUser( UINT nUserAID, int nReason );

	int EnterRoom( UINT nUserAID, WCHAR * pwzPassword, bool bLeader );

	void SendChatMessageToAll( int cLen, const WCHAR *pwszCharacterName, const WCHAR *pwszChatMsg );
	void SendProfileToAll( int nSessionID, TProfile & profile );

	void ChangeRoomOption( WCHAR * pwzRoomName, BYTE nRoomType, USHORT nRoomAllow, WCHAR * pwzPassword,
						   WCHAR * pwzPRLine1, WCHAR * pwzPRLine2, WCHAR * pwzPRLine3 );

	void SetRoom(CDNGameRoom* pRoom) {m_pGameRoom = pRoom;}
	CDNGameRoom* GetRoom() {return m_pGameRoom;}


protected:
	int _CanEnterRoom( UINT nUserAID, WCHAR * pwzPassword, bool bLeader );

	int _FindUserFromAID( UINT nUserAID );
	int _FindEmptyIndex();
};

