#pragma once


#ifdef PRE_ADD_CHAOSCUBE

#include "DnCustomDlg.h"

//-----------------------------------------------
// 카오스큐브 메인창.

class CDnChaosCubeStuffDlg : public CDnCustomDlg
{

private:

	CEtUIStatic * m_pStaticTitle; // 타이틀바.
	CEtUIStatic * m_pStaticHelp;  // 도움말.
	CEtUIStatic * m_pStaticPage;  // Page/MaxPage.

	CEtUIButton * m_pBtnPrev;     // 이전.
	CEtUIButton * m_pBtnNext;     // 다음.
	
	int m_DlgMode; // CDnChaosCubeDlg::EChildDlgMode. 재료창 or 결과물창.
	int m_nChaosNum; // 

	std::vector< CDnItem * > m_vItems; // 아이템목록.
	int m_nStuffItemID; // 재료아이템 아이디.

	std::map< int, INT64 > m_mapGold; // 결과 골드량.

	// Page처리.
	const int m_CountPerPage; // 페이지당 슬롯개수.
	int m_crrPage;			  // 현재페이지.
	int m_maxPage;			  // 전체페이지.
	

	std::vector< CDnItemSlotButton * > m_vSlotBtns; // 아이템슬롯들.
	std::vector< CEtUIStatic * > m_vSlotStatics;    //아이템슬롯 Static.

public:

	CDnChaosCubeStuffDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnChaosCubeStuffDlg(){
		ReleaseDlg();
	}

	void ReleaseDlg();

	// Override - CEtUIDialog //
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl * pControl, UINT uMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void Show( bool bShow );

public:

	// 재료창 or 결과물창.
	//void SetDlgMode( int mode, int nItemID, int nChaosNum, bool bSelf );
	void SetDlgMode( int mode, int nChaosNum, bool bSelf );

private:

	// 아이템목록 로드.
	//void LoadItems( int nChaosNum, int nItemID, bool bSelf );	
	void LoadItems( int nChaosNum, bool bSelf );	

	// 아이템목록 페이징처리.
	void RefreshList();

};

#endif