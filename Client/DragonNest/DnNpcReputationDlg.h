#pragma once
#include "DnCustomDlg.h"

#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM

const int NPC_REPUTATION_NPC_COUNT_PER_PAGE = 4;

class CDnReputationValueTooltipDlg;
class CDnItem;
// NPC 호감도 다이얼로그
class CDnNpcReputationDlg : public CDnCustomDlg,
							public CEtUICallback
{

private:
	enum
	{
		NPC_SORT_DEFAULT,
		NPC_SORT_FAVOR,
		NPC_SORT_MALICE,
		NPC_SORT_NAME,
		NPC_SORT_UNION,
	};

	enum eNPCAttrTabBtnType
	{
		eNPCATTRTAB_EXPLAIN,
		eNPCATTRTAB_GIFT,
#ifdef PRE_ADD_REPUTATION_EXPOSURE
		eNPCATTRTAB_REWARD,
#endif
		eNPCATTRTAB_MAX
	};

	int m_iNowPage;
	int m_iNowSelectedNpcSlotIndex;

	CDnReputationValueTooltipDlg* m_pTooltipDlg;

	CEtUIStatic* m_apStaticNpc[ NPC_REPUTATION_NPC_COUNT_PER_PAGE ];
	CEtUIStatic* m_apStaticEventReceiver[ NPC_REPUTATION_NPC_COUNT_PER_PAGE ];
	CEtUITextureControl* m_apTextureNpcFace[ NPC_REPUTATION_NPC_COUNT_PER_PAGE ];
	CEtUIStatic* m_apStaticNpcSelected[ NPC_REPUTATION_NPC_COUNT_PER_PAGE ];
	CEtUIStatic* m_apStaticNpcFavor[ NPC_REPUTATION_NPC_COUNT_PER_PAGE ];
	CEtUIStatic* m_apStaticNpcMalice[ NPC_REPUTATION_NPC_COUNT_PER_PAGE ];
	CEtUIProgressBar* m_apProgressFavor[ NPC_REPUTATION_NPC_COUNT_PER_PAGE ];
#ifdef PRE_MOD_REPUTE_NOMALICE
#else
	CEtUIProgressBar* m_apProgressMalice[ NPC_REPUTATION_NPC_COUNT_PER_PAGE ];
#endif
	CEtUITextureControl* m_apTextureNpcUnion[ NPC_REPUTATION_NPC_COUNT_PER_PAGE ];
	int					 m_TextureNpcUnionTableIDs[ NPC_REPUTATION_NPC_COUNT_PER_PAGE ];
	struct SNpcInPageInfo
	{
		int npcId;
		bool bAvail;

		SNpcInPageInfo() : npcId(-1), bAvail(false) {}
	};
	SNpcInPageInfo	 m_NpcIDs[ NPC_REPUTATION_NPC_COUNT_PER_PAGE ];

	int m_iNowSelectedSortMethod;
	CEtUIComboBox* m_pComboSort;

	CEtUIStatic* m_pStaticPage;
	CEtUIButton* m_pBtnPrevPage;
	CEtUIButton* m_pBtnNextPage;

	CEtUITextBox* m_pTextBoxNpcInfo;

	EtTextureHandle m_hNpcFaceTexture;
	EtTextureHandle		m_hHideNpcFaceTexture;
	EtTextureHandle		m_hUnionIconTexture;
	CEtUIRadioButton*	m_pNpcAttrBtns[eNPCATTRTAB_MAX];
	CEtUIStatic*		m_pUnAvailNpcSeal;
	CEtUIStatic*		m_pUnAvailNpcSealText;

	struct SFavorGiftUI
	{
		CEtUIStatic*					pGiftTitle;
		std::vector<CDnItemSlotButton*>	pGiftSlotBtn;

		SFavorGiftUI() : pGiftTitle(NULL) {}
		void Show(bool bShow);
		void ResetSlot();
		void SetItem(CDnItem* pItem);
#ifdef PRE_ADD_REPUTATION_EXPOSURE
		void SetRewardItem(CDnItem* pItem, bool bIsUsable);
#endif
	} m_FavorGiftUI;

	int m_iMaxPage;

	vector<int> m_vlAvailNpcReputeTableIDs;
	vector<int>	m_vlUnAvailNpcReputeTableIDs;

	eNPCAttrTabBtnType	GetCurrentNpcAttrTabType() const;
	int					GetCurrentSelectedUISlotIdx() const;
	void				UpdateNpcExplain(int uiSlotIdx);
	void				UpdateNpcFavorGift(int uiSlotIdx);

#ifdef PRE_ADD_REPUTATION_EXPOSURE
	SFavorGiftUI		m_RewardItemUI;
	void				UpdateNpcRewardGift(int uiSlotIdx);
#endif

protected:
	void _UpdateReputeNpcList( int iSortMethod );

public:
	CDnNpcReputationDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnNpcReputationDlg( void );

	void Initialize( bool bShow );
	void InitialUpdate( void );

	void RefreshToCurrentPage( void ) { UpdatePage( m_iNowPage ); };
	void UpdatePage( int iPage );
	void UpdateNpcSlotSelected( int iSelectedNpcSlotIndex );
	void Show( bool bShow );
	void Clear();

	void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	// 페이지 갱신 [2010/11/16 semozz]
	void RefreshPage(bool bInc);
	void SetNpcPage(int npcId);
};

#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM