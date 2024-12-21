#pragma once
#include "EtUIDialog.h"
#include "DnVIPDataMgr.h"

#ifdef PRE_ADD_VIP

class CDnPopupVIPDlg : public CEtUIDialog
{
public:
	CDnPopupVIPDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnPopupVIPDlg(void);

protected:
	CEtUIStatic*	m_pStatic[CDnVIPDataMgr::eMAX];
	CEtUIStatic*	m_pStaticExpireDate;
	CEtUIStatic*	m_pStaticPts;
	SUICoord		m_ParentUICoord;

public:
	virtual void	Initialize( bool bShow );
	virtual void	InitialUpdate();
	virtual void	Show( bool bShow );
	virtual bool	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	void			SetInfo(const SUICoord& iconCoord);
};

#endif // PRE_ADD_VIP