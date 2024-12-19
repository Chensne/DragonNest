#pragma once


// ��ģ���� Progress dlg. //

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

	void Accept();  // ����.
	void Refusal(); // ����.

public:

	// bOpen(false) - ĳ�ü� or ����ε� �����߿� ��ģ��û�� �°�� ����Ѵ�.
	void SetData( struct BestFriend::SCRegistReq * pData, bool bOpen=true );

};

#endif