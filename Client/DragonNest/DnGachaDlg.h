#pragma once
#include "DnCustomDlg.h"
#include "DnParts.h"

#ifdef PRE_ADD_GACHA_JAPAN

//#ifdef PRE_ADD_KALI
//const int GACHA_NUM_JOB_BTN = 6;
//#else
//const int GACHA_NUM_JOB_BTN = 5;
//#endif

#ifdef PRE_ADD_MACHINA
const int GACHA_NUM_JOB_BTN = 9;
#else
const int GACHA_NUM_JOB_BTN = 6;
#endif

class CDnGachaPreview;
class CDnGachaRouletteDlg;
class CDnGachaBuyConfirmDlg;
class CDnGachaResultDlg;
class CDnItem;

// 일본 버전 전용 가챠폰 
class CDnGachaDlg : public CDnCustomDlg,
					public CEtUICallback
{
private:
	enum
	{
		PREVIEW_DLG,
		GACHA_BUY_CONFIRM_DLG,
		GACHA_RESULT_DLG,
	};

	// 가챠폰 상태.
	enum
	{
		WAITING,
		ROULETTING,
		STOPPING,
		END,
	};

	int m_iGachaIndex;

	CDnGachaPreview* m_pGachaPreviewDlg;
	CDnGachaBuyConfirmDlg* m_pGachaBuyConfirm;
	CDnGachaResultDlg* m_pGachaResultDlg;
	CEtUIComboBox* m_pComboPartsSelect;
	CEtUIRadioButton* m_apRadioBtnJob[ GACHA_NUM_JOB_BTN ];
	CEtUIButton* m_pBtnOpenCashShop;
	CEtUIStatic* m_pStaticCoin;
	CEtUIButton* m_pBtnBuyGacha;
	CEtUIButton* m_pBtnClose;

	CEtUIStatic* m_pStaticItemNameArea;
	CEtUIStatic* m_pStaticItemStatArea;

	// SkillTreeContentDlg 와 마찬가지로 기본은 hide 상태이며 이 다이얼로그가 렌더링 될 때 직접 렌더링 한다.
	CDnGachaRouletteDlg* m_pNameRouletteDlg;
	CDnGachaRouletteDlg* m_pStatRouletteDlg;
	int m_iRouletteCount;
	deque<CDnItem*> m_dqItemBuffer;

	int m_iSelectedJobBtnID;
	bool m_bInitialized;
	bool m_bProcessRoulette;

	struct S_PART_GACHA_DROPITEMS_INFO
	{
		vector<int> avlItemsByPart[ CDnParts::CashPartsTypeEnum_Amount ];
	};

	// 지금 열은 가챠폰에서 나올 수 있는 모든 아이템 ID 를 모아놓는다.
	map<int, S_PART_GACHA_DROPITEMS_INFO> m_mapItemsByJob;
	//int m_aiDropTableIDs[ CDnParts::CashPartsTypeEnum_Amount ];
	int m_iSelectedPart;

	// 결과로 서버로부터 받은 아이템
	CDnItem* m_pResultItem;
	float m_fStopAccel;
	int m_iLeftStopSelCount;
	float m_fRouletteSpeed;
	float m_fLeftRouletteDistance;
	//bool m_bAllowStop;
	float m_fStopTimeStamp;
	float m_fRouletteStopTimeGap;
	int m_iResultItemStaticIndex;			// 결과물 아이템이 셋팅될 룰렛 인덱스.

	float m_fRouletteElapsedTime;

	int m_iPrevRouletteCount;

	// 상품 전달까지 일련의 과정이 끝났음.
	//bool m_bGachaCompleted;

	// 현재 가챠폰 상태
	int m_eGachaStatus;

	// 현재 룰렛이 돌아가고 있는 아이템 아이디를 모아놓고 랜덤으로 돌려준다.
	vector<int> m_vlMixedSelectedPartsItems;

	// 사운드 관련.
	int m_iRouletteChangeSound;
	int m_iRouletteResultSound;

protected:
	void _GatherGachaItems( void );
	void _InitializeComboBox( void );
	void _UpdateJobSelection( void );
	void _OnSelectPart( void );				// 파츠 선택됨. 가챠 돌아가기 시작.
	void _UpdateRouletteItemNameInfo( void );
	void _UpdateRouletteItemStatInfo( void );
	void _RenderRouletteDialog( float fDelta, CEtUIControl* pAreaControl, CDnGachaRouletteDlg* pRouletteDlg );
	void _MakeItemStatString( CDnState* pItemState, /*IN OUT*/ wstring& strItemStat );

public:
	CDnGachaDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGachaDlg(void);

	// 서버로부터 날아온 NPC에 등록된 가챠폰 인덱스
	void SetGachaIndex( int iGachaIndex ) { m_iGachaIndex = iGachaIndex; };

	void Initialize( bool bShow );
	void InitialUpdate( void );
	void Reset( void );
	void Show( bool bShow );
	void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	void Process( float fDelta );
	void Render( float fDelta );

	void UpdateGachaCoin( void );

	// CDnGachaRouletteDlg 에서 호출함.
	
	// 룰렛 돌아가는 동안엔 계속 호출함.
	void ProcessRoulette( bool bProcessRoulette ) { m_bProcessRoulette = bProcessRoulette; };

	// Gacha Preview 에서 이름 바꾸라고 호출함.
	void OnChangeItem( int iItemID );
	
	//////////////////////////////////////////////////////////////////////////

	// 룰렛 역할을 할 다이얼로그 셋팅됨.
	void SetRouletteDlg( CDnGachaRouletteDlg* pNameRouletteDlg, CDnGachaRouletteDlg* pStatRouletteDlg )
	{ 
		m_pNameRouletteDlg = pNameRouletteDlg; 
		m_pStatRouletteDlg = pStatRouletteDlg;
	};

	void OnChangeResolution();

	// 서버로부터 가챠폰 아이템 결과가 옴
	void OnRecvGachaResult( const SCRefreshCashInven* pPacket );
};
#endif //PRE_ADD_GACHA_JAPAN