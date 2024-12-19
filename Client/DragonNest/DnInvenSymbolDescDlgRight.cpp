#include "StdAfx.h"
#include "DnInvenSymbolDescDlgRight.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnInvenSymbolDescDlgRight::CDnInvenSymbolDescDlgRight( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
	, m_pStaticName(NULL)
	, m_pStaticText(NULL)
{
}

CDnInvenSymbolDescDlgRight::~CDnInvenSymbolDescDlgRight(void)
{
}

void CDnInvenSymbolDescDlgRight::Initialize(bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "InvenSymbolDescDlgRight.ui" ).c_str(), bShow );
}

void CDnInvenSymbolDescDlgRight::InitialUpdate()
{
	m_pStaticName = GetControl<CEtUIStatic>("ID_STATIC_NAME");
	m_pStaticText = GetControl<CEtUIStatic>("ID_STATIC_TEXT");
}

void CDnInvenSymbolDescDlgRight::SetDesc( const wchar_t *wszName, const wchar_t *wszDesc )
{
	m_pStaticName->ClearText();
	m_pStaticText->ClearText();

	m_pStaticName->SetText( wszName );
	m_pStaticText->SetText( wszDesc );
}