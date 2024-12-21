#include "StdAfx.h"
#include "DnMissionGainAlarmDlg.h"
#include "DnItemTask.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnMissionGainAlarmDlg::CDnMissionGainAlarmDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback  )
, m_pDescription(NULL)
, m_fElapsedTime(-1.0f)
, m_fConst(1.0f)
, m_fShowTime(0.0f)
, m_fShowRatio(0.8f)
{
}

CDnMissionGainAlarmDlg::~CDnMissionGainAlarmDlg(void)
{
}

void CDnMissionGainAlarmDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MainInfoDlg.ui" ).c_str(), bShow );
	SetFadeMode( CEtUIDialog::CancelRender );
}

void CDnMissionGainAlarmDlg::InitialUpdate()
{
	m_pDescription = GetControl<CEtUIStatic>( "ID_MISSION_INFO_TEXT" );
}

void CDnMissionGainAlarmDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( m_fElapsedTime < 0.0f )
	{
		if( IsShow() ) Show( false );
		return;
	}

	if( m_fShowTime > 0.0f )
	{
		m_fShowTime -= fElapsedTime;
		return;
	}
	else
	{
		Show( false );
	}

	m_fElapsedTime -= fElapsedTime;
}

void CDnMissionGainAlarmDlg::Show( bool bShow )
{ 
	if( bShow )
	{
	}
	else
	{
		m_pDescription->Show( false );
	}

	CEtUIDialog::Show( bShow );
}

void CDnMissionGainAlarmDlg::SetMissionAlarm( CDnMissionTask::MissionInfoStruct *pInfo, float fFadeTime )
{
	m_pDescription->Show( true );
	SetFadeTime( fFadeTime );
}

void CDnMissionGainAlarmDlg::SetFadeTime( float fFadeTime )
{
	m_fShowTime = fFadeTime * m_fShowRatio;
	m_fElapsedTime = fFadeTime - m_fShowTime;
	m_fConst = m_fElapsedTime;
}
