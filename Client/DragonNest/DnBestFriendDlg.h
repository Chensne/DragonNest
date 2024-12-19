#pragma once


// ��ģ����Dlg - Ŀ�´�Ƽ �ǿ� ���� Dlg. //

#ifdef PRE_ADD_BESTFRIEND

#include "EtUIDialog.h"


class CDnBestFriendDlg : public CEtUIDialog, public CEtUICallback
{

private:

	CEtUIStatic * m_pStaticTex0;
	CEtUIStatic * m_pStaticTex1;
	CEtUIStatic * m_pStaticTex2;
	CEtUIStatic * m_pStaticTex3;
	CEtUIStatic * m_pStaticTex4;
	CEtUIStatic * m_pStaticTex5;
	CEtUIStatic * m_pStaticTex6;
	CEtUIStatic * m_pStaticCond;
	CEtUIStatic * m_pStaticTex7;
	CEtUIStatic * m_pStaticTex8;	


	CEtUIStatic * m_pStaticDate; // ��ģ������¥.
	CEtUIStatic * m_pStaticDay;  // ��ģ���� ���κ��� ������¥.
	CEtUIStatic * m_pStaticName; 
	CEtUIStatic * m_pStaticLevel;
	CEtUIStatic * m_pStaticJob; 
	CEtUIStatic * m_pStaticLocation;
	CEtUIStatic * m_pStaticMemo;
	CEtUIStatic * m_pStaticFriendMemo;

	// ��ģ�ı��� ��� Static��.
	CEtUIStatic * m_pStatiBrokeupTime;
	CEtUIStatic * m_pStaticBrokeupNotice;

	CEtUIButton * m_pBtnBrokeup;       // ��ģ�ı�.
	CEtUIButton * m_pBtnBrokeupCancel; // ��ģ�ı����.
	CEtUIButton * m_pBtnBrokeupAgreement; // ��ģ�ı⵿��.

	CEtUIButton * m_pBtnParty; // ��Ƽ��û.
	CEtUIButton * m_pBtnMemo; // �޸��Է�. 
	
	// ��ģDBID.
	INT64 m_biCharacterDBID;

	std::vector< CEtUIStatic * > m_vStaticEnable; // ��ģ�ı�ÿ� Enable��ų Static��. 


	float m_timeOneMin; // 1�� 
	float m_CountDown; // ��ģ�ıⳲ���ð�.
	wchar_t * m_pStrCountDown;


	bool m_bBFState; // ��ģ(true), �ı�(false)
	bool m_bBFBrokeUpCancel; // �ı����.


	bool m_bAllowRequestBFInfo; // ��ģ������û ��뿩��.
	float m_TimeAllowBFInfo;

	
public:

	CDnBestFriendDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnBestFriendDlg(){
		ReleaseDlg();
	}
	
	void ReleaseDlg();


	// Override - CEtUIDialog //		
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();	
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );
	virtual void Show( bool bShow );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );

private:

	// ��ģ���¿����� ��Ʈ�����ó��.
	void SetCtrlBFInfo( bool bShow );
	void SetCtrlDestroyedBFInfo( bool bShow );

	
	void EnableCtrByGameTask();

	void RequestBFInfo(); // ��ģ������û.

public:

	void SetBFData( struct TBestFriendInfo & bfInfo ); // ��ģ����.

	void SetMemo( bool bFromMe, WCHAR * pStrMemo ); // �޸����.

	void InvitePartyBF(); // ��Ƽ�ʴ�.

	void BrokeupOrCancelBF( bool bCancel, WCHAR * strName ); // ��ģ�ı� or �ı����.
};


#endif