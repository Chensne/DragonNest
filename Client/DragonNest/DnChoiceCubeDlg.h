#pragma once


#ifdef PRE_ADD_CHOICECUBE

#include "DnCustomDlg.h"

//-----------------------------------------------
// 카오스큐브 메인창.

class CDnChoiceCubeDlg : public CDnCustomDlg, public CEtUICallback
{

private:

	// Controls ------------

	//---  재료슬롯들  ---//
	CDnItemSlotButton * m_pSlotBtnSelected; // 선택한 재료슬롯.	

	std::vector< CDnItemSlotButton * > m_vSlotBtnMaterials; // 재료슬롯들.
	std::vector< CEtUIStatic * > m_vStaticSelect; // 재료슬롯 선택 Static.

	CEtUIStatic * m_pStaticLight; // 반짝임.
	CEtUIStatic * m_pStaticTitle; // 타이틀바.
	CEtUIStatic * m_pStaticHelp;  // 도움말.
	CEtUIStatic * m_pStaticPage;  // Page/MaxPage.

	CEtUIButton * m_pBtnPrev;     // 이전.
	CEtUIButton * m_pBtnNext;     // 다음.
	CEtUIButton * m_pBtnMaterial; // 재료.
	CEtUIButton * m_pBtnProduct;  // 결과물.
	CEtUIButton * m_pBtnConfirm;  // 확인.
	

	//---  결과물슬롯들  ---//
	CDnItemSlotButton * m_pSlotBtnSelectedResult; // 선택한 결과물슬롯.
	CEtUIStatic * m_pStaticLightResult; // 반짝임.
	CEtUIStatic * m_pStaticPageResult;  // Page/MaxPage.

	std::vector< CDnItemSlotButton * > m_vSlotBtnMaterialsResult; // 슬롯들.
	std::vector< CEtUIStatic * > m_vStaticSelectResult; // 슬롯 선택 Static.
	
	CEtUIStatic * m_pWheelRegionStuff; // 재료슬롯 휠 영역.
	CEtUIStatic * m_pWheelRegionResult; // 결과물슬롯 휠 영역.

	std::vector< CEtUIStatic * > m_vPeriodItems; // 결과슬롯에 기간제표시용 Static.
	

	// -----------------------


	class CDnItem * m_pItemChaos; // UI오픈용 초이스아이템.
	INT64 m_nChaosSerial;

	std::map< int, std::vector< CDnItem * > > m_mapStuffItems; // <ItemID, 인벤슬롯의 아이템들> 인벤에서 검색한 재료아이템들.
	// 같은ID의 아이템이 여러슬롯에 나뉘어있을때 각각 소모개수가 달라질 수 있으므로 따로저장.

	std::vector< int > m_vStuffItemIDs;     // 재료아이템들.
	std::map< int, int > m_vNeedStuffCount; // <ItemID, 개수> 믹스에 필요한 재료개수.

	// Page처리.
	const int m_CountPerPage; // 페이지당 슬롯개수.
	int m_crrPage;			  // 현재페이지.
	int m_maxPage;			  // 전체페이지.

	// 선택된재료 빛남처리.
	float m_fAlphaColor; // 알파값.
	short m_signLight;   // 선택재료슬롯 빛남처리 부호.

	bool m_bSelfItemList; // 재료템과 같은 아이템 로딩여부.

	// 선택한 재료슬롯 인덱스.
	int m_SelectedSlotIndex;




	// 결과 --------------------------------------------//	
	struct SResultItem
	{
		CDnItem * pItem;
		int nPeriod; // 기간제여부.
		INT64 nGold; // 골드.
		SResultItem() : pItem(NULL), nPeriod(0), nGold(0) {}
	};
	std::vector< SResultItem > m_vResultItems;     // 결과아이템들.
	int m_crrPageResult;			  // 현재페이지.
	int m_maxPageResult;			  // 전체페이지.
		
	// 선택한 결과물슬롯 인덱스.
	int m_SelectedSlotIndexResult;
	float m_fAlphaColorResult; // 알파값.
	short m_signLightResult;   // 선택재료슬롯 빛남처리 부호.

public:



public:

	CDnChoiceCubeDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnChoiceCubeDlg(){
		ReleaseDlg();
	}

	void ReleaseDlg();

	// Override - CEtUIDialog //
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl * pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void Show( bool bShow );

	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg );

public:

	// 초이스아이템.
	void SetChaoseItem( class CDnItem * pItem );
	class CDnItem * GetChoiceItem(){
		return m_pItemChaos; // UI오픈용 초이스아이템.
	}

	void UpdateItemList(); // 아이템목록갱신.

	void MixComplete(); // 믹스완료.

	void RecvResultItemComplet(); // 결과아이템 수령완료.

	void RecvMixStart( bool bEnableBtn ); // 믹스시작 응답.

	int GetResultItemID();

	int GetPeriodItemByID( int nItemID ); // 아이템의 기간제여부( 반환값 : 기간 ).

private:

	// Dlg Open시에 재료를 검색하여 등록한다.
	void OpenDlg();

	// Dlg Close시에 정리.
	void CloseDlg();

	// 재료슬롯에 재료등록.
	void RefreshStuffItemSlot();

	// 재료아이템선택.
	void SelectionStuffItem( const char * strCtlName );

	// 결과물슬롯에 결과물등록.
	void RefreshResultItemSlot();

	// 결과물아이템선택.
	void SelectionResultItem( const char * strCtlName );

	// Mix.
	void MixStart();

	// 재료아이템과 결과물아이템이 같은지 비교.
	bool CompareStuffnResult(); 
};


#endif