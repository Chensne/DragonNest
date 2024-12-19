#pragma once

#if defined(PRE_ADD_TALISMAN_SYSTEM) 
#include "DnCustomDlg.h"
#include "DnCharStatusDlg.h"

class CDnTooltipTalismanDlg;
class CDnCharTalismanInfoDlg;
class CDnCharTalismanListDlg;
class CDnCharTalismanDlg : public CDnCustomDlg, public CEtUICallback
{
public:
	CDnCharTalismanDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnCharTalismanDlg();

protected:
	enum {
		E_MESSAGE_START_EDITMODE
	};
	// child class
	CDnCharTalismanInfoDlg*	m_pDetailInfoDlg;// 상세정보Dlg
	CDnCharTalismanListDlg* m_pSlotDlg;		 // 슬롯Dlg

	// ui controls
	CEtUIButton*	m_pChangeApplyBtn;  // 편집모드 버튼
	CEtUIButton*	m_pDetailInfoBtn;	// 상세정보 버튼
	CEtUIButton*	m_pCombineBtn;		// 합성 버튼
	CEtUIListBoxEx* m_pListBoxEx;
	CEtUIScrollBar* m_pScrollBar;
	CEtUIStatic*    m_pInfomationStaticText;

	// variable
	float	m_fOriginalPosX;
	float	m_fOriginalPosY;
	int		m_nScrollPos;
	int		m_nSlotMaxCount; // 테이블에서 읽어오는 슬롯 최대값.
	bool	m_bIsEditMode;	 // Edit Mode인지 체크
	bool    m_bIsEquipStart; // 장착중인지 체크
	int		m_nUseMoneySoundIndex;
	ITEM_SLOT_TYPE m_nCurrentSlotType;
	int		m_nSlotChangeCount;

protected:
	void Reset();
	void SetSlotOffset();
	
	
public:		
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void Render( float fElapsedTime );
	virtual void Process( float fElapsedTime );

	void  SetTalismanItem(int nEquipIndex, MIInventoryItem *pItem);	// used to set ItemSlot UI
	void  RemoveTalismanItem(int nEquipIndex);						// used to set ItemSlot UI

	bool RequestEquipTalismanItem( int nSlotIndex, MIInventoryItem* pInvenItem );
	float GetTalismanSlotRatio(int nSlotIndex); // 슬롯별 효율 불러오기.

	// 슬롯 오픈 요청
	void  SetOpenTalismanSlot(int nSlotOpenFlag);

	const bool IsEditMode()  { return m_bIsEditMode; }
	void SetEditMode(bool b) { m_bIsEditMode = b; }
	
	const bool IsOnlyEquipMode() { return m_bIsEquipStart; }
	
	void ShowTalismanSlotEfficiency(bool bShow);
	void RefreshDetailInfoDlg();
	void PlayTalismanMoveSound();

	void ReleaseTalismanClickFlag();
	CDnCharStatusDlg::eRetWearable GetWearableTalismanEquipType(CDnItem* pItem, std::wstring *strErrorMsg = NULL);

#ifdef PRE_ADD_EQUIPLOCK
	int GetWearedSlotIndex(const CDnItem* pItem) const;
#else
	int GetWearedSlotIndex(CDnItem* pItem);
#endif
	CDnItem* GetEquipTalisman(int nIndex);

	void SetWeableSlotBlank();
	CDnCharStatusDlg::eRetWearable ValidWearableTalismanItem( const MIInventoryItem *pItem, std::wstring *strErrorMsg );
	bool CheckEmptySlot(std::wstring *strErrorMsg);
	bool CheckLevel(CDnItem* pItem, std::wstring *strErrorMsg);
	bool CheckSameItemEquipped(CDnItem* pItem, std::wstring *strErrorMsg);	

	CEtUIListBoxEx* GetListBoxEX() { return m_pListBoxEx; }
	CEtUIScrollBar* GetScrollBar() { return m_pScrollBar; }

	void SetTalismanChangeText();
};

#endif // PRE_ADD_TALISMAN_SYSTEM