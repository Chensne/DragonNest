#include "StdAfx.h"
#include "DnRespawnModeHUD.h"
#include "DnPVPGameResultAllDlg.h" // ½ºÄÚ¾î¿¡¼­ BestUser ¾ÆÀÌµð¸¦ ¾ò¾î¿É´Ï´Ù.
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnRespawnModeHUD::CDnRespawnModeHUD( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnPVPBaseHUD( dialogType, pParentDialog, nID, pCallback )
#ifdef PRE_MOD_PVPOBSERVER
, m_bFirstTeam( false )
#endif // #ifdef PRE_MOD_PVPOBSERVER
{
}

CDnRespawnModeHUD::~CDnRespawnModeHUD(void)
{
}

void CDnRespawnModeHUD::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PvpRespawnInGame.ui" ).c_str(), bShow );
#ifdef PRE_MOD_PVPOBSERVER
	m_bFirstTeam = true;
#endif // #ifdef PRE_MOD_PVPOBSERVER
}


void CDnRespawnModeHUD::InitialUpdate()
{
	CDnPVPBaseHUD::InitialUpdate();

	m_pRoomName = GetControl<CEtUIStatic>("ID_PVP_RESPAWN_ROOMNAME");
	m_pMYTeam = GetControl<CEtUIStatic>("ID_PVP_RESPAWN_MYTEAM");
	m_pEnemyTeam = GetControl<CEtUIStatic>("ID_PVP_RESPAWN_ENEMY");
}


#ifdef PRE_MOD_PVPOBSERVER
void CDnRespawnModeHUD::GameStart()
{
	if( IsShow() )
		SetTextObserverTeam( m_pMYTeam, m_pEnemyTeam, 7856, 7857 ); // "Áú¼­ÀÇ µå·¡°ï", "È¥µ·ÀÇ µå·¡°ï"	
}

void CDnRespawnModeHUD::Process( float fElapsedTime )
{
	if( IsShow() && m_bFirstTeam )
	{		
		if( SetTextObserverTeam( m_pMYTeam, m_pEnemyTeam, 7856, 7857 ) ) // "Áú¼­ÀÇ µå·¡°ï", "È¥µ·ÀÇ µå·¡°ï"	
			m_bFirstTeam = false;
	}

	CDnPVPBaseHUD::Process( fElapsedTime );
}
#endif // PRE_MOD_PVPOBSERVER


