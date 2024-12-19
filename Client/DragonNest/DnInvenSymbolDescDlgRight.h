#pragma once
#include "EtUIDialog.h"

class CDnInvenSymbolDescDlgRight : public CEtUIDialog
{
public:
	CDnInvenSymbolDescDlgRight( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnInvenSymbolDescDlgRight(void);

protected:
	CEtUIStatic *m_pStaticName;
	CEtUIStatic *m_pStaticText;

public:
	void SetDesc( const wchar_t *wszName, const wchar_t *wszDesc );

public:
	virtual void Initialize(bool bShow);
	virtual void InitialUpdate();
};
