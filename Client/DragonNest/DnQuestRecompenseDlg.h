#pragma once
#include "DnCustomDlg.h"
#include "DnDataManager.h"
#include "DNItem.h"

class CDnQuestRecompenseDlg : public CDnCustomDlg
{
public:
	CDnQuestRecompenseDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnQuestRecompenseDlg(void);

protected:
	DWORD m_nSetItemCount;
	DWORD m_nSetAddItemCount;

	CEtUIStatic *m_pStaticGold;
	CEtUIStatic *m_pStaticSilver;
	CEtUIStatic *m_pStaticBronze;
	
	CEtUIStatic *m_pStaticExp;
	
	CEtUIStatic *m_pStaticComment;

	std::vector<CDnItemSlotButton*> m_vecItemSlotButton;

	CEtUIStatic * m_pStaticAddTitle;
	CEtUIStatic * m_pStaticAddBoard;
	std::vector<CDnItemSlotButton*> m_vecAddItemSlotButton;

	CEtUIStatic * m_pStaticAddOnlyTitle;
	CEtUIStatic * m_pStaticAddOnlyBoard;
	std::vector<CDnItemSlotButton*> m_vecOnlyAddItemSlotButton;

	CEtUIStatic *m_pBoardFTG;
	CEtUIStatic *m_pTextFTG;
#ifdef PRE_ADD_REMOTE_QUEST
	int m_nRewardCashItemStartSlotIndex;
	int m_nQuestIndex;
	int m_nTypeRecompense;
	int m_nAvailableCount;
	std::vector<CEtUIStatic*> m_vecItemSlotSelectStatic;
#endif // PRE_ADD_REMOTE_QUEST

protected:
	void ResetAllData();
	void DeleteVectorItem( std::vector<CDnItemSlotButton*> & vecItemSlotButton, const bool bShow );

	void SetCoinText( const UINT uiCoin );
	void SetExpText( const int nExp );
	void SetCommentText( const UINT nStringIndex );

	void SetItemOption( const int nTableID, const int nCount, OUT TItemInfo & itemInfo );
	bool GetNormalItemInfo( const int nItemID, const int nItemCount, OUT TItemInfo & itemInfo );
	CDnItem * CreateNormalItem( TItemInfo & itemInfo );

	bool GetCashItemInfo( const int nCashItemSN, OUT TItemInfo & itemInfo );
	CDnItem * CreateCashItem( const int nCashItemSN, TItemInfo & itemInfo );

	bool SetItemSlot( CDnItem * pItem );

	void InitAddRecompenseUI();
	bool SetAddItemSlot( CDnItem * pItem, std::vector<CDnItemSlotButton*> & vecItemSlotButton );
	void SetFTGText( const int nFTG );

public:
	void SetRecompense( TQuestRecompense& table );
	void SetLevelCapRecompense( const TQuestRecompense & questRecompese, const TQuestLevelCapRecompense & questLevelCapRecompense );
#ifdef PRE_ADD_REMOTE_QUEST
	int GetSlotSelectCount();
	void ResetSlotSelect();
	void GetRemoteQuestRecompenseItemArray( bool itemArray[], bool cashitemArray[] );
#endif // PRE_ADD_REMOTE_QUEST

public:
	virtual void Initialize(bool bShow);
	virtual void InitialUpdate();
	virtual void InitCustomControl( CEtUIControl *pControl );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};
