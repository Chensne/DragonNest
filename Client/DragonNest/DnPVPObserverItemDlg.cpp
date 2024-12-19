#include "StdAfx.h"
#include "DnPVPObserverItemDlg.h"


CDnPVPObserverItemDlg::CDnPVPObserverItemDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pStaticLevel(NULL)
, m_pStaticJob(NULL)
, m_pStaticName(NULL)
{
}

CDnPVPObserverItemDlg::~CDnPVPObserverItemDlg(void)
{
}

void CDnPVPObserverItemDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PvpObserverListDlg.ui" ).c_str(), bShow );
}

void CDnPVPObserverItemDlg::InitialUpdate()
{
	m_pStaticLevel = GetControl<CEtUIStatic>( "ID_TEXT_LEVEL" );
	m_pStaticJob =	GetControl<CEtUIStatic>( "ID_TEXT_JOB" );
	m_pStaticName = GetControl<CEtUIStatic>( "ID_TEXT_ID" );
}


void CDnPVPObserverItemDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnPVPObserverItemDlg::SetCharInfo(int Level,LPCWSTR Jobindex,LPCWSTR Name)
{

	m_pStaticLevel->SetIntToText(Level);
	m_pStaticLevel->Show(true);

	m_pStaticJob->SetText(Jobindex);
	m_pStaticJob->Show(true);

	m_pStaticName->SetText(Name);
	m_pStaticName->Show(true);
}
