#include "StdAfx.h"
#include "DnGameMovieOptDlg.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGameMovieOptDlg::CDnGameMovieOptDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CDnOptionDlg( dialogType, pParentDialog, nID, pCallback )
, m_pComboRecordResolution(NULL)
, m_pComboRecordFPS(NULL)
, m_pComboRecordVideoQuality(NULL)
, m_pComboRecordAudioQuality(NULL)
{
}

CDnGameMovieOptDlg::~CDnGameMovieOptDlg(void)
{
}

void CDnGameMovieOptDlg::Initialize( bool bShow )
{
	BaseClass::Initialize( CEtResourceMng::GetInstance().GetFullName( "GameMovieOptDlg.ui" ).c_str(), bShow );
}

void CDnGameMovieOptDlg::InitialUpdate()
{
	m_pComboRecordResolution = GetControl<CEtUIComboBox>( "ID_OP_RECORD_RESOLUTION" );
	m_pComboRecordResolution->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6123 ), NULL, 0 );
	m_pComboRecordResolution->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6124 ), NULL, 1 );
	m_pComboRecordResolution->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6126 ), NULL, 2 );
	m_pComboRecordResolution->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6125 ), NULL, 3 );
	m_pComboRecordResolution->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6133 ), NULL, 4 );
	m_pComboRecordResolution->SetSelectedByIndex(1);

	m_pComboRecordFPS = GetControl<CEtUIComboBox>( "ID_OP_RECORD_FPS" );
	m_pComboRecordFPS->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6130 ), NULL, 0 );
	m_pComboRecordFPS->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6129 ), NULL, 1 );
	m_pComboRecordFPS->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6128 ), NULL, 2 );
	m_pComboRecordFPS->SetSelectedByIndex(1);

	m_pComboRecordVideoQuality = GetControl<CEtUIComboBox>( "ID_OP_RECORD_VIDEO_QUALITY" );
	m_pComboRecordVideoQuality->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3030 ), NULL, 0 );
	m_pComboRecordVideoQuality->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3028 ), NULL, 1 );
	m_pComboRecordVideoQuality->SetSelectedByIndex(1);

	m_pComboRecordAudioQuality = GetControl<CEtUIComboBox>( "ID_OP_RECORD_AUDIO_QUALITY" );
	m_pComboRecordAudioQuality->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1349 ), NULL, 0 );
	m_pComboRecordAudioQuality->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3030 ), NULL, 1 );
	m_pComboRecordAudioQuality->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3028 ), NULL, 2 );
	m_pComboRecordVideoQuality->SetSelectedByIndex(2);
}

void CDnGameMovieOptDlg::ImportSetting()
{
	m_pComboRecordResolution->SetSelectedByIndex( CGameOption::GetInstance().m_nRecordResolution );
	m_pComboRecordFPS->SetSelectedByIndex( CGameOption::GetInstance().m_nRecordFPS );
	m_pComboRecordVideoQuality->SetSelectedByIndex( CGameOption::GetInstance().m_nRecordVideoQuality );
	m_pComboRecordAudioQuality->SetSelectedByIndex( CGameOption::GetInstance().m_nRecordAudioQuality );
}

void CDnGameMovieOptDlg::ExportSetting()
{
	CGameOption::GetInstance().m_nRecordResolution = m_pComboRecordResolution->GetSelectedIndex();
	CGameOption::GetInstance().m_nRecordFPS = m_pComboRecordFPS->GetSelectedIndex();
	CGameOption::GetInstance().m_nRecordVideoQuality = m_pComboRecordVideoQuality->GetSelectedIndex();
	CGameOption::GetInstance().m_nRecordAudioQuality = m_pComboRecordAudioQuality->GetSelectedIndex();
}

bool CDnGameMovieOptDlg::IsChanged()
{
	if( CGameOption::GetInstance().m_nRecordResolution != m_pComboRecordResolution->GetSelectedIndex() ||
		CGameOption::GetInstance().m_nRecordFPS != m_pComboRecordFPS->GetSelectedIndex() ||
		CGameOption::GetInstance().m_nRecordVideoQuality != m_pComboRecordVideoQuality->GetSelectedIndex() ||
		CGameOption::GetInstance().m_nRecordAudioQuality != m_pComboRecordAudioQuality->GetSelectedIndex() )
		return true;
	return false;
}