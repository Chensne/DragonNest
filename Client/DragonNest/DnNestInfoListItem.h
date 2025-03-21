#pragma once

#include "DnCustomDlg.h"
#include "DnMissionTask.h"

class CDnItem;
class CDnMissionAppellationTooltipDlg;
class CDnMissionRewardCoinTooltipDlg;
class CDnMissionDailyQuestDlg;

class CDnNestInfoListItem : public CDnCustomDlg
{
public:
	CDnNestInfoListItem( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnNestInfoListItem(void);

protected:
	CEtUIStatic *m_pNestName;			//네스트 이름
	CEtUIStatic *m_pNestClearInfo;		//네스트 클리어 여부
	CEtUIStatic *m_pNestEventIcon;		//네스트 이벤트 여부 아이콘
	CEtUIStatic *m_pTooltipControl;		//툴팁 표시용으로 더미 추가됨
	
	TCustomEventInfo m_NestInfo;		//네스트 정보

public:
	void SetInfo( TCustomEventInfo &pInfo, int nActivate, int uiStringID );
	

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );

	virtual void SetElementDialogShowState( bool bShow );
};
