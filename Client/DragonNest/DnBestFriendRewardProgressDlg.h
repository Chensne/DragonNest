#pragma once


// 보상아이템선택 Progress dlg. //

#ifdef PRE_ADD_BESTFRIEND

#include "EtUIDialog.h"

class CDnBestFriendRewardProgressDlg : public CEtUIDialog
{
public:
	CDnBestFriendRewardProgressDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnBestFriendRewardProgressDlg(void);

protected:
	
	CEtUIProgressBar *m_pProgressBarTime;

	float m_fTimer;	

	INT64 m_GiftSerial;
	int m_ItemID;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );


private:
	
	void Accept(); // 수락.
	void Cancel(); // 취소.

public:
	
	void SetData( INT64 GiftSerial, int ItemID ){
		m_GiftSerial = GiftSerial;  m_ItemID = ItemID;
	}
};

#endif