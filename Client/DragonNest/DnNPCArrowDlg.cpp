#include "Stdafx.h"


#ifdef PRE_ADD_STAMPSYSTEM

#include "DnNPCArrowDlg.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnNPCArrowDlg::CDnNPCArrowDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
,m_pStaticArrow(NULL)
{

}

void CDnNPCArrowDlg::InitialUpdate()
{	
	m_pStaticArrow = GetControl<CEtUIStatic>("ID_EVENT_ARROW");	
	m_pStaticArrow->SetTextureColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

	CEtUIStatic * pStatic = GetControl<CEtUIStatic>("ID_NOTIFY_ARROW");
	if( pStatic )
		pStatic->Show( false );

}

void CDnNPCArrowDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "NotifyArrow.ui" ).c_str(), bShow );
}

void CDnNPCArrowDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		m_pStaticArrow->SetTextureColor( 0xFFFFFFFF );
	}


	CEtUIDialog::Show( bShow );
}

#endif // PRE_ADD_STAMPSYSTEM