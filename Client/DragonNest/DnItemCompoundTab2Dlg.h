#pragma once

#include "DnCustomDlg.h"
#include "DnItem.h"
#include "DnItemTask.h"


class CDnItemCompoundMix2Dlg;
class CDnItemCompoundMixDlg;
class CDnItemCompoundTab2Dlg : public CDnCustomDlg, public CEtUICallback
{
public:
	CDnItemCompoundTab2Dlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnItemCompoundTab2Dlg(void);


	struct CompoundGroupStruct {
		CDnItem *pItem;
		CDnItemSlotButton *pItemSlotButton;
		CEtUIStatic *pStaticBase;
		CEtUIStatic *pStaticName;
		CEtUIStatic *pStaticLevel;
		CEtUIStatic *pStaticLevelText;
#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
		CEtUIStatic *pStaticNotifyFlag;
#endif
		bool bEnable;

		CompoundGroupStruct() {
			pItem = NULL;
			pItemSlotButton = NULL;
			pStaticBase = NULL;
			pStaticName = NULL;
			pStaticLevel = NULL;
			pStaticLevelText = NULL;
#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
			pStaticNotifyFlag = NULL;
#endif
			bEnable = true;
		};
		~CompoundGroupStruct() {
			SAFE_DELETE( pItem );
		};

		void SetInfo( int nGroupTableID );
		void Clear()
		{
			SAFE_DELETE( pItem );
			if( pItemSlotButton ) pItemSlotButton->ResetSlot();
			if( pStaticBase ) pStaticBase->Show( false );
			if( pStaticName ) pStaticName->SetText( L"" );
			if( pStaticLevel ) pStaticLevel->SetText( L"" );
			if( pStaticLevelText ) pStaticLevelText->Show( false );
#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
			if( pStaticNotifyFlag ) pStaticNotifyFlag->Show( false );
#endif
		}
		void Enable( bool bValue )
		{
			bEnable = bValue;
			if( pItemSlotButton ) pItemSlotButton->SetRegist( !bEnable );
			if( pStaticBase ) pStaticBase->Enable( bEnable );
			if( pStaticName ) pStaticName->Enable( bEnable );
			if( pStaticLevel ) pStaticLevel->Enable( bEnable );
			if( pStaticLevelText ) pStaticLevelText->Enable( bEnable );
		}
		void ForceBlend()
		{
			if( pStaticBase ) pStaticBase->ForceBlend();
			if( pStaticName ) pStaticName->ForceBlend();
			if( pStaticLevel ) pStaticLevel->ForceBlend();
			if( pStaticLevelText ) pStaticLevelText->ForceBlend();
		}
#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
		void EnableNotify( bool bEnable )
		{
			if( pStaticNotifyFlag ) pStaticNotifyFlag->Show( bEnable );
		}
#endif

		bool IsInsideItem( float fX, float fY )
		{
			SUICoord uiCoordsBase;
			pStaticBase->GetUICoord(uiCoordsBase);
			return uiCoordsBase.IsInside(fX, fY);
		}
	};

	struct CompoundOptionStruct {
		CDnItem *pItem;
		CDnItemSlotButton *pItemSlotButton;
		CEtUIStatic *pStaticBase;
		CEtUIStatic *pStaticName;
		CEtUIStatic *pStaticLevel;
		CEtUIStatic *pStaticLevelText;
		CEtUIStatic *pStaticType;
#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
		CEtUIStatic *pStaticNotifyFlag;
#endif
		bool bEnable;

		CompoundOptionStruct() {
			pItem = NULL;
			pItemSlotButton = NULL;
			pStaticBase = NULL;
			pStaticName = NULL;
			pStaticLevel = NULL;
			pStaticLevelText = NULL;
			pStaticType = NULL;
#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
			pStaticNotifyFlag = NULL;
#endif
			bEnable = true;
		};
		~CompoundOptionStruct() {
			SAFE_DELETE( pItem );
		};

		void SetInfo( int nCompoundID, int nItemID );
		void Clear()
		{
			SAFE_DELETE( pItem );
			if( pItemSlotButton ) pItemSlotButton->ResetSlot();
			if( pStaticBase ) pStaticBase->Show( false );
			if( pStaticName ) pStaticName->SetText( L"" );
			if( pStaticLevel ) pStaticLevel->SetText( L"" );
			if( pStaticLevelText ) pStaticLevelText->Show( false );
			if( pStaticType ) pStaticType->SetText( L"" );
#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
			if( pStaticNotifyFlag ) pStaticNotifyFlag->Show( false );
#endif
		}
		void Enable( bool bValue )
		{
			bEnable = bValue;
			if( pItemSlotButton ) pItemSlotButton->SetRegist( !bEnable );
			if( pStaticBase ) pStaticBase->Enable( bEnable );
			if( pStaticName ) pStaticName->Enable( bEnable );
			if( pStaticLevel ) pStaticLevel->Enable( bEnable );
			if( pStaticLevelText ) pStaticLevelText->Enable( bEnable );
			if( pStaticType ) pStaticType->Enable( bEnable );
		}
#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
		void EnableNotify( bool bEnable )
		{
			if( pStaticNotifyFlag ) pStaticNotifyFlag->Show( bEnable );
		}
#endif
		void ForceBlend()
		{
			if( pStaticBase ) pStaticBase->ForceBlend();
			if( pStaticName ) pStaticName->ForceBlend();
			if( pStaticLevel ) pStaticLevel->ForceBlend();
			if( pStaticLevelText ) pStaticLevelText->ForceBlend();
			if( pStaticType ) pStaticType->ForceBlend();
		}

