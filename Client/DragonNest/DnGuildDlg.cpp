#include "StdAfx.h"
#include "DnGuildDlg.h"
#include "DnGuildMemberListItem2Dlg.h"
#include "DnGuildMemberListItem3Dlg.h"
#include "DnGuildAuthDlg.h"
#include "DnGuildNoticeDlg.h"
#include "GameOption.h"
#include "DnGuildMemberAddDlg.h"
#include "DnGuildYesNoDlg.h"
#include "DnInterfaceString.h"
#include "DnGuildMemberPopupDlg.h"
#include "SyncTimer.h"
#include "DnWorld.h"
#include "DnInterface.h"
#include "DnGuildMemberInfoDlg.h"
#include "DnGuildRecruitTabDlg.h"
#include "DnGuildInfoDlg.h"
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
#include "DnSystemDlg.h"
#include "DnMainMenuDlg.h"
#endif

#ifdef _ADD_RENEWED_GUILDUI
#include "DnGuildTabDlg.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildDlg::CDnGuildDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pStaticName(NULL)
, m_pTextureGuildMark(NULL)
, m_pStaticNotice(NULL)
, m_pButtonAuth(NULL)
, m_pButtonNotice(NULL)
, m_pButtonInvite(NULL)
, m_pButtonQuit(NULL)
, m_pButtonInfo(NULL)
, m_pStaticGuildMemberCount(NULL)
, m_pButtonOption(NULL)
, m_pListBoxEx_1(NULL)
, m_pBase1_1(NULL)
, m_pBase2_1(NULL)
, m_pBase3_1(NULL)
, m_pButton1_1(NULL)
, m_pButton2_1(NULL)
, m_pButton3_1(NULL)
, m_pListBoxEx_2(NULL)
, m_pBase1_2(NULL)
, m_pBase2_2(NULL)
, m_pButton1_2(NULL)
, m_pButton2_2(NULL)
, m_pButtonRecruit(NULL)
#ifdef PRE_ADD_GUILD_EASYSYSTEM
#ifdef PRE_MOD_GUILD_EASYSYSTEM_NO_HOMEPAGE
#else
, m_pButtonHomepage(NULL)
#endif
#endif
, m_eCurListBoxExType(SHOW_FALSE)
, m_eCurListBoxInfoType(Level_Job)
, m_pGuildAuthDlg(NULL)
, m_pGuildNoticeDlg(NULL)
, m_pGuildMemberAddDlg(NULL)
, m_pGuildYesNoDlg(NULL)
, m_pGuildInfoDlg(NULL)
, m_pGuildMemberPopupDlg(NULL)
, m_pGuildMemberInfoDlg(NULL)
, m_pGuildRecruitTabDlg(NULL)
, m_pCheckBoxHide(NULL)
#ifdef _ADD_RENEWED_GUILDUI
, m_pGuildTabDlg(NULL)
#endif
{
}

CDnGuildDlg::~CDnGuildDlg(void)
{
	SAFE_DELETE(m_pGuildAuthDlg);
	SAFE_DELETE(m_pGuildNoticeDlg);
	SAFE_DELETE(m_pGuildMemberAddDlg);
	SAFE_DELETE(m_pGuildYesNoDlg);
	SAFE_DELETE(m_pGuildInfoDlg);
	SAFE_DELETE(m_pGuildMemberPopupDlg);
	SAFE_DELETE(m_pGuildMemberInfoDlg);
	SAFE_DELETE(m_pGuildRecruitTabDlg);

#ifdef _ADD_RENEWED_GUILDUI
	SAFE_DELETE(m_pGuildTabDlg);
#endif

	m_pListBoxEx_1->RemoveAllItems();
	m_pListBoxEx_2->RemoveAllItems();
}

void CDnGuildDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildDlg.ui" ).c_str(), bShow );
}

