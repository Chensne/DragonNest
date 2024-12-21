#pragma once
#include "DnCustomDlg.h"
#include "DnMovieControl.h"

class CDnMovieDlg : public CDnCustomDlg
{
public:
	CDnMovieDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnMovieDlg(void);

protected:
	float m_fTimer;

protected:
	CDnMovieControl *m_pMovieControl;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void InitCustomControl( CEtUIControl *pControl );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );

	void PlayMovie( const char *szFileName, float fTimer = -1.f );
};
