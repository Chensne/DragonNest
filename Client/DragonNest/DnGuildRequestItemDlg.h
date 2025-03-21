#pragma once
#include "DnCustomDlg.h"

class CDnGuildRequestItemDlg : public CDnCustomDlg
{
public:
	CDnGuildRequestItemDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildRequestItemDlg(void);

protected:
	CDnJobIconStatic *m_pJobIcon;
	CEtUIStatic *m_pStaticJob;
	CEtUIStatic *m_pStaticLevel;
	CEtUIStatic *m_pStaticName;
	INT64 m_nCharacterDBID;

public:
	void SetInfo( INT64 nCharacterDBID, int nJobID, const WCHAR *wszText1, const WCHAR *wszText2, const WCHAR *wszText3 );
	INT64 GetCharacterDBID() { return m_nCharacterDBID; }
	LPCWSTR GetName() { return m_pStaticName->GetText(); }

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
};