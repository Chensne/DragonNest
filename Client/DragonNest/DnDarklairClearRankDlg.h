#pragma once

#include "DnCustomDlg.h"
#include "DnBlindDlg.h"

class CDnDarklairClearRankDlg : public CDnCustomDlg, public CDnBlindCallBack
{
public:
	CDnDarklairClearRankDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnDarklairClearRankDlg();

protected:
	CEtUIStatic *m_pStaticPlayerInfo;

	CEtUIStatic *m_pStaticCurrentRound;
	CEtUIStatic *m_pStaticCurrentRank;
	CEtUIStatic *m_pStaticCurrentTime;
	CDnJobIconStatic* m_pStaticCurrentJob[4];
//	CEtUIStatic *m_pStaticCurrentJob[4][10];

	CEtUIStatic *m_pStaticBestRound;
	CEtUIStatic *m_pStaticBestRank;
	CEtUIStatic *m_pStaticBestTime;
	CDnJobIconStatic* m_pStaticBestJob[4];
//	CEtUIStatic *m_pStaticBestJob[4][10];

	CEtUIStatic *m_pStaticHistoryTitle;
	CEtUIStatic *m_pStaticTopHistoryRank[5];
	CEtUIStatic *m_pStaticTopHistoryName[5];
	CEtUIStatic *m_pStaticTopHistoryRound[5];
	CEtUIStatic *m_pStaticTopHistoryTime[5];
	CDnJobIconStatic* m_pStaticTopHistoryJob[5][4];
//	CEtUIStatic *m_pStaticTopHistoryJob[5][4][10];

protected:

public:
	// CDnCustomDLg
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void Render( float fElapsedTime );
	virtual void OnBlindClose();
	virtual void OnBlindClosed();

	void Refresh();
};
