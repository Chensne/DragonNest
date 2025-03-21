#include "StdAfx.h"
#include "DnPVPOptionDlg.h"
#include "PVPSendPacket.h"
#include "TaskManager.h"
#include "DnSkillTask.h"
#include "DnInterface.h"
#include "DnBaseRoomDlg.h"
#include "DnPVPLadderSystemDlg.h"

#define CHECK_BUTTON_DELAY_TIME 1.5f

CDnPVPOptionDlg::CDnPVPOptionDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pCheckBoxPvPFatigue(NULL)
, m_fEnableTime(-1.f)
, m_pCheckBoxSkillPrimary(NULL)
, m_pCheckBoxSkillSecondary(NULL)
{
}

CDnPVPOptionDlg::~CDnPVPOptionDlg(void)
{	
}

void CDnPVPOptionDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PvpOptionDlg.ui" ).c_str(), bShow );
}

void CDnPVPOptionDlg::InitialUpdate()
{
	m_pCheckBoxPvPFatigue = GetControl<CEtUICheckBox>( "ID_CHECKBOX_FTG" );
	m_pCheckBoxPvPFatigue->Enable(true);
	m_pCheckBoxPvPFatigue->SetChecked(false, false);

	m_pCheckBoxSkillPrimary = GetControl<CEtUICheckBox>( "ID_CHECKBOX_SKILL0" );
	m_pCheckBoxSkillPrimary->SetChecked(false , false);
	
	m_pCheckBoxSkillSecondary = GetControl<CEtUICheckBox>( "ID_CHECKBOX_SKILL1" );
	m_pCheckBoxSkillSecondary->SetChecked(false, false);

#if defined(PRE_ADD_HID_DUALSKILL)
	m_pCheckBoxSkillPrimary->Show( false );
	m_pCheckBoxSkillSecondary->Show( false );

	CEtUIStatic *pStaticPrimary = GetControl<CEtUIStatic>( "ID_TEXT1" );
	CEtUIStatic *pStaticSecondary = GetControl<CEtUIStatic>( "ID_TEXT2" );

	if( pStaticPrimary ) pStaticPrimary->Show( false );
	if( pStaticSecondary ) pStaticSecondary->Show( false );
#endif
}

void CDnPVPOptionDlg::Show( bool bShow )
{
	if( CDnSkillTask::IsActive() )
	{
		m_pCheckBoxSkillSecondary->Enable( (GetSkillTask().GetSkillPageCount() > DualSkill::Type::Secondary) );
		m_pCheckBoxSkillPrimary->Enable( (GetSkillTask().GetSkillPageCount() > DualSkill::Type::Secondary) );


		int nSkillPage = GetSkillTask().GetCurrentSkillPage();
		if(GetSkillTask().GetNeedRefreshSkillPage() != DualSkill::Type::None )
			nSkillPage = GetSkillTask().GetNeedRefreshSkillPage();

		switch(nSkillPage)
		{
		case DualSkill::Type::Primary:
			m_pCheckBoxSkillPrimary->SetChecked(true,false);
			m_pCheckBoxSkillSecondary->SetChecked(false, false);
			break;
		case DualSkill::Type::Secondary:
			m_pCheckBoxSkillPrimary->SetChecked(false , false);
			m_pCheckBoxSkillSecondary->SetChecked(true, false);
			break;
		}
	}

	CEtUIDialog::Show(bShow);
}

void CDnPVPOptionDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	ProcessEnableTime(fElapsedTime);
}


void CDnPVPOptionDlg::ProcessEnableTime( float fElapsedTime )
{
	if( GetParentDialog()->IsShow() && IsShow() && GetInterface().GetGameRoomDlg() && GetInterface().GetPvPLadderSystemDlg() )
	{
		UINT nUserState = GetInterface().GetGameRoomDlg()->GetLocalUserState();

		if( GetInterface().GetPvPLadderSystemDlg()->IsShow() )
		{
			nUserState = GetInterface().GetPvPLadderSystemDlg()->GetLocalUserState();
		}

		if( nUserState & PvPCommon::UserState::Starting ||
			nUserState & PvPCommon::UserState::Syncing ||
			nUserState & PvPCommon::UserState::Playing )
		{
			m_pCheckBoxSkillSecondary->Enable( false );
			m_pCheckBoxSkillPrimary->Enable( false );
			m_fEnableTime = CHECK_BUTTON_DELAY_TIME; 
			return;
		}
	}

	if(m_fEnableTime != -1.f)
	{
		m_fEnableTime -= fElapsedTime;

		if(m_fEnableTime <= 0.f)
		{
			if( CDnSkillTask::IsActive() )
			{
				m_pCheckBoxSkillSecondary->Enable( (GetSkillTask().GetSkillPageCount() > DualSkill::Type::Secondary));
				m_pCheckBoxSkillPrimary->Enable( (GetSkillTask().GetSkillPageCount() > DualSkill::Type::Secondary) );
			}

			m_fEnableTime = -1.f;
		}
	}
}

void CDnPVPOptionDlg::EnalbeFatigue(bool bTrue)
{
	m_pCheckBoxPvPFatigue->Enable(bTrue);
	if(!bTrue)
		m_pCheckBoxPvPFatigue->SetChecked(false, false);
}

void CDnPVPOptionDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_CHECKBOX_CHANGED )
	{
		if( IsCmdControl( "ID_CHECKBOX_FTG" ) )
		{			
			SendPvPFatigueOption(m_pCheckBoxPvPFatigue->IsChecked());
		}
		if( IsCmdControl( "ID_CHECKBOX_SKILL0" ) )
		{
			if( CDnSkillTask::IsActive() )
				GetSkillTask().SendChangeSkillPage(DualSkill::Type::Primary , true);

			m_pCheckBoxSkillPrimary->SetChecked(true , false);
			m_pCheckBoxSkillSecondary->SetChecked(false,false);

			m_pCheckBoxSkillPrimary->Enable( false );
			m_pCheckBoxSkillSecondary->Enable( false );
			m_fEnableTime = CHECK_BUTTON_DELAY_TIME;
		}

		if( IsCmdControl( "ID_CHECKBOX_SKILL1" ) )
		{
			if( CDnSkillTask::IsActive() )
				GetSkillTask().SendChangeSkillPage(DualSkill::Type::Secondary, true);
			
			m_pCheckBoxSkillPrimary->SetChecked(false,false);
			m_pCheckBoxSkillSecondary->SetChecked(true,false);

			m_pCheckBoxSkillPrimary->Enable( false );
			m_pCheckBoxSkillSecondary->Enable( false );
			m_fEnableTime = CHECK_BUTTON_DELAY_TIME; 
		}
	}
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}
