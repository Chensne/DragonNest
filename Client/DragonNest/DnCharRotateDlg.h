#pragma once
#include "EtUIDialog.h"


#ifdef PRE_MOD_SELECT_CHAR

class CDnCharRotateDlg : public CEtUIDialog, public CEtUICallback
{
private:

#ifdef PRE_ADD_NEWCOMEBACK
	CEtUIStatic * m_pStaticComeback;
	EtColor m_TextColor, m_crrTextColor;
	float m_sign;
#endif // PRE_ADD_NEWCOMEBACK

#ifdef PRE_ADD_DWC
	CEtUIStatic* m_pStaticDWCCharTitle;
	EtColor m_DWCTextColor, m_DWCcrrTextColor;
	float	m_DWCSign;
#endif // PRE_ADD_DWC

public:
	CDnCharRotateDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnCharRotateDlg();

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


#ifdef PRE_ADD_NEWCOMEBACK
	void SetComebackUser( bool bComeback );
#endif // PRE_ADD_NEWCOMEBACK

#ifdef PRE_ADD_DWC
	void SetDWCCharSelect( bool bSelect );
#endif //PRE_ADD_DWC

protected:
	CEtUIButton* m_pButtonRotateLeft;
	CEtUIButton* m_pButtonRotateRight;
	CEtUIButton* m_pButtonAvatarViewArea;

	float m_fAvatarViewMouseX;
	float m_fAvatarViewMouseY;
};

#endif // PRE_MOD_SELECT_CHAR