#pragma once
#include "DnCountDlg.h"
#include "DnInterface.h"

class CDnCpJudgeControl;
class CDnCpJudgeDlg : public CDnCustomDlg
{
public:
	CDnCpJudgeDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_BOTTOM, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCpJudgeDlg()	{}

	void			Initialize(bool bShow);
	void			InitCustomControl(CEtUIControl *pControl);

	void			SetCpJudge(CDnInterface::eCpJudgeType type, float relativeX, float relativeY, float hideTargetY, float delay);
	void			ShowCount(bool bShow);
	bool			IsShowCount();
	void			Hide(float waitingTime);

	virtual void	Process(float fElapsedTime);
	virtual void	Render(float fElapsedTime);

	float			GetDelayTime() const;
	void			SetDelayTime(float fDelayTime)	{}

private:
	bool			UpdateShowAni();
	bool			UpdateHideAni();
	void			SetCoords(SUICoord& coord);
	void			Close();
	bool			IsClosed() const				{ return m_bClosed; }

	CDnCpJudgeControl*	m_pJudgeCtrl;

	bool			m_bAction;
	float			m_fHideElapsedTime;
	float			m_fHideWaitingTime;
	SUICoord		m_TargetCoord;
	bool			m_bClosed;
	DWORD			m_Color;
	bool			m_bHide;
	float			m_HideDist;
	float			m_fHideLimitY;

	const float		m_HideYOffsetCoeff;
	const float		m_HideXOffsetCoeff;
	const float		m_HideXOffsetCoeff2;
	const DWORD		m_ShadowColorValue;
};
