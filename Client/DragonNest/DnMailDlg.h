#pragma once

#include "EtUITabDialog.h"

class CDnMailReadDlg;
class CDnMailWriteDlg;

#include "DnTradeMail.h"

class CDnMailListDlg;
class CDnItem;

class CDnMailDlg : public CEtUITabDialog, public CEtUICallback
{
public:
	CDnMailDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnMailDlg(void) {}

	virtual void	Initialize(bool bShow);
	virtual void	InitialUpdate();
	virtual void	Show( bool bShow );
	virtual void	Process( float fElapsedTime );

	bool			IsShowWriteDlg() const;
	bool			AttachItemToWriteDlg(CDnSlotButton* pFromSlot, bool bItemSplit);
	void			LockDlgs(bool bLock);
	bool			IsLockedDlgs() const	{ return m_bLock; }

	void			RefreshMailBoxList(const std::vector<CDnTradeMail::SMailBoxInfo>& mailList);
	void			RefreshMailReadDialog(const CDnTradeMail::SReadMailInfo& info);

	void			OpenMailReadDialog();
	void			OpenMailWriteDialog(const wchar_t *wszName, const wchar_t* wszTitle, bool bAfterMailSend);
	void			OpenMailListDialog();

	void			ClearMailReadDialog();
	void			ClearMailReadAttach();
	void			RemoveReadDialogAttachItem(INT64 attachItemSerial, bool bHasCoin);

	void			OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg );
	void			ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 ) override;

protected:
	CDnMailListDlg*		m_pMailListDlg;
	CDnMailReadDlg*		m_pMailReadDlg;
	CDnMailWriteDlg*	m_pMailWriteDlg;

	CEtUIRadioButton*	m_pMailListTab;
	CEtUIRadioButton*	m_pMailReadTab;
	CEtUIRadioButton*	m_pMailWriteTab;

	bool				m_bLock;
};