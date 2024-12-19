#pragma once
#include "EtUIDialog.h"

class CDnGuildWantedListPopupDlg;
class CDnGuildWantedMyListDlg : public CEtUIDialog, public CEtUICallback
{
public:
	CDnGuildWantedMyListDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildWantedMyListDlg(void);

	enum
	{
		REQUEST_CANCEL_DIALOG,
	};

protected:
	CEtUIListBoxEx *m_pListBoxEx;
	CEtUIStatic *m_pStaticCount;
	CEtUIButton *m_pButtonCancel;
	CEtUIButton *m_pButtonHomepage;

	CDnGuildWantedListPopupDlg *m_pGuildWantedListPopupDlg;

	int m_nRequestCount;
	int m_nMaxRequestCount;

	TGuildUID m_RequestGuildUID;
	std::vector<TGuildUID> m_vecMyRecruitGuildUID;

	void InitControl();

public:
	void OnRecvGetGuildRecruitRequestCount( GuildRecruitSystem::SCGuildRecruitRequestCount *pPacket );
	void OnRecvGetGuildRecruitMyList( GuildRecruitSystem::SCMyGuildRecruitList *pPacket );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
};
