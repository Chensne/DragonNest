#include "StdAfx.h"
#include "DnGameViewOptDlg.h"
#include "GameOption.h"
#include "DnGameOptionTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGameViewOptDlg::CDnGameViewOptDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CDnOptionDlg( dialogType, pParentDialog, nID, pCallback )
{
	m_pCheckUserName = NULL;
	m_pCheckPartyUserName = NULL;
	m_pCheckMonsterName = NULL;
	m_pCheckStateBarInsideCount = NULL;
	m_pCheckMyHPBar = NULL;
	m_pCheckPartyHPBar = NULL;
	m_pCheckMonsterHPBar = NULL;
	m_pCheckMyCombatInfo = NULL;
	m_pCheckPartycombatinfo = NULL;
	m_pCheckDirectionDamageMelee = NULL;
	m_pCheckDirectionDamageRange = NULL;
	m_pCheckGuildName = NULL;
	m_pCheckGameTip = NULL;
	m_pCheckSimpleGuild = NULL;
#ifdef PRE_ADD_CASH_AMULET
	m_pCheckCashChatBalloon = NULL;
#endif
#ifdef PRE_ADD_VIEW_OPITION_PARTY_HEAL
	m_pCheckPartyHealInfo = NULL;
#endif
#ifdef PRE_ADD_SHOWOPTION_PVPMARK
	m_pCheckPVPLevelMark = NULL;
#endif
#ifdef PRE_ADD_SKILLCOOLTIME
	m_pCheckSkillCoolTimeConter = NULL;
#endif
}

CDnGameViewOptDlg::~CDnGameViewOptDlg(void)
{
}

void CDnGameViewOptDlg::Initialize( bool bShow )
{
	BaseClass::Initialize( CEtResourceMng::GetInstance().GetFullName( "GameViewOptDlg.ui" ).c_str(), bShow );
}

void CDnGameViewOptDlg::InitialUpdate()
{
	m_pCheckUserName = GetControl<CEtUICheckBox>("ID_OP_VIEW_CHECK_USER");
	m_pCheckPartyUserName = GetControl<CEtUICheckBox>("ID_OP_VIEW_CHECK_PARTY");
	m_pCheckMonsterName = GetControl<CEtUICheckBox>("ID_OP_VIEW_CHECK_MONSTER");
	
	m_pCheckStateBarInsideCount = GetControl<CEtUICheckBox>("ID_OP_VIEW_CHECK_NUMBER");
	m_pCheckMyHPBar = GetControl<CEtUICheckBox>("ID_OP_VIEW_CHECK_HP");
	m_pCheckPartyHPBar = GetControl<CEtUICheckBox>("ID_OP_VIEW_CHECK_PARTY_HP");
	m_pCheckMonsterHPBar = GetControl<CEtUICheckBox>("ID_OP_VIEW_CHECK_MONSTER_HP");

	m_pCheckMyCombatInfo = GetControl<CEtUICheckBox>("ID_OP_VIEW_CHECK_COMBAT_SELF");
	m_pCheckPartycombatinfo = GetControl<CEtUICheckBox>("ID_OP_VIEW_CHECK_COMBAT_PARTY");
	
	m_pCheckDirectionDamageMelee = GetControl<CEtUICheckBox>("ID_OP_VIEW_CHECK_COMBAT_MELEE");	
	m_pCheckDirectionDamageRange = GetControl<CEtUICheckBox>("ID_OP_VIEW_CHECK_COMBAT_RANGE");	

	m_pCheckGuildName = GetControl<CEtUICheckBox>("ID_OP_VIEW_CHECK_GUILD");
	m_pCheckGameTip = GetControl<CEtUICheckBox>("ID_OP_VIEW_CHECK_COMBAT_CAPTION");
	m_pCheckSimpleGuild = GetControl<CEtUICheckBox>("ID_OP_VIEW_CHECK_GUIDE");

#ifdef PRE_ADD_CASH_AMULET
	m_pCheckCashChatBalloon =  GetControl<CEtUICheckBox>("ID_OP_VIEW_CHECK_CHAT");
#endif

#ifdef PRE_ADD_VIEW_OPITION_PARTY_HEAL
	m_pCheckPartyHealInfo = GetControl<CEtUICheckBox>("ID_OP_VIEW_CHECK_COMBAT_HEAL");
#endif
#ifdef PRE_ADD_SHOWOPTION_PVPMARK
	m_pCheckPVPLevelMark = GetControl<CEtUICheckBox>("ID_CHECKBOX_COLOMARK");
#endif
#ifdef PRE_ADD_SKILLCOOLTIME
	m_pCheckSkillCoolTimeConter = GetControl<CEtUICheckBox>("ID_OP_VIEW_CHECK_SKILL");
#endif
}

void CDnGameViewOptDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_CHECKBOX_CHANGED )
	{
		if( IsCmdControl( "ID_OP_VIEW_CHECK_USER" ) )
		{
			bool bCheck = m_pCheckUserName->IsChecked();
			m_pCheckPartyUserName->Enable( !bCheck );
			if( bCheck ) {
				m_pCheckPartyUserName->SetChecked( true );
			}
			m_pCheckGuildName->Enable( bCheck );
			if( !bCheck ) {
				m_pCheckGuildName->SetChecked( false );
			}
			return;
		}
	}

	BaseClass::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnGameViewOptDlg::ImportSetting()
{
	m_pCheckUserName->SetChecked( CGameOption::GetInstance().bUserName );
	m_pCheckPartyUserName->SetChecked( CGameOption::GetInstance().bPartyUserName );
	m_pCheckMonsterName->SetChecked( CGameOption::GetInstance().bMonsterName );
	m_pCheckStateBarInsideCount->SetChecked( CGameOption::GetInstance().bStateBarInsideCount );
	m_pCheckMyHPBar->SetChecked( CGameOption::GetInstance().bMyHPBar );
	m_pCheckPartyHPBar->SetChecked( CGameOption::GetInstance().bPartyHPBar );
	m_pCheckMonsterHPBar->SetChecked( CGameOption::GetInstance().bMonsterHPBar );
	m_pCheckMyCombatInfo->SetChecked( CGameOption::GetInstance().bMyCombatInfo );
	m_pCheckPartycombatinfo->SetChecked( CGameOption::GetInstance().bPartyCombatInfo );
	m_pCheckDirectionDamageMelee->SetChecked( CGameOption::GetInstance().bDamageDirMelee );
	m_pCheckDirectionDamageRange->SetChecked( CGameOption::GetInstance().bDamageDirRange );
	m_pCheckGuildName->SetChecked( CGameOption::GetInstance().bGuildName );
	m_pCheckGameTip->SetChecked( CGameOption::GetInstance().bGameTip );
	m_pCheckSimpleGuild->SetChecked( CGameOption::GetInstance().bSimpleGuide );
#ifdef PRE_ADD_CASH_AMULET
	m_pCheckCashChatBalloon->SetChecked(CGameOption::GetInstance().m_bShowCashChatBalloon);
#endif
#ifdef PRE_ADD_VIEW_OPITION_PARTY_HEAL
	m_pCheckPartyHealInfo->SetChecked(CGameOption::GetInstance().m_bShowPartyHealInfo);
#endif
#ifdef PRE_ADD_SHOWOPTION_PVPMARK
	m_pCheckPVPLevelMark->SetChecked( CGameOption::GetInstance().bPVPLevelMark );
#endif
#ifdef PRE_ADD_SKILLCOOLTIME
	m_pCheckSkillCoolTimeConter->SetChecked(CGameOption::GetInstance().bSkillCoolTimeCounter);
#endif

	bool bCheck = m_pCheckUserName->IsChecked();
	m_pCheckPartyUserName->Enable( !bCheck );
	m_pCheckGuildName->Enable( bCheck );

	if( bCheck ) {
		ASSERT( m_pCheckPartyUserName->IsChecked() );
	}
}

