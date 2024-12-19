#include "StdAfx.h"
#include "DnPasswordInputDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnPassWordInputDlg::CDnPassWordInputDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
, m_pStaticPassword(NULL)
, m_pEditBoxPassWord(NULL)
, m_pButtonJoin(NULL)
{
}

CDnPassWordInputDlg::~CDnPassWordInputDlg(void)
{
}

void CDnPassWordInputDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "AskPasswordDlg.ui" ).c_str(), bShow );
}

void CDnPassWordInputDlg::InitialUpdate()
{
	m_pStaticPassword = GetControl<CEtUIStatic>("ID_TEXT");
	m_pEditBoxPassWord = GetControl<CEtUIEditBox>("ID_EDITBOX_PASS");
	m_pButtonJoin = GetControl<CEtUIButton>("ID_OK");

	m_pStaticPassword->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100035 ) );
	m_pButtonJoin->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9 ) );
}

void CDnPassWordInputDlg::Show( bool bShow ) 
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

LPCWSTR CDnPassWordInputDlg::GetPassword()
{
	return m_pEditBoxPassWord->GetText();
}






#ifdef PRE_MOD_PVPOBSERVER
//---------------------------------------------------------------------------------------
// CDnPassWordInputEventDlg
//
CDnPassWordInputEventDlg::CDnPassWordInputEventDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CDnPassWordInputDlg( dialogType, pParentDialog, nID, pCallback  )
{
}

CDnPassWordInputEventDlg::~CDnPassWordInputEventDlg(void)
{
}

void CDnPassWordInputEventDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PvpPassDlg.ui" ).c_str(), bShow );
}

void CDnPassWordInputEventDlg::InitialUpdate()
{
	m_pEditBoxPassWord = GetControl<CEtUIEditBox>("ID_EDITBOX_PASS");	
}

void CDnPassWordInputEventDlg::Show( bool bShow ) 
{
	CDnPassWordInputDlg::Show( bShow );
}
#endif // PRE_MOD_PVPOBSERVER