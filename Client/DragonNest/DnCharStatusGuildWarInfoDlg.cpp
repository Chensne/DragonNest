#include "StdAfx.h"
#include "DnCharStatusGuildWarInfoDlg.h"
#include "DnGuildWarTask.h"
#include "DnGuildTask.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCharStatusGuildWarInfoDlg::CDnCharStatusGuildWarInfoDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
, m_pGuildWarPoint( NULL )
, m_pCountGoFinals( NULL )
, m_pCountWin( NULL )
, m_pGuildFestPoint(NULL)
{

}

CDnCharStatusGuildWarInfoDlg::~CDnCharStatusGuildWarInfoDlg()
{
}

void CDnCharStatusGuildWarInfoDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CharStatusGuildWarInfoDlg.ui" ).c_str(), bShow );
}

void CDnCharStatusGuildWarInfoDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
		SetGuildWarInfo();

	CEtUIDialog::Show( bShow );
}

void CDnCharStatusGuildWarInfoDlg::InitialUpdate()
{
	m_pGuildWarPoint = GetControl<CEtUIStatic>( "ID_TEXT_POINT" );
	m_pCountGoFinals = GetControl<CEtUIStatic>( "ID_STATIC8" );
 	m_pCountWin = GetControl<CEtUIStatic>( "ID_STATIC9" );

	m_pGuildFestPoint = GetControl<CEtUIStatic>("ID_TEXT_ADDPOINT");
}

void CDnCharStatusGuildWarInfoDlg::SetGuildWarInfo()
{
	m_pGuildWarPoint->SetIntToText( (int)GetGuildWarTask().GetGuildWarFestivalPoint() );

	WCHAR wszTemp[128] = { 0, };
	swprintf_s( wszTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126172 ), GetGuildTask().GetGuildInfo()->nGuildWarFinalCount );
	m_pCountGoFinals->SetText( wszTemp );
	swprintf_s( wszTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126173 ), GetGuildTask().GetGuildInfo()->nGuildWarFinalWinCount );
	m_pCountWin->SetText( wszTemp );

	//3377
	WCHAR wszGuildFestPoint[128] = { 0, };
	swprintf_s( wszGuildFestPoint, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3377 ), GetGuildTask().GetGuildRewardValue(GuildReward::EXTRA_FESTPOINT) );
	m_pGuildFestPoint->SetText(wszGuildFestPoint);
}
