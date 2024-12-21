#pragma once
#include "EtUIDialog.h"

#ifdef PRE_PARTY_RENEW_THIRD

class CDnPartyPassDlg : public CEtUIDialog
{
public:
	CDnPartyPassDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnPartyPassDlg(void);

	struct SPartyPassUnit
	{
		CEtUIStatic* pMasterMark;
		CEtUIStatic* pName;
		CEtUIStatic* pLevel;
		CEtUIStatic* pJob;
	};

protected:
	CEtUIEditBox* m_pEditBoxPassWord;
	CEtUIStatic* m_pStaticPartyName;
	CEtUIStatic* m_pStaticTargetStage;

	std::vector<SPartyPassUnit> m_pPartyMemberInfoList;

	CEtUIButton* m_pConfirmBtn;
	CEtUIButton* m_pCancelBtn;

public:
	LPCWSTR GetPassword();

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
};

#endif // PRE_PARTY_RENEW_THIRD