#include "StdAfx.h"
#include "DnGuildWantedListItemDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildWantedListItemDlg::CDnGuildWantedListItemDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pTextureGuildMark(NULL)
, m_pStaticName(NULL)
, m_pStaticCondition(NULL)
, m_pStaticLevel(NULL)
, m_pStaticCount(NULL)
#ifdef PRE_ADD_GUILD_EASYSYSTEM
, m_pStaticCategory(NULL)
, m_pStaticIntroduce(NULL)
#endif
, m_nCount( 0 )
, m_nCountMax( 0 )
, m_nLevel( 0 )
{
}

CDnGuildWantedListItemDlg::~CDnGuildWantedListItemDlg(void)
{
}

void CDnGuildWantedListItemDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildWantedADListDlg.ui" ).c_str(), bShow );
}

void CDnGuildWantedListItemDlg::InitialUpdate()
{
	m_pTextureGuildMark = GetControl<CEtUITextureControl>( "ID_TEXTUREL_GUILDMARK" );
	m_pStaticName = GetControl<CEtUIStatic>( "ID_TEXT_GUILDNAME" );
	m_pStaticCondition = GetControl<CEtUIStatic>( "ID_TEXT_CONDITION" );
	m_pStaticLevel = GetControl<CEtUIStatic>( "ID_TEXT_GUILDLEVEL" );
	m_pStaticCount = GetControl<CEtUIStatic>( "ID_TEXT_GUILDMEMBER" );
#ifdef PRE_ADD_GUILD_EASYSYSTEM
	m_pStaticCategory = GetControl<CEtUIStatic>( "ID_TEXT_CATEGORY" );
	m_pStaticIntroduce = GetControl<CEtUIStatic>( "ID_STATIC2" );
#endif
}

#ifdef PRE_ADD_GUILD_EASYSYSTEM
void CDnGuildWantedListItemDlg::SetInfo( TGuildUID GuildUID, EtTextureHandle hTexture, const WCHAR *wszText1, const WCHAR *wszText2, int nLevel, int nCount, int nCountMax, bool bWaitRequest, BYTE cPurposeCode, const WCHAR *wszHomepage, const WCHAR *wszGuildMasterName )
#else
void CDnGuildWantedListItemDlg::SetInfo( TGuildUID GuildUID, EtTextureHandle hTexture, const WCHAR *wszText1, const WCHAR *wszText2, int nLevel, int nCount, int nCountMax, bool bWaitRequest )
#endif
{
	m_GuildUID = GuildUID;
	m_nLevel = nLevel;
	m_nCount = nCount;
	m_nCountMax = nCountMax;
	m_wszNotice = wszText2;
#ifdef PRE_ADD_GUILD_EASYSYSTEM
	m_wszHomepage = wszHomepage;
	m_wszGuildMasterName = wszGuildMasterName;
#endif

	m_pTextureGuildMark->SetTexture( hTexture );
	m_pStaticName->SetText( wszText1 );

	WCHAR wszText[256] = {0,};
	swprintf_s( wszText, _countof(wszText), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3347 ), nLevel );
	m_pStaticLevel->SetText( wszText );
	swprintf_s( wszText, _countof(wszText), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3348 ), nCount, nCountMax );
	m_pStaticCount->SetText( wszText );
#ifdef PRE_ADD_GUILD_EASYSYSTEM
	swprintf_s( wszText, _countof(wszText), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3856 + cPurposeCode ) );
	m_pStaticCategory->SetText( wszText );
#endif
	if( bWaitRequest ) SetOnWaitRequest();
#ifdef PRE_ADD_GUILD_EASYSYSTEM
	else
	{
		std::wstring wszLine = m_wszNotice;
		wszLine.substr( 0, (min( wszLine.size(), 30 )) );
		std::wstring::size_type Pos = wszLine.find_first_of( L"\n" );
		if( Pos != std::wstring::npos )
			wszLine = wszLine.substr( 0, Pos );
		m_pStaticIntroduce->SetText( wszLine.c_str() );
	}
#endif
}

void CDnGuildWantedListItemDlg::SetOnWaitRequest()
{
	m_pStaticCondition->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3990 ) );
#ifdef PRE_ADD_GUILD_EASYSYSTEM
	m_pStaticIntroduce->ClearText();
#endif
}