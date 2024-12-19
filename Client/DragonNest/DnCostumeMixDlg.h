#pragma once

#include "DnCustomDlg.h"
#include "DnParts.h"

#define MAX_COSTUMEMIX_STUFF_COUNT		5
#define MAX_COSTUMEMIX_RESULT_COUNT		9
#define MAX_COSTUMEMIX_RESULT_COUNT_WITHOUT_RANDOM		MAX_COSTUMEMIX_RESULT_COUNT - 1
#define NO_ABILITY_COMBOBOX_SELECTED	-1
#define PROGRESS_BAR_TIME				3.f

class CDnItem;
class CDnQuickSlotButton;
class CDnCostumeMixPreviewDlg;
class CDnItemSlotButton;
class CDnCostumeMixProgressDlg;
class CDnCostumeMixCompleteDlg;
class CDnCostumeMixDlg : public CDnCustomDlg, public CEtUICallback
{
public:
	enum
	{
		ePROGRESSDLG,
		eDISABLEDLG,
	};

	enum eERR_COSMIX
	{
		eECM_GENERAL,
		eECM_NOSELECT_RESULT,
		eECM_STUFFCOUNT_SHORT,
		eECM_NOSELECT_ABILITY,
		eECM_CANT_MIX_STUFF,
		eECM_DIFF_PARTS_TYPE,
		eECM_STUFF_FULL,
		eECM_STUFF_ETERNITY_ONLY,
		eECM_STUFF_DIFFCLASS,
		eECM_STUFFCOUNT_NONE
	};

	CDnCostumeMixDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCostumeMixDlg(void);

	virtual void Initialize(bool bShow);
	virtual void InitialUpdate();
	virtual void Show(bool bShow);
	void		 AddAttachItem(CDnSlotButton* pFromButton);
	bool		 ReleaseStuffItemSlotBtn(CDnSlotButton* pStuffSlot);
	void		 HandleError(eERR_COSMIX errType);
	void		 CompleteMix(ITEMCLSID resultItem, char cOption);
	void		 DisableAllDlgs(bool bEnable, const std::wstring& text);

protected:
	virtual void Process(float fElapsedTime);
	virtual void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0);
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg );
	virtual bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	struct SSlotCacheUnit
	{
		int				stuffSlotIdx;
		CDnSlotButton*	pInvenSlotBtn;

		SSlotCacheUnit() { stuffSlotIdx = -1; pInvenSlotBtn = NULL; }
		SSlotCacheUnit(int idx, CDnSlotButton* pBtn)	{ stuffSlotIdx = idx; pInvenSlotBtn = pBtn; }
		bool operator==(const SSlotCacheUnit& rhs)		{ return (pInvenSlotBtn == rhs.pInvenSlotBtn); }
	};

	struct SSelectSlotUnit
	{
		int					resultSlotIdx;
		CEtUIStatic*		pSelectStatic;

		SSelectSlotUnit() { resultSlotIdx = -1; pSelectStatic = NULL; }
		SSelectSlotUnit(int idx, CEtUIStatic* pSel) { resultSlotIdx = idx; pSelectStatic = pSel; }
		bool IsRandomSelected() const	{ return (resultSlotIdx == 0); }
		bool IsEmpty() const			{ return (pSelectStatic == NULL || resultSlotIdx == -1); }
	};

	bool		 AddAttachItem(CDnSlotButton* pFromSlot, CDnItemSlotButton* pAttachSlot, int itemCount);
	bool		 IsMixableItem(const CDnParts* pItem) const;
	void		 SetResultItemList();
	void		 ShowOffResultSelectSlot();
	void		 SetAbilityComboBox(CDnParts::PartsTypeEnum type);

	int			 GetEmptyStuffSlot() const;
	int			 GetEmptyStuffSlotCount() const;
	int			 GetStuffCountInSlot() const;
	int			 GetResultSlotUISize();
	CDnCostumeMixDlg::SSlotCacheUnit* GetInvenSlotCache(int index);
	CDnCostumeMixDlg::SSlotCacheUnit* GetInvenSlotCacheBySlotIndex(int slotIndex);

	void		 ClearInvenSlotCache();
	void		 ClearStuffSlot();
	void		 ClearResultSlot();
	void		 OnEmptyStuffSlots();

	void		 ReleaseInvenSlotCache(const int slotIdx);

	bool		 RevertPartsFromPreview();
	void		 AttachPartsToPreview(ITEMCLSID itemId);
	bool		 CheckMix();
	void		 DoMix();

	void		 ShowResultItemToolTipOn(CDnItemSlotButton* pBtn, float fX, float fY);
	void		 ShowResultItemToolTipOff();

	CEtUIButton*	m_pMixBtn;
	CEtUIButton*	m_pCancelBtn;
	CEtUIButton*	m_pRandomResult;
	CEtUIButton*	m_pCloseBtn;

	CEtUIButton*	m_pPrevResultBtn;
	CEtUIButton*	m_pNextResultBtn;
	CEtUIStatic*	m_pPageStatic;

	CEtUIComboBox*	m_pAbilityComboBox;

	std::vector<CDnItemSlotButton*>		m_pStuffSlotList;
	std::vector<SSlotCacheUnit>			m_pInvenSlotCacheList;

	std::map<int, CDnItemSlotButton*>	m_pResultSlotList;
	std::map<int, CEtUIStatic*>			m_pResultSlotSelectList;
	SSelectSlotUnit						m_ResultSlotSelectCache;

	CDnCostumeMixPreviewDlg*			m_pPreviewDlg;
	CDnCostumeMixProgressDlg*			m_pProgressDlg;
	CDnCostumeMixCompleteDlg*			m_pCompleteDlg;

	int							m_CurPageNum;
	CDnParts::PartsTypeEnum		m_CurResultPartsType;

	int							m_CompleteMixSound;
	bool						m_bSelectTooltipOn;
};