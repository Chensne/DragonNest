#pragma once
#include "DnCustomDlg.h"


#define FISHING_POWER_TIME	100.0f

struct stFishingInfo
{
	float m_fStrengthTime;		// 체력 게이지 완료 시간
	float m_fPowerSpeedTime;	// 힘 게이지 속도
	float m_fPowerTime;			// 힘 게이지 완료 시간
	float m_fSpeedUpStartTime;	// 힘 조절로 속도가 빨라지는 구간의 시작 시간
	float m_fSpeedUpEndTime;	// 힘 조절로 속도가 빨라지는 구간의 끝 시간
	float m_fSpeedUpFactor;		// 힘 조절때 빨라지는 속도의 인자값

	stFishingInfo() : m_fStrengthTime( 0.0f ), m_fPowerSpeedTime( 0.0f ), m_fPowerTime( 0.0f ), 
					m_fSpeedUpStartTime( 0.0f ), m_fSpeedUpEndTime( 0.0f ), m_fSpeedUpFactor( 0.0f ) {}
};

class CDnLifeSkillFishingDlg : public CDnCustomDlg, public CEtUICallback
{
public:
	CDnLifeSkillFishingDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnLifeSkillFishingDlg();

protected:
	CEtUIStatic* m_pStaticState;
	CEtUIStatic* m_pControlBar;
	CEtUIProgressBar* m_pProgressStrength;
	CDnFishingProgressBar* m_pProgressPower;
	CEtUIButton* m_pButtonControlGuide;
	CEtUIButton* m_pButtonAutoFishing;
	CEtUIButton* m_pButtonCancel;

	// Guid UI
	CEtUIStatic* m_pGuideBoard;
	CEtUIStatic* m_pGuideMouse;
	CEtUIStatic* m_pGuideMouseLeftButton;
	CEtUIStatic* m_pGuideMouseRightButton;
	CEtUIStatic* m_pGuideLeftChatBalloon;
	CEtUIStatic* m_pGuideLeftChatBalloonTail;
	CEtUIStatic* m_pGuideRightChatBalloon;
	CEtUIStatic* m_pGuideRightChatBalloonTail;
	CEtUIButton* m_pButtonCloseGuide;

	SUICoord m_ProgressStrengthUiCoord;

	bool m_bAutoFishing;			// 자동낚시 플래그
	bool m_bShowGuide;				// 가이드 창 플래그
	bool m_bSpeedUp;				// 마우스 왼쪽 버튼 눌렀을 경우 속도 증가 플래그
	bool m_bDoFishingEnd;			// 낚기 시작 시 플래그
	bool m_bMouseLButtonDown;		// 마우스 왼쪽 버튼 눌름 플래그
	bool m_bRecieveResult;

	stFishingInfo m_FishingInfo;
	float m_fElapsedStrengthTime;	// 체력 소모 시간
	float m_fElapsedStrengthTemp;	// 속도증가 적용 안된 임시 체력 소모 시간
	float m_fElapsedPowerTime;		// 힘 조절 시간

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

public:
	void SetShowGuide( bool bShow );
	void SetFishingInfo( stFishingInfo& finshinginfo );
	bool IsAutoFishing() { return m_bAutoFishing; }
	void FishingTimeSync( int nReduceTick, int nFishingGauge );
	void RecieveResult( bool bRecieveResult );

private:
	void ShowGuide( bool bShow );
	bool IsValidFishingState();
	void ResetFishingInfo();
	void ToggleLeftChatBalloon( bool bShow );
	void ToggleRightChatBalloon( bool bShow );
};