void CDnGuildDlg::InitialUpdate()
{
	m_pStaticName = GetControl<CEtUIStatic>("ID_STATIC17");
	m_pTextureGuildMark = GetControl<CEtUITextureControl>("ID_TEXTUREL_GUILDMARK");
	m_pStaticNotice = GetControl<CEtUIStatic>("ID_STATIC15");
	m_pButtonAuth = GetControl<CEtUIButton>("ID_BUTTON_AUTH");
	m_pButtonNotice = GetControl<CEtUIButton>("ID_BUTTON_NOTICE");
	m_pButtonInvite = GetControl<CEtUIButton>("ID_BUTTON_INVITE");

	m_pButtonQuit = GetControl<CEtUIButton>("ID_BUTTON_QUIT");
	m_pButtonInfo = GetControl<CEtUIButton>("ID_BUTTON_HISTORY");
	m_pStaticGuildMemberCount = GetControl<CEtUIStatic>("ID_TEXT_COUNT");
	m_pButtonOption = GetControl<CEtUIButton>("ID_BUTTON_OPTION");

	m_pListBoxEx_1 = GetControl<CEtUIListBoxEx>("ID_LISTBOXEX_LIST1");
	m_pBase1_1 = GetControl<CEtUIStatic>("ID_STATIC11");
	m_pBase2_1 = GetControl<CEtUIStatic>("ID_BASE1");
	m_pBase3_1 = GetControl<CEtUIStatic>("ID_BASE2");
	m_pButton1_1 = GetControl<CEtUIButton>("ID_BUTTON_LIST1");
	m_pButton2_1 = GetControl<CEtUIButton>("ID_BUTTON_LIST2");
	m_pButton3_1 = GetControl<CEtUIButton>("ID_BUTTON_LIST3");

	m_pListBoxEx_2 = GetControl<CEtUIListBoxEx>("ID_LISTBOXEX_LIST2");
	m_pBase1_2 = GetControl<CEtUIStatic>("ID_STATIC12");
	m_pBase2_2 = GetControl<CEtUIStatic>("ID_BASE3");
	m_pButton1_2 = GetControl<CEtUIButton>("ID_BUTTON_LIST4");
	m_pButton2_2 = GetControl<CEtUIButton>("ID_BUTTON_LIST5");

	m_pCheckBoxHide = GetControl<CEtUICheckBox>("ID_CHECKBOX_HIDE");

	if( m_pCheckBoxHide )
		m_pCheckBoxHide->SetChecked( CGameOption::GetInstance().m_bCommGuildHideOffline );

	m_pButtonRecruit = GetControl<CEtUIButton>("ID_BUTTON_WANTED");
#ifdef PRE_ADD_GUILD_EASYSYSTEM
#ifdef PRE_MOD_GUILD_EASYSYSTEM_NO_HOMEPAGE
#else
	m_pButtonHomepage = GetControl<CEtUIButton>("ID_BUTTON0");
#endif
#endif

	m_pGuildAuthDlg = new CDnGuildAuthDlg( UI_TYPE_MODAL, NULL, GUILD_AUTH_DIALOG, NULL );
	m_pGuildAuthDlg->Initialize( false );

	m_pGuildNoticeDlg = new CDnGuildNoticeDlg( UI_TYPE_MODAL, NULL, GUILD_NOTICE_DIALOG, NULL );
	m_pGuildNoticeDlg->Initialize( false );

	m_pGuildMemberAddDlg = new CDnGuildMemberAddDlg( UI_TYPE_MODAL, NULL, GUILD_MEMBERADD_DIALOG, NULL );
	m_pGuildMemberAddDlg->Initialize( false );

	m_pGuildYesNoDlg = new CDnGuildYesNoDlg( UI_TYPE_MODAL, NULL, GUILD_YESNO_DIALOG, NULL );	// 여기선 모달로 해야되서, 별도로 만든다.
	m_pGuildYesNoDlg->Initialize( false );
	m_pGuildYesNoDlg->SetGuildYesNoType( CDnGuildYesNoDlg::GuildYesNo_Leave );

	m_pGuildInfoDlg = new CDnGuildInfoDlg( UI_TYPE_MODAL, NULL, GUILD_INFO_DIALOG, NULL );
	m_pGuildInfoDlg->Initialize( false );

	m_pGuildMemberPopupDlg = new CDnGuildMemberPopupDlg( UI_TYPE_CHILD, this, GUILD_MEMBERPOPUP_DIALOG, NULL );
	m_pGuildMemberPopupDlg->Initialize( false );

	m_pGuildMemberInfoDlg = new CDnGuildMemberInfoDlg( UI_TYPE_CHILD, this, GUILD_MEMBERINFO_DIALOG, NULL );
	m_pGuildMemberInfoDlg->Initialize( false );
	m_pGuildMemberInfoDlg->SetGuildDlg( this );
#ifdef _ADD_RENEWED_GUILDUI
	m_pGuildTabDlg = new CDnGuildTabDlg(UI_TYPE_MODAL, NULL, GUILD_RECRUIT_DIALOG, NULL);
	m_pGuildTabDlg->Initialize( false );
#else
	m_pGuildRecruitTabDlg = new CDnGuildRecruitTabDlg( UI_TYPE_MODAL, NULL, GUILD_RECRUIT_DIALOG, NULL );
	m_pGuildRecruitTabDlg->Initialize( false );
#endif

	for( int i = 0; i < CDnGuildTask::GuildMemberSort_Amount; ++i )
		m_bCurrentReverse[i] = false;

}

void CDnGuildDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		GetGuildTask().RequestGetGuildInfo( true );
		RefreshGuildDlg();
	}
	else
	{
		// 기본적으로 모든 컨트롤들은 다 비활성으로 해두고, 정보를 받으면 해당 정보에 따라 적절히 바꾼다.
		InitControl();
		if( m_pGuildMemberPopupDlg && m_pGuildMemberPopupDlg->IsShow() )
			ShowChildDialog( m_pGuildMemberPopupDlg, false );
	}

	CEtUIDialog::Show( bShow );
}

