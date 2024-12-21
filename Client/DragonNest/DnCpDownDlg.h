
#pragma once

#include "DnCountDlg.h"

class CDnCpDownDlg : public CDnCountDlg
{
public:
	CDnCpDownDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_BOTTOM, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnCpDownDlg(void)	{}

	void			ShowCpDownCount(int score, float rightX, float posY);

	virtual void	InitialUpdate();
	virtual void	Initialize( bool bShow );
	virtual void	Process( float fElapsedTime );
	virtual void	Render( float fElapsedTime );

private:
	bool			UpdateShowAni();
	int				CalcAlpha(bool bShowType, const SUICoord& uiCoord, float delta);
	void			Close();

	bool			m_bStart;
	bool			m_bHide;
	bool			m_bClosed;
	float			m_ShowTime;
	float			m_HideLimitY;
	int				m_CurAlpha;

	const float		m_CpScoreShowTime;
	const float		m_HideDistConst;
	const float		m_HideYOffsetCoeffConst;
};
