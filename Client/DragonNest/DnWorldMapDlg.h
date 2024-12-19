#pragma once
#include "DnCustomDlg.h"

class CDnWorldMapDlg : public CDnCustomDlg
{
public:
	CDnWorldMapDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnWorldMapDlg(void);

public:
	enum {
		WORLD_ICON_COUNT = 28,
	};

protected:
	CEtUIStatic *m_pStaticIcon[CLASSKINDMAX];
	CEtUIStatic *m_pCurIcon;
	CEtUIButton *m_pQuestTraceButton;

	float m_fElapsedTime;
	int m_nCurMapIndex;
	SUICoord m_sUICoord;

	int m_nTraceQuest;

	std::vector< boost::tuple< CDnWorldMapButton* , int, int > > m_vecButton;
	std::vector< std::pair< CEtUIStatic* , int > > m_vecRoadImage;
	std::vector< std::pair< CEtUIStatic* , int > > m_vecBridgeImage;
	std::vector< std::pair< CEtUIStatic* , int > > m_vecFogImage;

protected:	
	void UpdateButton();
	void UpdatePlayerIcon();
	void UpdateTraceQuest();
	void ProcessTraceQuest();
	CDnWorldMapButton *FindButton( int nMapIndex );	
	int GetLevelLimit( int nMapIndex );

public:
	void GetQuestTrace( int nQuestID )	{ m_nTraceQuest = nQuestID; }

	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

public:
	void OnLevelUp( int nLevel );
};