void CDnGuildDlg::InitControl()
{
	m_pStaticName->SetText( L"" );
	m_pTextureGuildMark->Show( false );
	m_pStaticNotice->SetText( L"" );

	m_pButtonAuth->Show( false );
	m_pButtonNotice->Show( false );

	m_pButtonInvite->Enable( false );
	m_pButtonQuit->Enable( false );

	m_pButtonInfo->Enable( false );

	m_pButtonRecruit->Enable( false );

#ifdef PRE_ADD_GUILD_EASYSYSTEM
#ifdef PRE_MOD_GUILD_EASYSYSTEM_NO_HOMEPAGE
#else
	m_pButtonHomepage->Enable( false );
	m_pButtonHomepage->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3873 ) );
#endif
#endif

	m_pStaticGuildMemberCount->SetText(L"");

	m_pCheckBoxHide->Enable( false );

	InitListBoxExControl();
}

void CDnGuildDlg::InitListBoxExControl( eListBoxExType eType )
{
	if( m_eCurListBoxExType == THREE_COLUMN || eType == SHOW_FALSE ) {
		m_pListBoxEx_2->RemoveAllItems();
		m_pListBoxEx_2->Show( false );
		m_pBase1_2->Show( false );
		m_pBase2_2->Show( false );
		m_pButton1_2->Show( false );
		m_pButton2_2->Show( false );
	}
	if( m_eCurListBoxExType == TWO_COLUMN || eType == SHOW_FALSE ) {
		m_pListBoxEx_1->RemoveAllItems();
		m_pListBoxEx_1->Show( false );
		m_pBase1_1->Show( false );
		m_pBase2_1->Show( false );
		m_pBase3_1->Show( false );
		m_pButton1_1->Show( false );
		m_pButton2_1->Show( false );
		m_pButton3_1->Show( false );
	}

	if( eType == THREE_COLUMN ) {
		m_pListBoxEx_1->RemoveAllItems();
		m_pListBoxEx_1->Show( true );
		m_pBase1_1->Show( true );
		m_pBase2_1->Show( true );
		m_pBase3_1->Show( true );
		m_pButton1_1->Show( true );
		m_pButton2_1->Show( true );
		m_pButton3_1->Show( true );
	}
	else if( eType == TWO_COLUMN ) {
		m_pListBoxEx_2->RemoveAllItems();
		m_pListBoxEx_2->Show( true );
		m_pBase1_2->Show( true );
		m_pBase2_2->Show( true );
		m_pButton1_2->Show( true );
		m_pButton2_2->Show( true );
	}

	if( eType == SHOW_FALSE )
		m_pButtonOption->Show( false );
	else
		m_pButtonOption->Show( true );

	m_eCurListBoxExType = eType;

	ShowChildDialog( m_pGuildMemberInfoDlg, false );
}

void CDnGuildDlg::RefreshGuildDlg()
{
	InitControl();
	if( m_pGuildMemberPopupDlg && m_pGuildMemberPopupDlg->IsShow() )
		ShowChildDialog( m_pGuildMemberPopupDlg, false );

	if( !CDnGuildTask::IsActive() ) return;
	TGuild *pGuild = GetGuildTask().GetGuildInfo();
	if( !pGuild->IsSet() )
	{
#ifdef _ADD_RENEWED_GUILDUI
		if (m_pGuildTabDlg && m_pGuildTabDlg->IsShow())
			m_pGuildTabDlg->Show(false);
		return;
#else
		// 길드창 열어둔채 추방당하면 다른창도 다 닫아야하는게 맞긴 한데, 다른창은 에러메세지같은게 정상적이라 패스합니다.
		if( m_pGuildRecruitTabDlg && m_pGuildRecruitTabDlg->IsShow() )
			m_pGuildRecruitTabDlg->Show( false );
		return;
#endif

	}

	WCHAR wszName[1024] = {0,};
	if( wcslen(pGuild->GuildView.wszGuildName) < (int)_countof(wszName) ) {
#ifdef PRE_MOD_CONSTANT_TEXT_CONVERT_TO_TABLE_TEXT
		swprintf_s( wszName, _countof(wszName), L"%s (%s %d)", pGuild->GuildView.wszGuildName, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7888 ), pGuild->wGuildLevel );
#else
		swprintf_s( wszName, _countof(wszName), L"%s (Lv %d)", pGuild->GuildView.wszGuildName, pGuild->wGuildLevel );
#endif 
	}
	m_pStaticName->SetText( wszName );
	const TGuildView &GuildView = pGuild->GuildView;
	if( GetGuildTask().IsShowGuildMark( GuildView ) )
	{
		EtTextureHandle hGuildMark = GetGuildTask().GetGuildMarkTexture( GuildView );
		m_pTextureGuildMark->SetTexture( hGuildMark );
		m_pTextureGuildMark->Show( true );
	}
	m_pStaticNotice->SetText( pGuild->wszGuildNotice );

	if( GetGuildTask().IsMaster() )
		m_pButtonAuth->Show( true );
	else if( GetGuildTask().GetMyRole() == GUILDROLE_TYPE_SUBMASTER )
		m_pButtonAuth->Show( true );

	if( GetGuildTask().GetAuth( static_cast<eGuildRoleType>(GetGuildTask().GetMyRole()), GUILDAUTH_TYPE_NOTICE ) )
		m_pButtonNotice->Show( true );

	if( GetGuildTask().GetAuth( static_cast<eGuildRoleType>(GetGuildTask().GetMyRole()), GUILDAUTH_TYPE_INVITE ) )
		m_pButtonInvite->Enable( true );

	m_pButtonQuit->Enable( !GetGuildTask().IsMaster() );

