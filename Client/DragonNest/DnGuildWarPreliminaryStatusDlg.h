#pragma once

#include "DnCustomDlg.h"
#include "DnBlindDlg.h"

class CDnGuildWarPreliminaryStatusDlg : public CDnCustomDlg, public CDnBlindCallBack
{
public:
	CDnGuildWarPreliminaryStatusDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnGuildWarPreliminaryStatusDlg();

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );

public:
	void SetPresentCondition( int nBlueTeamSocre, int nRedTeamScore, int nMySocre, int nMyGuildScore );

protected:
	CEtUIStatic*	m_pStaticInterimResult[3];	// Áß°£°á°ú (0:Ã»ÆÀ¿ì¼¼, 1:È«ÆÀ¿ì¼¼, 2:µ¿Á¡)
	CEtUIStatic*	m_pStaticBlueTeamScore;		// Ã»ÆÀ Á¡¼ö
	CEtUIStatic*	m_pStaticRedTeamScore;		// È«ÆÀ Á¡¼ö
	CEtUIStatic*	m_pStaticMyScore;			// ³» Á¡¼ö
	CEtUIStatic*	m_pStaticMyGuildScore;		// ³» ±æµå Á¡¼ö
};
