#pragma once
#include "EtUIDialog.h"
#include "DnSecurityBase.h"


class CDnSecurityNUMPADDeleteDlg : public CEtUIDialog , public CDnSecurityBase , public CEtUICallback
{
public:
	CDnSecurityNUMPADDeleteDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnSecurityNUMPADDeleteDlg(void);

	enum emAuthPwFocus
	{
		NewPw,	// 비밀번호
		EnterPw,// 재입력 
	};

	enum emAuthCheckType
	{
		Login, 
		LoginCharDel, 
		Game, 
	};


protected:
	std::vector<std::wstring> m_vecNumberStr;

	SecondPassStruct m_SecondPass1;
	SecondPassStruct m_SecondPass2;

	std::vector<CEtUIButton*> m_pBtnNumPad;

	CEtUIEditBox* m_pEditCurPw;
	CEtUIEditBox* m_pEditEnterPw;
	CEtUITextBox* m_pText_Unlock;

	emAuthPwFocus  m_eAuthPwFocus;
	emAuthCheckType m_emAuthCheckType; 
#ifdef PRE_MOD_SECURITYNUMPAD_RENEWAL
	std::vector<std::wstring> m_vecNumberStrCopy;
#endif

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow , int AuthType = 2 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );

	void InitializeNumber();
	void SetCurrentFocus(emAuthPwFocus AuthPwFocus);
	bool IsCurPwFocus(emAuthPwFocus AuthPwFocus);
	void ShuffleNumber();

#ifdef PRE_MOD_SECURITYNUMPAD_RENEWAL
	bool CheckAreaSecurityNumber( float fMouseX, float fMouseY );
#endif 
};