#ifdef PRE_ADD_BEGINNERGUILD
	bool bEnableInfoButton = (GetGuildTask().IsBeginnerGuild() == false);

	m_pButtonInfo->Enable(bEnableInfoButton);
	m_pButtonRecruit->Enable(bEnableInfoButton); // 길드 정보와 동일하게 셋팅해도 상관없을 것으로 보여 변수공용. 이후 필요에 따라 분리할 것. by kalliste
	
#else // PRE_ADD_BEGINNERGUILD
	m_pButtonInfo->Enable( true );
	m_pButtonRecruit->Enable( true );		
#endif //  PRE_ADD_BEGINNERGUILD

#ifdef PRE_ADD_GUILD_EASYSYSTEM
#ifdef PRE_MOD_GUILD_EASYSYSTEM_NO_HOMEPAGE
#else
	if( wcslen( pGuild->wszGuildHomePage ) > 0 )
	{
		m_pButtonHomepage->Enable( true );
		m_pButtonHomepage->SetTooltipText( L"" );
	}

	#ifdef PRE_ADD_BEGINNERGUILD
	if (GetGuildTask().IsBeginnerGuild())
		m_pButtonHomepage->SetTooltipText(L"");
	#endif

#endif
#endif

	std::wstring wszCount;
	WCHAR wszTemp[32];
	wszCount = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3723 );
	swprintf_s( wszTemp, _countof(wszTemp), L" %d / %d (", GetGuildTask().GetGuildMemberCount(), pGuild->wGuildSize );
	wszCount += wszTemp;
	swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1572 ), GetGuildTask().GetGuildMemberLogonCount() );
	wszCount += wszTemp;
	swprintf_s( wszTemp, _countof(wszTemp), L" %s)", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3798 ) );
	wszCount += wszTemp;
	m_pStaticGuildMemberCount->SetText( wszCount );

#ifdef PRE_DEBUG_GUILDAUTH
	m_pButtonAuth->Show( true );
	m_pButtonNotice->Show( true );
	m_pButtonInvite->Enable( true );
	m_pButtonQuit->Enable( true );
#endif	// #ifdef PRE_DEBUG_AUTH

	m_pCheckBoxHide->Enable( true );

	RefreshGuildMemberList();
}

