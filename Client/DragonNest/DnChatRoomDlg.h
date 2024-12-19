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
		// �ӽ÷� �����ϴ� ���ϵ�.
		NUM_TEMP_CHILD = 4,				// CDnInvenTabDlg, CDnTooltipDlg, CDnAcceptRequestDlg, CDnGuildInviteReqDlg. CDnPrivateMarketDlg�� CDnInvenTab�ڽ��̶� ���ص� �ȴ�.
		NUM_TEMP_CHILD_RENDER_PASS = 2,	// �ӽ÷� ������ child�߿� child�� �������� �ʰ� �׳� �н��� ���̾�α� ���� 2��. CDnAcceptRequestDlg, CDnGuildInviteReqDlg��.
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

	// Show-Hide�ɶ� ������ ���ϵ� �����ϴ� �Լ�.
	bool m_bLinked;
	void LinkChildDialog( bool bLink );
	CEtUIDialog *m_pTempChildDialog[NUM_TEMP_CHILD];
	CEtUIDialog *m_pRenderPassChildDlg[NUM_TEMP_CHILD_RENDER_PASS];

	// Note : IME����Ʈ �ڽ��� ���ؼ� �ԷµǴ� �޼���
	//
	bool AddEditBoxString();

	// m_strLastPrivateUserName�� m_listPrivateName�� ���� �ִ°� �̻��غ�������,
	// ��������ֱ� �̷��� ��������־, �׸��� ���� �Ѵ� ����ϹǷ� �׳� �̷��� ����Ѵ�.
	//
	// ��·�� �޼����� ���� interface���� m_strLastPrivateUserName ���� �� AddPrivateName�Ѵ�.
	// m_strLastPrivateUserName�� ������ ���뵵�� ���ǰ�,
	// m_listPrivateName�� ������ �Ӹ� ��븦 ��ȯ�Ҷ� ���ȴ�.
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

	// ���� ����
	float m_fPaperingRemainTime;
	std::list<float> m_listRecentChatTime;
	bool CheckPapering();

public:
	// Ÿ��Ʋ ���� �Լ�.

	// Ÿ����
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

	// �ؽ�Ʈ�ڽ�
	void AddChatMessage( eChatType eType, LPCWSTR wszFromCharName, LPCWSTR wszChatMsg );
	void AppendChatEditBox(LPCWSTR wszString, bool bMoveCaret);

	virtual bool OnParseTextItemInfo(const std::wstring& argString);
	virtual bool OnParseTextUserName(const std::wstring& name);

	// ��,����������
	void SetRoomInfo( int nRoomID, UINT nSessionID, LPCWSTR pwszRoomName, bool bMasterIsMe = false );
	int GetRoomID();
	bool IsMaster() { return m_bMasterIsMe; }
	bool IsEmptyRoom() const;

	// �÷��̾� ����Ʈ
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