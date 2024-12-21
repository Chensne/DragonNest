#include "StdAfx.h"
#include "DnCharStatusPVPInfoItemDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnCharStatusPVPInfoItemDlg::CDnCharStatusPVPInfoItemDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pStaticItem(NULL)
, m_pStaticState(NULL)
{
}

CDnCharStatusPVPInfoItemDlg::~CDnCharStatusPVPInfoItemDlg(void)
{
}

void CDnCharStatusPVPInfoItemDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CharStatusPvpInfoBar.ui" ).c_str(), bShow );
}

void CDnCharStatusPVPInfoItemDlg::InitialUpdate()
{
	m_pStaticItem = GetControl<CEtUIStatic>( "ID_ITEM" );
	m_pStaticState = GetControl<CEtUIStatic>( "ID_STATE" );	
}

void CDnCharStatusPVPInfoItemDlg::SetInfo(const  wchar_t * wszItemText , INT64 iData )
{	
	WCHAR wszIStateText[100];

	SecureZeroMemory(wszIStateText,sizeof(wszIStateText) );


	if( wszItemText )
	{
		if( iData >= 0 )
			wsprintf(wszIStateText,L"%d", iData);
				
		m_pStaticItem->SetText( wszItemText );	
		m_pStaticState->SetText( wszIStateText );	
	}	
}

void CDnCharStatusPVPInfoItemDlg::SetInfo(const  wchar_t * wszItemText , const wchar_t *wszDataText )
{
	m_pStaticItem->SetText( wszItemText );
	m_pStaticState->SetText( wszDataText );
}