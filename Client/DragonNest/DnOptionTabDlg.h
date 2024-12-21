#pragma once
#include "EtUITabDialog.h"
#include "DnOptionDlg.h"

class CDnOptionTabDlg : public CEtUIDialog, public CDnOptionBase
{
public:
	CDnOptionTabDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnOptionTabDlg();

private:
	DWORD m_dwPrevTabID;;
	bool m_bShowChangeMsg;

	std::vector< boost::tuple<DWORD, CEtUIControl*, CDnOptionBase* > >  m_vecTabControl;
	CEtUIDialogGroup m_groupTabDialog;

protected:
	void AddTabDialog( CEtUIControl *pControl, CDnOptionDlg *pDialog );
	void AddTabDialog( CEtUIControl *pControl, CDnOptionTabDlg *pDialog );
	void AddTabDialog( CEtUIControl *pControl, CEtUIDialog *pDialog, CDnOptionBase *pOptionBase );
	void SetCheckedTab( DWORD dwControlID );

public:
	DWORD					GetCurrentTabID();
	CDnOptionBase*		GetOptionBase( DWORD dwTabID );
	int GetFrontTabID();

public:
	void ImportSetting();
	void ExportSetting();
	bool IsChanged();

public:
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg ) override;
};

