#pragma once
#include "EtUIDialog.h"

#ifdef PRE_ADD_ALTHEIA_NEWCAPTION_TRIGGER

class CDnImageLeftBlindNpcDlg : public CEtUIDialog
{
public:
	CDnImageLeftBlindNpcDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnImageLeftBlindNpcDlg();

protected:
	CEtUITextureControl* m_pTextureControl;
	CEtUIStatic*		 m_pNameStatic;
	EtTextureHandle		 m_hTexture;

public:
	virtual void Initialize(bool bShow);
	virtual void InitialUpdate();
	virtual void Show( bool bShow );

	void SetNpcTexture(WCHAR* wszNPCName, const char* szTextureFileName, bool bIsFlipH = false);
	void Reset();
};

#endif