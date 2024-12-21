#pragma once

#include "EtUIDialog.h"

class CDnNpcTalkReturnDlg : public CEtUIDialog
{
public:
	CDnNpcTalkReturnDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnNpcTalkReturnDlg(void);

protected:
	CEtUIButton* m_pBackButton;

public:
	virtual void Initialize(bool bShow);
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

#if defined(PRE_ADD_QUEST_BACK_DIALOG_BUTTON)
protected:
	bool m_bIsClickedBackButton;
public:
	void SetButtonToggle(bool bToggle) { m_bIsClickedBackButton = bToggle; }
	bool GetButtonToggle() { return m_bIsClickedBackButton; }
#endif

#if defined(PRE_ADD_MAILBOX_OPEN) || defined(PRE_ADD_68286)
protected:
	bool m_bSkipOpen;

#ifdef PRE_MOD_ITEM_COMPOUND_DIALOG_RENEWAL
	public:
	void ShowBackButton( bool bShow )	{ m_bShow = bShow; }
#endif

public:
	void SetSkipOpen(bool bSkip)		{ m_bSkipOpen = bSkip; }
	void SetButtonEnable(bool bEnable)	{ m_pBackButton->Enable(bEnable); }
#endif // PRE_ADD_MAILBOX_OPEN
};