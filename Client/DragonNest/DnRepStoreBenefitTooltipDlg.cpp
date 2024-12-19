#include "StdAfx.h"
#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
#include "DnRepStoreBenefitTooltipDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnRepStoreBenefitTooltipDlg::CDnRepStoreBenefitTooltipDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback ) : CEtUIDialog( dialogType, pParentDialog, nID, pCallback ), 
																																						 m_pTBRepStoreBenefit( NULL )
{
}

CDnRepStoreBenefitTooltipDlg::~CDnRepStoreBenefitTooltipDlg(void)
{
}

void CDnRepStoreBenefitTooltipDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "SimpleTooltipDlg.ui" ).c_str(), bShow );
}

void CDnRepStoreBenefitTooltipDlg::InitialUpdate()
{
	m_pTBRepStoreBenefit = GetControl<CEtUITextBox>( "ID_TEXTBOX" );
}

void CDnRepStoreBenefitTooltipDlg::SetText( const wchar_t* pText )
{
	_ASSERT( pText );
	m_pTBRepStoreBenefit->ClearText();
	m_pTBRepStoreBenefit->AddColorText( pText, textcolor::FONT_ORANGE );
}
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM