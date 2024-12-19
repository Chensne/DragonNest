#pragma once
#include "DnCountDlg.h"
#include "DnInterface.h"

class CDnCpScoreCountDlg : public CDnCountDlg
{
public:
	CDnCpScoreCountDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_BOTTOM, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCpScoreCountDlg(void)	{}

	void			ShowCpScoreCount(int cp, float rightX, bool showAction);
	void			Shake(float offsetX);

	virtual void	InitialUpdate();
	virtual void	Initialize(bool bShow);
	virtual void	Process(float fElapsedTime);
	virtual void	Render(float fElapsedTime);
	void			Hide();

	float			GetControlWidth()	{ return m_pCountCtl->GetControlWidth(); }
	float			GetCpPointStaticWidth() const;

private:
	bool			ProcessShowAction();

	const float		m_fXGapConst;
	const float		m_fYGapConst;
	bool			m_bShowAction;

//	float			m_DlgTargetPosX;
	float			m_DlgTargetPosY;

	const float		m_fShowActionYGap;
	const float		m_ShowActionOffset;
};
