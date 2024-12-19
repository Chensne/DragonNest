#pragma once
#include "EtUIDialog.h"

class CEtUIStatic;
class CEtUIEditBox;

class CDnPassWordInputDlg : public CEtUIDialog
{
public:
	CDnPassWordInputDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnPassWordInputDlg(void);

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




#ifdef PRE_MOD_PVPOBSERVER
//---------------------------------------------------------------------------------------
// CDnPassWordInputEventDlg
// : 위의 CDnPassWordInputDlg 와 거의 동일한 Dlg - 크기가 조금 넓음.
class CDnPassWordInputEventDlg : public CDnPassWordInputDlg
{
public:
	CDnPassWordInputEventDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnPassWordInputEventDlg();

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
};
#endif // PRE_MOD_PVPOBSERVER
