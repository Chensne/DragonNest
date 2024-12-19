#pragma once

#include "EtUIDialog.h"

class CDnSkillGuideInitItemDlg : public CEtUIDialog
{
public:
	CDnSkillGuideInitItemDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallBack = NULL );
	virtual ~CDnSkillGuideInitItemDlg(void);

protected:
	CEtUIStatic * m_pCommentStatic;

public:
	virtual void Initialize(bool bShow);
	virtual void InitialUpdate();

	void SetResetGuideComment();
	void SetItemSkillComment();
};