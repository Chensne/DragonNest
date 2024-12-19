#pragma once

#include "EtUIDialog.h"

class CDnFadeInOutDlg : public CEtUIDialog
{
public:
	enum emFADE_MODE
	{
		modeBegin,
		modeEnd,
	};

public:
	CDnFadeInOutDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnFadeInOutDlg(void);

protected:
	SUICoord m_ScreenCoord;
	
	emFADE_MODE m_FadeMode;

	EtColor m_srcColor;
	EtColor m_destColor;
	EtColor m_renderColor;

	float m_fConst;
	float m_fFadeTime;

	bool m_bCallFunc;

protected:
	void CheckFadeComplete();

public:
	void SetFadeColor( DWORD srcColor, DWORD destColor, bool bForceApplySrcColor );
	void SetFadeTime( float fFadeTime );
	void SetFadeMode( emFADE_MODE emFadeMode );
	emFADE_MODE GetFadeMode() { return m_FadeMode; }

	DWORD GetSrcColor() { return m_srcColor; }
	DWORD GetDestColor() { return m_destColor; }

public:
	virtual void InitialUpdate();
	virtual void Render( float fElapsedTime );
	virtual void Process( float fElapsedTime );
	virtual void OnChangeResolution();
};

class CDnFadeForBlowDlg : public CDnFadeInOutDlg
{
public:
	CDnFadeForBlowDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnFadeForBlowDlg(void);

protected:
	int m_nStep;
	float m_fFadeTimeForBlow;
	bool m_bUseInvert;

public:
	virtual void Render( float fElapsedTime );
	virtual void Process( float fElapsedTime );
	virtual bool IsShow() const;

	void SetFadeForBlow( float fFadeTime, DWORD dwColor = 0xFFFFFFFF );
};