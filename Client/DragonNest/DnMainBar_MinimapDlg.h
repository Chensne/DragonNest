#pragma once

#include "EtUIDialog.h"
#define RIGHT_DEGREE_MAX	0.750491f
#define	LEFT_DEGREE_MAX		-3.893829f
class CDnMainBar_MinimapDlg : public CEtUIDialog
{
public:
	CDnMainBar_MinimapDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnMainBar_MinimapDlg(void);

protected:

	EtTextureHandle m_hBackLight;
	CEtUIStatic *m_pStaticBackLight;
	EtTextureHandle m_hBackBlack;
	CEtUIStatic *m_pStaticBackBlack;

	bool isShow;

	bool m_bShowOption;
	bool m_bShowDiceOption;
public:
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void Show(bool bShow);
	virtual void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0);
	virtual void Render(float fElapsedTime);
	virtual bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual void DrawMinimapOutInfo();
	virtual void ShowMinimapOption(bool bShow);
	virtual void ShowAutoDiceOption(bool bShow);
	virtual void UpateAutoDiceOption(bool bReset);
	virtual void UpdateMinimapOption();
	virtual void DrawIcon(CEtUIControl *pControl, float fAngle, float fRadius, float fValue);
};
