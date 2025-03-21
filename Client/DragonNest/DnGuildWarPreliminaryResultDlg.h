#pragma once

#include "DnCustomDlg.h"
#include "DnBlindDlg.h"
#include "EtUITabDialog.h"
#include "DnGuildWarTask.h"

class CDnGuildWarPreliminaryGuildFestivalResultDlg;
class CDnGuildWarPreliminaryMyResultDlg;
class CDnGuildWarPreliminaryGuildRankDlg;
class CDnGuildWarPreliminaryGuildEventRankingDlg;
class CDnGuildWarPreliminaryPersonalEventRankingDlg;
class CDnGuildWarPreliminarySpecialRankDlg;

class CDnGuildWarPreliminaryResultDlg : public CEtUITabDialog, public CDnBlindCallBack
{
public:
	CDnGuildWarPreliminaryResultDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnGuildWarPreliminaryResultDlg();

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );

public:
	void SetGuildWarPreliminaryResultDlg();

protected:
	CDnGuildWarPreliminaryGuildFestivalResultDlg*	m_pDnGuildWarPreliminaryGuildFestivalResultDlg;
	CDnGuildWarPreliminaryMyResultDlg*				m_pDnGuildWarPreliminaryMyResultDlg;
	CDnGuildWarPreliminaryGuildRankDlg*				m_pDnGuildWarPreliminaryGuildFinalsDlg;
	CDnGuildWarPreliminaryGuildEventRankingDlg*		m_pDnGuildWarPreliminaryGuildRankingDlg;
	CDnGuildWarPreliminaryPersonalEventRankingDlg*	m_pDnGuildWarPreliminaryPersonalRankingDlg;
	CDnGuildWarPreliminarySpecialRankDlg*			m_pDnGuildWarPreliminarySpecialRankDlg;

	CEtUIRadioButton*	m_pButtonGuildFestivalResult;	// 길드 축제 결과
	CEtUIRadioButton*	m_pButtonMyResult;				// 내 결과
	CEtUIRadioButton*	m_pButtonGuildFinals;			// 본선 진출 길드
	CEtUIRadioButton*	m_pButtonGuildRanking;			// 길드 순위
	CEtUIRadioButton*	m_pButtonIndividualRanking;		// 개인 순위
	CEtUIRadioButton*	m_pButtonSpecialRank;			// 특별상
};

class CDnGuildWarPreliminaryGuildFestivalResultDlg : public CDnCustomDlg
{
public:
	CDnGuildWarPreliminaryGuildFestivalResultDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnGuildWarPreliminaryGuildFestivalResultDlg();

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );

public:
	void SetFestivalResult( int nBlueTeamSocre, int nRedTeamScore, int nMySocre = 0, int nMyGuildScore = 0 );

protected:
	CEtUIStatic*	m_pStaticBlueTeamWin;
	CEtUIStatic*	m_pStaticRedTeamWin;
	CEtUIStatic*	m_pStaticBlueTeamScore;
	CEtUIStatic*	m_pStaticRedTeamScore;
	CEtUIStatic*	m_pStaticInterimResult[3];	// 중간결과 (0:청팀우세, 1:홍팀우세, 2:동점)
	CEtUIStatic*	m_pStaticMyScore;			// 내 점수
	CEtUIStatic*	m_pStaticMyGuildScore;		// 내 길드 점수
};

class CDnGuildWarPreliminaryMyResultDlg : public CDnCustomDlg
{
public:
	CDnGuildWarPreliminaryMyResultDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnGuildWarPreliminaryMyResultDlg();

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );

public:
	void SetMyResult();

protected:
	CEtUIStatic*	m_pStaticResultWin;
	CEtUIStatic*	m_pStaticResultLose;
	CEtUIStatic*	m_pStaticWin;
	CEtUIStatic*	m_pStaticLose;
	CEtUIStatic*	m_pStaticPointCount;
	CEtUIStatic*	m_pStaticSpecial;
	CEtUIButton*	m_pButtonReward;
};

