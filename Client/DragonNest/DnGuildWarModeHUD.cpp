#include "StdAfx.h"
#include "DnGuildWarModeHUD.h"
#include "DnPVPGameResultAllDlg.h" // 스코어에서 BestUser 아이디를 얻어옵니다.
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildWarModeHUD::CDnGuildWarModeHUD( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnPVPBaseHUD( dialogType, pParentDialog, nID, pCallback )
, m_pBluePoint(NULL)
, m_pRedPoint(NULL)
, m_pResourcePoint(NULL)
, m_pTime(NULL)
{
}

CDnGuildWarModeHUD::~CDnGuildWarModeHUD(void)
{

}

void CDnGuildWarModeHUD::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildWarGameDlg.ui" ).c_str(), bShow );
}


void CDnGuildWarModeHUD::InitialUpdate()
{
	CDnPVPBaseHUD::InitialUpdate();

	m_pRoomName = GetControl<CEtUIStatic>("ID_PVP_RESPAWN_ROOMNAME");
	m_pMYTeam = GetControl<CEtUIStatic>("ID_PVP_RESPAWN_MYTEAM");
	m_pEnemyTeam = GetControl<CEtUIStatic>("ID_PVP_RESPAWN_ENEMY");

	m_pRedPoint = GetControl<CEtUIStatic>("ID_TEXT_REDCOUNT");
	m_pBluePoint = GetControl<CEtUIStatic>("ID_TEXT_BLUECOUNT");
	m_pResourcePoint = GetControl<CEtUIStatic>("ID_TEXT_POINT");
	m_pTime = GetControl<CEtUIStatic>("ID_TEXT_TIME");

	m_pRedPoint->SetIntToText( 0 );
	m_pBluePoint->SetIntToText( 0 );
	m_pResourcePoint->SetIntToText( 0 );
	m_pTime->SetText( L"-- : --" );
}

void CDnGuildWarModeHUD::Process(float fElapsedTime)
{
	CDnPVPBaseHUD::Process( fElapsedTime );

	WCHAR wszTime[256];

	if( -1 == m_nMin )
		m_pTime->SetText( L"-- : --" );
	else
	{
		wsprintf( wszTime, L"%02d : %02d", m_nMin, m_nSec );
		m_pTime->SetText( wszTime );
	}
}

void CDnGuildWarModeHUD::SetRedPoint( int nPoint, int nTotalScore )	
{ 
	WCHAR wszString[512];
	swprintf_s( wszString, _countof(wszString), L"%s : %d / %s : %d", 
		GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126291 ),	// 거점
		nPoint,
		GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7115 ),	// 점수
		nTotalScore );

	m_pRedPoint->SetText( wszString );
}

void CDnGuildWarModeHUD::SetBluePoint( int nPoint, int nTotalScore ) 
{ 
	WCHAR wszString[512];
	swprintf_s( wszString, _countof(wszString), L"%s : %d / %s : %d", 
		GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126291 ),	// 거점
		nPoint,
		GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7115 ),	// 점수
		nTotalScore );

	m_pBluePoint->SetText( wszString );
}
