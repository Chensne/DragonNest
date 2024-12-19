#include "stdafx.h"
#include "DnSkillTreePreSetDlg.h"
#include "DnSkillTreeDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

#if defined( PRE_ADD_PRESET_SKILLTREE )

CDnSkillTreePreSetDlg::CDnSkillTreePreSetDlg( UI_DIALOG_TYPE dialogType , CEtUIDialog *pParentDialog , int nID , CEtUICallback *pCallBack , bool bAutoCursor )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallBack, bAutoCursor )
, m_pSaveButton( NULL )
, m_pDelButton( NULL )
, m_pSkillTreeDlg( NULL )
{

}

CDnSkillTreePreSetDlg::~CDnSkillTreePreSetDlg()
{

}

void CDnSkillTreePreSetDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "SkillTreeFreeSetDlg.ui" ).c_str(), bShow );

	SetElementDialog( true );
}

void CDnSkillTreePreSetDlg::InitialUpdate()
{
	m_pSaveButton = GetControl<CEtUIButton>( "ID_BT_SAVE" );
	m_pDelButton = GetControl<CEtUIButton>( "ID_BT_DELETE" );

	m_pSaveButton->Show( false );
	m_pDelButton->Show( false );

	m_pSaveButton->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 754 ) );	// UISTRING : 현재 설정된 스킬 트리를 저장합니다.
	m_pDelButton->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 755 ) );		// UISTRING : 현재 설정된 스킬 트리를 삭제합니다.
}

void CDnSkillTreePreSetDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
	SetCmdControlName( pControl->GetControlName() );

	if( EVENT_BUTTON_CLICKED == nCommand )
	{
		if( strstr( pControl->GetControlName(), "ID_BT_SAVE" ) )
		{
			m_pSkillTreeDlg->ShowPresetSkillTreeSaveMessage();
		}
		else if( strstr( pControl->GetControlName(), "ID_BT_DELETE" ) )
		{
			m_pSkillTreeDlg->ShowPresetSkillTreeDelMessage();
		}
	}
}

void CDnSkillTreePreSetDlg::Enable( const bool bEnable )
{
	m_pSaveButton->Enable( bEnable );
	m_pDelButton->Enable( bEnable );
}

void CDnSkillTreePreSetDlg::SetSkillTreeDlg( CDnSkillTreeDlg * pSkillTreeDlg )
{
	m_pSkillTreeDlg = pSkillTreeDlg;
}

void CDnSkillTreePreSetDlg::RefreshButtonShow( const bool bSave_Show, const bool bSave_Enable, const bool bDel_Show, const bool bDel_Enable )
{
	m_pSaveButton->Show( bSave_Show );
	m_pSaveButton->Enable( bSave_Enable );
	m_pDelButton->Show( bDel_Show );
	m_pDelButton->Enable( bDel_Enable );
}
#endif	// #if defined( PRE_ADD_PRESET_SKILLTREE )