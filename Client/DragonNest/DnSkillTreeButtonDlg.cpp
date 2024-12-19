#include "stdafx.h"
#include "DnSkillTreeButtonDlg.h"
#include "DnSkillTreeDlg.h"
#include "DnGoldReservationDlg.h"
#include "DnGoldNoReservationDlg.h"
#include "DnSkillTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )

CDnSkillTreeButtonDlg::CDnSkillTreeButtonDlg( UI_DIALOG_TYPE dialogType , CEtUIDialog *pParentDialog , int nID , CEtUICallback *pCallBack , bool bAutoCursor )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallBack, bAutoCursor )
, m_pResetButton( NULL )
, m_pApplyButton( NULL )
, m_pSkillTreeDlg( NULL )
, m_pReservationGoldDlg( NULL )
, m_pReservationGlodNoDlg( NULL )
{

}

CDnSkillTreeButtonDlg::~CDnSkillTreeButtonDlg()
{
	SAFE_DELETE( m_pReservationGoldDlg );
	SAFE_DELETE( m_pReservationGlodNoDlg );
}

void CDnSkillTreeButtonDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "SkillTreeButtonDlg.ui" ).c_str(), bShow );

	SetElementDialog( true );
}

void CDnSkillTreeButtonDlg::InitialUpdate()
{
	m_pResetButton = GetControl<CEtUIButton>( "ID_BT_RESET" );
	m_pApplyButton = GetControl<CEtUIButton>( "ID_BT_OK" );

	m_pReservationGoldDlg = new CDnReservationGoldDlg( UI_TYPE_CHILD_MODAL, this );
	m_pReservationGoldDlg->Initialize( false );

	m_pReservationGlodNoDlg = new CDnReservationGoldNoDlg( UI_TYPE_CHILD_MODAL, this );
	m_pReservationGlodNoDlg->Initialize( false );
}

void CDnSkillTreeButtonDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if( NULL == m_pSkillTreeDlg )
			return;

		if (IsCmdControl("ID_BT_RESET"))
		{
			m_pSkillTreeDlg->ResetButtonCommand();
		}
		else if (IsCmdControl("ID_BT_OK"))
		{
			if( 0 != GetSkillTask().GetReservationSkillNeedMoney() )
				ShowChildDialog( m_pReservationGoldDlg, true );
			else
				ShowChildDialog( m_pReservationGlodNoDlg, true );
		}
	}
}

void CDnSkillTreeButtonDlg::SetButtonEnable( const bool bEnable )
{
	m_pResetButton->Enable( bEnable );
	m_pApplyButton->Enable( bEnable );
}

void CDnSkillTreeButtonDlg::SetSkillTreeDlg( CDnSkillTreeDlg * pSkillTreeDlg )
{
	m_pSkillTreeDlg = pSkillTreeDlg;
	m_pReservationGoldDlg->SetSkillTreeDlg( m_pSkillTreeDlg );
	m_pReservationGlodNoDlg->SetSkillTreeDlg( m_pSkillTreeDlg );
}

#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )