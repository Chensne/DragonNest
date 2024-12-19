#include "StdAfx.h"
#include "DnEventDungeonEnterDlg.h"
#include "DnEventDungeonEnterLevelDlg.h"
#include "PartySendPacket.h"
#include "DnDungeonExpectDlg.h"

CDnEventDungeonEnterDlg::CDnEventDungeonEnterDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnDungeonEnterDlg( dialogType, pParentDialog, nID, pCallback )
{
}

CDnEventDungeonEnterDlg::~CDnEventDungeonEnterDlg()
{
}

void CDnEventDungeonEnterDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "DungeonEventEnterDlg.ui" ).c_str(), bShow );
}

void CDnEventDungeonEnterDlg::InitialUpdate()
{
	m_pStaticDungeonName = GetControl<CEtUIStatic>("ID_STATIC_DUNGEONNAME");
	m_pTextureDungeonImage = GetControl<CEtUITextureControl>("ID_TEXTURE_DUNGEONIMAGE");
	m_pTextureDungeonImage->GetUICoord( m_uiTextureImageBack );

	m_pStaticBottomBack = GetControl<CEtUIStatic>("ID_BOTTOM_BLACK");
	m_pStaticBottomBack->GetUICoord( m_uiBottomBlackBack );

	char szButtonGate[32]={0};
	SDungeonGateInfo dungeonGateInfo;

	for( int i=0; i<DUNGEON_GATE_COUNT; i++ )
	{
		sprintf_s( szButtonGate, 32, "ID_BUTTON_DUN_%02d", i+1 );
		dungeonGateInfo.m_pButtonGate = GetControl<CDnStageEnterButton>(szButtonGate);

		dungeonGateInfo.Show( false );
		dungeonGateInfo.Clear();
		m_vecDungeonGateInfo.push_back(dungeonGateInfo);
	}

	InitializeEnterLevelDlg();
}

void CDnEventDungeonEnterDlg::InitializeEnterLevelDlg()
{
	m_pDungeonEnterLevelDlg = new CDnEventDungeonEnterLevelDlg( UI_TYPE_CHILD, this );
	m_pDungeonEnterLevelDlg->Initialize( false );
	m_pDungeonEnterLevelDlg->SetDialogID( GetDialogID() );

	m_pDungeonExpectDlg = new CDnDungeonExpectDlg( UI_TYPE_CHILD, this );
	m_pDungeonExpectDlg->Initialize( false );
}

void CDnEventDungeonEnterDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	CDnDungeonEnterDlg::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );

	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_RADIOBUTTON_CHANGED )
	{
		CEtUIRadioButton *pRadioButton = (CEtUIRadioButton*)pControl;
		if( !pRadioButton ) return;

		if( strstr( pControl->GetControlName(), "ID_BUTTON_DUN" ) )
		{
			m_nGateIndex = pRadioButton->GetTabID();
			if( m_nGateIndex >= 0 )
			{
				::SendSelectDungeonInfo( m_vecDungeonGateInfo[m_nGateIndex].m_nMapIndex, 0 );
				CDnDungeonEnterDlg::SDungeonGateInfo *pInfo = &m_vecDungeonGateInfo[ m_nGateIndex ];
				if( pInfo )
				{
					if( m_pDungeonExpectDlg )
						m_pDungeonExpectDlg->UpdateDungeonExpectReward( pInfo->m_nMapIndex, m_pDungeonEnterLevelDlg->GetDungeonLevel() );
				}
			}
			return;
		}
	}

}