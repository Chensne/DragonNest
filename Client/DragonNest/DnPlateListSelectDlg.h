#pragma once
#include "DnCustomDlg.h"
#include "EtUIDialogGroup.h"
#include "DnItemTask.h"

class CDnPlateListDlg;

class CDnPlateListSelectDlg : public CDnCustomDlg, public CEtUICallback
{
public:
	CDnPlateListSelectDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPlateListSelectDlg(void);

	struct RuneInfo{
		int m_nID[NUM_MAX_PLATE_COMPOUND_VARI];
		int m_nNum[NUM_MAX_PLATE_COMPOUND_VARI];
	};

	enum{
		SLOT_NUM = 5,
	};

private :

	CDnPlateListDlg *	m_pPlateListDlg;
	CDnItem	*			m_pNeedRune[SLOT_NUM];
	CDnItemSlotButton *	m_pNeedRuneSlotButton[SLOT_NUM];

	CEtUIButton *		m_pButtonNext;
	CEtUIButton *		m_pButtonPrev;
	CEtUIStatic *		m_pStaticPage;
	CEtUIStatic *		m_pStaticGold;
	CEtUIStatic *		m_pStaticSilver;
	CEtUIStatic *		m_pStaticBronze;
	DWORD				m_dwColorGold;
	DWORD				m_dwColorSilver;
	DWORD				m_dwColorBronze;

	int			m_nCurSelect;
	RuneInfo	m_RuneInfo[SLOT_NUM];
	int			m_pPlateInfo[NUM_MAX_PLATE_COMPOUND_VARI];
	std::vector<CDnItemTask::ItemCompoundInfo> m_vNeedItemInfo;

public:
	void RefreshPlateList();
	int GetSelectedCompoundID();

	int GetPlateID( int nIndex )	{ return m_pPlateInfo[nIndex]; }
	void SelectPlate( int nIndex );
	int * GetPlateInfo()			{ return m_pPlateInfo; }
	std::vector<CDnItemTask::ItemCompoundInfo> & GetCompoundInfo()	{ return m_vNeedItemInfo; }

	bool EnoughCoin();
	bool EnoughRune();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};

/*
class CDnPlateListDlg;
class CDnPlateMixViewDlg;

class CDnPlateListSelectDlg : public CDnCustomDlg, public CEtUICallback
{
public:
	CDnPlateListSelectDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPlateListSelectDlg(void);

	enum
	{
		COMPOUND_CONFIRM_DIALOG,
		COMPOUND_MOVIE_DIALOG,	// 진행중 무비 연출 다이얼로그
	};

	enum emPlateStep
	{
		stepBase,
		stepPlateRegist,
		stepListSelect,
		stepSlotItem,	// 슬롯에 보옥 넣는 단계
	};

	enum emSLOT_DLG_ID
	{
		// 슬롯 다이얼로그는 5개까지만.
		SLOT_DLG_00,
		SLOT_DLG_01,
		SLOT_DLG_02,
		SLOT_DLG_03,
		SLOT_DLG_04
	};

protected:

	emPlateStep m_emStep;

	// 플레이트 아이템과 슬롯.
	CDnItem *m_pItem;
	CDnItemSlotButton *m_pItemSlotButton;
	CEtUIStatic *m_pStaticFix;

	CEtUIButton *m_pButtonOK;
	CEtUIButton *m_pButtonCancel;

	CEtUIStatic *m_pStaticBack;

	CEtUIStatic *m_pStaticGold;
	CEtUIStatic *m_pStaticSilver;
	CEtUIStatic *m_pStaticBronze;

	// 여기도 등록하는 형태니 QuickSlot포인터 하나 필요.
	CDnQuickSlotButton *m_pQuickSlotButton;

	// 자식 다이얼로그들
	CDnPlateListDlg *m_pPlateListDlg;
	CEtUIDialogGroup m_SlotDlgGroup;
	CDnPlateMixViewDlg *m_pPlateMixViewDlg;
	//CDn

	// 플레이트 가데이터
	// 62 000 000 1		파괴 없음
	// 62 000 000 2		실패시 보옥 파괴
	// 62 000 000 3		실패시 플레이트 파괴
	// 62 000 000 4		실패시 둘다 파괴


	void RefreshPlateList();
	void ClosePlateList();
	void ShowPlateMixView();
	void ClosePlateMixView();

	// 보옥슬롯에 들어갈 아이템 종류와 갯수를 체크한다.
	void CheckSlotItems();

	bool IsEnableRegisterPlateItem(CDnSlotButton* pDragButton, CDnItem* pItem) const;

	// 서버에 전송할 필요아이템 구조체
	vector<CDnItemTask::ItemCompoundInfo> m_vNeedItemInfo;
public:

	// 인벤에서 우클릭 바로등록.
	void SetPlateItem( CDnQuickSlotButton *pPressedButton );
	int GetPlateItemID();

	void ChangeStep( emPlateStep emStep );


	void OnRecvEmblemCompoundOpen();
	void OnRecvEmblemCompound();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
};
*/