void CDnGuildDlg::RefreshGuildMemberList()
{
	// 먼저 출력할 정보가 있는지 판단.
	if( !CDnGuildTask::IsActive() ) return;
	TGuild *pGuild = GetGuildTask().GetGuildInfo();
	if( !pGuild->IsSet() ) return;

	int nCount = GetGuildTask().GetGuildMemberCount();
	if( nCount <= 0 ) return;

	// 현재 출력할 타입에 따라 적절한 리스트 고른 후
	eListBoxExType nColumnType = (eListBoxExType)0;
	switch( m_eCurListBoxInfoType )
	{
	case Level_Job:
	case Role_LastTime:
		nColumnType = THREE_COLUMN;
		break;
	case Location:
	case Introduce:
		nColumnType = TWO_COLUMN;
		break;
	default:
		_ASSERT(0&&"길드멤버리스트 타입이 이상한 값으로 설정되어있습니다.");
		return;
	}

	bool bHideOffline = CGameOption::GetInstance().m_bCommGuildHideOffline;

	// 목록 업데이트
	InitListBoxExControl( nColumnType );
	if( m_eCurListBoxInfoType == Level_Job ) {
		m_pButton1_1->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 11 ) );
		m_pButton2_1->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 64 ) );
		m_pButton3_1->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4011 ) );
		for( int i = 0; i < nCount; ++i ) {
			TGuildMember *pMember = GetGuildTask().GetGuildMemberFromArrayIndex(i);
			if( !pMember ) continue;
			if( bHideOffline && pMember->Location.cServerLocation == _LOCATION_NONE ) continue;

			CDnGuildMemberListItem3Dlg *pItemDlg = GetListBox(nColumnType)->AddItem<CDnGuildMemberListItem3Dlg>();

			// 레벨과 직업 구한 후
			WCHAR wszTemp[64];
			swprintf_s( wszTemp, _countof(wszTemp), L"%d", pMember->cLevel );
			pItemDlg->SetInfo( pMember->wszCharacterName, wszTemp, DN_INTERFACE::STRING::GetJobString(pMember->nJob), (pMember->Location.cServerLocation != _LOCATION_NONE) );
		}
	}
	else if( m_eCurListBoxInfoType == Location ) {
		m_pButton1_2->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 11 ) );
		m_pButton2_2->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3718 ) );
		for( int i = 0; i < nCount; ++i ) {
			TGuildMember *pMember = GetGuildTask().GetGuildMemberFromArrayIndex(i);
			if( !pMember ) continue;
			if( bHideOffline && pMember->Location.cServerLocation == _LOCATION_NONE ) continue;

			CDnGuildMemberListItem2Dlg *pItemDlg = GetListBox(nColumnType)->AddItem<CDnGuildMemberListItem2Dlg>();

			// 로케이션을 텍스트로.
			std::wstring wszStr;
			DN_INTERFACE::STRING::GetLocationText(wszStr, pMember->Location);
			pItemDlg->SetInfo( pMember->wszCharacterName, wszStr.c_str(), (pMember->Location.cServerLocation != _LOCATION_NONE) );
		}
	}
	else if( m_eCurListBoxInfoType == Role_LastTime ) {
		m_pButton1_1->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 11 ) );
		m_pButton2_1->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3719 ) );
		m_pButton3_1->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3720 ) );
		for( int i = 0; i < nCount; ++i ) {
			TGuildMember *pMember = GetGuildTask().GetGuildMemberFromArrayIndex(i);
			if( !pMember ) continue;
			if( bHideOffline && pMember->Location.cServerLocation == _LOCATION_NONE ) continue;

			CDnGuildMemberListItem3Dlg *pItemDlg = GetListBox(nColumnType)->AddItem<CDnGuildMemberListItem3Dlg>();

			std::wstring wszLogout;
			if( pMember->Location.cServerLocation != _LOCATION_NONE )
			{
				// 접속중이면 접속 중 표시.
				wszLogout = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3798 );
			}
			else
			{
				// 역할 이름과 최근 접속종료시간 구한 후
				// 만약 0이면 한번도 로그아웃 한게 없으니 그냥 안찍고,
				// -1이면 잘못보내준거니 안찍고,
				// 그 외 값이면 현재부터 시간값 차이 구해서, 
				if( pMember->LastLogoutDate != 0 && pMember->LastLogoutDate != -1 ) {
					GetLogoutText( wszLogout, pMember->LastLogoutDate );
				}
			}

			WCHAR wszRole[32];
#ifdef PRE_ADD_BEGINNERGUILD
			if (CDnGuildTask::IsActive() && GetGuildTask().IsBeginnerGuild())
			{
				swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1385 ) ); // UISTRING : 초보자
			}
			else
			{
				switch( pMember->btGuildRole )
				{
				case GUILDROLE_TYPE_MASTER:		swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3733 ) );	break;
				case GUILDROLE_TYPE_SUBMASTER:	swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3734 ) );	break;
				case GUILDROLE_TYPE_SENIOR:		swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3735 ) );	break;
				case GUILDROLE_TYPE_REGULAR:	swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3736 ) );	break;
				case GUILDROLE_TYPE_JUNIOR:		swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3737 ) );	break;
				}
			}
#else
			switch(pMember->btGuildRole)
			{
			case GUILDROLE_TYPE_MASTER:		swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3733 ) );	break;
			case GUILDROLE_TYPE_SUBMASTER:	swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3734 ) );	break;
			case GUILDROLE_TYPE_SENIOR:		swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3735 ) );	break;
			case GUILDROLE_TYPE_REGULAR:	swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3736 ) );	break;
			case GUILDROLE_TYPE_JUNIOR:		swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3737 ) );	break;
			}
#endif

			pMember->LastLogoutDate;
			pItemDlg->SetInfo( pMember->wszCharacterName, wszRole, wszLogout.c_str(), (pMember->Location.cServerLocation != _LOCATION_NONE) );
		}
	}
	else if( m_eCurListBoxInfoType == Introduce ) {
		m_pButton1_2->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 11 ) );
		m_pButton2_2->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3721 ) );
		for( int i = 0; i < nCount; ++i ) {
			TGuildMember *pMember = GetGuildTask().GetGuildMemberFromArrayIndex(i);
			if( !pMember ) continue;
			if( bHideOffline && pMember->Location.cServerLocation == _LOCATION_NONE ) continue;

			CDnGuildMemberListItem2Dlg *pItemDlg = GetListBox(nColumnType)->AddItem<CDnGuildMemberListItem2Dlg>();
			pItemDlg->SetInfo( pMember->wszCharacterName, pMember->wszGuildMemberIntroduce, (pMember->Location.cServerLocation != _LOCATION_NONE) );
		}
	}
}

