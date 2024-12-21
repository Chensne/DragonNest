#pragma once


// 절친정보Dlg - 커뮤니티 탭에 들어가는 Dlg. //

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


	CEtUIStatic * m_pStaticDate; // 절친맺은날짜.
	CEtUIStatic * m_pStaticDay;  // 절친맺은 날로부터 지난날짜.
	CEtUIStatic * m_pStaticName; 
	CEtUIStatic * m_pStaticLevel;
	CEtUIStatic * m_pStaticJob; 
	CEtUIStatic * m_pStaticLocation;
	CEtUIStatic * m_pStaticMemo;
	CEtUIStatic * m_pStaticFriendMemo;

	// 절친파괴시 출력 Static들.
	CEtUIStatic * m_pStatiBrokeupTime;
	CEtUIStatic * m_pStaticBrokeupNotice;

	CEtUIButton * m_pBtnBrokeup;       // 절친파기.
	CEtUIButton * m_pBtnBrokeupCancel; // 절친파기취소.
	CEtUIButton * m_pBtnBrokeupAgreement; // 절친파기동의.

	CEtUIButton * m_pBtnParty; // 파티신청.
	CEtUIButton * m_pBtnMemo; // 메모입력. 
	
	// 절친DBID.
	INT64 m_biCharacterDBID;

	std::vector< CEtUIStatic * > m_vStaticEnable; // 절친파기시에 Enable시킬 Static들. 


	float m_timeOneMin; // 1분 
	float m_CountDown; // 절친파기남은시간.
	wchar_t * m_pStrCountDown;


	bool m_bBFState; // 절친(true), 파기(false)
	bool m_bBFBrokeUpCancel; // 파기취소.


	bool m_bAllowRequestBFInfo; // 절친정보요청 허용여부.
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

	// 절친상태에따른 컨트롤출력처리.
	void SetCtrlBFInfo( bool bShow );
	void SetCtrlDestroyedBFInfo( bool bShow );

	
	void EnableCtrByGameTask();

	void RequestBFInfo(); // 절친정보요청.

public:

	void SetBFData( struct TBestFriendInfo & bfInfo ); // 절친정보.

	void SetMemo( bool bFromMe, WCHAR * pStrMemo ); // 메모수신.

	void InvitePartyBF(); // 파티초대.

	void BrokeupOrCancelBF( bool bCancel, WCHAR * strName ); // 절친파기 or 파기취소.
};


#endif