#pragma once

#include "DnCustomDlg.h"
#include "DnTimeEventTask.h"

class CDnItem;
class CDnTimeEventDlg;

class CDnTimeEventListItem : public CDnCustomDlg
{
public:
	CDnTimeEventListItem( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnTimeEventListItem();

protected:
	int m_nEventID;
	CEtUIStatic *m_pStaticTitle[2];
	CEtUIStatic *m_pStaticRemainTime[2];
	CEtUIStatic *m_pStaticPeriod;
	CEtUIStatic *m_pStaticIcon[3];
	CEtUIStatic *m_pStaticStatus;
	CDnItemSlotButton *m_pItemSlotReward[3];
	CEtUIStatic *m_pStaticItemBoard[3];
//	CDnItem *m_pRewardItem[3];
	CEtUIStatic *m_pStaticCoin;
	CEtUICheckBox *m_pCheckBox;
	CEtUIStatic *m_pStaticDailyOnly;

	CDnTimeEventDlg *m_pTimeEventDlg;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );

	void SetInfo( CDnTimeEventDlg *pTimeEventDlg, CDnTimeEventTask::TimeEventInfoStruct *pStruct );
	void UpdateRemainTime( INT64 nRemainTime, bool bDailyOnly );
	void SetCheckBox( bool bCheck );

	int GetEventID() { return m_nEventID; }
};