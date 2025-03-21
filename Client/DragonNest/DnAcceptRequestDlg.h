#pragma once
#include "EtUIDialog.h"

class CEtUIStatic;
class CEtUIProgressBar;

// 수락-거절창과 요청취소창은 한번에 하나만 떠 있을 수 있으므로 이렇게 하나의 다이얼로그를 사용하게 되었다.
class CDnAcceptRequestDlg : public CEtUIDialog
{
public:
	enum eAcceptRequestType
	{
		eType01,
		eType02,
		eTypeMax
	};
	CDnAcceptRequestDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnAcceptRequestDlg(void);

protected:
	CEtUIHtmlTextBox* m_pMainText[eTypeMax];
	CEtUIProgressBar *m_pProgressBarTime;

	CEtUIButton *m_pButtonOK;		// 수락
	CEtUIButton *m_pButtonReject;	// 거절
	CEtUIButton *m_pButtonCancel;	// (요청)취소

	float m_fTotalTime;				// 전체시간
	float m_fElapsedTime;			// 남은시간(보통 처음 Show될때는 위와 값을 동일하게 설정)

	bool m_bAccept;

public:
	// bAccept가 true면 수락-거절창으로, false면 요청취소 창으로 뜬다.
	void SetInfo( LPCWSTR pwszMessage, float fTotalTime, bool bAccept, eAcceptRequestType type, int nID = -1, CEtUICallback *pCall = NULL );

	// 직접 프로그래스바를 조절하려면 fElapsedTime값을 새로 넣어주면 된다.
	void SetElapsedTime( float fElapsedTime );

	//bool IsEnd();
	void OnTimeIsUp();

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
};
