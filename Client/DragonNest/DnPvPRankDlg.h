#pragma once

#ifdef PRE_ADD_PVP_RANKING
#include "DnCustomDlg.h"

class CDnPvPRankDlg : public CDnCustomDlg
{
private:
	enum ERankType
	{
		ERankType_None = -1,
		ERankType_Colosseum = 0,
		ERankType_Ladder,
#ifdef PRE_ADD_DWC
		ERankType_DWC,
#endif
	};
	ERankType m_RankType;

	// Controls //
	CEtUIListBoxEx * m_pListRank;
	CEtUITreeCtl * m_pTreeJob;
	CEtUIComboBox * m_pComboCondition;
	CEtUIButton * m_pBtnSearch;
	CEtUIRadioButton * m_pBtnColosseum;
	CEtUIRadioButton * m_pBtnLadder;
	CEtUIIMEEditBox * m_pIMEEditBox;
#ifdef PRE_ADD_PVPRANK_INFORM
	CEtUIButton* m_pBtnRankInform;
#endif

	CEtUIButton * m_pStaticBack;  // <
	CEtUIButton * m_pStaticNext;  // >
	CEtUIButton * m_pStaticFirst; // <<
	CEtUIButton * m_pStaticLast;  // >>

	CEtUIStatic * m_pStaticMyPoint;  // 평점.
	CEtUIStatic * m_pStaticMyResult; // 결과.


	CEtUIStatic * m_pStaticTotRank;		// 전체순위.
	CEtUIStatic * m_pStaticFirstRank;	// 1차직업순위.
	CEtUIStatic * m_pStaticSecondRank;	// 2차직업순위.
	CEtUIStatic * m_pStaticGuild;		// 길드명.
	CEtUIStatic * m_pStaticName;		// 캐릭명.
	CEtUIStatic * m_pStaticPoint;		// 포인트.
	CEtUIStatic * m_pStaticResult;		// 결과.
	CEtUIStatic * m_pStaticSubTitle;
	CEtUITextureControl * m_pTextureControlGuild; // 길드아이콘.
	CEtUITextureControl * m_pTextureControl; // 콜로세움등급아이콘.

	CDnJobIconStatic * m_pJobIcon; // 직업아이콘. 

	struct SPvPTypeControl
	{
		CEtUIStatic * pResultTab;
		CEtUIStatic * pPointTab;
		CEtUIStatic * pResultText;
		CEtUIStatic * pPointText;
		SPvPTypeControl() : pResultTab( NULL ), pPointTab( NULL ), pResultText( NULL ), pPointText( NULL ) {}
	};
#define PVPTYPESIZE (ERankType::ERankType_Ladder + 1)
	SPvPTypeControl m_arrControls[ PVPTYPESIZE ];	
	
	std::map< int, CTreeItem * > m_mapParentJob; // 대분류 TreeItem.
	std::map< int, int > m_mapParentJobBySub;    // 서브클래스의 직종.
	
	std::vector< CEtUIStatic * > m_vecPages;

	enum EComboCondition
	{
		EComboCondition_NONE = 0,
		EComboCondition_Character = 1,
		EComboCondition_Guild,
	};

	int m_CrrPage;	   // 현재출력 Page.
	int m_RequestPage; // 요청 Page - RANKINGMAX 단위.
	int m_totScrollPage; // '>>' 버튼으로 넘길 수 있는 최대 페이지수.

	int m_Job;
	int m_SubClass;
	wchar_t * m_StrSearch;

	// 검색한 리스트정보.
	struct SCPvPRankList * m_pListColosseum;
	struct SCPvPLadderRankList * m_pListLadder;
#ifdef PRE_ADD_DWC
	struct SCGetDWCRankPage * m_pListDWC;
#endif // PRE_ADD_DWC

	int m_crrMouseOverPage;
	int m_crrSelectPage; // 현재 선택한 페이지번호 1~5.

	bool m_bScroll; // 스크롤여부 -  (<,>) 버튼:false,  (<<,>>) 버튼:true.

	LadderSystem::MatchType::eCode m_eCurrentLadderType;

public:
	CDnPvPRankDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnPvPRankDlg(){
		ReleaseDlg();
	}
	void ReleaseDlg();

	// Override - CEtUIDialog //
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl * pControl, UINT uMsg = 0 );		
	virtual void Show( bool bShow );


private:
	// job의 상위 직업 반환.
	int GetParentJob( int job );

	// 직업별 TreeControl 구축.
	void BuildTreeJob();

	// 직업선택.
	void SelectionTreeJob();

	// 내정보요청.
	void RequestMyInfo( ERankType rank , LadderSystem::MatchType::eCode eMatchType = LadderSystem::MatchType::None );
	

	void PageChange(); // 1Page씩 변경 & 유저리스트 설정.
	void PageScroll( int n ); // RANKINGPAGECOUNT 단위 변경.
	
	void RefreshPageControls();

	void Reset(); // 내정보초기화.

	void ResetList(); // 목록초기화.
	
	void ResetPvPList();


	std::wstring GetSubString( ERankType type );

	// 길드마크.
	void SetGuildIcon();

	// 직업별 순위설정.
	void SetJobRank( int job, INT64 classRank, INT64 subClassRank );

#ifdef PRE_ADD_DWC
	void ChangeStaticControlsState();
	void SetUIDWCTeamData(SCGetDWCRankPage* pInfo);
#endif // PRE_ADD_DWC

public:

	// 내정보.
	void SetInfoMyRankColosseum( struct TPvPRankingDetail * pInfo );    // 콜로세움.
	void SetInfoMyRankLadder( struct TPvPLadderRankingDetail * pInfo ); // 레더.

	// 캐릭터정보.
	void SetInfoColosseum( struct TPvPRankingDetail * pInfo ); // 콜로세움.
	void SetInfoLadder( struct TPvPLadderRankingDetail * pInfo ); // 레더.
	
	// 목록.
	void SetListColosseum( struct SCPvPRankList * pInfo ); // 콜로세움.
	void SetListLadder( SCPvPLadderRankList * pInfo ); // 레더.

#ifdef PRE_ADD_DWC
	void SetListDWC(SCGetDWCRankPage* pInfo); // DWC 대회.	
	void SetFindListDWC(SCGetDWCFindRank* pInfo); // DWC 검색 결과 셋팅
#endif // PRE_ADD_DWC
};

#endif // PRE_ADD_PVP_RANKING