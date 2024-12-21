#include "EtUIDialog.h"
#include "DnMasterTask.h"

class CDnMasterJoinDlg;

class CDnMasterListDlg : public CEtUIDialog
{
	enum eJobNumber
	{
		eJOB_First = 0,
		eJOB_Second = 1,
		eJOB_Third = 2,
	};

	struct SMasterInfo
	{
		CEtUIStatic * m_pStaticTitle;
		CEtUIStatic * m_pStaticLevel;
		CEtUIStatic * m_pStaticClass;
		CEtUIStatic * m_pStaticGender;
		CEtUIStatic * m_pStaticName;
		CEtUIStatic * m_pStaticRespect;
		CEtUIStatic * m_pStaticBase;

		SMasterInfo() :
			m_pStaticTitle( NULL ),
			m_pStaticLevel( NULL ),
			m_pStaticClass( NULL ),
			m_pStaticGender( NULL ),
			m_pStaticName( NULL ),
			m_pStaticRespect( NULL ),
			m_pStaticBase( NULL )
		{}

		~SMasterInfo() {}

		void Clear()
		{
			m_pStaticTitle->SetText(L"");
			m_pStaticLevel->SetText(L"");
			m_pStaticClass->SetText(L"");
			m_pStaticGender->SetText(L"");
			m_pStaticName->SetText(L"");
			m_pStaticRespect->SetText(L"");
			m_pStaticBase->Show( false );
		}

		bool IsInsideItem( float fX, float fY )
		{
			SUICoord uiCoords;
			m_pStaticBase->GetUICoord(uiCoords);
			return uiCoords.IsInside(fX, fY);
		}
	};

public :
	CDnMasterListDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnMasterListDlg(void);

protected :

	CEtUIStatic *		m_pStaticSelect;
	CEtUIStatic *		m_pStaticPage;
	CDnMasterJoinDlg *	m_pMasterJoinDlg;
	SMasterInfo			m_pMasterInfo[MasterSystem::Max::MasterPageRowCount];
	int					m_nCurSelect;
	bool				m_bRefresh;
	float				m_fRefreshDelta;
	UINT				m_nCurPage;
	UINT				m_nPrevPage;
	BYTE				m_cJob;
	BYTE				m_cGender;

	bool				m_bSearchButton;
	bool				m_bRenewButton;
	bool				m_bArrowButton;
	float				m_fSearchDelta;
	float				m_fRenewDelta;
	float				m_fArrowDelta;

	std::vector<int>	m_vJobID;

	int FindInsideItem( float fX, float fY );
	void UpdateSelectBar();
	void UpdateSelectPage();
	void ResetList();

public :
	void RefreshList();
	void RefreshSimple();
	void RefreshMaster();

public :
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};
