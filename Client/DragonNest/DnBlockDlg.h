#pragma once
#include "EtUIDialog.h"
#include "DnIsolate.h"

class CDnBlockAddDlg;
class CDnBlockDlg : public CEtUIDialog, public CEtUICallback
{
public:
	enum eChildDlgID
	{
		CHILD_BLOCK_ADD
	};

	CDnBlockDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnBlockDlg(void);

protected:
	CEtUIListBox*	m_pNameList;
	CEtUIButton*	m_pButtonAdd;
	CEtUIButton*	m_pButtonDel;
	CDnBlockAddDlg*	m_pBlockAddDlg;

public:
	virtual void	Initialize( bool bShow );
	virtual void	InitialUpdate();
	virtual void	Show( bool bShow );
	virtual void	ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

	void			RefreshList();
	void			ShowAddDlg(bool bShow);
	void			HandleServerErrorCode(int nRet);

protected:
	void			Add( const wchar_t *wszName );
	void			Delete( const wchar_t *wszName );
	void			Reset();
	void			OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg );
	bool			MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	void			Process( float fElapsedTime );
};