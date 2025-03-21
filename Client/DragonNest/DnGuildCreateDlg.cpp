#include "StdAfx.h"
#include "DnGuildCreateDlg.h"
#include "DnInterface.h"
#include "DnGuildTask.h"
#include "DnTableDB.h"
#include "DnItemTask.h"
#include "DnPlayerActor.h"
#include "DnInterfaceString.h"
#include "DNCountryUnicodeSet.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildCreateDlg::CDnGuildCreateDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pEditBoxGuildName(NULL)
, m_pButtonOK(NULL)
, m_pStaticGold(NULL)
, m_pStaticSilver(NULL)
, m_pStaticBronze(NULL)
, m_bEnoughTax(false)
, m_dwColorGold( 0 )
, m_dwColorSilver( 0 )
, m_dwColorBronze( 0 )
{
}

CDnGuildCreateDlg::~CDnGuildCreateDlg(void)
{
}

void CDnGuildCreateDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildCreateDlg.ui" ).c_str(), bShow );
}

void CDnGuildCreateDlg::InitialUpdate()
{
	m_pEditBoxGuildName = GetControl<CEtUIIMEEditBox>("ID_IMEEDITBOX_GUILDNAME");
	m_pButtonOK = GetControl<CEtUIButton>("ID_OK");
	m_pStaticGold = GetControl<CEtUIStatic>("ID_GOLD");
	m_pStaticSilver = GetControl<CEtUIStatic>("ID_SILVER");
	m_pStaticBronze = GetControl<CEtUIStatic>("ID_BRONZE");
	m_dwColorGold = m_pStaticGold->GetTextColor();
	m_dwColorSilver = m_pStaticSilver->GetTextColor();
	m_dwColorBronze = m_pStaticBronze->GetTextColor();
}

void CDnGuildCreateDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_OK") )
		{
			std::wstring szName = m_pEditBoxGuildName->GetText();
// 			if( DN_INTERFACE::UTIL::CheckSpecialCharacter( szName ) )

			if (!g_CountryUnicodeSet.Check(szName.c_str())) 
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3772 ), MB_OK, 0, this );
				return;
			}

			if( DN_INTERFACE::UTIL::CheckAccount( szName ) || DN_INTERFACE::UTIL::CheckChat( szName ) )
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3772 ), MB_OK, 0, this );
				return;
			}

			GetGuildTask().RequestCreateGuild( m_pEditBoxGuildName->GetText() );
			m_pButtonOK->Enable( false );
			return;
		}

		if( IsCmdControl("ID_CANCEL") || IsCmdControl("ID_BUTTON_CLOSE") )
		{
			Show(false);
			return;
		}
	}
	else if( nCommand == EVENT_EDITBOX_CHANGE || nCommand == EVENT_EDITBOX_IME_CHANGE )
	{
		if( IsCmdControl("ID_IMEEDITBOX_GUILDNAME") )
		{
			bool bCreatable = true;
			{
				if( !m_bEnoughTax ) bCreatable = false;
				if( CDnActor::s_hLocalActor ) {
					CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
					if( pPlayer->IsJoinGuild() ) bCreatable = false;
				}
			}

			std::wstring szName = m_pEditBoxGuildName->GetText();
#ifdef PRE_FIX_GUILDCREATE_DISABLEBTN
			std::wstring composingString;
			m_pEditBoxGuildName->GetIMECompositionString(composingString);
			int composingLength = (int)composingString.length();
			if (composingLength <= 0)
			{
				if (szName.empty())
					bCreatable = false;
			}

			OutputDebug("composingLength:%d\n", composingLength);

			m_pButtonOK->Enable(composingLength + m_pEditBoxGuildName->GetTextLength() > 0 && bCreatable);
#else
			szName = boost::algorithm::trim_copy(szName);
			if (szName.empty())
				bCreatable = false;

			if( m_pEditBoxGuildName->GetTextLength() + CEtUIIME::GetCompStringLength() > 0 && bCreatable )
				m_pButtonOK->Enable( true );
			else
				m_pButtonOK->Enable( false );
#endif
		}
	}
	else if( nCommand == EVENT_EDITBOX_STRING )
	{
		focus::ReleaseControl();
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnGuildCreateDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_OK" ) )
		{
			RequestFocus( m_pEditBoxGuildName );
			m_pButtonOK->Enable( true );
		}
	}
}

void CDnGuildCreateDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		m_pButtonOK->Enable( false );
		UpdateTax();
		m_pEditBoxGuildName->ClearText();
		RequestFocus( m_pEditBoxGuildName );
	}
	else
	{
		m_pStaticGold->SetTextColor( m_dwColorGold, true );
		m_pStaticSilver->SetTextColor( m_dwColorSilver, true );
		m_pStaticBronze->SetTextColor( m_dwColorBronze, true );
		GetInterface().CloseBlind();
		//GetInterface().CloseNpcTalkReturnDlg();
	}

	CEtUIDialog::Show( bShow );
}

void CDnGuildCreateDlg::UpdateTax()
{
	int nTax = GetGuildTask().GetGuildCreateTex();
	int nGold = nTax/10000;
	int nSilver = (nTax%10000)/100;
	int nBronze = nTax%100;
	m_pStaticGold->SetIntToText( nGold );
	m_pStaticSilver->SetIntToText( nSilver );
	m_pStaticBronze->SetIntToText( nBronze );

	m_bEnoughTax = false;
	if( CDnItemTask::IsActive() ) {
		INT64 biTax = (INT64)nTax;
		if( GetItemTask().GetCoin() >= biTax )
			m_bEnoughTax = true;
		else
		{
			if( nTax >= 10000 ) m_pStaticGold->SetTextColor( 0xFFFF0000, true );
			if( nTax >= 100 ) m_pStaticSilver->SetTextColor( 0xFFFF0000, true );
			m_pStaticBronze->SetTextColor( 0xFFFF0000, true );
		}
	}
}