class CDnGuildWarPreliminaryGuildRankDlg : public CDnCustomDlg
{
public:
	CDnGuildWarPreliminaryGuildRankDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnGuildWarPreliminaryGuildRankDlg();

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );

protected:
	void SetPageUI();
	void ClearPageUI();
	void SetPagePreliminaryUI();
	void SetPageFinalUI();

protected:
	CEtUIStatic*	m_pStaticResultWin;
	CEtUIStatic*	m_pStaticResultLose;
	CEtUIListBoxEx* m_pGuildRankListBox;
	CEtUIStatic*	m_pStaticMyGuildRanking;
	CEtUIStatic*	m_pStaticMyGuildName;
	CEtUIStatic*	m_pStaticMyGuildMasterName;
	CEtUIStatic*	m_pStaticMyGuildUserCount;
	CEtUIStatic*	m_pStaticMyGuildTotalPoints;
};

class CDnGuildWarPreliminaryGuildRankDlgItem : public CDnCustomDlg
{
public:
	CDnGuildWarPreliminaryGuildRankDlgItem( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildWarPreliminaryGuildRankDlgItem();

public:
	virtual void Initialize( bool bShow );

public:
	void SetPreliminaryGuildRankInfo( CDnGuildWarTask::stJoinGuildRankInfo* pInfo, bool bBlind = false );

};

#define MAX_EVENT_LIST		12

class CDnGuildWarPreliminaryGuildEventRankingDlg : public CDnCustomDlg
{
public:
	CDnGuildWarPreliminaryGuildEventRankingDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnGuildWarPreliminaryGuildEventRankingDlg();

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );

protected:
	void SetPageUI();
	void ClearPageUI();

protected:
	struct stUIGuildWarPreliminaryGuildEventRankInfo
	{
		CEtUIStatic* m_pStaticRank;		// 순위
		CEtUIStatic* m_pStaticName1;	// 길드명
		CEtUIStatic* m_pStaticName2;	// 길드장
		CEtUIStatic* m_pStaticPoints;	// 점수
	};

	stUIGuildWarPreliminaryGuildEventRankInfo	m_UIGuildWarPreliminaryEventRankInfo[ MAX_EVENT_LIST ];		// 종목별 랭크 0~5 Top랭크 6~MAX 내랭크
};

class CDnGuildWarPreliminaryPersonalEventRankingDlg : public CDnCustomDlg
{
public:
	CDnGuildWarPreliminaryPersonalEventRankingDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnGuildWarPreliminaryPersonalEventRankingDlg();

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );

protected:
	void SetPageUI();
	void ClearPageUI();

protected:
	struct stUIGuildWarPreliminaryIndividualEventRankInfo
	{
		CEtUIStatic* m_pStaticRank;		// 순위
		CEtUIStatic* m_pStaticName1;	// 이름
		CEtUIStatic* m_pStaticName2;	// 길드명
		CEtUIStatic* m_pStaticPoints;	// 점수
	};

	stUIGuildWarPreliminaryIndividualEventRankInfo	m_UIGuildWarPreliminaryEventRankInfo[ MAX_EVENT_LIST ];		// 종목별 랭크 0~5 Top랭크 6~MAX 내랭크
};

#define MAX_PRIZE_COUNT			6
#define MAX_DAILY_PRIZE_COUNT	7

class CDnGuildWarPreliminarySpecialRankDlg : public CDnCustomDlg
{
public:
	CDnGuildWarPreliminarySpecialRankDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnGuildWarPreliminarySpecialRankDlg();

protected:
	void SetPageUI();
	void ClearPageUI();

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );

protected:
	CEtUIStatic*	m_pPrize[MAX_PRIZE_COUNT];				// 대상, 토벌상, 포획상, 수확상, 요리상, 특별상
	CEtUIStatic*	m_pDailyPrize[MAX_DAILY_PRIZE_COUNT];	// 1~7일째 최고 점수상
};
