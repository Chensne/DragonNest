#pragma once

#if defined( PRE_ADD_STAGE_CLEAR_ADD_REWARD )

#include "DnCustomDlg.h"

class CDnDungeonExtraRewardDlg : public CDnCustomDlg
{
public:
	CDnDungeonExtraRewardDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_TOP, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnDungeonExtraRewardDlg();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );

protected:
	enum{
		E_NEED_ITEM_COUNT = 3,
	};

	CEtUIStatic * m_pSlotCover[ E_NEED_ITEM_COUNT ];
	CDnItemSlotButton * m_pSlotItem[ E_NEED_ITEM_COUNT ];
	CEtUIButton * m_pOKButton;
	DWORD m_dwPropIndex;
	int m_nSelectIndex;

public:
	void ResetNeedItemSlot();
	void HideSlotCover();
	bool IsMyInventoryExist( int nItemID );

	void SetPropIndex( DWORD dwPropIndex ) { m_dwPropIndex = dwPropIndex; }
	void SetNeedItemSlot();
	void Initialize_NeedItemIndex( std::vector<int> & vecNeedItemIndex );
	void Initialize_NeedItemSlot( std::vector<int> & vecNeedItemIndex );
	void SelectSlot( const char * szSlotName );
};

#endif	// #if defined( PRE_ADD_STAGE_CLEAR_ADD_REWARD )