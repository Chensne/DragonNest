#include "stdafx.h"
#include "DnSkillTreeSetMessageDlg.h"
#include "DnSkillTreeDlg.h"
#include "DnInterfaceString.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

#if defined( PRE_ADD_PRESET_SKILLTREE )

CDnSkillTreeSetMessageDlg::CDnSkillTreeSetMessageDlg( UI_DIALOG_TYPE dialogType , CEtUIDialog *pParentDialog , int nID , CEtUICallback *pCallBack , bool bAutoCursor )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallBack, bAutoCursor )
, m_pNameEditBox( NULL )
, m_pNameBoard( NULL )
, m_pNameComment( NULL )
, m_pDelComment( NULL )
, m_pSkillTreeDlg( NULL )
, m_pApplyButton( NULL )
, m_nMode( E_ADD_SKILLTREE_PRESET )
{

}
CDnSkillTreeSetMessageDlg::~CDnSkillTreeSetMessageDlg()
{

}

void CDnSkillTreeSetMessageDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "SkillTreeSetMessageDlg.ui" ).c_str(), bShow );
}

void CDnSkillTreeSetMessageDlg::InitialUpdate()
{
	m_pNameEditBox = GetControl<CEtUIIMEEditBox>( "ID_IMEEDITBOX_NAME" );
	m_pNameBoard = GetControl<CEtUIStatic>( "ID_STATIC_BAR" );
	m_pNameComment = GetControl<CEtUIStatic>( "ID_TEXT_ASK" );
	m_pDelComment = GetControl<CEtUIStatic>( "ID_TEXT_DELETEASK" );
	m_pApplyButton = GetControl<CEtUIButton>( "ID_BT_OK" );
}

void CDnSkillTreeSetMessageDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );
}

void CDnSkillTreeSetMessageDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( !IsShow() )
		return;

	if( E_ADD_SKILLTREE_PRESET == m_nMode )
	{
		std::wstring wszName = std::wstring( m_pNameEditBox->GetText() );

		if( wszName.empty() )
			m_pApplyButton->Enable( false );
		else
			m_pApplyButton->Enable( true );
	}
}

void CDnSkillTreeSetMessageDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
	SetCmdControlName( pControl->GetControlName() );

	if( EVENT_BUTTON_CLICKED == nCommand )
	{
		if( strstr( pControl->GetControlName(), "ID_BT_OK" ) )
		{
			if( E_ADD_SKILLTREE_PRESET == m_nMode )
			{
				std::wstring wszName = std::wstring( m_pNameEditBox->GetText() );

				if( wszName.empty() )
					return;

				if( DN_INTERFACE::UTIL::CheckChat( wszName ) )
				{
					GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4303 ) );	// UISTRING : 금칙어이므로 추가할 수 없습니다.
					return;
				}

				m_pSkillTreeDlg->PresetSkillTreeSave( wszName );
			}
			else if( E_DEL_SKILLTREE_PRESET == m_nMode )
			{
				m_pSkillTreeDlg->PresetSkillTreeDel();
			}
			Show( false );
		}
		else if( strstr( pControl->GetControlName(), "ID_BT_CANCEL" ) || strstr( pControl->GetControlName(), "ID_BT_CLOSE" ) )
		{
			Show( false );
		}
	}
}

void CDnSkillTreeSetMessageDlg::SetModeShow( const int eMode )
{
	bool bAddUI = false, bDelUI = false;
	if( E_ADD_SKILLTREE_PRESET == eMode )
	{
		bAddUI = true;

		focus::SetFocus( m_pNameEditBox );
		m_pApplyButton->Enable( false );
		m_nMode = E_ADD_SKILLTREE_PRESET;
	}
	else if( E_DEL_SKILLTREE_PRESET )
	{
		bDelUI = true;

		m_pApplyButton->Enable( true );
		m_nMode = E_DEL_SKILLTREE_PRESET;
	}

	m_pNameEditBox->Show( bAddUI );
	m_pNameEditBox->ClearText();
	m_pNameBoard->Show( bAddUI );
	m_pNameComment->Show( bAddUI );

	m_pDelComment->Show( bDelUI );

	Show( true );
}

void CDnSkillTreeSetMessageDlg::SetSkillTreeDlg( CDnSkillTreeDlg * pSkillTreeDlg )
{
	m_pSkillTreeDlg = pSkillTreeDlg;
}

#endif	// #if defined( PRE_ADD_PRESET_SKILLTREE )