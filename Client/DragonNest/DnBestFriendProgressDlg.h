#pragma once


// 절친수락 Progress dlg. //

#ifdef PRE_ADD_BESTFRIEND

#include "EtUIDialog.h"

class CDnBestFriendProgressDlg : public CEtUIDialog
{
public:
	CDnBestFriendProgressDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnBestFriendProgressDlg(void);

protected:
	CEtUIStatic * m_pText;
	CEtUIButton *m_pCancelButton;
	CEtUIProgressBar *m_pProgressBarTime;

	float m_fTimer;	

	struct BestFriend::SCRegistReq * m_pReqData;
	
public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );


private:

	void Accept();  // 수락.
	void Refusal(); // 거절.

public:

	// bOpen(false) - 캐시샵 or 블라인드 상태중에 절친요청이 온경우 취소한다.
	void SetData( struct BestFriend::SCRegistReq * pData, bool bOpen=true );

};

#endif