#include "StdAfx.h"
#include "DnGachaBuyConfirm.h"

#ifdef PRE_ADD_GACHA_JAPAN


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGachaBuyConfirmDlg::CDnGachaBuyConfirmDlg( UI_DIALOG_TYPE dialogType /* = UI_TYPE_FOCUS */, 
										  CEtUIDialog *pParentDialog /* = NULL */, 
										  int nID /* = -1 */, CEtUICallback *pCallback /* = NULL */ ) : CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
										  //m_pStaticBuyConfirm( NULL )
{
	
}

CDnGachaBuyConfirmDlg::~CDnGachaBuyConfirmDlg(void)
{

}


void CDnGachaBuyConfirmDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "Jp_Confirm.ui" ).c_str(), bShow );
}


void CDnGachaBuyConfirmDlg::InitialUpdate( void )
{
	//m_pStaticBuyConfirm = GetControl<CEtUIStatic>( "ID_STATIC0" );
	m_SmartMove.SetControl( GetControl<CEtUIButton>( "ID_BUTTON_OK" ) );
}

void CDnGachaBuyConfirmDlg::Show( bool bShow )
{
	CEtUIDialog::Show( bShow );
	if( bShow )
		m_SmartMove.MoveCursor();
	else
		m_SmartMove.ReturnCursor();
}

#endif //PRE_ADD_GACHA_JAPAN