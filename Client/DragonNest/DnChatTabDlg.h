#pragma once
#include "EtUITabDialog.h"

class CEtUIIMEEditBox;
class CEtUIRadioButton;
class CEtUIButton;
class CDnChatDlg;
class CDnChatHelpDlg;
#ifdef PRE_MOD_CHATBG
class CDnChatBoardDlg;
#endif

#ifdef PRE_ADD_WORLD_MSG_RED
class CDnChatTabDlg : public CEtUITabDialog, public CEtUICallback, public CEtUINameLinkInterface
#else // PRE_ADD_WORLD_MSG_RED
class CDnChatTabDlg : public CEtUITabDialog, public CEtUICallback
#endif // PRE_ADD_WORLD_MSG_RED
{
protected:
	enum CHATDLG_SIZE_OLD
	{
		SIZE_STEP_SMALL,
		SIZE_STEP_MIDDLE,
		SIZE_STEP_BIG,
	};

	enum
	{
		CHAT_HELP_DIALOG,
	};

	enum
	{
		CMD_MSG_HELP,
		CMD_MSG_INVITE,
		CMD_MSG_MAKEPARTY,
		CMD_MSG_LEAVEPARTY,
		CMD_MSG_BAN,
		CMD_MSG_TRADE,
		CMD_MSG_DICE,
		CMD_MSG_LOCK,
		CMD_MSG_UNLOCK,
		CMD_MSG_ESCAPE,
		CMD_MSG_RAIDNOTICE,
#ifdef PRE_PRIVATECHAT_CHANNEL
		CMD_MSG_CHANNEL_CREATE,
		CMD_MSG_CHANNEL_JOIN,
		CMD_MSG_CHANNEL_PASSWORD,
		CMD_MSG_CHANNEL_INVITE,
		CMD_MSG_CHANNEL_LEAVE,
		CMD_MSG_CHANNEL_BAN,
		CMD_MSG_CHANNEL_MASTER,
		CMD_MSG_CHANNEL_HELP,
#endif // PRE_PRIVATECHAT_CHANNEL
	};

	enum CHATDLG_SIZE
	{
		SIZE_BASE,	// 기본상태
		SIZE_OVER,	// 마우스 올림
		SIZE_DRAG	// 눌러서 조절 가능한 상태
	};

#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
public:
#endif // PRE_ADD_PRIVATECHAT_CHANNEL
#ifdef PRE_PRIVATECHAT_CHANNEL
	enum
	{
		CHANNEL_NAME_MIN = 2,
		CHANNEL_NAME_MAX = 10,
	};
#endif // PRE_PRIVATECHAT_CHANNEL

public:
	CDnChatTabDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnChatTabDlg(void);

protected:
	CEtUIRadioButton *m_pNormalTab;
#ifdef PRE_ADD_CHAT_RENEWAL
	CEtUIRadioButton * m_pWhisperTab; // 귓속말.
#endif
	CEtUIRadioButton *m_pPartyTab;
	CEtUIRadioButton *m_pGuildTab;
	CEtUIRadioButton *m_pSystemTab;

	CEtUIButton *m_pButtonDummy;
	int m_nSizeMode;
	float m_fBasisY;	// 드래그시작때의 y위치

	CDnChatDlg *m_pChatNormalDlg;
#ifdef PRE_ADD_CHAT_RENEWAL
	CDnChatDlg * m_pChatWhisperDlg; // 귓속말.
#endif
	CDnChatDlg *m_pChatPartyDlg;
	CDnChatDlg *m_pChatGuildDlg;
	CDnChatDlg *m_pChatSystemDlg;

	CDnChatHelpDlg *m_pChatHelpDlg;
#ifdef PRE_MOD_CHATBG
	CDnChatBoardDlg* m_pChatBoardDlg;
#endif

#ifdef PRE_PRIVATECHAT_CHANNEL
	CEtUIRadioButton* m_pPrivateChannelTab;
	CDnChatDlg* m_pPrivateChannelDlg;
#endif // PRE_PRIVATECHAT_CHANNEL
#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
	CEtUIStatic* m_pStaticPrivateChannelNoticeText;
	CEtUIStatic* m_pStaticPrivateChannelNoticeBalloon;
	CEtUIStatic* m_pStaticPrivateChannelTabHighlight;
	bool m_bJoinPrivateChannelNotify;
	float m_fJoinPrivateChannelNotify;
#endif // PRE_ADD_PRIVATECHAT_CHANNEL

	typedef std::vector<CDnChatDlg*>	CHATDLG_VEC;
	typedef CHATDLG_VEC::iterator		CHATDLG_VEC_ITER;
	CHATDLG_VEC m_vecChatDlg;

	int m_nChatMode;
#ifdef PRE_ADD_CHAT_RENEWAL	
	bool m_bChangeChatMode;
#endif
	CEtUIButton *m_pButtonModeUp;
	CEtUIButton *m_pButtonModeDown;
	CEtUIButton *m_pButtonOption;
	CEtUIButton *m_pButtonHelp;
	CEtUIStatic *m_pStaticMode;
	CEtUIIMEEditBox *m_pIMEEditBox;
	CEtUIButton *m_pButtonReport;

#ifdef PRE_ADD_WORLD_MSG_RED
	CEtUIStatic* m_pWorldChatRedBoard;
	CEtUITextBox* m_pWorldChatRed;
	float m_fWorldMsgRedCoolTime;
	float m_fWorldMsgRed1Sec;
	int m_nBlinkCount;
#endif // PRE_ADD_WORLD_MSG_RED

	bool m_bCustomizeChatMode;          // 활성/비활성
	int m_nCustomizeModeType;           // 파티모드 일반모드 길드모드등 모드선택
	LPCWSTR m_wszCustomizeModeHeader;   // [파티] , [일반] , [길드] 등등의 메세지 상황에 따른 변경 가능
	DWORD m_dwCustomizeModeTextColor;   // 색상 가능 -일단 지금은 하나만 변경가능하도록 설정 되어 있습니다. 여러가지를 동시에 변경할때는 Vector로 구성해주도록 합니다 -

	typedef std::map<std::wstring,int>	COMMANDMODE_MAP;
	typedef COMMANDMODE_MAP::iterator	COMMANDMODE_MAP_ITER;
	COMMANDMODE_MAP m_mapCommandMode;

	// 모드와의 구분을 위해 별도로 만들겠다.
	typedef std::map<std::wstring,int>		COMMANDMESSAGE_MAP;
	typedef COMMANDMESSAGE_MAP::iterator	COMMANDMESSAGE_MAP_ITER;
	COMMANDMESSAGE_MAP m_mapCommandMessage;

	bool m_bShowOptDlg;

	// 현재 사용되지 않는 변수다. 안정되면 제거해야한다.
	CHATDLG_SIZE_OLD m_ChatDlgSize;

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

	bool m_bMouseIn;
	bool m_bSetFocusEditBox;
	bool m_IsPVPGameChat; //pvp게임 채팅인가 
	bool m_bPrivateModeChecker;
	bool m_bPVPLobbyChat;

	// 귓속말왔을때 탭이 활성화되지 않았다면 효과음 출력.
	int m_nPrivateAlarmSoundIndex;
	std::wstring m_ChatTextWrapper;

	int m_nChatModeBeforePrivate;

	bool m_bIgnoreShowFunc;
#ifdef PRE_ADD_ANTI_CHAT_SPAM
	bool m_bSpammer;
#endif

protected:
	// Note : IME에디트 박스를 통해서 입력되는 메세지
	//
	bool AddEditBoxString();

public:
	int GetChatMode()							{ return m_nChatMode; }
	bool SetChatMode( int nChatMode, bool bParty = false );
	void ResetChatMode();
	int GetChatTabMode()						{ return GetCurrentTabID(); }
	void SetChatTabMode( int nChatTabMode )		{ SetCheckedTab( nChatTabMode ); }

	void CustomizeChatMode(bool bTrue,int nModeType,LPCWSTR wszHeader,DWORD dwTextColor);
	DWORD GetCustomizeChatColor(){return m_dwCustomizeModeTextColor;}
	int GetCustomizeChatMode(){return m_nCustomizeModeType;}
	bool IsCustomizeChatMode(){return m_bCustomizeChatMode;}

	// 블라인드열릴때 닫히는거 막기. 호출시 1회만 작동하게 해둡니다.
	void SetIgnoreShowFunc( bool bIgnore ) { m_bIgnoreShowFunc = bIgnore; }

protected:
	void SetChatModeText();
	void SetChatPrevMode();
	void SetChatNextMode();

	void CheckCommandMode();
	bool CheckCommandMode( LPCWSTR wszMsg );
	void ResizeChatBox( float fHeightDelta );
	void ResizeIMECCtl( LPCWSTR wszHeader );
	void SetPosBottomCtl( float fResizeRatio );
	void SetPosChatDlg( float fResizeRatio );
	void ShowControl( bool bShow );

	bool CommandMessage( LPCWSTR wszMsg );

	void AddChatDlg( LPCWSTR wszMsg, int nChatType );
	void _AddChatMessage( int nChatType, LPCWSTR wszFromCharName, LPCWSTR wszMessage );
	void AddPrivateName( const std::wstring strPrivateName );
	void ChangePrivateName();

	void AddChatHistotry( LPCWSTR wszChat );
	void SetPrevChatHistory();
	void SetNextChatHistory();

	// 반복금지 처리
	std::wstring m_wszPrevString;
	UINT m_nRepeat;
	bool CheckRepeat();
	bool CheckRepeat(const std::wstring& chatMsg);

	// 도배 금지
	float m_fPaperingRemainTime;
	std::list<float> m_listRecentChatTime;
	bool CheckPapering();
#ifdef PRE_CHAT_FIX_DICE_PAPERING
	bool HandleChatPapering(const std::wstring& chatMsg);
#endif // PRE_CHAT_FIX_DICE_PAPERING

	bool IsGuildWarMaster( LPCWSTR wszFromCharName );
	UINT  GetGuildWarUserState( LPCWSTR wszFromCharName );

public:
	// Note : 사용자에 의해서 입력되는 일반 적인 채팅 메세지
	//
#ifdef PRE_ADD_DOORS
	void AddChatMessage( int nChatType, LPCWSTR wszFromCharName, LPCWSTR wszMessage, bool isAppend = false, bool hasColor = false, DWORD colorValue = 0xffffffff, DWORD bgColorValue = 0, bool bIsDoorsMobile = false);
#else
	void AddChatMessage( int nChatType, LPCWSTR wszFromCharName, LPCWSTR wszMessage, bool isAppend = false, bool hasColor = false, DWORD colorValue = 0xffffffff, DWORD bgColorValue = 0);
#endif

	// 시스템 메세지는 화면 중앙에 안뜰때도 있어야한다.
	void AddSystemMessage( LPCWSTR wszFromCharName, LPCWSTR wszMessage, bool bShowCaption = true );

	// 공지 메세지는 화면 중앙에 항상 보인다.
	void AddNoticeMessage( LPCWSTR wszFromCharName, LPCWSTR wszMessage );
	void AppendChatEditBox(LPCWSTR wszString, bool bMoveCaret);

	void AddChatQuestMsg( LPCWSTR wszMsg );
	void AddChatQuestReceipt( LPCWSTR wszQuestName );
	void AddChatQuestCompletion( LPCWSTR wszQuestName );

	void ShowEx( bool bShow );
	void ShowChatDlg();
	CDnChatDlg *GetCurrentChatDlg();

	bool GetLastPrivateName( std::wstring &strLastPrivateName );
	void SetLastPrivateName( LPCWSTR wszLastPrivateName );
	void DelPrivateName( const std::wstring &strPrivateName );
	void SetPrivateName( const std::wstring &strPrivateName );
	std::wstring GetPrivateName() { return m_strPrivateUserName; }

	// IMEEditBox
	bool IsEditBoxFocus() { return m_pIMEEditBox->IsFocus(); }
	void SetEditBoxFocus() { RequestFocus(m_pIMEEditBox); }
	CEtUIIMEEditBox *GetIMEEditBox() { return m_pIMEEditBox; }
	bool GetGameMode() {return m_IsPVPGameChat;}
	void ClearText();
	void SendChat(int chatMode, std::wstring& chatMsg, const std::wstring& userName);

#ifdef PRE_MOD_CHATBG
	bool IsBackGroundMode() const;
#endif
#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
	void UpdatePrivateChannelNotify( float fElapsedTime );
	void SetPrivateChannelNotify( bool bNotify );
#endif // PRE_ADD_PRIVATECHAT_CHANNEL

public:
	virtual void Process( float fElapsedTime );
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Render( float fElapsedTime );
	virtual void Show( bool bShow );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	virtual void SetChatGameMode( bool isPVP );

	virtual void OnChangeResolution();
#ifdef PRE_ADD_WORLD_MSG_RED
	virtual bool OnParseTextItemInfo( const std::wstring& argString );
#endif // PRE_ADD_WORLD_MSG_RED

#if defined(PRE_FIX_57852)
protected:
	bool m_bPvPLobbyChatOtherAreaInside;	//PvP로비 오른쪽 영역 마우스 처리
#endif // PRE_FIX_57852

};