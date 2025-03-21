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

	TChatRoomView m_ChatRoomView;	// 자신이 개설한 방 들고있는다.(속해있는 방?)
	int m_nChatRoomAllow;
	std::wstring m_wszPassword;

	// 룸뷰는 길드뷰와 달리 너무 덩치가 커서 EnterUser에 포함시키지 않았다.
	// 그래서 아예 플레이어액터에 ChatRoomView를 넣지 않기로 했다.
	// 이 결과 이동중에 캐릭이 시야안에 들어왔을때 EnteredCallback이 다 처리되서 로딩되기 전에 이 룸뷰가 와버려서 그냥 넘어가는 경우가 생겼다.
	// 그래서 액터를 못찾을 경우 큐에 넣어뒀다가 프로세스돌면서 찾아보기로 한다.
	std::list<SCChatRoomView> m_listSCChatRoomView;

	// 룸뷰와 마찬가지로 SCChatRoomEnterUser도 EnterUser와 별개로 오기때문에,
	// 채널 입장할때 다른 유저 로딩이 다 되기 전에 SCChatRoomEnterUser가 먼저 올 수 있다.
	// 이 정보는 한번만 처리되고 버리면 되는 거라,
	// 처음 패킷 들어왔을때 주변 검색해서 캐릭터가 있으면 원래대로 처리하고 끝내고, 캐릭터가 없으면 넣어뒀다가 다음에 처리 후 버린다.
	std::list<SCChatRoomEnterUser> m_listSCChatRoomEnterUser;

	TChatRoomView *FindChatRoomView( UINT nSessionID );
	void OnSetChatRoomView( CDnPlayerActor *pActor, TChatRoomView *pView );
	void OnNotifyRoomEnter( CDnPlayerActor *pActor, bool bEnter, int nRoomType = 0 );

	int m_nRequestRoomID;

	// 일본에서 자꾸 채널이동이랑 채팅방이랑 이상한거 시도해서 이렇게 별도처리 해둔다.
	bool m_bRequestWait;

public:
	void RequestCreateChatRoom( const WCHAR *pRoomName, const WCHAR *pPassword, int nRoomType, int nRoomAllow, const WCHAR *pPR1, const WCHAR *pPR2, const WCHAR *pPR3 );
	void RequestConfigChatRoom( const WCHAR *pRoomName, const WCHAR *pPassword, int nRoomType, int nRoomAllow, const WCHAR *pPR1, const WCHAR *pPR2, const WCHAR *pPR3 );

	bool IsEnableChatRoomEnter( UINT nSessionID );
	void RequestEnterChatRoom( UINT nSessionID );
	void RequestKickUser( LPCWSTR pwszName, bool bForever );

	// 현재 방만들면서 저장된 정보다. 아래 둘은 RoomView에 안들어있어서 별도로 저장한다.
	TChatRoomView &GetChatRoomView() { return m_ChatRoomView; }
	int GetChatRoomAllow() { return m_nChatRoomAllow; }
	LPCWSTR GetPassword() { return m_wszPassword.c_str(); }


	bool IsRequestWait() { return m_bRequestWait; }

	// 시야에서의 Entered, Leaved 처리. 플레이어가 룸뷰를 가지고 있는 형태가 아니라서 이렇게 되었다.
	void OnCharUserEntered( DnActorHandle hActor );
	void OnCharUserLeaved( DnActorHandle hActor );
	void ClearChatRoomInfoList();

	// 예외처리. 강제로 호출해야할 상황이 생겨서 추가한다.
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