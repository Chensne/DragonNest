#pragma once
#include "EtUIDialog.h"

#if defined( PRE_ALTEIAWORLD_EXPLORE )

class CDnAlteaClearDlg : public CEtUIDialog
{
public:
	CDnAlteaClearDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnAlteaClearDlg(void);

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Process( float fElapsedTime );

	void Show_ClearDlg( bool bClear );

protected:
	CEtUIStatic * m_pStatic_Clear;
	CEtUIStatic * m_pStatic_Fail;

	float m_fElapsedTime;
};

#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )