#pragma once
#include "EtUIDialog.h"

class CDnMovieProcessDlg : public CEtUIDialog
{
public:
	CDnMovieProcessDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnMovieProcessDlg(void);

protected:
	CEtUIStatic *m_pStatic;
	CEtUIButton *m_pProcessComplete;	// 안보이는 더미버튼.
	CEtUIButton *m_pCancelButton;
	CEtUIProgressBar *m_pProgressBarTime;

	float m_fMaxTimer;
	float m_fTimer;
	char m_cFlag;

public:
	void DisableFlag() { m_cFlag = 0; }	// UI CallBack으로 Cancel처리 안하고 임의로 Dialog 닫아야 할 경우 호출
	void SetInfo( LPCWSTR wszMessage, float fTimer, int nID, CEtUICallback *pCall, bool bShowButton = true );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};