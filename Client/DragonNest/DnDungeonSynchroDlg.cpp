#include "StdAfx.h"
#include "DnDungeonSynchroDlg.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnDungeonSynchroDlg::CDnDungeonSynchroDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, false )
, m_pStaticSyncroSuccess( NULL )
, m_pStaticSyncroSuccessBg( NULL )
, m_pStaticSyncroFail( NULL )
, m_pStaticSyncroFailBg( NULL )
{
}

CDnDungeonSynchroDlg::~CDnDungeonSynchroDlg()
{
}

void CDnDungeonSynchroDlg::InitialUpdate()
{
	m_pStaticSyncroSuccess = GetControl<CEtUIStatic>( "ID_TEXT_SYNCHRO" );
	m_pStaticSyncroSuccess->Show( false );
	m_pStaticSyncroSuccessBg = GetControl<CEtUIStatic>( "ID_STATIC_SYNCHRO" );
	m_pStaticSyncroSuccessBg->Show( false );
	m_pStaticSyncroFail = GetControl<CEtUIStatic>( "ID_TEXT_SYNCHROFAILED" );
	m_pStaticSyncroFail->Show( false );
	m_pStaticSyncroFailBg = GetControl<CEtUIStatic>( "ID_STATIC_SYNCHROFAILED" );
	m_pStaticSyncroFailBg->Show( false );
}

void CDnDungeonSynchroDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "DungeonSynchroDlg.ui" ).c_str(), bShow );
}

void CDnDungeonSynchroDlg::Show( bool bShow )
{
	if( m_bShow == bShow ) return;
	CDnCustomDlg::Show( bShow );
}

void CDnDungeonSynchroDlg::SetSynchro( bool bSynchro )
{
	if( bSynchro )
	{
		m_pStaticSyncroSuccess->Show( true );
		m_pStaticSyncroSuccessBg->Show( true );
		m_pStaticSyncroFail->Show( false );
		m_pStaticSyncroFailBg->Show( false );
	}
	else
	{
		m_pStaticSyncroSuccess->Show( false );
		m_pStaticSyncroSuccessBg->Show( false );
		m_pStaticSyncroFail->Show( true );
		m_pStaticSyncroFailBg->Show( true );
	}
}