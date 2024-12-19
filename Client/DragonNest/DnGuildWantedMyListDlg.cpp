#include "StdAfx.h"
#include "DnGuildWantedMyListDlg.h"
#include "DnGuildTask.h"
#include "DnInterface.h"
#include "DnGuildWantedListItemDlg.h"
#include "DnGuildWarTask.h"
#include "DnGuildWantedListPopupDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildWantedMyListDlg::CDnGuildWantedMyListDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pListBoxEx(NULL)
, m_pStaticCount(NULL)
, m_pButtonCancel(NULL)
, m_pButtonHomepage(NULL)
, m_pGuildWantedListPopupDlg(NULL)
, m_nRequestCount(0)
, m_nMaxRequestCount(0)
{
}

CDnGuildWantedMyListDlg::~CDnGuildWantedMyListDlg(void)
{
	m_pListBoxEx->RemoveAllItems();
	SAFE_DELETE( m_pGuildWantedListPopupDlg );
}

void CDnGuildWantedMyListDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildApplyListDlg.ui" ).c_str(), bShow );
}

void CDnGuildWantedMyListDlg::InitialUpdate()
{
	m_pListBoxEx = GetControl<CEtUIListBoxEx>("ID_LISTBOXEX_LIST");
	m_pStaticCount = GetControl<CEtUIStatic>("ID_TEXT_COUNT");
	m_pButtonCancel = GetControl<CEtUIButton>("ID_BUTTON_CANCEL");
	m_pButtonHomepage = GetControl<CEtUIButton>("ID_BT_HOMEPAGE");
	m_pButtonHomepage->Enable( false );
#ifdef PRE_MOD_GUILD_EASYSYSTEM_NO_HOMEPAGE
	m_pButtonHomepage->Show( false );
#endif

	m_pGuildWantedListPopupDlg = new CDnGuildWantedListPopupDlg( UI_TYPE_CHILD, this );
	m_pGuildWantedListPopupDlg->Initialize( false );
}

void CDnGuildWantedMyListDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		GetGuildTask().RequestGetGuildRecruitMyList();
	}
	else
	{
		InitControl();
		ShowChildDialog( m_pGuildWantedListPopupDlg, false );
	}

	CEtUIDialog::Show( bShow );
}

void CDnGuildWantedMyListDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BUTTON_CANCEL") )
		{
			int nSelectIndex = m_pListBoxEx->GetSelectedIndex();
			if( nSelectIndex != -1 )
			{
				CDnGuildWantedListItemDlg *pItem = m_pListBoxEx->GetItem<CDnGuildWantedListItemDlg>(nSelectIndex);
				if( pItem )
				{
					WCHAR wszMsg[256] = {0,};
					swprintf_s( wszMsg, _countof(wszMsg), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3998 ), pItem->GetGuildName() );
					GetInterface().MessageBox( wszMsg, MB_YESNO, REQUEST_CANCEL_DIALOG, this, false );
				}
			}
			return;
		}
		else if( IsCmdControl("ID_BT_HOMEPAGE") )
		{
			int nSelectIndex = m_pListBoxEx->GetSelectedIndex();
			if( nSelectIndex != -1 )
			{
				CDnGuildWantedListItemDlg *pItem = m_pListBoxEx->GetItem<CDnGuildWantedListItemDlg>(nSelectIndex);
				if( pItem )
				{
#ifdef PRE_ADD_GUILD_EASYSYSTEM
#ifdef PRE_MOD_GUILD_EASYSYSTEM_NO_HOMEPAGE
#else
					std::wstring wszHomepage = pItem->GetHomepage();
					std::string url = "";
					url.assign(wszHomepage.begin(), wszHomepage.end());
					GetInterface().OpenBrowser(url, (float)CEtDevice::GetInstance().Width(), (float)CEtDevice::GetInstance().Height(), CDnInterface::eBPT_CENTER, eGBT_SIMPLE);
#endif
#endif
				}
			}
		}
	}
	else if( nCommand == EVENT_LISTBOX_SELECTION )
	{
		m_pButtonCancel->Enable( true );

		bool bHomepage = false;
		int nSelectIndex = m_pListBoxEx->GetSelectedIndex();
		if( nSelectIndex != -1 )
		{
			CDnGuildWantedListItemDlg *pItem = m_pListBoxEx->GetItem<CDnGuildWantedListItemDlg>(nSelectIndex);
			if( pItem )
			{
#ifdef PRE_ADD_GUILD_EASYSYSTEM
#ifdef PRE_MOD_GUILD_EASYSYSTEM_NO_HOMEPAGE
#else
				std::wstring wszHomepage = pItem->GetHomepage();
				if( !wszHomepage.empty() ) bHomepage = true;
#endif
#endif
			}
		}
		m_pButtonHomepage->Enable( bHomepage );
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

bool CDnGuildWantedMyListDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	m_hWnd = hWnd;

	if( !m_bShow )
	{
		return false;
	}

	if( m_pGuildWantedListPopupDlg && m_pGuildWantedListPopupDlg->IsShow() )
	{
		if( uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_MOUSEMOVE )
			ShowChildDialog( m_pGuildWantedListPopupDlg, false );
	}

	bool bRet = CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
	if( bRet && !drag::IsValid() )
	{
		POINT MousePoint;
		MousePoint.x = short( LOWORD( lParam ) );
		MousePoint.y = short( HIWORD( lParam ) );

		float fMouseX, fMouseY;
		fMouseX = MousePoint.x / GetScreenWidth();
		fMouseY = MousePoint.y / GetScreenHeight();

		switch( uMsg )
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			{
				bool bIsInsideItem = false;
				for( int i = m_pListBoxEx->GetScrollBar()->GetTrackPos(); i < m_pListBoxEx->GetSize(); ++i ) {
					CDnGuildWantedListItemDlg *pDlg = m_pListBoxEx->GetItem<CDnGuildWantedListItemDlg>(i);
					if( !pDlg ) continue;
					SUICoord uiCoord;
					pDlg->GetDlgCoord( uiCoord );
					if( uiCoord.IsInside( fMouseX, fMouseY ) ) {
						bIsInsideItem = true;
						m_pGuildWantedListPopupDlg->SetPosition( fMouseX, fMouseY );
						static float fXRGap(4.0f/DEFAULT_UI_SCREEN_WIDTH);
						SUICoord sDlgCoord;
						m_pGuildWantedListPopupDlg->GetDlgCoord( sDlgCoord );
						if( (sDlgCoord.Right()+fXRGap) > GetScreenWidthRatio() )
							sDlgCoord.fX -= (sDlgCoord.Right()+fXRGap - GetScreenWidthRatio());
						m_pGuildWantedListPopupDlg->SetDlgCoord( sDlgCoord );
#ifdef PRE_ADD_GUILD_EASYSYSTEM
						m_pGuildWantedListPopupDlg->SetInfo( pDlg->GetGuildName(), pDlg->GetLevel(), pDlg->GetCount(), pDlg->GetCountMax(), pDlg->GetNotice(), pDlg->GetHomepage(), pDlg->GetGuildMasterName() );
#else
						m_pGuildWantedListPopupDlg->SetInfo( pDlg->GetGuildName(), pDlg->GetLevel(), pDlg->GetCount(), pDlg->GetCountMax(), pDlg->GetNotice() );
#endif
						break;
					}
				}
				ShowChildDialog( m_pGuildWantedListPopupDlg, bIsInsideItem );
			}
			break;
		}
	}
	return bRet;
}

void CDnGuildWantedMyListDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED ) 
	{
		if( IsCmdControl("ID_YES") )
		{
			switch( nID )
			{
			case REQUEST_CANCEL_DIALOG:
				{
					int nSelectIndex = m_pListBoxEx->GetSelectedIndex();
					if( nSelectIndex != -1 )
					{
						CDnGuildWantedListItemDlg *pItem = m_pListBoxEx->GetItem<CDnGuildWantedListItemDlg>(nSelectIndex);
						if( pItem )
							GetGuildTask().RequestJoinGuild( pItem->GetGuildUID(), true );
					}
				}
				break;
			}
		}
	}
}


void CDnGuildWantedMyListDlg::InitControl()
{
	m_pListBoxEx->RemoveAllItems();
	m_pButtonCancel->Enable( false );
}

void CDnGuildWantedMyListDlg::OnRecvGetGuildRecruitRequestCount( GuildRecruitSystem::SCGuildRecruitRequestCount *pPacket )
{
	m_nRequestCount = pPacket->cRequestCount;
	m_nMaxRequestCount = pPacket->cMaxRequestCount;

	WCHAR wszMsg[256] = {0,};
	swprintf_s( wszMsg, _countof(wszMsg), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3345 ), pPacket->cRequestCount, pPacket->cMaxRequestCount );
	m_pStaticCount->SetText( wszMsg );
}

void CDnGuildWantedMyListDlg::OnRecvGetGuildRecruitMyList( GuildRecruitSystem::SCMyGuildRecruitList *pPacket )
{
	m_pListBoxEx->RemoveAllItems();
	m_vecMyRecruitGuildUID.clear();

	for( int i = 0; i < pPacket->cCount; ++i ) {
		TGuildRecruitInfo *pRecruit = &pPacket->MyGuildRecruitList[i];
		CDnGuildWantedListItemDlg *pItemDlg = m_pListBoxEx->AddItem<CDnGuildWantedListItemDlg>();
		m_vecMyRecruitGuildUID.push_back( pRecruit->GuildUID );

		EtTextureHandle hTexture;
		TGuildView GuildView;
		GuildView.GuildUID.Set(1, 1);
		GuildView.wGuildMark = pRecruit->wGuildMark;
		GuildView.wGuildMarkBG = pRecruit->wGuildMarkBG;
		GuildView.wGuildMarkBorder = pRecruit->wGuildMarkBorder;
		if( GetGuildTask().IsShowGuildMark( GuildView ) )
			hTexture = GetGuildTask().GetGuildMarkTexture( GuildView );

#ifdef PRE_ADD_GUILD_EASYSYSTEM
		pItemDlg->SetInfo( pRecruit->GuildUID, hTexture, pRecruit->wszGuildName, pRecruit->wszGuildRecruitNotice, pRecruit->wGuildLevel, pRecruit->wGuildMemberPresentSize, pRecruit->wGuildMemberMaxSize, true, pRecruit->cPurposeCode, pRecruit->wszGuildHomePage, pRecruit->wszGuildMasterName );
#else
		pItemDlg->SetInfo( pRecruit->GuildUID, hTexture, pRecruit->wszGuildName, pRecruit->wszGuildRecruitNotice, pRecruit->wGuildLevel, pRecruit->wGuildMemberPresentSize, pRecruit->wGuildMemberMaxSize, true );
#endif
	}
}