CEtUIListBoxEx *CDnGuildDlg::GetListBox( eListBoxExType Type )
{
	if( Type == THREE_COLUMN ) return m_pListBoxEx_1;
	else if( Type == TWO_COLUMN ) return m_pListBoxEx_2;
	return NULL;
}

bool CDnGuildDlg::CheckValidCommand()
{
	if( CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeVillage )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3797 ) );
		return false;
	}
	return true;
}

void CDnGuildDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BUTTON_AUTH") )
		{
			if( CheckValidCommand() )
				m_pGuildAuthDlg->Show( true );
		}
		else if( IsCmdControl("ID_BUTTON_NOTICE") )
		{
			if( CheckValidCommand() )
				m_pGuildNoticeDlg->Show( true );
		}
		else if( IsCmdControl("ID_BUTTON_INVITE") )
		{
			if( CheckValidCommand() )
				m_pGuildMemberAddDlg->Show( true );
		}
		else if( IsCmdControl("ID_BUTTON_QUIT") )
		{
			if( CheckValidCommand() )
				m_pGuildYesNoDlg->Show( true );
		}
		else if( IsCmdControl("ID_BUTTON_HISTORY") )
		{
			if( CheckValidCommand() )
				m_pGuildInfoDlg->Show( true );
		}
		else if( IsCmdControl("ID_BUTTON_OPTION") )
		{
			m_eCurListBoxInfoType = (eListBoxExInfoType)((int)m_eCurListBoxInfoType + 1);
			if( m_eCurListBoxInfoType >= ListBoxExInfoType_Amount )
				m_eCurListBoxInfoType = Level_Job;
			InitListBoxExControl(SHOW_FALSE);
			RefreshGuildMemberList();
		}
		else if( IsCmdControl("ID_BUTTON_LIST1") || IsCmdControl("ID_BUTTON_LIST4") )
		{
			// 이름을 눌렀을 경우
			GetGuildTask().SortGuildMember( CDnGuildTask::GuildMemberSort_Name, m_bCurrentReverse[CDnGuildTask::GuildMemberSort_Name] );
			m_bCurrentReverse[CDnGuildTask::GuildMemberSort_Name] ^= true;
			RefreshGuildMemberList();
		}
		else if( IsCmdControl("ID_BUTTON_LIST2") )
		{
			if( m_eCurListBoxInfoType == Level_Job ) {
				// 레벨 눌렀을 경우
				GetGuildTask().SortGuildMember( CDnGuildTask::GuildMemberSort_Level, m_bCurrentReverse[CDnGuildTask::GuildMemberSort_Level] );
				m_bCurrentReverse[CDnGuildTask::GuildMemberSort_Level] ^= true;
			}
			else if( m_eCurListBoxInfoType == Role_LastTime ) {
				// 직급 눌렀을 경우
				GetGuildTask().SortGuildMember( CDnGuildTask::GuildMemberSort_Role, m_bCurrentReverse[CDnGuildTask::GuildMemberSort_Role] );;
				m_bCurrentReverse[CDnGuildTask::GuildMemberSort_Role] ^= true;
			}
			RefreshGuildMemberList();
		}
		else if( IsCmdControl("ID_BUTTON_LIST3") )
		{
			if( m_eCurListBoxInfoType == Level_Job ) {
				// 직업 눌렀을 경우
				GetGuildTask().SortGuildMember( CDnGuildTask::GuildMemberSort_Job, m_bCurrentReverse[CDnGuildTask::GuildMemberSort_Job] );
				m_bCurrentReverse[CDnGuildTask::GuildMemberSort_Job] ^= true;
			}
			else if( m_eCurListBoxInfoType == Role_LastTime ) {
				// 접속시간 눌렀을 경우
				GetGuildTask().SortGuildMember( CDnGuildTask::GuildMemberSort_LastLogoutDate, m_bCurrentReverse[CDnGuildTask::GuildMemberSort_LastLogoutDate] );
				m_bCurrentReverse[CDnGuildTask::GuildMemberSort_LastLogoutDate] ^= true;
			}
			RefreshGuildMemberList();
		}
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
		else if( IsCmdControl("ID_BT_SMALLHELP") )
		{
			CDnMainMenuDlg* pMainMenuDlg = GetInterface().GetMainMenuDialog();
			CDnSystemDlg* pSystemDlg = pMainMenuDlg->GetSystemDlg();
			if(pSystemDlg)
				pSystemDlg->ShowChoiceHelpDlg(HELP_SHORTCUT_GUILD);
		}
