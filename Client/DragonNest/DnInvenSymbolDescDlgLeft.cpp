#include "StdAfx.h"
#include "DnInvenSymbolDescDlgLeft.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnInvenSymbolDescDlgLeft::CDnInvenSymbolDescDlgLeft( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
	, m_pStaticName(NULL)
	, m_pStaticText(NULL)
{
}

CDnInvenSymbolDescDlgLeft::~CDnInvenSymbolDescDlgLeft(void)
{
}

void CDnInvenSymbolDescDlgLeft::Initialize(bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "InvenSymbolDescDlgLeft.ui" ).c_str(), bShow );
}

void CDnInvenSymbolDescDlgLeft::InitialUpdate()
{
	m_pStaticName = GetControl<CEtUIStatic>("ID_STATIC_NAME");
	m_pStaticText = GetControl<CEtUIStatic>("ID_STATIC_TEXT");
}

void CDnInvenSymbolDescDlgLeft::SetDesc( const wchar_t *wszName, const wchar_t *wszDesc )
{
	m_pStaticName->ClearText();
	m_pStaticText->ClearText();

	m_pStaticName->SetText( wszName );
	m_pStaticText->SetText( wszDesc );
}