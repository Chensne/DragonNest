#pragma once

#include "DnCustomDlg.h"

class CDnPvPOrderListDlg : public CDnCustomDlg
{
	enum eOrderListCommon
	{
		GROUPCAPTAIN_POSITION = 0,
		MAX_PLAYER = 8,
		TIME_LIMIT = 10,
	};

	struct sSelectSlot
	{
		UINT nUserSessionID;
		bool bIsAlive;
		CEtUIStatic *pStatic_Selection;
		CEtUIStatic *pStatic_Ready;
		CEtUIStatic *pStatic_Name;

		sSelectSlot()
		{
			nUserSessionID = 0;
			bIsAlive = true;
			pStatic_Selection = NULL;
			pStatic_Ready = NULL;
			pStatic_Name = NULL;
		}
	};

protected:

	sSelectSlot m_sSelectSlot[eOrderListCommon::MAX_PLAYER];
	CEtUIStatic *m_pStaticGroupCaptain;
	CEtUIStatic *m_pStaticTimeLimit;
	CEtUIStatic *m_pStaticTitleMsg;
	float m_fTimeLimit;

	UINT m_uiGroupCaptainSessionID;
	int m_nCurrentUserSize;

public:
	CDnPvPOrderListDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPvPOrderListDlg();

	virtual void Show( bool bShow );
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


public:
	void SelectPlayer( int nUserSessionID );
	void SelectGroupCaptain( int nUserSessionID );
	void MakePlayerList();
	void StartCountDown();

};