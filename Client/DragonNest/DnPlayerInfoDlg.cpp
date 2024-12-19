#include "StdAfx.h"
#include "DnPlayerInfoDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnPlayerInfoDlg::CDnPlayerInfoDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback  )
	, m_nJobClassID(-1)
{
	m_pJobIcon = NULL;
	m_pStaticSlotStateBoard = NULL;
	m_pStaticSlotStateStr = NULL;
	m_pPlayerLevel = NULL;
	m_pPlayerName = NULL;
	m_pVillageName = NULL;
}

CDnPlayerInfoDlg::~CDnPlayerInfoDlg(void)
{
}

void CDnPlayerInfoDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PlayerInfo.ui" ).c_str(), bShow );
}

void CDnPlayerInfoDlg::InitialUpdate()
{
	m_pPlayerLevel = GetControl< CEtUIStatic >( "ID_LEVEL" );
	m_pPlayerName = GetControl< CEtUIStatic >( "ID_PLAYER_NAME" );
	m_pVillageName = GetControl< CEtUIStatic >( "ID_VILLAGE_NAME" );
	m_pJobIcon = GetControl< CDnJobIconStatic >( "ID_JOBICON" );
	m_pStaticSlotStateBoard = GetControl< CEtUIStatic >( "ID_STATIC2" );
	m_pStaticSlotStateStr = GetControl< CEtUIStatic >( "ID_STATIC1" );

	m_pStaticSlotStateBoard->Show( false );
	m_pStaticSlotStateStr->Show( false );
}

void CDnPlayerInfoDlg::SetInfomation( LPCWSTR pwszPlayerName, LPCWSTR pwszVillageName, int nLevel, int nJobClassID )
{
	WCHAR wszPlayerLevel[12] = {0};

#ifdef PRE_MOD_CONSTANT_TEXT_CONVERT_TO_TABLE_TEXT
	swprintf_s( wszPlayerLevel, _countof(wszPlayerLevel), L"%s %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7888 ), nLevel );  // UISTRING : Lv. 
#else
	swprintf_s( wszPlayerLevel, _countof(wszPlayerLevel), L"Lv. %d", nLevel ); 
#endif
	m_pPlayerLevel->SetText( wszPlayerLevel );
	m_pPlayerName->SetText( pwszPlayerName );
	m_pVillageName->SetText( pwszVillageName );

	m_nJobClassID = nJobClassID;
	m_pJobIcon->SetIconID(m_nJobClassID);
}

bool CDnPlayerInfoDlg::IsInfoValid()
{
	if( m_nJobClassID == -1 )
		return false;

	return true;
}

void CDnPlayerInfoDlg::ClearInfo()
{
	m_nJobClassID = -1;
	m_pPlayerLevel->ClearText();
	m_pPlayerName->ClearText();
	m_pVillageName->ClearText();
	m_pJobIcon->SetIconID(-1);
}