#endif

		else if( IsCmdControl("ID_BUTTON_WANTED") )
		{
			if (CheckValidCommand())
#ifdef _ADD_RENEWED_GUILDUI
				m_pGuildTabDlg->Show(true);
#else
				m_pGuildRecruitTabDlg->Show( true );
#endif
		}
#ifdef PRE_ADD_GUILD_EASYSYSTEM
#ifdef PRE_MOD_GUILD_EASYSYSTEM_NO_HOMEPAGE
#else
		else if( IsCmdControl("ID_BUTTON0") )
		{
			TGuild *pGuild = GetGuildTask().GetGuildInfo();
			if( pGuild->IsSet() )
			{
				std::wstring wszHomepage = pGuild->wszGuildHomePage;
				std::string url = "";
				url.assign(wszHomepage.begin(), wszHomepage.end());
				GetInterface().OpenBrowser(url, (float)CEtDevice::GetInstance().Width(), (float)CEtDevice::GetInstance().Height(), CDnInterface::eBPT_CENTER, eGBT_SIMPLE);
			}
		}
#endif
#endif
	}
	else if( nCommand == EVENT_CHECKBOX_CHANGED )
	{
		if( IsCmdControl("ID_CHECKBOX_HIDE" ) )
		{
			CGameOption::GetInstance().m_bCommGuildHideOffline = m_pCheckBoxHide->IsChecked();
			RefreshGuildMemberList();
			return;
		}
	}
	else if( nCommand == EVENT_LISTBOX_SELECTION )
	{
		LPCWSTR pwszName = NULL;
		if( m_eCurListBoxExType == THREE_COLUMN )
		{
			int nSelectIndex = m_pListBoxEx_1->GetSelectedIndex();
			if( -1 != nSelectIndex )
			{
				CDnGuildMemberListItem3Dlg *pDlg = m_pListBoxEx_1->GetItem<CDnGuildMemberListItem3Dlg>( nSelectIndex );
				if( pDlg ) pwszName = pDlg->GetName();
			}
		}
		else if( m_eCurListBoxExType == TWO_COLUMN )
		{
			int nSelectIndex = m_pListBoxEx_2->GetSelectedIndex();
			if( -1 != nSelectIndex )
			{
				CDnGuildMemberListItem2Dlg *pDlg = m_pListBoxEx_2->GetItem<CDnGuildMemberListItem2Dlg>( nSelectIndex );
				if( pDlg ) pwszName = pDlg->GetName();
			}
		}

		if( pwszName != NULL )
		{
			TGuildMember *pMember = GetGuildTask().GetGuildMemberFromName( pwszName );
			if( pMember )
			{
				m_pGuildMemberInfoDlg->SetGuildMemberDBID( pMember->nCharacterDBID );
				ShowChildDialog( m_pGuildMemberInfoDlg, true );
			}
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}


bool CDnGuildDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	m_hWnd = hWnd;

	if( !m_bShow )
	{
		return false;
	}

	if( !drag::IsValid() )
	{
		POINT MousePoint;
		MousePoint.x = short( LOWORD( lParam ) );
		MousePoint.y = short( HIWORD( lParam ) );

		float fMouseX, fMouseY;
		fMouseX = MousePoint.x / GetScreenWidth();
		fMouseY = MousePoint.y / GetScreenHeight();

		switch( uMsg )
		{
		case WM_RBUTTONDOWN:
			{
				std::wstring wszName;
				bool bIsInsideItem = false;
				if( m_eCurListBoxExType == THREE_COLUMN ) {
					for( int i = m_pListBoxEx_1->GetScrollBar()->GetTrackPos(); i < m_pListBoxEx_1->GetSize(); ++i ) {
						CDnGuildMemberListItem3Dlg *pDlg = m_pListBoxEx_1->GetItem<CDnGuildMemberListItem3Dlg>(i);
						if( !pDlg ) continue;
						SUICoord uiCoord;
						pDlg->GetDlgCoord( uiCoord );
						if( uiCoord.IsInside( fMouseX, fMouseY ) ) {
							bIsInsideItem = true;
							wszName = pDlg->GetName();
							break;
						}
					}
				}
				else if( m_eCurListBoxExType == TWO_COLUMN ) {
					for( int i = m_pListBoxEx_2->GetScrollBar()->GetTrackPos(); i < m_pListBoxEx_2->GetSize(); ++i ) {
						CDnGuildMemberListItem2Dlg *pDlg = m_pListBoxEx_2->GetItem<CDnGuildMemberListItem2Dlg>(i);
						if( !pDlg ) continue;
						SUICoord uiCoord;
						pDlg->GetDlgCoord( uiCoord );
						if( uiCoord.IsInside( fMouseX, fMouseY ) ) {
							bIsInsideItem = true;
							wszName = pDlg->GetName();
							break;
						}
					}
				}

				if( bIsInsideItem ) {
					// 현재 보여지고 있더라도 방금 탈퇴된 멤버일 수 있다.
					TGuildMember *pMember = GetGuildTask().GetGuildMemberFromName( wszName.c_str() );
					if( pMember ) {
						ShowChildDialog( m_pGuildMemberPopupDlg, false );
						m_pGuildMemberPopupDlg->SetPosition( fMouseX, fMouseY );
						static float fXRGap(4.0f/DEFAULT_UI_SCREEN_WIDTH), fYBGap(4.0f/DEFAULT_UI_SCREEN_HEIGHT);
						SUICoord sDlgCoord;
						m_pGuildMemberPopupDlg->GetDlgCoord( sDlgCoord );
						if( (sDlgCoord.Right()+fXRGap) > GetScreenWidthRatio() )
							sDlgCoord.fX -= (sDlgCoord.Right()+fXRGap - GetScreenWidthRatio());
						m_pGuildMemberPopupDlg->SetDlgCoord( sDlgCoord );
						m_pGuildMemberPopupDlg->SetGuildMemberDBID( pMember->nCharacterDBID );
						ShowChildDialog( m_pGuildMemberPopupDlg, true );
					}
				}
			}
			break;
		case WM_LBUTTONDOWN:
			{
				if( IsMouseInDlg() ) {
					if( m_pGuildMemberPopupDlg->IsShow() ) {
						SUICoord uiCoord;
						m_pGuildMemberPopupDlg->GetDlgCoord( uiCoord );
						if( !uiCoord.IsInside( fMouseX, fMouseY ) ) {
							ShowChildDialog( m_pGuildMemberPopupDlg, false );
							return true;
						}
					}
				}
			}
			break;
		}
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnGuildDlg::GetLogoutText(std::wstring &wszStr, __time64_t LogoutTime)
{
	// CurTime
	__time64_t tCurTime = CSyncTimer::GetInstance().GetCurTime();

	// Diff
	INT64 biDiff = tCurTime - LogoutTime;

	// Hour단위로 전환
	INT64 biHourDiff = biDiff / 3600;

	// 다시 Day, Hour로 변환
	INT64 biDayDiff = biHourDiff / 24;
	biHourDiff = biHourDiff % 24;

	WCHAR wszTemp[64] = {0,};
	if( biDayDiff > 28 ) {
		swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3796 ), 28 );
	}
	else if( biDayDiff > 0 ) {
		swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3795 ), biDayDiff );
	}
	else if( biHourDiff ) {
		swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3794 ), biHourDiff );
	}
	else
	{
		// 1시간으로 적어야하나?
		swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3794 ), 1 );
	}
	wszStr = wszTemp;
}

