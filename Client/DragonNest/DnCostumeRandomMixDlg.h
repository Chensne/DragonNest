#pragma once

#include "DnCustomDlg.h"
#include "DnParts.h"
#include "DnCostumeMixDefine.h"

#ifdef PRE_ADD_COSRANDMIX

class CDnCostumeMixPreviewDlg;
class CDnCostumeMixCompleteDlg;
class CDnRandomProgressMessageBox;
class CDnDummyModalDlg;
#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
class CDnMoneyControl;
#endif
class CDnCostumeRandomMixDlg : public CDnCustomDlg, public CEtUICallback
{
public:
	enum
	{
		ePROGRESSDLG,
		eDISABLEDLG,
		eCONFIRMDLG,
	};

	enum eERR_COSRANDOMMIX
	{
		eECRMIX_GENERAL = 40000,
		eECRMIX_ILLEGAL_DMIX_FROM_SLOT,	// 합성할 수 없는 아이템입니다.
		eECRMIX_CANT_DMIX_STUFF,		// 합성할 수 없는 아이템입니다.
		eECRMIX_NO_CASH_STUFF,			// 캐시 아이템이 아닙니다.
		eECRMIX_STUFF_DIFFCLASS,		// 자신과 같은 클래스의 아이템만 합성할 수 있습니다.
		eECRMIX_STUFF_ETERNITY_ONLY,	// 영구 아이템만 합성할 수 있습니다.
		eECRMIX_STUFF_NOPARTS,			// 파츠 아이템만 합성할 수 있습니다.
		eECRMIX_NORELEASE_BUFFSLOT,		// 코스튬 합성 중 문제가 발생했습니다.
		eECRMIX_DIFF_PARTS_TYPE,		// 같은 파츠 타입끼리만 합성할 수 있습니다.
		eECRMIX_STUFFCOUNT_NONE,		// 합성할 아이템을 올려주세요.
		eECRMIX_STUFFCOUNT_SHORT,
		eECRMIX_CANT_MIX_ALREADY_DMIX,  // 합성된 아이템은 다시 합성 할 수 없습니다.
		eECRMIX_NO_ITEMLIST,			// 합성할 수 없는 아이템입니다.
		eECRMIX_CANT_MIX_SETITEM,		// 세트아이템은 합성할 수 없습니다.
		eECRMIX_FEE_INSUFFICIENT,		// 수수료가 부족합니다 (NPC 합성)
	};

#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	struct SFeeForNpcMixUI
	{
		CEtUIStatic* pBackground;
		CEtUIStatic* pFeeTitle;
		CEtUIStatic* pMoneyBase;

		CEtUIStatic* pGold;
		CEtUIStatic* pSilver;
		CEtUIStatic* pCopper;

		DWORD dwColorGold;
		DWORD dwColorSilver;
		DWORD dwColorCopper;
		bool bInit;

		SFeeForNpcMixUI();
		void SetInit(bool bInit) { (*this).bInit = bInit; }
		void Show(bool bShow);
		void SetMoney(INT64 nMoney);
		bool IsShow() const;
	};
#endif

	CDnCostumeRandomMixDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCostumeRandomMixDlg(void);

	virtual void Initialize(bool bShow);
	virtual void InitialUpdate();
	virtual void Show(bool bShow);
	void DisableAllDlgs(bool bEnable, const std::wstring& text);

	void AddAttachItem(CDnSlotButton *pFromButton);
	bool AddAttachItem(CDnSlotButton* pFromSlot, CDnItemSlotButton* pAttachSlot, int itemCount);
	bool ReleaseStuffItemSlotBtn(CDnSlotButton* pBtn);
	void CompleteMix(int nResultItemId);

	int GetEmptyStuffSlotIndex() const;
	void ClearAndInitializeMix();

	void OnMixStartOrEnd(bool bStart);
	void HandleErrorOnMakePreviewItemList() const;
	void SetResultFromServer(const ITEMCLSID& resultItemId);

	bool IsEnableClose() const;

#ifdef PRE_ADD_COSTUMEMIX_TOOLTIP
	void ResetSameItemFromCashInven();
	void EnableSameItemFromCashInven( CDnSlotButton* pSlotButton );
#endif 

protected:
	virtual void Process(float fElapsedTime);
	virtual void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0);
	virtual bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	virtual void OnUICallbackProc(int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0);
#endif

private:
	bool ReleaseInvenSlotCache(int slotIdx);
	int GetStuffCountInSlot() const;
	void SetResultSlot(const ITEMCLSID& resultItemId);

	void ClearStuffSlot();
	void ClearInvenSlotCache();
	void ClearItemCache();

	void HandleError(eERR_COSRANDOMMIX code) const;
	bool IsRandomMixableItem(CDnParts* pPartsItem) const;
	void SetDrawTimer();

	bool CheckMix();
	void DoMix();

	bool StartDrawProgress();
	void EndDrawProgress();
	bool OnDrawProgress(float fElapsedTime);
	ITEMCLSID DrawRandomItemIdFromPreviewItemList();

	void StartPreviewDemo();
	void StopPreviewDemo();

	bool MakePreviewItemList();
	bool SetDummyModalDlg(bool bSet);
#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	void StartMixProgress();
#endif

	CDnItemSlotButton* m_pStuffSlots[MAX_COSTUME_RANDOMMIX_STUFF];
	CDnSlotButton* m_pInvenSlotsCache[MAX_COSTUME_RANDOMMIX_STUFF];
	CDnItemSlotButton* m_pResultSlot;
	CDnCostumeMixPreviewDlg* m_pPreviewDlg;
	CDnCostumeMixCompleteDlg* m_pCompleteDlg;

	CEtUIButton* m_pMixBtn;
	CEtUIButton* m_pCancelBtn;
	CEtUIButton* m_pCloseBtn;

	CEtUIStatic* m_pExpectItemName;
	CEtUIProgressBar* m_pProgressBarTime;
	CEtUIStatic* m_pProgressBarText;
#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	SFeeForNpcMixUI m_FeeUIs;
#endif

	CDnParts::PartsTypeEnum m_CurPartsType;
	int	m_CompleteMixSound;

	bool m_bInDrawProgress;
	float m_fMaxTimerSec;
	float m_fTimer;
	float m_fMaxTimerOffsetSec;
	float m_fTimerOffset;
#ifdef PRE_ADD_MTRANDOM_CLIENT
	CMtRandomLocalUsable m_Random;
#else
	CMtRandom m_Random;
#endif
	CDnItem* m_pPreviewItem;
	std::vector<SRandomProgressData> m_PreviewItemList;

	float m_fCompleteTimer;
	ITEMCLSID m_CurrentResultItemId;

	bool m_bPreviewDemo;
	float m_fPreviewChangePartsTimer;
	float m_fPreviewChangePartsTermSec;
#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	float m_fPreviewChangeAccessoryTermSec;
#endif
	CDnDummyModalDlg* m_pDummyModalDlg;
};

#endif // PRE_ADD_COSRANDMIX 