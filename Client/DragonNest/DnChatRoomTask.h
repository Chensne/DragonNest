#pragma once

#include "Task.h"
#include "MessageListener.h"

class CDnChatRoomTask : public CTask, public CTaskListener, public CEtUICallback, public CSingleton<CDnChatRoomTask>
{
	enum
	{
		CHATROOM_PASSWORD_DIALOG,
	};

public:
	CDnChatRoomTask();
	virtual ~CDnChatRoomTask();

protected:
	void OnRecvChatRoomMessage( int nSubCmd, char *pData, int nSize );
	void OnRecvRoomMessage( int nSubCmd, char *pData, int nSize );

	void OnRecvChatRoomCreateRoom( SCCreateChatRoom *pPacket );
	void OnRecvChatRoomView( SCChatRoomView *pPacket );
	void OnRecvChatRoomLeave( SCChatRoomLeaveUser *pPacket );
	void OnRecvChatRoomEnterRoom( SCChatRoomEnterRoom *pPacket );
	void OnRecvChatRoomEnterUser( SCChatRoomEnterUser *pPacket );
	void OnRecvChatRoomChangeRoomOption( SCChatRoomChangeRoomOption *pPacket );
	void OnRecvChatRoomKickUser( SCChatRoomKickUser *pPacket );
	void OnRecvRoomSyncChatRoomInfo( SCROOM_SYNC_CHATROOMINFO *pPacket );

	TChatRoomView m_ChatRoomView;	// �ڽ��� ������ �� ����ִ´�.(�����ִ� ��?)
	int m_nChatRoomAllow;
	std::wstring m_wszPassword;

	// ���� ����� �޸� �ʹ� ��ġ�� Ŀ�� EnterUser�� ���Խ�Ű�� �ʾҴ�.
	// �׷��� �ƿ� �÷��̾���Ϳ� ChatRoomView�� ���� �ʱ�� �ߴ�.
	// �� ��� �̵��߿� ĳ���� �þ߾ȿ� �������� EnteredCallback�� �� ó���Ǽ� �ε��Ǳ� ���� �� ��䰡 �͹����� �׳� �Ѿ�� ��찡 �����.
	// �׷��� ���͸� ��ã�� ��� ť�� �־�״ٰ� ���μ������鼭 ã�ƺ���� �Ѵ�.
	std::list<SCChatRoomView> m_listSCChatRoomView;

	// ���� ���������� SCChatRoomEnterUser�� EnterUser�� ������ ���⶧����,
	// ä�� �����Ҷ� �ٸ� ���� �ε��� �� �Ǳ� ���� SCChatRoomEnterUser�� ���� �� �� �ִ�.
	// �� ������ �ѹ��� ó���ǰ� ������ �Ǵ� �Ŷ�,
	// ó�� ��Ŷ �������� �ֺ� �˻��ؼ� ĳ���Ͱ� ������ ������� ó���ϰ� ������, ĳ���Ͱ� ������ �־�״ٰ� ������ ó�� �� ������.
	std::list<SCChatRoomEnterUser> m_listSCChatRoomEnterUser;

	TChatRoomView *FindChatRoomView( UINT nSessionID );
	void OnSetChatRoomView( CDnPlayerActor *pActor, TChatRoomView *pView );
	void OnNotifyRoomEnter( CDnPlayerActor *pActor, bool bEnter, int nRoomType = 0 );

	int m_nRequestRoomID;

	// �Ϻ����� �ڲ� ä���̵��̶� ä�ù��̶� �̻��Ѱ� �õ��ؼ� �̷��� ����ó�� �صд�.
	bool m_bRequestWait;

public:
	void RequestCreateChatRoom( const WCHAR *pRoomName, const WCHAR *pPassword, int nRoomType, int nRoomAllow, const WCHAR *pPR1, const WCHAR *pPR2, const WCHAR *pPR3 );
	void RequestConfigChatRoom( const WCHAR *pRoomName, const WCHAR *pPassword, int nRoomType, int nRoomAllow, const WCHAR *pPR1, const WCHAR *pPR2, const WCHAR *pPR3 );

	bool IsEnableChatRoomEnter( UINT nSessionID );
	void RequestEnterChatRoom( UINT nSessionID );
	void RequestKickUser( LPCWSTR pwszName, bool bForever );

	// ���� �游��鼭 ����� ������. �Ʒ� ���� RoomView�� �ȵ���־ ������ �����Ѵ�.
	TChatRoomView &GetChatRoomView() { return m_ChatRoomView; }
	int GetChatRoomAllow() { return m_nChatRoomAllow; }
	LPCWSTR GetPassword() { return m_wszPassword.c_str(); }


	bool IsRequestWait() { return m_bRequestWait; }

	// �þ߿����� Entered, Leaved ó��. �÷��̾ ��並 ������ �ִ� ���°� �ƴ϶� �̷��� �Ǿ���.
	void OnCharUserEntered( DnActorHandle hActor );
	void OnCharUserLeaved( DnActorHandle hActor );
	void ClearChatRoomInfoList();

	// ����ó��. ������ ȣ���ؾ��� ��Ȳ�� ���ܼ� �߰��Ѵ�.
	void ForceCloseChatRoom();

public:
	bool Initialize();
	void Finalize();

	virtual void OnDisconnectTcp( bool bValidDisconnect );
	virtual void OnDisconnectUdp();

	virtual void OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize );

	// CEtUICallback 
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
};

#define GetChatRoomTask()		CDnChatRoomTask::GetInstance()