void CDnGuildDlg::OnRecvGetGuildHistoryList( SCGetGuildHistoryList *pPacket )
{
	if( !m_pGuildInfoDlg ) return;
	m_pGuildInfoDlg->OnRecvGetGuildHistoryList( pPacket );
}
#ifdef _ADD_RENEWED_GUILDUI
void CDnGuildDlg::OnRecvGuildRecruitRegisterInfo(GuildRecruitSystem::SCGuildRecruitRegisterInfo *pPacket)
{
	if (!m_pGuildTabDlg) return;
	m_pGuildTabDlg->OnRecvGuildRecruitRegisterInfo(pPacket);
}

void CDnGuildDlg::OnRecvGuildRecruitCharacter(GuildRecruitSystem::SCGuildRecruitCharacterList *pPacket)
{
	if (!m_pGuildTabDlg) return;
	m_pGuildTabDlg->OnRecvGuildRecruitCharacter(pPacket);
}

void CDnGuildDlg::OnRecvGuildRecruitAcceptResult(GuildRecruitSystem::SCGuildRecruitAccept *pPacket)
{
	if (!m_pGuildTabDlg) return;
	m_pGuildTabDlg->OnRecvGuildRecruitAcceptResult(pPacket);
}
#else
void CDnGuildDlg::OnRecvGuildRecruitRegisterInfo( GuildRecruitSystem::SCGuildRecruitRegisterInfo *pPacket )
{
	if( !m_pGuildRecruitTabDlg ) return;
	m_pGuildRecruitTabDlg->OnRecvGuildRecruitRegisterInfo( pPacket );
}

void CDnGuildDlg::OnRecvGuildRecruitCharacter( GuildRecruitSystem::SCGuildRecruitCharacterList *pPacket )
{
	if( !m_pGuildRecruitTabDlg ) return;
	m_pGuildRecruitTabDlg->OnRecvGuildRecruitCharacter( pPacket );
}

void CDnGuildDlg::OnRecvGuildRecruitAcceptResult( GuildRecruitSystem::SCGuildRecruitAccept *pPacket )
{
	if( !m_pGuildRecruitTabDlg ) return;
	m_pGuildRecruitTabDlg->OnRecvGuildRecruitAcceptResult( pPacket );
}
#endif