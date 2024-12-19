#pragma once

#ifdef PRE_ADD_STAMPSYSTEM

#include "EtUIDialog.h"

class CDnNPCArrowDlg : public CEtUIDialog
{

private:
	CEtUIStatic * m_pStaticArrow;

public:
	CDnNPCArrowDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnNPCArrowDlg(){}

	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void Show( bool bShow );
	
	CEtUIStatic * GetStaticArrow(){
		return m_pStaticArrow;
	}
};

#endif // PRE_ADD_STAMPSYSTEM