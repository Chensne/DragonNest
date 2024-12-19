#pragma once

#if defined(PRE_ADD_TALISMAN_SYSTEM)
#include "DnCustomDlg.h"
#include "DnCharStatusDlg.h"

class CDnTooltipTalismanDlg;
class CDnCharTalismanListDlg : public CDnCustomDlg, public CEtUICallback
{
public:
	CDnCharTalismanListDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	~CDnCharTalismanListDlg();

public:
	struct sTalismanSlotData
	{
		int		nSlotIndex;				// 슬롯의 번호 - 1부터 시작 -
		bool	bIsServiceSlot;			// 해당 슬롯의 서비스 유무 - 활성1 / 비활성0 -
		int		nSlotOpenLevel;			// 해당 슬롯을 Open할수있는 레벨.
		int		nSlotOpenAmount;		// 해당 슬롯을 Open할때 드는 비용.
		int		nSlotOpenNeedItemID;	// 해당 슬롯을 Open할때 드는 아이템 아이디.
		float	fEfficiency;			// 슬롯의 효율 
		int		nToolTipStringMid;		// 툴팁 String MID
		int		nSlotOpenPeriod;		// 해달 슬롯의 Open 기간.
		bool	bIsOpenSlot;			// 열렸는지 체크( 서버에서 주는 정보로 설정된다 )
		eTalismanSlotType	nSlotType;	// 슬롯 타입 (TALISMAN_BASIC = 0,  TALISMAN_EXPANSION = 1, TALISMAN_CASH_EXPANSION = 2)
		std::string			strSlotHoldTextureFileName;	// 잠금 슬롯에 표시되는 텍스쳐 이름.

		CEtUIStatic*		pBackImage;
		CDnItemSlotButton*	pSlotButton;
		CEtUIStatic*		pPercentText;
		CEtUIStatic*		pCoverStatic;
		CEtUIStatic*		pCashCoverStatic;

		sTalismanSlotData()
			: nSlotIndex(0)
			, bIsServiceSlot(false)
			, nSlotOpenLevel(0)
			, nSlotOpenAmount(0)
			, nSlotOpenNeedItemID(0)
			, fEfficiency(.0f)
			, nToolTipStringMid(0)
			, nSlotOpenPeriod(0)
			, nSlotType(TALISMAN_BASIC)
			, pBackImage(NULL)
			, pSlotButton(NULL)
			, pPercentText(NULL)
			, pCoverStatic(NULL)
			, pCashCoverStatic(NULL)
			, bIsOpenSlot(false)
		{}
	};

protected:
	std::vector<sTalismanSlotData>	m_vItemSlotBtnList;
	CDnItem*				m_pRemoveItem;
	CDnCharTalismanDlg*		m_pParentDlg;
	CDnTooltipTalismanDlg*	m_pToolTipDlg;
	bool					m_bIsTalismanToTalisman;
	bool					m_bIsEditMode;
	CEtUIStatic*			m_pStaticControl;
	

protected:
	void MakeSlotData();
	void SetWeableSlotBlank();

public:	
	const int FindEmptySlotIndex();
	void	  RefreshTalismanSlot();
	void	  RefreshTalismanCashSlot();
	bool	  CheckParam(int nParam);
	const std::vector<sTalismanSlotData> GetItemSlotBtnList() { return m_vItemSlotBtnList; }

	// ui셋팅
	void  SetSlotItem(int nIndex, MIInventoryItem* pItem);
	bool  RemoveSlotItem(int nIndex);
	void  SetOpenTalismanSlot(int nOpenSlotFlag);
	float GetTalismanSlotRatio(int nSlotIndex);
	void  ShowTalismanSlotEfficiency(bool bShow);
	void  SetAllOpenSlot(); // 살펴보기 전용
	CDnTooltipTalismanDlg* GetTalismanToolTip() { return m_pToolTipDlg; }
	void  ReleaseTalismanClickFlag() { m_bIsTalismanToTalisman = false; }
#ifdef PRE_ADD_EQUIPLOCK
	int	  GetWearedSlotIndex(const CDnItem* pItem) const;
#else
	int	  GetWearedSlotIndex(CDnItem* pItem);
#endif
	CDnItem* GetEquipTalisman(int nIndex);
	float GetBGImageHeight() { if(m_vItemSlotBtnList.empty()) return 0.0f; return m_vItemSlotBtnList[0].pBackImage->GetUICoord().fHeight; }

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void Process( float fElapsedTime );
	virtual void Render( float fElapsedTime );
};

#endif // PRE_ADD_TALISMAN_SYSTEM