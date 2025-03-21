#pragma once
#include "EtUIDialog.h"
#include "DnRoundModeHUD.h"
#include "DnPvPOrderListDlg.h"
#include "DnPVPAllKilledDlg.h"

#ifdef PRE_MOD_PVPOBSERVER
#include "DnPvPObserverOrderListDlg.h"
#endif // PRE_MOD_PVPOBSERVER

class CDnAllKillModeHUD : public CDnRoundModeHUD
{
protected:
	CDnPvPOrderListDlg *m_pOrderListDlg;
	CDnPvPAllKilledDlg *m_pAllkilledDlg;

#ifdef PRE_MOD_PVPOBSERVER
	CDnPvPObserverOrderListDlg * m_pObserverOrderListDlg;
	int m_OrderListCnt;
#endif // PRE_MOD_PVPOBSERVER

public:
	CDnAllKillModeHUD( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnAllKillModeHUD(void);
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Process( float fElapsedTime );

public:

	void SetStageByUserCount( int iPrimary_StageNum , int iSecondary_StageNum );
	void ShowOrderListDlg( bool bShow );
	void ShowAllKilledDlg( bool bShow );
	void SelectOrderListPlayer(int nUserSessionID );
	void SelectOrderListCaptain( int nUserSessionID );

#ifdef PRE_MOD_PVPOBSERVER
	CDnPvPObserverOrderListDlg *GetObserverOrderListDlg(){ return m_pObserverOrderListDlg; }
#endif

};