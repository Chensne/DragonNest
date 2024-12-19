#pragma once
#include "EtUIDialog.h"

class CDnGuildWantedListPopupDlg;
class CDnGuildWantedListDlg : public CEtUIDialog, public CEtUICallback
{
public:
	CDnGuildWantedListDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildWantedListDlg(void);

	enum
	{
		REQUEST_JOIN_DIALOG,
#ifdef PRE_ADD_GUILD_EASYSYSTEM
#else
		REQUEST_CANCEL_DIALOG,
#endif
	};

protected:
#ifdef PRE_ADD_GUILD_EASYSYSTEM
#else
	CEtUIRadioButton *m_pWantedTabButton;
	CEtUIRadioButton *m_pJoinTabButton;
#endif
	CEtUIListBoxEx *m_pListBoxEx;

	CEtUIButton *m_pButtonPagePrev;
	CEtUIButton *m_pButtonPageNext;
	CEtUIStatic *m_pStaticPage;
	int m_nPage;
	int m_nPrevPage;

	CEtUIStatic *m_pStaticCount;

	CEtUIButton *m_pButtonOK;
#ifdef PRE_ADD_GUILD_EASYSYSTEM
	CEtUIComboBox *m_pComboCategory;
	CEtUIComboBox *m_pComboSort;
	CEtUIIMEEditBox *m_pEditBoxGuildName;
	CEtUIStatic *m_pStaticGuildName;
	CEtUIButton *m_pButtonSearch;
	CEtUIButton *m_pButtonReset;
	CEtUIButton *m_pButtonHomepage;
#else
	CEtUIButton *m_pButtonCancel;
#endif

	CDnGuildWantedListPopupDlg *m_pGuildWantedListPopupDlg;

	int m_nRequestCount;
	int m_nMaxRequestCount;

	TGuildUID m_RequestGuildUID;
	std::vector<TGuildUID> m_vecMyRecruitGuildUID;

	void InitControl();
#ifdef PRE_ADD_GUILD_EASYSYSTEM
	BYTE GetSearchPurpose();
	const WCHAR *GetSearchGuildName();
	BYTE GetSearchSort();
	void RequestSearch();
#endif

public:
	void OnRecvGetGuildRecruitList( GuildRecruitSystem::SCGuildRecruitList *pPacket );
	void OnRecvGetGuildRecruitRequestCount( GuildRecruitSystem::SCGuildRecruitRequestCount *pPacket );
	void OnRecvGetGuildRecruitMyList( GuildRecruitSystem::SCMyGuildRecruitList *pPacket );
	void OnRecvGuildRecruitRequest( GuildRecruitSystem::SCGuildRecruitRequest *pPacket );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
};