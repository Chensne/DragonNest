#include "stdafx.h"
#include "DnMiniPlayerGuildWarMainTabDlg.h"
#include "DnGuildWarSituationMng.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )

CDnMiniPlayerGuildWarMainTabDlg::CDnMiniPlayerGuildWarMainTabDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_bMaster( false )
, m_bCross( false )
, m_pGuildWarSituationMng( NULL )
, m_pButtonCross(NULL)
, m_pButtonOK(NULL)
, m_pCheckParty(NULL)
, m_pStaticName(NULL)
{}

CDnMiniPlayerGuildWarMainTabDlg::~CDnMiniPlayerGuildWarMainTabDlg()
{}

void CDnMiniPlayerGuildWarMainTabDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MiniPlayerGuildWarMainTab.ui" ).c_str(), bShow );
}

void CDnMiniPlayerGuildWarMainTabDlg::InitialUpdate()
{
	char szControlName[256];
	for( int itr = 0; itr < PARTY_COUNT; ++itr )
	{
		sprintf_s( szControlName, _countof(szControlName), "ID_STATIC_BAR%d", itr );
		m_pStaticBar[itr] = GetControl<CEtUIStatic>( szControlName );
		m_pStaticBar[itr]->Show( false );
	}

	m_pStaticName = GetControl<CEtUIStatic>( "ID_STATIC1" );
	m_pButtonOK = GetControl<CEtUIButton>( "ID_BT_OK" );
	m_pButtonCross = GetControl<CEtUIButton>( "ID_BT_CROSS" );
	m_pCheckParty = GetControl<CEtUICheckBox>( "ID_CHECKBOX_SHOW" );

	m_pButtonOK->Show( false );
	m_pButtonCross->Show( false );
}

void CDnMiniPlayerGuildWarMainTabDlg::Show(bool bShow)
{
	if( bShow == m_bShow )
		return;

	CEtUIDialog::Show( bShow );
}

void CDnMiniPlayerGuildWarMainTabDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BT_CROSS" ) )
		{
			m_pButtonCross->Show( false );
			m_pButtonOK->Show( true );

			m_bCross = true;

			m_pGuildWarSituationMng->SetSlotMove( true );
		}
		else if( IsCmdControl( "ID_BT_OK" ) )
		{
			m_pButtonCross->Show( true );
			m_pButtonOK->Show( false );

			m_bCross = false;

			m_pGuildWarSituationMng->SetSlotMove( false );
		}
	}
	else if( nCommand == EVENT_CHECKBOX_CHANGED )
	{
		if( IsCmdControl( "ID_CHECKBOX_SHOW" ) )
		{
			bool bIsChecked = m_pCheckParty->IsChecked();
			m_pGuildWarSituationMng->PartyShow( bIsChecked );
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );

	focus::ReleaseControl();
}

void CDnMiniPlayerGuildWarMainTabDlg::SetMaster(bool bMaster)
{ 
	if( m_bMaster == bMaster )
		return;

	m_bMaster = bMaster;

	if( m_bMaster )
	{
		m_pButtonCross->Show( true );
		m_pButtonOK->Show( false );
	}
	else
	{
		m_pButtonCross->Show( false );
		m_pButtonOK->Show( false );
		m_pGuildWarSituationMng->SetSlotMove( false );
	}
}

void CDnMiniPlayerGuildWarMainTabDlg::SetGuildWarSituation( CDnGuildWarSituationMng * pGuildWarSituationMng )
{
	m_pGuildWarSituationMng = pGuildWarSituationMng;
}

void CDnMiniPlayerGuildWarMainTabDlg::OnChangeResolution()
{
	CEtUIDialog::OnChangeResolution();
	m_pGuildWarSituationMng->NeedUpdate();
}

void CDnMiniPlayerGuildWarMainTabDlg::SetPartyCount( BYTE cCount)
{
	for( int itr = 0; itr < PARTY_COUNT; ++itr )
		m_pStaticBar[itr]->Show( false );

	m_pStaticBar[cCount]->Show( true );

	SUICoord BarCoord = m_pStaticBar[cCount]->GetUICoord();

	m_pCheckParty->SetPosition( BarCoord.fX + 0.015f, m_pCheckParty->GetUICoord().fY );
	m_pStaticName->SetPosition( BarCoord.fX + 0.035f, m_pStaticName->GetUICoord().fY );

	m_pButtonOK->SetPosition( BarCoord.fX + BarCoord.fWidth - 0.035f, m_pButtonOK->GetUICoord().fY );
	m_pButtonCross->SetPosition( BarCoord.fX + BarCoord.fWidth - 0.035f, m_pButtonCross->GetUICoord().fY );
}
