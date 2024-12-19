#pragma once


// 절친등록유저확인 Dlg. //

#ifdef PRE_ADD_BESTFRIEND

#include "EtUIDialog.h"


class CDnIdentifyBestFriendDlg : public CEtUIDialog
{

private:


	CEtUIStatic * m_pStaticBFID;
	CEtUIStatic * m_pStaticLevel;
	CEtUIStatic * m_pStaticJob;


public:

	CDnIdentifyBestFriendDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnIdentifyBestFriendDlg(){
		ReleaseDlg();
	}

	void ReleaseDlg();


	// Override - CEtUIDialog //		
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();	
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );


public:

	void SetSearchBF( struct BestFriend::SCSearch * pData ); // 절친확인정보.

	void RequestBF(); // 절친등록요청.

};



#endif