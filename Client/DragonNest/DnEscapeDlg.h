#pragma once
#include "EtUIDialog.h"

#ifdef PRE_FIX_ESCAPE

class CDnEscapeDlg : public CEtUIDialog
{
public:
	enum eEscape
	{
		Immediate = 0,
		ProgressTime = 3,
		GlobalTime = 30,
	};

public:
	CDnEscapeDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnEscapeDlg();

	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );

protected:
	CEtUIStatic *m_pStaticText;
	CEtUIProgressBar *m_pProgressBar;

	float m_fProgressBarTime;
	static float s_fProgressGlobalTime;

};

#endif