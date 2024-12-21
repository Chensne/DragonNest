
#pragma once

#include "DnCustomDlg.h"
#include "DnParts.h"

#define COSTUMEDESIGNMIX_PROGRESS_BAR_TIME				3.f

#include "DnParts.h"

class CDnCostumeMixPreviewDlg;
class CDnCostumeMixProgressDlg;
class CDnCostumeMixCompleteDlg;
class CDnCostumeDesignMixDlg : public CDnCustomDlg, public CEtUICallback
{
public:
	enum
	{
		ePROGRESSDLG,
		eDISABLEDLG,
		eWARNINGDIFFDLG,
	};

	enum eSTUFF_TYPE
	{
		eDMIX_STUFF_EMPTY = -1,
		eDMIX_STUFF_MIN,
		eABILITY = eDMIX_STUFF_MIN,
		eDESIGN,
		eDMIX_STUFF_MAX,
		eDMIX_RESULT = eDMIX_STUFF_MAX
	};

	enum eERR_COSDMIX
	{
		eECDMIX_GENERAL = 30000,
		eECDMIX_ILLEGAL_DMIX_FROM_SLOT,	// 합성할 수 없는 아이템입니다.
		eECDMIX_CANT_DMIX_STUFF,		// 합성할 수 없는 아이템입니다.
		eECDMIX_NO_CASH_STUFF,			// 캐시 아이템이 아닙니다.
		eECDMIX_STUFF_DIFFCLASS,		// 자신과 같은 클래스의 아이템만 합성할 수 있습니다.
		eECDMIX_STUFF_ETERNITY_ONLY,	// 영구 아이템만 합성할 수 있습니다.
		eECDMIX_STUFF_NOPARTS,			// 파츠 아이템만 합성할 수 있습니다.
		eECDMIX_NORELEASE_BUFFSLOT,		// 코스튬 합성 중 문제가 발생했습니다.
		eECDMIX_DIFF_PARTS_TYPE,		// 같은 파츠 타입끼리만 합성할 수 있습니다.
		eECDMIX_STUFFCOUNT_NONE,		// 합성할 아이템을 올려주세요.
		eECDMIX_CANT_DMIX_ALREADY_DMIX,	// 합성한 아이템은 합성할 수 없습니다.
	};

	enum eRET_CHECKMIX
	{
		eRET_ABLE,
		eRET_UNABLE,
		eRET_WARNING_DIFF_RANK,
	};

	CDnCostumeDesignMixDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCostumeDesignMixDlg(void);

	virtual void Initialize(bool bShow);
	virtual void InitialUpdate();
	virtual void Show(bool bShow);
	virtual void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg /* = 0 */);
	virtual void Process(float fElapsedTime);
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg );

	void		 HandleError(eERR_COSDMIX code);
	void		 AddAttachItem(CDnSlotButton *pFromButton);
	bool		 AddAttachItem(CDnSlotButton* pFromSlot, CDnItemSlotButton* pAttachSlot, int itemCount);
	bool		 ReleaseStuffItemSlotBtn(CDnSlotButton* pStuffSlot, int slotIdxIfSlotChange = eDMIX_STUFF_EMPTY, bool bNoReleaseResultSlot = false);
	void		 DisableAllDlgs(bool bEnable, const std::wstring& text);
	void		 CompleteMix(CDnItem* pCompleteInvenItem);

private:
	eSTUFF_TYPE	 CheckStuffSlotType() const;
	bool		 IsDesignMixableItem(CDnParts* pPartsItem) const;
	int			 GetEmptyStuffSlot() const;
	void		 MakeResultSlot();

	void		 ReleaseResultSlot();
	void		 AttachResultToPreview();
	bool		 RevertPartsFromPreview();

	void		 ClearStuffSlot(bool bNoReleaseResultSlot);
	eRET_CHECKMIX CheckMix();
	void		 DoMix();

	CDnItemSlotButton*	m_pResultSlot;
	CDnItemSlotButton*	m_pStuffSlots[eDMIX_STUFF_MAX];
	CDnSlotButton*		m_pInvenSlotsCache[eDMIX_STUFF_MAX];

	CDnCostumeMixPreviewDlg*	m_pPreviewDlg;
	CDnCostumeMixProgressDlg*	m_pProgressDlg;
	CDnCostumeMixCompleteDlg*	m_pCompleteDlg;

	CDnParts::PartsTypeEnum		m_CurPartsType;
	int							m_CompleteMixSound;
};