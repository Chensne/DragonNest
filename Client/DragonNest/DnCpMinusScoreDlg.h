
#pragma once

class CDnCpMinusScoreDlg : public CEtUIDialog
{
public:
	CDnCpMinusScoreDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_BOTTOM, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnCpMinusScoreDlg(void)	{}

	void			ShowCpMinusScore(int score, float posXRatio, float posYRatio, bool bBlink = false);

	virtual void	InitialUpdate();
	virtual void	Initialize( bool bShow );
	virtual void	Process( float fElapsedTime );
	virtual void	Render( float fElapsedTime );

	float			GetDelayTime() const;
	void			SetDelayTime( float fDelayTime ) {}
	bool			IsShowCount();
	void			ShowCount( bool bShow );

private:
	bool			UpdateShowAni();
	void			Close();
	int				CalcAlpha(bool bShowType, const SUICoord& uiCoord, float delta);

	CEtUIStatic*	m_CpScoreStatic;
	bool			m_bStart;
	bool			m_bHide;
	bool			m_bClosed;
	bool			m_bBlink;
	bool			m_bBlinker;
	float			m_ShowWaitingTime;
	float			m_HideLimitY;
	int				m_CurAlpha;

	const DWORD		m_CpScoreColorConst;
	const DWORD		m_CpScoreShadowColorConst;
	const float		m_CpScoreShowTime;
	const float		m_HideDistConst;
	const float		m_HideYOffsetCoeffConst;
};
