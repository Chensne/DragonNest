#pragma once
#include "DnCustomDlg.h"
#include "DnItemTask.h"

class CDnItem;

class CDnPlateItemSlotDlg : public CDnCustomDlg
{
public:
	CDnPlateItemSlotDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPlateItemSlotDlg(void);

protected:

	int m_nMaxItemSlot;

	CDnItem *m_pItem[5];
	CDnItemSlotButton *m_pItemSlotButton[5];
	CEtUIStatic *m_pItemSlotEffect[5];

public:
	// 별도의 Init함수 가진다. 인자는 1~5사이값. 생성 후 이 함수로 한번만 초기화 할 것.
	void Init( int nMaxItemSlot );

	// 아이템 종류와 갯수 조건이 맞으면 true리턴.
	bool SetCompoundInfo(int *pItemID, int *pItemCount);

	// 개별 슬롯 설정
	void SetItemSlot( int nSlotIndex, int nItemID, int nNeedItemCount, int nCurItemCount );
public:
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
};