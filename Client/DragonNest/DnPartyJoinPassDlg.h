#pragma once
#include "EtUIDialog.h"

class CEtUIStatic;
class CEtUIEditBox;

// 현재 사용하지 않는 클래스. 후에 제거하자.
class CDnPartyJoinPassDlg : public CEtUIDialog
{
public:
	CDnPartyJoinPassDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnPartyJoinPassDlg(void);

protected:
	CEtUIStatic *m_pStaticPassword;
	CEtUIEditBox *m_pEditBoxPassWord;
	CEtUIButton *m_pButtonJoin;

public:
	LPCWSTR GetPassword();

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
};