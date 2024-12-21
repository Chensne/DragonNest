#pragma once
#include "EtUIDialog.h"
#include "DnQuestPathFinder.h"

struct Journal;
class CDnNotifierControls
{
public:
	CDnNotifierControls();

public:
	CEtUIStatic *m_pIcon;
	CEtUIStatic *m_pTitle;
	CEtUIStatic *m_pDistance;

	CEtUIStatic *m_pArrowBase;
	CEtUIStatic *m_pArrow;		
	CEtUIStatic *m_pArrowArrival;
	CEtUIStatic *m_pArrowWrong;
	CEtUIStatic *m_pArrowBattle;
	CEtUIStatic *m_pArrowFind;
	CEtUIStatic *m_pArrowArrivalDistance;

	CEtUIStatic *m_pTraceArrow;		
	CEtUIStatic *m_pTraceArrowArrival;
	CEtUIStatic *m_pTraceArrowWrong;
	CEtUIStatic *m_pTraceArrowBattle;
	CEtUIStatic *m_pTraceArrowFind;
	CEtUIStatic *m_pTraceArrowArrivalDistance;

	CEtUIStatic	*m_Board;
	CEtUIStatic	*m_OverBoard;

	SUICoord	m_IconCoord;
	SUICoord	m_TitleCoord;
	SUICoord	m_TextBoxCoord;
	SUICoord	m_DistanceCoord;
	SUICoord	m_ArrowCoord;
	SUICoord	m_BoardCoord;
	SUICoord	m_SelectCoord;

	CEtUITextBox *m_pTextBox;

#ifdef PRE_FIX_48865
	CEtUIButton *m_pButtonClose;
#endif

	CEtUIDialog *m_pDialog;
	Journal	*m_pJournal;
	std::deque<PathInfo>	m_pathResult;

	float   m_PosY;
	float	m_fPosY;
	float	m_fSmoothY;
	float	m_fDistance;

	int m_nNotifierIndex;
	int m_nArrowState;

	bool m_bShow;
	bool m_bMission;
	bool m_bMouseOver;
	bool m_bClicked;

	bool m_bTraceQuest;
	bool m_bNowTraceQuest;

	void Show( bool bShow );
	bool IsShow() { return m_bShow;}
	void SetPosY( float fY );	
	void PostRender();
	void DrawArrow();
	void SetNotifier( int nNotifier );
	void SetDialog( CEtUIDialog *pDialog ) { m_pDialog = pDialog; }
	void Refresh( const wchar_t *szTitle, const wchar_t *szDest, std::vector< boost::tuple<std::wstring, int, int> > &vecGetList, std::wstring szProgress );
	void UpdateDestinationMode();
	void ChangeArrowStateByPathCheck();
	void MoveY( float fY ) { m_fPosY = fY; }
	void SetDistance( float fDistance ) { m_fDistance = fDistance;}
	void Process( float fElapsedTime );
	void SetMission( bool bMission ) { m_bMission = bMission; }
	bool IsMission() { return m_bMission;}
	void SetPath( std::deque<PathInfo> path ) { m_pathResult = path; }
	void MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool IsMouseOver() { return m_bMouseOver; }
	void SetClicked( bool bClick ) {m_bClicked = bClick;}
	bool IsClicked() { return m_bClicked;} 
	void SetJournal( Journal *pJournal ) { m_pJournal = pJournal;}
	Journal *GetJournal() { return m_pJournal;}
	void SetQuestInfo(TQuest *Quest){m_Quest = Quest;}
	void ClearQuestInfo(){m_Quest = NULL;}
	void SetTraceQuest( bool bTrace )	{ m_bNowTraceQuest = bTrace; }

	static EtVector2 CalcArrowDestination( int nQuestIndex , int nJurnalStep );
	static int GetNotifyArrowState( int nQuestID , int nJurnalStep );
	static int CheckNotifyArrowStateByMap( int nArrowState , int nDestinationMapIndex );

protected:
	TQuest* m_Quest;
};

class CDnQuestSummaryInfoDlg : public CEtUIDialog
{
public:
	
	CDnQuestSummaryInfoDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	
	virtual ~CDnQuestSummaryInfoDlg(void);
	virtual bool FindControl( std::vector< CEtUIControl* > &vecControl, int nTypeCount, UI_CONTROL_TYPE *pType, bool bCheckCoveredControl, std::vector<SUICoord> &vecDlgCoord );
	virtual void OnChangeResolution();

public:
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) override;
	void Process( float fElapsedTime )  override;
	void InitialUpdate() override;
	void Initialize(bool bShow) override;
	void Render( float fElapsedTime ) override;

public:
	void RefreshNotifier();
	void RefreshMainNotifier();
	void RefreshSubNotifier();
	void RefreshMissionNotifier();
	void ClearAll();

	bool IsCanProgressQuest(int nQuestIndex);
	void SetPriorityMode(bool bTrue){m_bPriorityMode = bTrue;} 
	void RefreshPriortyNotifier();

	void Show(bool bShow);

protected:

	CDnNotifierControls m_NotifierControls[ DNNotifier::RegisterCount::Total ];	
	typedef CEtUIDialog BaseClass;

	std::vector< int >  m_renderIndices;
	std::map< int , float > m_fLastPos;
	SUICoord		m_SelectCoord;
	SUICoord		m_BlurCoord;
	
	int					m_nClickIndex;
	int					m_nOverIndex;
	int					m_lastOverIndex;
	float				m_fSelectAlpha;
	float				m_fOverIndexSmooth;
	float				m_fControlDistance;
	float				m_lastHeight;
	bool                m_bPriorityMode;        // �켱���� ���ı��
};

