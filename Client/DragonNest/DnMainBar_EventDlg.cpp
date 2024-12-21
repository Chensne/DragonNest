#include "StdAfx.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnMainDlg.h"
#include "DnTimeEventTask.h"
#include "DnSystemDlg.h"
#include "DnFadeInOutDlg.h"
#include "DnMainBar_EventDlg.h"
#include "DnMinimap.h"
#include "GameOption.h"
#include "DnVillageTask.h"
#include "TaskManager.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnMainBar_EventDlg::CDnMainBar_EventDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CEtUIDialog(dialogType, pParentDialog, nID, pCallback)
{
#if defined( PRE_ALTEIAWORLD_EXPLORE )
	m_pAlteaDiceIcon = NULL;
	m_bAlteaDiceShow = false;
	m_bAlteaDiceAlarm = false;
	m_fAlteaDiceAlarmTime = 0.0f;
#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )

}

CDnMainBar_EventDlg::~CDnMainBar_EventDlg(void)
{

}



void CDnMainBar_EventDlg::InitialUpdate()
{
#if defined( PRE_ALTEIAWORLD_EXPLORE )
	m_pAlteaDiceIcon = GetControl<CEtUIButton>("ID_BT_ALTEA");
	m_pAlteaDiceIcon->SetTooltipText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7973));
#endif	
	//Ascunde butoanele
	 GetControl<CEtUIButton>( "ID_BT_FISHING" )->Show(false);
	 GetControl<CEtUIButton>( "ID_BT_DRAGON" )->Show(false);
	 GetControl<CEtUIButton>( "ID_BT_SECRET" )->Show(false);

	 //To be used later:
	 GetControl<CEtUIButton>("ID_BT_CHALLENGE")->Show(false);
	 GetControl<CEtUIButton>("ID_BT_DNGIRL")->Show(false);
	 GetControl<CEtUIButton>("ID_BT_MINI")->Show(false);
	 GetControl<CEtUIButton>("ID_BT_SEAL")->Show(false);
	 GetControl<CEtUIButton>("ID_STATIC_MINI")->Show(false);
	 GetControl<CEtUIButton>("ID_STATIC_SEAL")->Show(false);


}

void CDnMainBar_EventDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("mainbar_eventsidedlg.ui").c_str(), bShow);
}

void CDnMainBar_EventDlg::Show(bool bShow)
{
	CEtUIDialog::Show(bShow);
}

void CDnMainBar_EventDlg::Render( float fElapsedTime )
{
		CEtUIDialog::Render( fElapsedTime );
}

void CDnMainBar_EventDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( !GetInterface().IsLockMainMenu() ) {
		if( nCommand == EVENT_BUTTON_CLICKED )
		{
			if( GetInterface().GetFadeDlg() && (GetInterface().GetFadeDlg()->GetFadeMode() != CDnFadeInOutDlg::modeEnd) )
				return;
			
			CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();


#if defined( PRE_ALTEIAWORLD_EXPLORE )
			if( IsCmdControl("ID_BT_ALTEA") )
			{
				if( CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeVillage )
					return;
				pMainMenuDlg->ToggleShowDialog( CDnMainMenuDlg::ALTEA_TAB_DIALOG );
			}
#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )
	

		}
	}

}

#if defined( PRE_ALTEIAWORLD_EXPLORE )
void CDnMainBar_EventDlg::SetAlteaDiceIcon()
{
	m_bAlteaDiceShow = false;

	CDnVillageTask* pVillageTask = (CDnVillageTask*)(CTaskManager::GetInstance().GetTask("VillageTask"));
	if( NULL == pVillageTask || CVillageClientSession::WorldVillage != pVillageTask->GetVillageType())
		return;

	DNTableFileFormat * pSox = GetDNTable( CDnTableDB::TALTEIATIME );
	if( NULL == pSox )
		return;

	CTimeSet tCurTime;
	for( int itr = 0; itr < pSox->GetItemCount(); ++itr )
	{
		int nItemID = pSox->GetItemID( itr );

		char *szPeriod = pSox->GetFieldFromLablePtr( nItemID, "_EventPeriod" )->GetString();
		std::vector<string> vEventPeriod;
		TokenizeA( szPeriod, vEventPeriod, ":" );
		if( vEventPeriod.size() != 2 ) continue;

		CTimeSet tBeginSet( vEventPeriod[0].c_str(), true );
		CTimeSet tEndSet( vEventPeriod[1].c_str(), true );

		if( tCurTime.GetTimeT64_LC() < tEndSet.GetTimeT64_LC()
			&& tCurTime.GetTimeT64_LC() > tBeginSet.GetTimeT64_LC() )
		{
			m_bAlteaDiceShow = true;
			break;
		}
	}

	if( false == m_bAlteaDiceShow )
		return;

	m_bAlteaDiceShow = false;

	if( !CDnActor::s_hLocalActor )
		return;

	const int nLevel = CDnActor::s_hLocalActor->GetLevel();
	const int nLimitLevel = (int)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::AlteiaWorldEnterLimitLevel );

	if( nLimitLevel <= nLevel )
		m_bAlteaDiceShow = true;
}

void CDnMainBar_EventDlg::SetAlteaDiceAlarm()
{
	m_bAlteaDiceAlarm = true;
	m_fAlteaDiceAlarmTime = 0.0f;
}

void CDnMainBar_EventDlg::UpdateAlteaDice( float fElapsedTime )
{
	if( true == m_bAlteaDiceShow )
	{
		if( m_bAlteaDiceAlarm )
		{
			if( 3.0f <= m_fAlteaDiceAlarmTime )
			{
				m_bAlteaDiceAlarm = false;
				m_pAlteaDiceIcon->Show( false );
			}
			else
			{
				static int nTimeBack = 0;
				int nBlinkTime = int(m_fAlteaDiceAlarmTime * 10.0f);

				if( ((nBlinkTime%3) == 0) && (nTimeBack != nBlinkTime) )
				{
					nTimeBack = nBlinkTime;
					m_pAlteaDiceIcon->Show( !m_pAlteaDiceIcon->IsShow() );
				}
				m_fAlteaDiceAlarmTime += fElapsedTime;
			}
		}
		else
			m_pAlteaDiceIcon->Show( true );
	}
	else
		m_pAlteaDiceIcon->Show( false );
}
#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )


