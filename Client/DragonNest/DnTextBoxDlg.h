#pragma once
#include "DnCommDlg.h"

class CDnTextBoxDlg : public CDnCommDlg
{
public:
	CDnTextBoxDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_TOP_MSG, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnTextBoxDlg(void);

protected:
	CEtUITextBox *m_pTextBoxText;

	DWORD m_dwCountDown;
	float m_fElapsed_CountDownTime;

	std::wstring m_strText;
	float m_fX;
	float m_fY;
	int m_nPos;
	int m_nFormat;
#ifdef PRE_FIX_TEXTDLG_TIMER
	bool m_bTimer;
#endif

protected:
	void UpdateTextBox();
	void Process_CountDown( float fElapsedTime );
	void SetTextBox_VariableType( int nType );

public:
	void SetText( LPCWSTR wszStr, float fX, float fY, float fWidth, float fDelayTime );
	void SetText( LPCWSTR wszStr, float fX, float fY, int nPos, int nFormat, float fDelayTime );
	void SetCountDown( int nCountDown, float fX, float fY, int nPos, int nFormat, float fDelayTime );

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );

	virtual void OnChangeResolution();
};