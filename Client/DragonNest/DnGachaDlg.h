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

// �Ϻ� ���� ���� ��í�� 
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

	// ��í�� ����.
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

	// SkillTreeContentDlg �� ���������� �⺻�� hide �����̸� �� ���̾�αװ� ������ �� �� ���� ������ �Ѵ�.
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

	// ���� ���� ��í������ ���� �� �ִ� ��� ������ ID �� ��Ƴ��´�.
	map<int, S_PART_GACHA_DROPITEMS_INFO> m_mapItemsByJob;
	//int m_aiDropTableIDs[ CDnParts::CashPartsTypeEnum_Amount ];
	int m_iSelectedPart;

	// ����� �����κ��� ���� ������
	CDnItem* m_pResultItem;
	float m_fStopAccel;
	int m_iLeftStopSelCount;
	float m_fRouletteSpeed;
	float m_fLeftRouletteDistance;
	//bool m_bAllowStop;
	float m_fStopTimeStamp;
	float m_fRouletteStopTimeGap;
	int m_iResultItemStaticIndex;			// ����� �������� ���õ� �귿 �ε���.

	float m_fRouletteElapsedTime;

	int m_iPrevRouletteCount;

	// ��ǰ ���ޱ��� �Ϸ��� ������ ������.
	//bool m_bGachaCompleted;

	// ���� ��í�� ����
	int m_eGachaStatus;

	// ���� �귿�� ���ư��� �ִ� ������ ���̵� ��Ƴ��� �������� �����ش�.
	vector<int> m_vlMixedSelectedPartsItems;

	// ���� ����.
	int m_iRouletteChangeSound;
	int m_iRouletteResultSound;

protected:
	void _GatherGachaItems( void );
	void _InitializeComboBox( void );
	void _UpdateJobSelection( void );
	void _OnSelectPart( void );				// ���� ���õ�. ��í ���ư��� ����.
	void _UpdateRouletteItemNameInfo( void );
	void _UpdateRouletteItemStatInfo( void );
	void _RenderRouletteDialog( float fDelta, CEtUIControl* pAreaControl, CDnGachaRouletteDlg* pRouletteDlg );
	void _MakeItemStatString( CDnState* pItemState, /*IN OUT*/ wstring& strItemStat );

public:
	CDnGachaDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGachaDlg(void);

	// �����κ��� ���ƿ� NPC�� ��ϵ� ��í�� �ε���
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

	// CDnGachaRouletteDlg ���� ȣ����.
	
	// �귿 ���ư��� ���ȿ� ��� ȣ����.
	void ProcessRoulette( bool bProcessRoulette ) { m_bProcessRoulette = bProcessRoulette; };

	// Gacha Preview ���� �̸� �ٲٶ�� ȣ����.
	void OnChangeItem( int iItemID );
	
	//////////////////////////////////////////////////////////////////////////

	// �귿 ������ �� ���̾�α� ���õ�.
	void SetRouletteDlg( CDnGachaRouletteDlg* pNameRouletteDlg, CDnGachaRouletteDlg* pStatRouletteDlg )
	{ 
		m_pNameRouletteDlg = pNameRouletteDlg; 
		m_pStatRouletteDlg = pStatRouletteDlg;
	};

	void OnChangeResolution();

	// �����κ��� ��í�� ������ ����� ��
	void OnRecvGachaResult( const SCRefreshCashInven* pPacket );
};
#endif //PRE_ADD_GACHA_JAPAN