#pragma once


#ifdef PRE_ADD_AUTOUNPACK

//------------------------------------------------------
//					CDnAutoUnPack
//
// : ����� ������( �������� �Ǵ� ����������(����� �ŷ¾����۸�) ) n���� �ڵ����� Ǯ���ش�.
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

	INT64 m_ItemSerial; // �ڵ����� ������ serial.

	CDnItem * m_pResigtedItem; // �ڵ����� �� ������.
	int m_ItemClassID;
	int m_invenType;
	bool m_bUnPackIng; // �ڵ�Ǯ���߿���.

	float m_fUnPackTimePerOwn; // ���� ���������ð�.

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

	// �ڵ����� �ߴ�.
	void EngUnpack();

	// �ڵ���������� sn.
	void SetItemSerial( INT64 sn ){
		 m_ItemSerial = sn;
	}

	// �ڵ����� �� �����۵��.
	bool RegisterItem( class CDnItem * pItem );

	// �ڵ��������.
	void UnPacking();

	// �������º��� - �ڵ������� â���� "�����ߴ�" �� �Ѱ�쿡 �ڵ������Ͼ������� �ٽ� ��ϵǵ��� �Ѵ�.
	void PreStateRestore();

	// �����ۿ������ ���.
	void OnRecvRequestCharmItem( char cInvenType, short sInvenIndex, INT64 biInvenSerial );
	
	// UnPacking �غ�.
	void ReadyUnPack();
	
	// KeyItem ���� ����.
	void SetKeyItemCount( CDnItem * pItem );

	// ���翭�������ð� % ����.
	void SetUnPackTime( int nTime );

	// ���� ���������ð�.
	float GetUnPackTime(){
		return m_fUnPackTimePerOwn; 
	}

};


#endif