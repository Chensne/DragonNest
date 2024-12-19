#pragma once

#ifdef PRE_ADD_AUTOUNPACK

#include "DnCustomDlg.h"


class CDnAutoUnPackResultDlg : public CDnCustomDlg
{

private:

	CDnItemSlotButton * m_pItemSlotBtn;
	CEtUIProgressBar * m_pResultProgress; 	
	CEtUIStatic * m_pStaticItemName;
	
	float m_fOrgProgressTime;
	float m_fProgressTime;

	bool m_bUnpacking;
	class CDnAutoUnPackDlg * m_pAutoUnPackDlg; // 자동열기창.
	CDnItem *m_pResultItem;

public:

	CDnAutoUnPackResultDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnAutoUnPackResultDlg();

	// Override - CEtUIDialog //
	virtual void Show( bool bShow );
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );


	// 결과 아이템 출력.
	void SetResultItem( int nItemID, int nCount, int nPeriod = 0, INT64 nCoin = 0 );
};

#endif