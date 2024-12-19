#pragma once

#include "EtUIDialog.h"
#include "DnInterface.h"

#define MAX_CONTEXTMENU_BTNS	6
#define MAX_CONTEXTMENU_STATICS	2

class CDnContextMenuDlg : public CEtUIDialog, public CEtUICallback
{
public:
	CDnContextMenuDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnContextMenuDlg(void) {}

	struct SContextMenuInfo {};

protected:
	struct SButtonInfo
	{
		CEtUIButton* pBtn;
		SButtonInfo() : pBtn(NULL) {}
	};

	struct SStaticInfo
	{
		CEtUIStatic* pStatic;
		CEtUIStatic* pBGStatic;

		SStaticInfo() : pStatic(NULL), pBGStatic(NULL) {}
	};

	virtual void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg /* = 0 */);
	virtual void Process(float fElapsedTime);
	virtual void OnUICallbackProc(int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg /* = 0 */) {}

	bool		 CheckFriendCondition(const WCHAR* name);
	bool		 m_bWaitClose;

public:
	virtual void SetInfo(const SContextMenuInfo* pInfo) {}
	virtual void PreBakeMenu() {}

	void AddButton(const std::wstring& text);
	void AddStatic(const std::wstring& text);

	void SetButton(int index, const std::wstring& text);
	void SetStatic(int index, const std::wstring& text);
	void SetTitle(LPCWSTR wszTitle);

	void EnableButton(const std::wstring& btnText, bool bEnable);
	void EnableStatic(const std::wstring& staticText, bool bEnable);

	bool IsEnableButton(const std::wstring& btnText) const;

	SUICoord GetBackgroundUISize();

	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void Show(bool bShow);
	virtual bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	void			RefreshDlg();
	CEtUIButton*	GetUsableButton();
	SStaticInfo*	GetUsableStaticInfo();

	std::vector<SButtonInfo>	m_pButtonList;
	std::vector<SStaticInfo>	m_pStaticList;
	CEtUIStatic*				m_pTitle;
	CEtUIStatic*				m_pBackGround;
};

//////////////////////////////////////////////////////////////////////////

class CDnPartyContextMenuDlg : public CDnContextMenuDlg
{
public:
	struct SContextMenuPartyInfo : public CDnContextMenuDlg::SContextMenuInfo
	{
		DWORD			dwSessionID;
		std::wstring	name;
		int				level;
		int				job;
		
		SContextMenuPartyInfo()
		{
			dwSessionID = 0;
			level = job = -1;
		}
	};

	CDnPartyContextMenuDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual void SetInfo(const SContextMenuInfo* pInfo);
	virtual void PreBakeMenu();
	virtual void Show(bool bShow);

protected:
	virtual void Process(float fElapsedTime);
	virtual void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg);
	virtual void OnUICallbackProc(int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg /* = 0 */);
	virtual void OnHandleMsg(int protocol, char* pData);

private:
	enum
	{
		eNAME,
		eJOB,
	};
	bool		 CheckValidActor();
	bool		 IsFriend(const WCHAR *name);
	void		 Clear();

	union
	{
		struct
		{
			//use eCommunityAcceptableOptionType
			BYTE m_cPartyInviteAcceptable : 2;
			BYTE m_cGuildInviteAcceptable : 2;
			BYTE m_cTradeRequestAcceptable : 2;
			BYTE m_cDuelRequestAcceptable : 2;
			BYTE m_cObserveStuffAcceptable : 2;
		};
		char m_cCommunityOption[13];			//13byte
	};

	DWORD			m_SessionID;
	std::wstring	m_Name;
	bool			m_bRequestFriend;
	bool			m_bReceivePermission;
};

//////////////////////////////////////////////////////////////////////////

class CDnChatContextMenuDlg : public CDnContextMenuDlg
{
public:
	struct SContextMenuChatInfo : public CDnContextMenuDlg::SContextMenuInfo
	{
		std::wstring	name;
	};

	CDnChatContextMenuDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual void PreBakeMenu();
	virtual void Show(bool bShow);
	virtual void SetInfo(const SContextMenuInfo* pInfo);

protected:
	virtual void Process(float fElapsedTime);
	virtual void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg);
	virtual void OnUICallbackProc(int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg);

private:
	bool SetCurrentPartyMember();
	void Clear();

	std::wstring m_Name;
	bool		 m_bRequestFriend;

#ifdef PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP
	bool		 m_bRequstPartyWithChat;
public:
	inline void SetReqPartyWithChat( bool bRequstParty )			{ m_bRequstPartyWithChat = bRequstParty; }
	inline bool IsReqPartyWithChat()								{ return m_bRequstPartyWithChat; }
#endif

};