void CDnGameViewOptDlg::ExportSetting()
{
	if( IsChanged() ) 
	{
		CGameOption::GetInstance().bUserName = m_pCheckUserName->IsChecked();
		CGameOption::GetInstance().bPartyUserName = m_pCheckPartyUserName->IsChecked();
		CGameOption::GetInstance().bMonsterName = m_pCheckMonsterName->IsChecked();
		CGameOption::GetInstance().bStateBarInsideCount = m_pCheckStateBarInsideCount->IsChecked();
		CGameOption::GetInstance().bMyHPBar = m_pCheckMyHPBar->IsChecked();
		CGameOption::GetInstance().bPartyHPBar = m_pCheckPartyHPBar->IsChecked();
		CGameOption::GetInstance().bMonsterHPBar = m_pCheckMonsterHPBar->IsChecked();
		CGameOption::GetInstance().bMyCombatInfo = m_pCheckMyCombatInfo->IsChecked();
		CGameOption::GetInstance().bPartyCombatInfo = m_pCheckPartycombatinfo->IsChecked();
		CGameOption::GetInstance().bDamageDirMelee = m_pCheckDirectionDamageMelee->IsChecked();
		CGameOption::GetInstance().bDamageDirRange = m_pCheckDirectionDamageRange->IsChecked();
		CGameOption::GetInstance().bGuildName = m_pCheckGuildName->IsChecked();
		CGameOption::GetInstance().bGameTip = m_pCheckGameTip->IsChecked();
		CGameOption::GetInstance().bSimpleGuide = m_pCheckSimpleGuild->IsChecked();
#ifdef PRE_ADD_CASH_AMULET
		CGameOption::GetInstance().m_bShowCashChatBalloon = m_pCheckCashChatBalloon->IsChecked();
#endif
#ifdef PRE_ADD_VIEW_OPITION_PARTY_HEAL
		CGameOption::GetInstance().m_bShowPartyHealInfo = m_pCheckPartyHealInfo->IsChecked();
#endif
#ifdef PRE_ADD_SHOWOPTION_PVPMARK
		CGameOption::GetInstance().bPVPLevelMark = m_pCheckPVPLevelMark->IsChecked();
#endif
#ifdef PRE_ADD_SKILLCOOLTIME
		CGameOption::GetInstance().bSkillCoolTimeCounter = m_pCheckSkillCoolTimeConter->IsChecked();
#endif

		// Save to Server
		GetGameOptionTask().ReqSendSetGameOption( *static_cast<TGameOptions*>(CGameOption::GetInstancePtr()) );
	}
}

bool CDnGameViewOptDlg::IsChanged()
{
	if( CGameOption::GetInstance().bUserName != m_pCheckUserName->IsChecked() ||
		CGameOption::GetInstance().bPartyUserName != m_pCheckPartyUserName->IsChecked() ||
		CGameOption::GetInstance().bMonsterName != m_pCheckMonsterName->IsChecked() ||
		CGameOption::GetInstance().bStateBarInsideCount != m_pCheckStateBarInsideCount->IsChecked() ||
		CGameOption::GetInstance().bMyHPBar != m_pCheckMyHPBar->IsChecked() ||
		CGameOption::GetInstance().bPartyHPBar != m_pCheckPartyHPBar->IsChecked() ||
		CGameOption::GetInstance().bMonsterHPBar != m_pCheckMonsterHPBar->IsChecked() ||
		CGameOption::GetInstance().bMyCombatInfo != m_pCheckMyCombatInfo->IsChecked() ||
		CGameOption::GetInstance().bPartyCombatInfo != m_pCheckPartycombatinfo->IsChecked() ||
		CGameOption::GetInstance().bDamageDirMelee != m_pCheckDirectionDamageMelee->IsChecked() || 
		CGameOption::GetInstance().bDamageDirRange != m_pCheckDirectionDamageRange->IsChecked() || 
		CGameOption::GetInstance().bGuildName != m_pCheckGuildName->IsChecked() ||
		CGameOption::GetInstance().bGameTip != m_pCheckGameTip->IsChecked() ||
		CGameOption::GetInstance().bSimpleGuide != m_pCheckSimpleGuild->IsChecked() 
#ifdef PRE_ADD_CASH_AMULET
		|| CGameOption::GetInstance().m_bShowCashChatBalloon != m_pCheckCashChatBalloon->IsChecked()
#endif
#ifdef PRE_ADD_VIEW_OPITION_PARTY_HEAL
		|| CGameOption::GetInstance().m_bShowPartyHealInfo != m_pCheckPartyHealInfo->IsChecked()
#endif
#ifdef PRE_ADD_SHOWOPTION_PVPMARK
		|| CGameOption::GetInstance().bPVPLevelMark != m_pCheckPVPLevelMark->IsChecked()
#endif
#ifdef PRE_ADD_SKILLCOOLTIME
		|| CGameOption::GetInstance().bSkillCoolTimeCounter != m_pCheckSkillCoolTimeConter->IsChecked()
#endif
		)
		return true;
	return false;
}