#pragma once
#include "EtUIDialog.h"


#ifdef PRE_MOD_SELECT_CHAR


class CDnCharCreateSelectDlg : public CEtUIDialog
{
public:
	CDnCharCreateSelectDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnCharCreateSelectDlg();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

public:
	void SetCharCreateSelect( int nClassIndex );
	void ResetButton();
	void SetFocusCharIndex( int nFocusCharIndex ) { m_nFocusCharIndex = nFocusCharIndex; }

protected:
	CEtUIStatic*		m_pStaticJobName[CLASSKINDMAX];
#ifdef _ADD_NEWLOGINUI
	CEtUIRadioButton*	m_pRadioButton[20]; //alocate 20.
#else
	CEtUIRadioButton*	m_pRadioButton[CLASSKINDMAX];
#endif
	std::map<int, int>	m_mapTabJobIndex;
	int					m_nFocusCharIndex;
};


#endif // PRE_MOD_SELECT_CHAR