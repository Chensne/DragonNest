#include "Stdafx.h"

#ifdef PRE_ADD_CASHINVENTAB

#include "DnInvenCashDlg.h"
#include "DnInvenCashTabDlg.h"


CDnInvenCashTabDlg::CDnInvenCashTabDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
{

}


CDnInvenCashTabDlg::~CDnInvenCashTabDlg()
{
	m_vecRadioBtn.clear();
	m_vecStaticNew.clear();
}

void CDnInvenCashTabDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "InvenCashTabDlg.ui" ).c_str(), bShow );
}


void CDnInvenCashTabDlg::InitialUpdate()
{
	

	char buf[32];
	int size = 0;
	size = ETABTYPE::ETABTYPE_MAX;
	for( int i=0; i<size; ++i )
	{		
		memset( buf, 0, 32 );
		sprintf_s( buf, 32, "ID_RBT_TAB%d", i );		
		CEtUIRadioButton * pRad = GetControl<CEtUIRadioButton>( buf );

		sprintf_s( buf, 32, "ID_STATIC_NEW%d", i );		
		CEtUIStatic * pStatic = GetControl<CEtUIStatic>( buf );
		pStatic->Show( false );

		m_vecRadioBtn.push_back( pRad );
		m_vecStaticNew.push_back( pStatic );
	}
	m_vecRadioBtn[0]->SetChecked( true );
	
}


void CDnInvenCashTabDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{


	}
	else
	{
		int size = (int)m_vecStaticNew.size();
		for( int i=0; i<size; ++i )
			m_vecStaticNew[ i ]->Show( false ); // New.
	}


	CEtUIDialog::Show( bShow );
}


void CDnInvenCashTabDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{

	}

	// RadioButton.
	else if( nCommand == EVENT_RADIOBUTTON_CHANGED )
	{
		// Tab.
		if( strstr( pControl->GetControlName(), "ID_RBT_TAB" ) )
		{
			//std::string str( pControl->GetControlName() );
			//str = str.substr( strlen("ID_RBT_TAB"), str.size() );
			//ChangeTab( ::atoi( str.c_str() ) );
			CEtUIRadioButton * pRadioBtn = (CEtUIRadioButton *)pControl;			
			static_cast< CDnInvenCashDlg * >( m_pParentDialog )->ChangeTab( pRadioBtn->GetTabID() );
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnInvenCashTabDlg::AddNewItem( int idx )
{	
	m_vecStaticNew[ idx ]->Show( true );
}

//void CDnInvenCashTabDlg::MoveDialog( float fX, float fY )
//{
//	
//		
//	CEtUIDialog::MoveDialog( fX, fY );
//	/*
//
//	int size = (int)m_pVecTypeInvenSlotDlg.size();
//	for( int i=0; i<size; ++i )
//	{
//		STabInfo * pTab = m_pVecTypeInvenSlotDlg[ i ];
//		for( int x = 0; x < (int)pTab->vecInvenSlotDlg.size(); ++x )
//			pTab->vecInvenSlotDlg[x]->MoveDialog( fX, fY );
//	}*/
//
//}

#endif // PRE_ADD_CASHINVENTAB