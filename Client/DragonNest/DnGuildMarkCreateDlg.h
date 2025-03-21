#pragma once
#include "EtUIDialog.h"

class CDnGuildMarkInfoDlg;
class CDnItem;

class CDnGuildMarkCreateDlg : public CEtUIDialog, public CEtUICallback
{
public:
	CDnGuildMarkCreateDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildMarkCreateDlg(void);

#ifdef PRE_ADD_BEGINNERGUILD
	enum eMarkViewType
	{
		NoUse = 0,
		ShowForUserCreate = 1,
		CustomShow = 2,
	};
#endif

	struct SMarkInfo
	{
		int nMarkID;
		int nIconIndex;
		int nPriority;
		bool bMarkOnly;
		bool bCash;
#ifdef PRE_ADD_BEGINNERGUILD
		eMarkViewType markViewType;
#endif
	};

protected:
	enum
	{
		NUM_SELECT = 3,
		NUM_MARK = 16,
	};

	CDnItem *m_pGuildMarkItem;

	CEtUITextureControl *m_pTextureGuildMark0;
	CEtUITextureControl *m_pTextureGuildMark1;
	CEtUIStatic *m_pStaticGuildName;

	struct SMarkSelectCtrl
	{
		CEtUIStatic *m_pStaticBoard;
		CEtUIStatic *m_pStaticEmpty;
		CEtUIStatic *m_pStaticSlot[NUM_MARK];
		int m_nMarkID[NUM_MARK];
		CEtUITextureControl *m_pTextureMark[NUM_MARK];
		int m_nIconIndex[NUM_MARK];
		CEtUIStatic *m_pStaticCash[NUM_MARK];
		CEtUIStatic *m_pStaticSelect[NUM_MARK];
		CEtUIButton *m_pButtonPrior;
		CEtUIButton *m_pButtonNext;
		CEtUIStatic *m_pStaticPage;

		void ClearSlot()
		{
			m_pStaticEmpty->Show( false );
			for( int i = 0; i < NUM_MARK; ++i )
			{
				m_nMarkID[i] = 0;
				m_pTextureMark[i]->Show( false );
				m_nIconIndex[i] = -1;
				m_pStaticCash[i]->Show( false );
				m_pStaticSelect[i]->Show( false );
			}
		}
	};

	SMarkSelectCtrl m_sMarkSelectCtrl[NUM_SELECT];
	CEtUIButton *m_pButtonInfo;
	CEtUIButton *m_pButtonOK;

	CDnGuildMarkInfoDlg *m_pGuildMarkInfoDlg;

	int m_nCurPage[NUM_SELECT];
	int m_nMaxPage[NUM_SELECT];
	int m_nCurSelect[NUM_SELECT];

	DnEtcHandle m_hGuildMarkCreateEffect;

	void LoadGuildMark();
	void UpdateGuildMarkList();

	void UpdatePage( int nIndex );
	void UpdateMarkSlot( int nIndex );
	void OnChangeSelect();
	void UpdateGuildMark();

	std::vector<SMarkInfo> m_vecMarkInfo[NUM_SELECT];

public:
	void SetGuildMarkItem( CDnItem *pItem );
	void OnRecvGuildMark( short wGuildMarkBG, short wGuildMarkBorder, short wGuildMark );

	SMarkInfo GetMarkInfo( int nMarkID );
#ifdef PRE_ADD_BEGINNERGUILD
	int GetCreateEnableMarkCount(int type);
#endif

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
};