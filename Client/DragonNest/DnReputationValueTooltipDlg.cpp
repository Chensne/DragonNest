#include "StdAfx.h"
#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
#include "DnReputationValueTooltipDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnReputationValueTooltipDlg::CDnReputationValueTooltipDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback ) : CEtUIDialog( dialogType, pParentDialog, nID, pCallback ), 
																																						 m_pTBReputationValue( NULL )
{
}

CDnReputationValueTooltipDlg::~CDnReputationValueTooltipDlg(void)
{
}

void CDnReputationValueTooltipDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "SimpleTooltipDlg.ui" ).c_str(), bShow );
}

void CDnReputationValueTooltipDlg::InitialUpdate()
{
	m_pTBReputationValue = GetControl<CEtUITextBox>( "ID_TEXTBOX" );
}

void CDnReputationValueTooltipDlg::SetReputationPercent( float fCurReputationValue, float fMaxReputationValue )
{
	wchar_t szTemp[ 256 ]={ 0 };
	swprintf_s( szTemp, 256, L"%2.2f/%2.2f", fCurReputationValue, fMaxReputationValue );
	m_pTBReputationValue->ClearText();

	m_pTBReputationValue->AddText( szTemp, textcolor::FONT_ORANGE );
}
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM