#include "StdAfx.h"
#include "DnGuildRequestItemDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildRequestItemDlg::CDnGuildRequestItemDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
, m_pJobIcon(NULL)
, m_pStaticJob(NULL)
, m_pStaticLevel(NULL)
, m_pStaticName(NULL)
{
	m_nCharacterDBID = 0;
}

CDnGuildRequestItemDlg::~CDnGuildRequestItemDlg(void)
{
}

void CDnGuildRequestItemDlg::Initialize( bool bShow )
{
#ifdef _ADD_RENEWED_GUILDUI
	CDnCustomDlg::Initialize(CEtResourceMng::GetInstance().GetFullName("Wanted_RequestListDlg.ui").c_str(), bShow);
#else
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildRequestListDlg.ui" ).c_str(), bShow );
#endif
}

void CDnGuildRequestItemDlg::InitialUpdate()
{
	m_pJobIcon = GetControl<CDnJobIconStatic>( "ID_STATIC_CLASS" );
	m_pStaticJob = GetControl<CEtUIStatic>( "ID_TEXT_CLASS" );
#ifdef _ADD_RENEWED_GUILDUI
	m_pStaticLevel = GetControl<CEtUIStatic>("ID_TEXT_LV");
#else
	m_pStaticLevel = GetControl<CEtUIStatic>("ID_TEXT_LEVEL");
#endif
	m_pStaticName = GetControl<CEtUIStatic>( "ID_TEXT_NAME" );	
}

void CDnGuildRequestItemDlg::SetInfo( INT64 nCharacterDBID, int nJobID, const WCHAR *wszText1, const WCHAR *wszText2, const WCHAR *wszText3 )
{
	m_nCharacterDBID = nCharacterDBID;
	m_pJobIcon->SetIconID( nJobID );
	m_pStaticJob->SetText( wszText1 );
	m_pStaticLevel->SetText( wszText2 );
	m_pStaticName->SetText( wszText3 );
}