#pragma once

#include "EtUIDialog.h"

#ifdef PRE_MOD_REPUTE_RENEW

class CDnRepUnionMembershipMarkDlg : public CEtUIDialog
{
public:
	CDnRepUnionMembershipMarkDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnRepUnionMembershipMarkDlg(void);

	void Initialize(bool bShow);
	void InitialUpdate();

	void SetUnionMark(int iconIdx);

private:
	CEtUITextureControl*	m_pMark;
	EtTextureHandle			m_hMarkTexture;
};

#endif // PRE_MOD_REPUTE_RENEW