		bool IsInsideItem( float fX, float fY )
		{
			SUICoord uiCoordsBase;
			pStaticBase->GetUICoord(uiCoordsBase);
			return uiCoordsBase.IsInside(fX, fY);
		}
	};


	struct GroupList {
		int nGroupTableID;
		int nCompoundGroupID;
		int nCompoundGroupType;
		int nItemID;
		int nLevel;
		int nRank;
		int nJob;
		int nActivateLevel;
		bool bIsPreview;
		int nPreviewDescUIStringID;

		std::vector<int> nVecCompoundList;
		std::vector<int> nVecResultCompoundList;
	};
	enum{
		COMPOUND_CONFIRM_DIALOG,
		COMPOUND_MOVIE_DIALOG,
	};

	enum{
		COMPOUNT_SUNDRIES_ID = 8,	// 잡화 아이디
	};

protected:
	int m_nSelectPartsType;
	int m_nSelectJobType;
	int m_nSelectSortType;
	bool m_bCheckHideLowLevel;
	bool m_bCheckShowPossibleOnly;

	int m_nCurCompoundListID;
	std::vector<GroupList *> m_pVecCompoundGroupList;

	std::vector<GroupList *> m_pVecResultGroupList;
	int m_nCurGroupPage;
	int m_nSelectGroup;

	int m_nCurOptionPage;
	int m_nSelectOption;

	INT64 m_biLastNeedItemSerialID;

	CEtUIComboBox *m_pComboBoxJob;
	CEtUIComboBox *m_pComboBoxSort;
	CEtUICheckBox *m_pCheckBoxLowLevel;
	CEtUICheckBox *m_pCheckBoxPossibleOnly;

	CEtUIStatic *m_pStaticGroupPage;
	CEtUIStatic *m_pStaticSelectGroup;

	CEtUIStatic *m_pStaticOptionPage;
	CEtUIStatic *m_pStaticSelectOption;
	CEtUIStatic *m_pStaticOptionDesc;

	CDnItem *m_pNeedItem[5];
	CDnItemSlotButton *m_pNeedItemSlotButton[5];

	CEtUIButton *m_pButtonOK;
	CEtUIButton *m_pButtonCancel;

	CEtUIStatic *m_pStaticGold;
	CEtUIStatic *m_pStaticSilver;
	CEtUIStatic *m_pStaticBronze;
	DWORD m_dwColorGold;
	DWORD m_dwColorSilver;
	DWORD m_dwColorBronze;

	CompoundGroupStruct m_CompoundGroupList[6];
	CompoundOptionStruct m_CompoundOptionList[6];

	CDnItemCompoundMixDlg *m_pCompoundMixDlg;
	CDnItemCompoundMix2Dlg *m_pCompoundMix2Dlg;
	std::vector<CDnItemTask::ItemCompoundInfo> m_vNeedItemInfo;

#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
	CEtUIStatic *m_pTextNotifyItem;
	CEtUICheckBox *m_pCheckNotifyItem;
	CEtUIButton *m_pButtonClose;
	CEtUIRadioButton *m_pRadioButtonCreate;
	CEtUIRadioButton *m_pRadioButtonEnchant;
	bool m_bForceOpenMode;
#endif

	int m_nRemoteItemID;

protected:
	void UpdateGroupList();

	void UpdateSelectGroup();
	void UpdateSelectOption();
	void RefreshCompoundItem();

	void SetNeedItemSlot( int nSlotIndex, int nItemID, int nNeedItemCount, int nCurItemCount, char nOptionIndex = -1 );
	void ShowConfirmDlg( int nCompoundID );
	int GetSelectResultCompoundID();
	const GroupList* GetSelectedResultGroupList( void );

	static bool Sort_UpRank( CDnItemCompoundTab2Dlg::GroupList *a, CDnItemCompoundTab2Dlg::GroupList *b );
	static bool Sort_DownRank( CDnItemCompoundTab2Dlg::GroupList *a, CDnItemCompoundTab2Dlg::GroupList *b );
	static bool Sort_UpLevel( CDnItemCompoundTab2Dlg::GroupList *a, CDnItemCompoundTab2Dlg::GroupList *b );
	static bool Sort_DownLevel( CDnItemCompoundTab2Dlg::GroupList *a, CDnItemCompoundTab2Dlg::GroupList *b );

	void Sort_Rank();

	void ResetList();

	int GetMaxGroupPage();
	int GetMaxOptionPage();

	INT64 GetDiscountedCost( INT64 iCost );

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );

	void RefreshGroupList( bool bRefreshSelect = true );
	void RefreshOptionList( bool bRefreshSelect = true );
	void SelectCompoundItemFromIndex( int nCompoundIndex );
	void SelectCompoundPartsType( int nType ){ m_nSelectPartsType = nType; }
	void RefreshCompoundGroupByLevel( int nLevel );

	void SetCompoundListID( int nListID );
	void SetRemoteItemID( int nRemoteItemID );
	void OnRecvItemCompoundOpen();
	void OnRecvItemCompound();

#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
	void SetForceOpenMode( bool bShow );
	void ResetForceOpenMode();
#endif
};