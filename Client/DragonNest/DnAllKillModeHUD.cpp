#include "StdAfx.h"
#include "DnAllKillModeHUD.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnAllKillModeHUD::CDnAllKillModeHUD( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnRoundModeHUD( dialogType, pParentDialog, nID, pCallback )
{
	m_pOrderListDlg = NULL;
	m_pAllkilledDlg = NULL;

#ifdef PRE_MOD_PVPOBSERVER
	m_OrderListCnt = 0;
	m_pObserverOrderListDlg = NULL;	
#endif // PRE_MOD_PVPOBSERVER

}

CDnAllKillModeHUD::~CDnAllKillModeHUD(void)
{
	SAFE_DELETE(m_pOrderListDlg);
	SAFE_DELETE(m_pAllkilledDlg);

#ifdef PRE_MOD_PVPOBSERVER
	SAFE_DELETE( m_pObserverOrderListDlg );
#endif // PRE_MOD_PVPOBSERVER

}

void CDnAllKillModeHUD::Initialize( bool bShow )
{
	CDnRoundModeHUD::Initialize( bShow );

	m_pOrderListDlg = new CDnPvPOrderListDlg(UI_TYPE_CHILD,this);
	m_pOrderListDlg->Initialize(false);

#ifdef PRE_MOD_PVPOBSERVER
	m_pObserverOrderListDlg = new CDnPvPObserverOrderListDlg( UI_TYPE_CHILD, this );
	m_pObserverOrderListDlg->Initialize( false );
#endif // PRE_MOD_PVPOBSERVER

	m_pAllkilledDlg = new CDnPvPAllKilledDlg(UI_TYPE_CHILD,this);
	m_pAllkilledDlg->Initialize(false);

}

void CDnAllKillModeHUD::InitialUpdate()
{
	CDnRoundModeHUD::InitialUpdate();
}

void CDnAllKillModeHUD::Process( float fElapsedTime )
{
	CDnRoundModeHUD::Process( fElapsedTime );
}

void CDnAllKillModeHUD::ShowOrderListDlg( bool bShow )
{
#ifdef PRE_MOD_PVPOBSERVER
	if( CDnActor::s_hLocalActor && CDnActor::s_hLocalActor->GetTeam() == PvPCommon::Team::eTeam::Observer )
	{
		if( IsShow() && m_bFirstObserver )
		{		
			if( SetTextObserverTeam( m_pStaticFriendly, m_pStaticEnemy, 7856, 7857 ) ) // "Áú¼­ÀÇ µå·¡°ï", "È¥µ·ÀÇ µå·¡°ï"	
				m_bFirstObserver = false;
		}

		m_pObserverOrderListDlg->StartCountDown();

		if( bShow )
			++m_OrderListCnt;
		else
			m_OrderListCnt = 0;
	}
	else
	{
		m_pOrderListDlg->StartCountDown();
	}
#else
	m_pOrderListDlg->Show( bShow );
#endif // PRE_MOD_PVPOBSERVER

}

void CDnAllKillModeHUD::ShowAllKilledDlg( bool bShow )
{
	m_pAllkilledDlg->Show( bShow );
}

void CDnAllKillModeHUD::SelectOrderListPlayer(int nUserSessionID )
{
#ifdef PRE_MOD_PVPOBSERVER
	if( CDnActor::s_hLocalActor && CDnActor::s_hLocalActor->GetTeam() == PvPCommon::Team::eTeam::Observer )
		m_pObserverOrderListDlg->SelectPlayer( nUserSessionID, m_OrderListCnt );
	else
		m_pOrderListDlg->SelectPlayer( nUserSessionID );
#else
	m_pOrderListDlg->SelectPlayer( nUserSessionID );
#endif // PRE_MOD_PVPOBSERVER

}

void CDnAllKillModeHUD::SelectOrderListCaptain(int nUserSessionID )
{
#ifdef PRE_MOD_PVPOBSERVER
	if( CDnActor::s_hLocalActor && CDnActor::s_hLocalActor->GetTeam() == PvPCommon::Team::eTeam::Observer )
		m_pObserverOrderListDlg->SelectGroupCaptain( nUserSessionID );
	else
		m_pOrderListDlg->SelectGroupCaptain( nUserSessionID );
#else
	m_pOrderListDlg->SelectGroupCaptain( nUserSessionID );
#endif // PRE_MOD_PVPOBSERVER
	
}

void CDnAllKillModeHUD::SetStageByUserCount( int iPrimary_StageNum , int iSecondary_StageNum )
{
	if( iPrimary_StageNum > PvPCommon::s_iMaxRoundNum || iSecondary_StageNum > PvPCommon::s_iMaxRoundNum )
	{		
		ErrorLog("CDnRoundModeHUD::SetStage :: stage num is wrong!");
		return;
	}

	for(int iNum = 0 ; iNum < PvPCommon::s_iMaxRoundNum ; iNum++)
	{
		m_pMyTeamEmptyMark[iNum]->Show( iNum < iPrimary_StageNum ? true : false );
		m_pEnemyTeamEmptyMark[iNum]->Show( iNum < iSecondary_StageNum ? true : false );
	}
}
