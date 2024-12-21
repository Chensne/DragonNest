#pragma once
#include "DnZoneMapDlg.h"

class CDnPVPZoneMapDlg : public CDnZoneMapDlg
{	
public:
	CDnPVPZoneMapDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnPVPZoneMapDlg(void);

protected:
	bool m_IsAllPlayerShow;//모든 플레이어 보이게 

protected:
	virtual void ProcessParty( float fElapsedTime );
	virtual void CreatePartyControl();
public:	

public:
	void SetAllPlayerShow(bool bSet ){m_IsAllPlayerShow =  bSet;};
	virtual void Render( float fElapsedTime );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	//virtual void Initialize(bool bShow);
	//virtual void InitialUpdate();
	//virtual void InitCustomControl( CEtUIControl *pControl );
	//virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	//virtual void Show( bool bShow );
	//virtual void Process( float fElapsedTime );
};