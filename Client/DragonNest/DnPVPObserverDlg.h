#pragma once
#include "EtUIDialog.h"
#include "DnPVPPopupDlg.h"

class CDnPVPObserverDlg : public CEtUIDialog
{
public:
	CDnPVPObserverDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPVPObserverDlg(void);

protected:
	
	CEtUIListBoxEx *m_pObserverListBox; 
	CDnPVPPopupDlg *m_pPVPPopupDlg;

public:

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void Show( bool bShow );
	
	void ShowPVPPopUpDlg(bool bShow){ShowChildDialog( m_pPVPPopupDlg, bShow );}

	CEtUIListBoxEx *GetObserverListBox(){return m_pObserverListBox;};// 리뉴얼에 개선이되어서 이 클래스가 리스트에 대한 정보를 가지고 있습니다. 결국 얘를 넘겨다줍니다.

};
