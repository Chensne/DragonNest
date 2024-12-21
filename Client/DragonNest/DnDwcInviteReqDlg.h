#pragma once
#include "EtUIDialog.h"

class CDnDwcInviteReqDlg : public CEtUIDialog
{
public:
	CDnDwcInviteReqDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnDwcInviteReqDlg(void);

protected:
	CEtUIProgressBar *m_pProgressBarTime;

	CEtUIStatic *m_pTitleName;
	CEtUIStatic *m_pCharacterName;
	CEtUIButton *m_pButtonOK;		// 가입 수락
	CEtUIButton *m_pButtonCancel;	// 거절

	float m_fTotalTime;				// 전체시간
	float m_fElapsedTime;			// 남은시간(보통 처음 Show될때는 위와 값을 동일하게 설정)

public:
	void SetInfo( LPCWSTR pwszGuildName, LPCWSTR wszCharacterName, float fTotalTime, int nID = -1, CEtUICallback *pCall = NULL );

	// 직접 프로그래스바를 조절하려면 fElapsedTime값을 새로 넣어주면 된다.
	void SetElapsedTime( float fElapsedTime );

	// Send
	void SendReject();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
};