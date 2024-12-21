#include "stdafx.h"
#include "DnAlteaClearDlg.h"
#include "DnAlteaTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

#if defined( PRE_ALTEIAWORLD_EXPLORE )

#define ALTEA_CLEAR_DLG_SHOW_TIME	3

CDnAlteaClearDlg::CDnAlteaClearDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pStatic_Clear( NULL )
, m_pStatic_Fail( NULL )
, m_fElapsedTime( 0.0f )
{

}

CDnAlteaClearDlg::~CDnAlteaClearDlg(void)
{

}

void CDnAlteaClearDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "AlteaClearDlg.ui" ).c_str(), bShow );
}

void CDnAlteaClearDlg::InitialUpdate()
{
	m_pStatic_Clear = GetControl<CEtUIStatic>("ID_STATIC_CLEAR");
	m_pStatic_Fail = GetControl<CEtUIStatic>("ID_STATIC_FAIL");
}

void CDnAlteaClearDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( false == IsShow() )
		return;

	if( m_fElapsedTime > 0.f )
	{
		m_fElapsedTime -= fElapsedTime;
		if( m_fElapsedTime <= 0.f )
		{
			m_fElapsedTime = 0.f;
			Show( false );
		}
	}
}

void CDnAlteaClearDlg::Show_ClearDlg( bool bClear )
{
	m_fElapsedTime = ALTEA_CLEAR_DLG_SHOW_TIME;

	m_pStatic_Clear->Show( bClear );
	m_pStatic_Fail->Show( !bClear );
}

#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )