#include "StdAfx.h"

#if defined(PRE_ADD_DWC)
#include "DnPVPDWCTabDlg.h"
#include "DnInterface.h"
#include "TaskManager.h"
#include "PVPSendPacket.h"
#include "DnTableDB.h"
#include "DnDWCTask.h"

#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
#include "DnSystemDlg.h"
#include "DnMainMenuDlg.h"
#endif // PRE_ADD_SHORTCUT_HELP_DIALOG

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnPVPDWCTabDlg::CDnPVPDWCTabDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
, m_nSelectedLadderChannel(LadderSystem::MatchType::eCode::None)
, m_eSelectedGradeChannel(PvPCommon::RoomType::eRoomType::dwc)
{   
	for(int i = 0 ; i < eChannel_MAX ; ++i)
	{
		m_pChannelRadioBtn[i] = NULL;
	}
}

CDnPVPDWCTabDlg::~CDnPVPDWCTabDlg()
{
}

void CDnPVPDWCTabDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PvpDWCTabDlg.ui" ).c_str(), bShow );
}

void CDnPVPDWCTabDlg::InitialUpdate()
{
	const char* cControlName[] = { "ID_RBT_COLO" , "ID_RBT_DWC", "ID_RBT_DWCEX" };
	for(int i = 0 ; i < eChannel_MAX ; ++i)
	{
		m_pChannelRadioBtn[i] = GetControl<CEtUIRadioButton>(cControlName[i]);
		if(i == eChannel_Normal)
			m_pChannelRadioBtn[i]->SetChecked(true);
	}
}

void CDnPVPDWCTabDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{	
	SetCmdControlName(pControl->GetControlName());

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
		if( IsCmdControl("ID_BT_SMALLHELP") )
		{
			CDnMainMenuDlg* pMainMenuDlg = GetInterface().GetMainMenuDialog();
			CDnSystemDlg* pSystemDlg = pMainMenuDlg->GetSystemDlg();
			if(pSystemDlg)
				pSystemDlg->ShowChoiceHelpDlg(HELP_SHORTCUT_PVP);
		}
#endif
	}
	else if(nCommand == EVENT_RADIOBUTTON_CHANGED)
	{
		if(IsCmdControl("ID_RBT_COLO")) // 콜로세움 일반
		{
			if(m_nSelectedLadderChannel != LadderSystem::MatchType::eCode::None)
				SendPvPLeaveChannel();

			m_nSelectedLadderChannel = LadderSystem::MatchType::eCode::None;

#ifdef PRE_ADD_COLOSSEUM_BEGINNER
			if( bTriggeredByUser )
			{
				m_eSelectedGradeChannel = PvPCommon::RoomType::eRoomType::dwc;
				SendPVPChangeChannel( PvPCommon::RoomType::eRoomType::dwc );
			}
#endif // #ifdef PRE_ADD_COLOSSEUM_BEGINNER
			return;
		}
		else if( IsCmdControl("ID_RBT_DWC") ) // DWC 본선
		{
			// 래더플래그 설정
			m_nSelectedLadderChannel = LadderSystem::MatchType::eCode::_3vs3_DWC;
			if(bTriggeredByUser) 
				SendPvPEnterChannel(LadderSystem::MatchType::eCode::_3vs3_DWC);
			return;
		}
		else if( IsCmdControl("ID_RBT_DWCEX") ) // DWC 연습
		{
			// 래더플래그 설정
			m_nSelectedLadderChannel = LadderSystem::MatchType::eCode::_3vs3_DWC_PRACTICE;
			if(bTriggeredByUser) 
				SendPvPEnterChannel(LadderSystem::MatchType::eCode::_3vs3_DWC_PRACTICE);
			return;
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnPVPDWCTabDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if(IsShow())
	{
		if(GetDWCTask().HasDWCTeam() == false)
		{
			m_pChannelRadioBtn[eChannel_DWC]->Enable(false);
			m_pChannelRadioBtn[eChannel_DWC_Practice]->Enable(false);
			return;
		}

		const TDWCChannelInfo sDWCChannelInfo = GetDWCTask().GetDWCChannelInfo();
		bool bEnable = false;

		// 본선은 STATUS_NORMAL && 진행기간일때만 진입가능
		const TDWCChannelInfo sDWCTime = GetDWCTask().GetDWCChannelInfo();
		__time64_t tCurrentTime;
		time(&tCurrentTime);
		if( tCurrentTime > sDWCTime.tStartDate && tCurrentTime < sDWCTime.tEndDate )
		{
			if( sDWCChannelInfo.cStatus == DWC::DWCStatus::STATUS_NORMAL )
				bEnable = true;
			m_pChannelRadioBtn[eChannel_DWC]->Enable(bEnable);
		}

		// 연습모드는 STATUS_NORMAL / STATUS_PAUSE에서도 열림. ( 게임중/일시정지 )
		bool bEnable2 = false;
		if( GetDWCTask().IsDWCRankSession() )
			bEnable2 = true;
		m_pChannelRadioBtn[eChannel_DWC_Practice]->Enable(bEnable2);
	}
}

void CDnPVPDWCTabDlg::Show( bool bShow ) 
{ 
	CEtUIDialog::Show( bShow );
}

void CDnPVPDWCTabDlg::SetDWCRadioButton(eDWCChannel eChannelType)
{
	if(eChannelType > eChannel_MAX || eChannelType < eChannel_Normal)
		return;

	m_pChannelRadioBtn[eChannelType]->SetChecked(true);
}

void CDnPVPDWCTabDlg::SetDWCRadioButton(LadderSystem::MatchType::eCode eCode)
{
	switch(eCode)
	{
	case LadderSystem::MatchType::_3vs3_DWC:
		{
			if( false == m_pChannelRadioBtn[eChannel_DWC]->IsChecked() )
				m_pChannelRadioBtn[eChannel_DWC]->SetChecked(true);
		}
		break;

	case LadderSystem::MatchType::_3vs3_DWC_PRACTICE:
		{
			if( false == m_pChannelRadioBtn[eChannel_DWC]->IsChecked() )
				m_pChannelRadioBtn[eChannel_DWC_Practice]->SetChecked(true);
		}
		break;
	}
}

void CDnPVPDWCTabDlg::PushUIRadioButton(const char* szControlName)
{
	if( strlen(szControlName) <= 0 )
		return;

	CEtUIRadioButton* pRadioBtn = GetControl<CEtUIRadioButton>(szControlName);
	if(pRadioBtn)
		pRadioBtn->SetChecked(true);
}
#endif // #if defined(PRE_ADD_DWC)