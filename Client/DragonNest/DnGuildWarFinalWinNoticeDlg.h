#pragma once
#include "DnCustomDlg.h"

class CDnGuildWarFinalWinNoticeDlg : public CDnCustomDlg
{
public:
	CDnGuildWarFinalWinNoticeDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildWarFinalWinNoticeDlg();

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );

public:
	void SetFianlWinNoticeMsg( const std::wstring &strMsg );

private:
	float	m_fFadeAwayTime;
};