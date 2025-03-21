#pragma once
#include "DnCustomDlg.h"


#ifdef PRE_MOD_APPELLATIONBOOK_RENEWAL

struct stUIAppellationItem
{
	CEtUIStatic* m_StaticIn;			// 칭호 소유
	CEtUIStatic* m_StaticOut;			// 칭호 미소유
	CEtUIStatic* m_StaticNewMark;		// 새로 습득한 New마크

	CEtUIStatic* m_StaticInfo[2];		// 기존의 칭호 설명
	CEtUIStatic* m_StaticNewInfo[2];	// 새로운 칭호 설명
};

#define MAX_APPITEM_PER_PAGE	10

struct stUIAppellationPage
{
	CEtUIStatic* m_StaticTitle;			// 컬렉션 제목
	CEtUIStatic* m_StaticDescription;	// 컬렉션 내용
	CEtUIStatic* m_StaticPercent;		// 컬렉션 수집률
	CEtUIStatic* m_StaticReward;		// 컬렉션 보상

	CEtUIStatic*		m_pStaticClearMark;		// 다 모았을때 클리어 도장마크
	CDnItemSlotButton*	m_pStaticItemSlot[2];	// 아이템 슬롯.
	CEtUIStatic*		m_pStaticItemCoin[2];	// 아이템 Coin

	stUIAppellationItem m_AppellationItem[ MAX_APPITEM_PER_PAGE ];
};

class CDnMissionAppellationTooltipDlg;
class CDnAppellationRewardCoinTooltipDlg;
class CDnAppellationBookDlg : public CDnCustomDlg
{
public:
	CDnAppellationBookDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnAppellationBookDlg( void );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

public:
	void SetMaxPage( int nMaxPage ) { m_nMaxPage = nMaxPage-1; }
	void SetPageUI( int nPage );							// 해당 페이지 컬렉션 정보 가지고 와서 셋팅
	void SetCurrentPageUI() { SetPageUI( m_nCurrentPage ); }
	void ClearPageUI();
	void ShowAppellationTooltip( float fMouseX, float fMouseY, int i, int j = -1 );

private:
	enum
	{
		E_IN = 0 ,
		E_OUT = 1,
		E_MAX
	};
	int					m_nCurrentPage;						// 현재 페이지
	int					m_nMaxPage;							// 최대 페이지 수
	CEtUIStatic*		m_pStaticCurrentPage;
	stUIAppellationPage	m_stUIAppellationPage;				// 페이지 표시 UI 컨트롤 모음
	CDnMissionAppellationTooltipDlg* m_pAppellationTooltip;	// 칭호 툴팁

	
	// 새로운 컨트롤
	CEtUIListBoxEx*		m_pListBoxEx;			// 리스트 박스
	bool				m_bIsSelectListBox;		// 리스트 박스 선택했는가?
	bool				m_bIsHaveCoinReward;	// 보상으로 Coin이 있는가?
	int					m_nRewardCoinValue;

	CDnAppellationRewardCoinTooltipDlg* m_pRewardCoinToolTipDlg;

public:
	// 새로운 함수
	void SetContents();
	void SelectContents(int nIndex);
	void SetAppellationToolTip(std::map<int, int>::iterator it, std::wstring& wszTooltip, bool bIsOpenAppellation);
	CEtUIListBoxEx* GetListBoxPtr()	{ return m_pListBoxEx; }
};


#else


struct stUIAppellationItem
{
	CEtUIStatic* m_StaticIn;			// 칭호 소유
	CEtUIStatic* m_StaticOut;			// 칭호 미소유
	CEtUIStatic* m_StaticInfo;			// 칭호 설명
};

#define MAX_APPITEM_PER_PAGE	10

struct stUIAppellationPage
{
	CEtUIStatic* m_StaticTitle;			// 컬렉션 제목
	CEtUIStatic* m_StaticDescription;	// 컬렉션 내용
	CEtUIStatic* m_StaticPercent;		// 컬렉션 수집률
	CEtUIStatic* m_StaticReward;		// 컬렉션 보상

	stUIAppellationItem m_AppellationItem[ MAX_APPITEM_PER_PAGE ];
};

class CDnMissionAppellationTooltipDlg;

class CDnAppellationBookDlg : public CDnCustomDlg
{
public:
	CDnAppellationBookDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnAppellationBookDlg( void );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

public:
	void SetMaxPage( int nMaxPage ) { m_nMaxPage = nMaxPage; }
	void SetPageUI( int nPage );							// 해당 페이지 컬렉션 정보 가지고 와서 셋팅
	void SetCurrentPageUI() { SetPageUI( m_nCurrentPage ); }
	void ClearPageUI();
	void ShowAppellationTooltip( float fMouseX, float fMouseY, int i, int j = -1 );

private:
	int					m_nCurrentPage;						// 현재 페이지
	int					m_nMaxPage;							// 최대 페이지 수
	CEtUIStatic*		m_pStaticCurrentPage1;
	CEtUIStatic*		m_pStaticCurrentPage2;
	stUIAppellationPage	m_stUIAppellationPage[2];			// 페이지 표시 UI 컨트롤 모음
	CDnMissionAppellationTooltipDlg* m_pAppellationTooltip;	// 칭호 툴팁
};


#endif //PRE_MOD_APPELLATIONBOOK_RENEWAL
