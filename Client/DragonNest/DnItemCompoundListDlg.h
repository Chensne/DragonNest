#pragma once
#ifdef PRE_MOD_ITEM_COMPOUND_DIALOG_RENEWAL
#include "DnCustomDlg.h"
#include "DnItem.h"
#include "DnItemTask.h"
#include "DnCompoundBase.h"


class CDnItemCompoundListDlg : public CDnCustomDlg
{

public:
	CDnItemCompoundListDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnItemCompoundListDlg();

	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	void SetInfo( const int& nCompoundID, const int& nItemID, /*CDnCompoundBase::ItemGroupStruct* const pCompoundGroupData,*/ CEtUIDialog* pParent );
	void Clear();
	void ForceBlend();
	void Enable( bool bValue );
	void Reset();

	static INT64 GetDiscountedCost( INT64 iCost );
	static bool IsNeedItemList( int nCompoundID );

	void EnableNotify( bool bEnable );
	void UpdateSelectOption();
	void RefreshCompoundItem();
	void SetNeedItemSlot( int nSlotIndex, int nItemID, int nNeedItemCount, int nCurItemCount, char nOptionIndex = -1 );
	void OnRecvItemCompound();
#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
	void SetForceOpenMode( bool bShow );
	void ResetForceOpenMode();
#endif 
	void SetNotifyCheckBox( bool bCheck );

	bool IsInsideItem( float fX, float fY );
	bool IsNotifyCheck();

	CDnItem* GetItem() {  return m_pItem; }	
	int		GetCompoundID()  { return m_nCompoundID; }

	CDnItemTask::CompoundItemPackage GetCompoundItemPackage() { return m_CompoundItemPackage; }
	bool IsEnableCreateButton()	{ return m_pCreateButton->IsEnable(); }
	
	static int m_nRemoteItemID;

protected:
	
	CEtUIStatic* m_pStaticBase;
	CDnItemSlotButton*	m_pItemSlotButton;
	CDnItem*	m_pItem;
	CEtUIStatic* m_pStaticName;
	CEtUIStatic* m_pStaticLevel;
	CEtUIStatic* m_pStaticTypeText;
	CEtUICheckBox* m_pCheckBoxCompoundCheck;
	CEtUIStatic* m_pTextNotifyItem;

	CEtUIStatic* m_pStaticGold;
	CEtUIStatic* m_pStaticSilver;
	CEtUIStatic* m_pStaticBronze;
	CDnItem* m_pNeedItem[5];
	CDnItemSlotButton* m_pNeedItemButtonSlot[5];	
	CEtUIButton* m_pCreateButton;
	CEtUIStatic* m_pStaticBackBoard;

#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
	CEtUIStatic* m_pStaticNotifyFlag;
#endif
	
	std::vector<CDnItemTask::ItemCompoundInfo> m_vNeedItemInfo;

	CDnCompoundBase::ItemGroupStruct m_pCompoundGroupData;

	CDnItemTask::CompoundItemPackage	m_CompoundItemPackage;

	int m_nCompoundID;
	int m_nItemID;

	DWORD m_dwColorGold;
	DWORD m_dwColorSilver;
	DWORD m_dwColorBronze;

	bool m_bSelected;
	bool m_bEnable;
	bool m_bForceOpenMode;
	
		
};
#endif 