#pragma once

#include "DnCustomDlg.h"

class CDnCaptionDlg_05 : public CDnCustomDlg
{
public:
	CDnCaptionDlg_05(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCaptionDlg_05(void);

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();

	void AddCaption(LPCWSTR wszMsg, DWORD dwColor);
	void CloseCaption();

protected:
	CDnMessageStatic*	m_pStaticMessage;
	virtual void Process(float fElapsedTime);

private:
	int				m_CurPointIdx;
	std::wstring	m_Msg;
	float			m_PointLetterAnimTerm;
};