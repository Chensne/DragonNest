#pragma once


#ifdef PRE_ADD_AUTOUNPACK

//------------------------------------------------------
//					CDnAutoUnPack
//
// : 등록한 아이템( 보물상자 또는 랜덤아이템(현재는 매력아이템만) ) n개를 자동으로 풀어준다.
//
#include "DnCustomDlg.h"

#ifdef PRE_ADD_INSTANT_CASH_BUY
class CDnAutoUnPackDlg : public CDnCustomDlg, public CEtUICallback
#else // PRE_ADD_INSTANT_CASH_BUY
class CDnAutoUnPackDlg : public CDnCustomDlg
#endif // PRE_ADD_INSTANT_CASH_BUY
{

private:

	CDnItemSlotButton * m_pCtrItemSlot;
	CEtUIStatic * m_pStaticKeyCount;

	class CDnInvenTabDlg * m_pInvenTabDlg;

	INT64 m_ItemSerial; // 자동열기 아이템 serial.

	CDnItem * m_pResigtedItem; // 자동열기 할 아이템.
	int m_ItemClassID;
	int m_invenType;
	bool m_bUnPackIng; // 자동풀기중여부.

	float m_fUnPackTimePerOwn; // 개당 열기지연시간.

protected:


public:
	CDnAutoUnPackDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnAutoUnPackDlg(){
		ReleaseDlg();
	}
	
	void ReleaseDlg();

	// Override - CEtUIDialog //
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();		
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );
#ifdef PRE_ADD_INSTANT_CASH_BUY
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
#endif // PRE_ADD_INSTANT_CASH_BUY

public:

	// 자동열기 중단.
	void EngUnpack();

	// 자동열기아이템 sn.
	void SetItemSerial( INT64 sn ){
		 m_ItemSerial = sn;
	}

	// 자동열기 할 아이템등록.
	bool RegisterItem( class CDnItem * pItem );

	// 자동열기시작.
	void UnPacking();

	// 이전상태복원 - 자동열기결과 창에서 "열기중단" 을 한경우에 자동열기등록아이템이 다시 등록되도록 한다.
	void PreStateRestore();

	// 아이템열기시작 결과.
	void OnRecvRequestCharmItem( char cInvenType, short sInvenIndex, INT64 biInvenSerial );
	
	// UnPacking 준비.
	void ReadyUnPack();
	
	// KeyItem 개수 설정.
	void SetKeyItemCount( CDnItem * pItem );

	// 개당열기지연시간 % 설정.
	void SetUnPackTime( int nTime );

	// 개당 열기지연시간.
	float GetUnPackTime(){
		return m_fUnPackTimePerOwn; 
	}

};


#endif