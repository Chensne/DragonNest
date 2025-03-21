#pragma once
#include "EtUIDialog.h"

class CDnChatRoomConfigDlg;
class CDnChatRoomPopupDlg;
class CDnChatRoomDlg : public CEtUIDialog, public CEtUICallback, public CEtUINameLinkInterface
{
public:
	CDnChatRoomDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnChatRoomDlg(void);

protected:

	enum
	{
		// 임시로 연결하는 차일드.
		NUM_TEMP_CHILD = 4,				// CDnInvenTabDlg, CDnTooltipDlg, CDnAcceptRequestDlg, CDnGuildInviteReqDlg. CDnPrivateMarketDlg는 CDnInvenTab자식이라 안해도 된다.
		NUM_TEMP_CHILD_RENDER_PASS = 2,	// 임시로 연결한 child중에 child로 렌더하지 않고 그냥 패스할 다이얼로그 개수 2개. CDnAcceptRequestDlg, CDnGuildInviteReqDlg다.
	};

	enum
	{
		MESSAGEBOX_EXITROOM,
	};

	CEtUIStatic *m_pStaticTitle;

	int m_nChatMode;
	CEtUIButton *m_pButtonModeUp;
	CEtUIButton *m_pButtonModeDown;
	CEtUIStatic *m_pStaticMode;
	CEtUIIMEEditBox *m_pIMEEditBox;
	CEtUIButton *m_pButtonReport;

	CEtUITextBox *m_pChatTextBox;
	CEtUIButton *m_pButtonDummy;

	CEtUIStatic *m_pSelectMaster;
	CEtUIStatic *m_pStaticMasterLevel;
	CEtUIStatic *m_pStaticMasterJob;
	CEtUIStatic *m_pStaticMasterName;
	CEtUIListBoxEx *m_pListBoxExPlayer;

	CDnChatRoomConfigDlg *m_pChatRoomConfigDlg;
	CDnChatRoomPopupDlg *m_pChatRoomPopupDlg;

	int m_nRoomID;
	bool m_bMasterIsMe;
	UINT m_nMasterSessionID;
	std::vector<UINT> m_vecSessionID;

	// Show-Hide될때 강제로 차일드 연결하는 함수.
	bool m_bLinked;
	void LinkChildDialog( bool bLink );
	CEtUIDialog *m_pTempChildDialog[NUM_TEMP_CHILD];
	CEtUIDialog *m_pRenderPassChildDlg[NUM_TEMP_CHILD_RENDER_PASS];

	// Note : IME에디트 박스를 통해서 입력되는 메세지
	//
	bool AddEditBoxString();

	// m_strLastPrivateUserName과 m_listPrivateName가 따로 있는게 이상해보이지만,
	// 만들어져있길 이렇게 만들어져있어서, 그리고 현재 둘다 사용하므로 그냥 이렇게 사용한다.
	//
	// 어쨌든 메세지가 오면 interface에서 m_strLastPrivateUserName 설정 후 AddPrivateName한다.
	// m_strLastPrivateUserName는 마지막 기억용도로 사용되고,
	// m_listPrivateName는 탭으로 귓말 상대를 전환할때 사용된다.
	std::wstring m_strPrivateUserName;
	std::wstring m_strLastPrivateUserName;

	typedef std::list<std::wstring>		PRIVATENAME_LIST;
	typedef PRIVATENAME_LIST::iterator	PRIVATENAME_LIST_ITER;
	PRIVATENAME_LIST m_listPrivateName;

	typedef std::list<std::wstring>		CHAT_HISTORY_LIST;
	typedef CHAT_HISTORY_LIST::iterator	CHAT_HISTORY_LIST_ITER;
	CHAT_HISTORY_LIST m_listChatHistory;

	typedef std::map<std::wstring,int>	COMMANDMODE_MAP;
	typedef COMMANDMODE_MAP::iterator	COMMANDMODE_MAP_ITER;
	COMMANDMODE_MAP m_mapCommandMode;
	bool m_bPrivateModeChecker;

	// 
	void AddChat( LPCWSTR wszName, LPCWSTR szMessage, int nChatType, DWORD dwBackgroundColor = 0 );

	// 도배 금지
	float m_fPaperingRemainTime;
	std::list<float> m_listRecentChatTime;
	bool CheckPapering();

public:
	// 타이틀 설정 함수.

	// 타이핑
	int GetChatMode() { return m_nChatMode; }
	bool SetChatMode( int nChatMode );
	void SetChatModeText();
	void SetChatPrevMode();
	void SetChatNextMode();
	void ResizeIMECCtl( LPCWSTR wszHeader );

	void AddChatHistotry( LPCWSTR wszChat );
	void SetPrevChatHistory();
	void SetNextChatHistory();

	void AddPrivateName( const std::wstring strPrivateName );
	void ChangePrivateName();

	bool GetLastPrivateName( std::wstring &strLastPrivateName );
	void SetLastPrivateName( LPCWSTR wszLastPrivateName );
	void DelPrivateName( const std::wstring &strPrivateName );
	void SetPrivateName( const std::wstring &strPrivateName );
	std::wstring GetPrivateName() { return m_strPrivateUserName; }

	void CheckCommandMode();
	bool CheckCommandMode( LPCWSTR wszMsg );

	void SendChat( int chatMode, std::wstring& chatMsg, const std::wstring& userName );

	// 텍스트박스
	void AddChatMessage( eChatType eType, LPCWSTR wszFromCharName, LPCWSTR wszChatMsg );
	void AppendChatEditBox(LPCWSTR wszString, bool bMoveCaret);

	virtual bool OnParseTextItemInfo(const std::wstring& argString);
	virtual bool OnParseTextUserName(const std::wstring& name);

	// 방,마스터정보
	void SetRoomInfo( int nRoomID, UINT nSessionID, LPCWSTR pwszRoomName, bool bMasterIsMe = false );
	int GetRoomID();
	bool IsMaster() { return m_bMasterIsMe; }
	bool IsEmptyRoom() const;

	// 플레이어 리스트
	bool IsMyChatRoomMember( UINT nSessionID );
	void AddUser( UINT nSessionID, bool bNotify = false );
	void AddUser( DnActorHandle hActor, bool bNotify = false );
	void DelUser( UINT nSessionID, BYTE cLeaveReason );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void Render( float fElapsedTime );
};