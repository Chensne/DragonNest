#include "StdAfx.h"
#include "DnPVPLadderInviteConfirmDlg.h"
#include "DnInterface.h"
#include "TaskManager.h"
#include "DnWorld.h"
#include "PvPSendPacket.h"
#ifdef PRE_ADD_DWC
#include "DnDWCTask.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnPVPLadderInviteConfirmDlg::CDnPVPLadderInviteConfirmDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pProgressBarTime(NULL)
, m_pStaticName(NULL)
, m_pStaticPoint(NULL)
, m_pStaticNumber(NULL)
, m_pButtonOK(NULL)
, m_pButtonCancel(NULL)
, m_fTotalTime(LadderSystem::Common::InviteValidTick / 1200)
, m_fElapsedTime(LadderSystem::Common::InviteValidTick / 1200) // 서버의 응답 대기 갭이 있기때문에 딜레이 맥스 타임을 클라는 좀 이르게 계산하도록 한다.
{
}

CDnPVPLadderInviteConfirmDlg::~CDnPVPLadderInviteConfirmDlg(void)
{
}

void CDnPVPLadderInviteConfirmDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PvpLadderInviteConfirmDlg.ui" ).c_str(), bShow );
}

void CDnPVPLadderInviteConfirmDlg::InitialUpdate()
{
	m_pProgressBarTime = GetControl<CEtUIProgressBar>("ID_PROGRESSBAR_TIME");
	m_pStaticName = GetControl<CEtUIStatic>("ID_TEXT_NAME");
	m_pStaticPoint = GetControl<CEtUIStatic>("ID_TEXT_LADDERPOINT");
	m_pStaticNumber = GetControl<CEtUIStatic>("ID_TEXT_NUMBER");
	m_pButtonOK = GetControl<CEtUIButton>("ID_OK");
	m_pButtonCancel = GetControl<CEtUIButton>("ID_CANCEL");
#ifdef PRE_ADD_DWC
	m_pStaticPoint->Show(!GetDWCTask().IsDWCChar());
#endif

	m_wszName.clear();
}

void CDnPVPLadderInviteConfirmDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( !IsShow() )
		return;

	if( m_fElapsedTime <= 0.0f )
	{
		Show(false);
	}
	else
	{
		m_fElapsedTime -= fElapsedTime;
		if( m_fTotalTime != 0.0f)
			m_pProgressBarTime->SetProgress( m_fElapsedTime / m_fTotalTime * 100.0f );
		else
			m_pProgressBarTime->SetProgress( 0.0f );
	}
}

void CDnPVPLadderInviteConfirmDlg::Show( bool bShow )
{ 
	if( bShow == m_bShow )
		return;

	if( !bShow )
	{
		if(!m_wszName.empty() && m_fElapsedTime <= 0.0f)
			SendLadderInviteConfirm(m_wszName,false);

		m_fTotalTime = LadderSystem::Common::InviteValidTick / 1200;
		m_fElapsedTime = LadderSystem::Common::InviteValidTick / 1200;
		m_wszName.clear();
	}
	else
	{
		GetInterface().CloseMessageBox();
	}

	CEtUIDialog::Show( bShow );
}

void CDnPVPLadderInviteConfirmDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_CANCEL"))	
		{
			if(!m_wszName.empty())
				SendLadderInviteConfirm(m_wszName,false);
			Show(false);
		}
		if( IsCmdControl("ID_BUTTON_CLOSE"))	
		{
			if(!m_wszName.empty())
				SendLadderInviteConfirm(m_wszName,false);
			Show(false);
		}
		if( IsCmdControl("ID_OK"))	
		{
			if(!m_wszName.empty())
				SendLadderInviteConfirm(m_wszName,true);
			Show(false);
		}
	}
}

void CDnPVPLadderInviteConfirmDlg::SetInviteInfo(LadderSystem::SC_INVITE_CONFIRM_REQ *pData)
{
	m_wszName.clear();
	m_wszName =  pData->wszCharName;
	m_pStaticName->SetText(FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126243 ), m_wszName.c_str() ));
	m_pStaticPoint->SetText(FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126244 ), pData->iAvgGradePoint ));

	WCHAR wszString[256];

#ifdef PRE_ADD_DWC
	WCHAR wszString2[256];
#endif

	switch(pData->MatchType)
	{
	case LadderSystem::MatchType::_2vs2:
		{
			wsprintf(wszString, L"%s (%d/%d)",GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126027 ),pData->iCurUserCount,pData->MatchType);
			m_pStaticNumber->SetText(wszString);
			break;
		}
	case LadderSystem::MatchType::_3vs3:
		{
			wsprintf(wszString, L"%s (%d/%d)",GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126028 ),pData->iCurUserCount,pData->MatchType);
			m_pStaticNumber->SetText(wszString);
			break;
		}
	case LadderSystem::MatchType::_4vs4:
		{
			wsprintf(wszString, L"%s (%d/%d)",GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126029 ),pData->iCurUserCount,pData->MatchType);
			m_pStaticNumber->SetText(wszString);
			break;
		}
#ifdef PRE_ADD_DWC
	case LadderSystem::MatchType::_3vs3_DWC:
	case LadderSystem::MatchType::_3vs3_DWC_PRACTICE:
		{
			int nMaxCount = DWC::eDWCMember::DWC_MAX_MEMBERISZE;
			std::wstring wszStr = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126028);
			wsprintf(wszString2, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1571), pData->iCurUserCount);
			wsprintf(wszString, _T("%s%s"), wszStr.c_str(), wszString2);
			m_pStaticNumber->SetText(wszString);
			break;
		}
#endif
	default:
		break;
	}
}


void CDnPVPLadderInviteConfirmDlg::SetElapsedTime( float fElapsedTime )
{
	m_fTotalTime = fElapsedTime;
	m_fElapsedTime = fElapsedTime;
}
