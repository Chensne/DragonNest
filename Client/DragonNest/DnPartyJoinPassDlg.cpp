#include "StdAfx.h"
#include "DnPartyJoinPassDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnPartyJoinPassDlg::CDnPartyJoinPassDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
	, m_pStaticPassword(NULL)
	, m_pEditBoxPassWord(NULL)
	, m_pButtonJoin(NULL)
{
}

CDnPartyJoinPassDlg::~CDnPartyJoinPassDlg(void)
{
}

void CDnPartyJoinPassDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PartyPassDlg.ui" ).c_str(), bShow );
}

void CDnPartyJoinPassDlg::InitialUpdate()
{
	m_pStaticPassword = GetControl<CEtUIStatic>("ID_PASSWORD");
	m_pEditBoxPassWord = GetControl<CEtUIEditBox>("ID_EDITBOX_PASS");
	m_pButtonJoin = GetControl<CEtUIButton>("ID_OK");

	m_pStaticPassword->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100035 ) );
	m_pButtonJoin->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9 ) );
}

void CDnPartyJoinPassDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );

	if( bShow )
	{
		m_pEditBoxPassWord->ClearText();
		RequestFocus(m_pEditBoxPassWord);
	}
}

LPCWSTR CDnPartyJoinPassDlg::GetPassword()
{
	return m_pEditBoxPassWord->GetText();
}