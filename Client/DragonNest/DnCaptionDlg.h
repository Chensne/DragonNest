#pragma once
#include "EtUIDialog.h"

class CDnCaptionDlg : public CEtUIDialog
{
public:
	CDnCaptionDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnCaptionDlg(void);

protected:
	CEtUIStatic *m_pCaption;
	float m_fShowTime;
	float m_fShowRatio;
	float m_fElapsedTime;
	float m_fConst;
	EtColor m_TextColor;

public:
	void SetCaption( LPCWSTR wszMsg, DWORD dwColor, float fFadeTime );
	void CloseCaption();

public:
	virtual void InitialUpdate();
	virtual void Process( float fElapsedTime );
	virtual void Render( float fElapsedTime );
};
