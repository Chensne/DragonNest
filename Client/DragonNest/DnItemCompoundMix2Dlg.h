#pragma once
#include "DnCustomDlg.h"
#include "DnSmartMoveCursor.h"

class CDnItem;
class CDnItemSlotButton;

class CDnItemCompoundMix2Dlg : public CDnCustomDlg
{
public:
	CDnItemCompoundMix2Dlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnItemCompoundMix2Dlg(void);

	enum
	{
		NUM_NEED_ITEM = 5,
	};

protected:
	CEtUIButton *m_pButtonOK;
	CEtUIButton *m_pButtonCancel;

	CEtUIStatic *m_pProb;
	CEtUIStatic *m_pDescription;
	CEtUIStatic *m_pName;
	CEtUIStatic *m_pStaticSelect;

	// 재료템 슬롯
	CDnItem *m_pItem[NUM_NEED_ITEM];
	CDnItemSlotButton *m_pItemSlotButton[NUM_NEED_ITEM];

	// 슬롯 설정
	void SetItemSlot( int nSlotIndex, CDnItem *pItem, char nOptionIndex = -1 );

	// 스마트무브
	CDnSmartMoveCursor m_SmartMove;

	int m_nSelectSlotIndex;

protected:
	void RefreshSelectItemSlot();

public:
	// 확률 설정
	void SetProb( int nProb );

	// 재료 리스트 설정
	void SetCompoundInfo( std::vector<CDnItem *> &pVecList, char nOptionIndex = -1 );
	void SetDescription( const WCHAR *wszStr );

	// OK 버튼 설정
	void SetOK( bool bOK );

	void EnableButton( bool bEnable );
	INT64 GetSelectItemSerialID();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};