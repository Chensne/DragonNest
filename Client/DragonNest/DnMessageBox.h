#pragma once
#include "EtUIDialog.h"
#include "DnInterface.h"
#include "DnSmartMoveCursor.h"

class CDnMessageBox : public CEtUIDialog
{
public:
	CDnMessageBox( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnMessageBox(void);

protected:
	CDnInterface::InterfaceTypeEnum m_emInterfaceType;

	CEtUIButton *m_pButtonOK;
	CEtUIButton *m_pButtonCancel;
	CEtUIButton *m_pButtonRetry;
	CEtUIButton *m_pButtonYes;
	CEtUIButton *m_pButtonNo;
	CEtUIButton *m_pButtonAuthPw;
	CEtUIButton *m_pButtonWaterBottle;
	CEtUIButton *m_pButtonWater;

	CEtUIStatic *m_pStaticMessage;
	CEtUIStatic *m_pStaticTitle;

	SUICoord m_ButtonCoord[3];
	std::vector<CEtUIButton*> m_vecButtonControl;

	std::wstring m_strTitle;
	DWORD m_dwTitleColor;

	// 스마트무브
	CDnSmartMoveCursor m_SmartMove;

	UINT	m_BtnType;
	bool m_bTopMost;
	bool m_bSmartMoveNoButton;	// No로 스마트무브가 가길 원한다.
	bool m_bIgnoreEnterHotkey;

	// 기본적으로 SetMessageBox 함수가 호출되면 기존의 내용을 덮는다.
	// 하지만 해당 메세지박스가 사용자 인풋을 받아 없어지기 전까지 유지되길 원한다면, 이걸 true로 설정하면 된다.
	bool m_bNoOverwrite;

protected:
	void ShowControl( bool bShow );

public:
	void SetTitle( LPCWSTR wszTitle, DWORD dwColor = EtInterface::textcolor::WHITE );
	bool IsTopMost() { return m_bTopMost; }

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void SetMessageBox( CDnInterface::InterfaceTypeEnum emType, LPCWSTR pwszMessage, UINT uType = MB_OK, int nID = -1, CEtUICallback *pCall = NULL, bool bTopMost = false, bool bSmartMoveNoButton = false, bool bNoBtn = false, bool bNoOverwrite = false );
};


//////////////////////////////////////////////////////////////////////////
// TextBox사용한 MessageBox

class CDnMessageBoxTextBox : public CDnMessageBox , public CEtUINameLinkInterface
{
public:
	CDnMessageBoxTextBox( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnMessageBoxTextBox();

protected:
	CEtUITextBox*	m_pTextMessage;

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void SetMessageBox( CDnInterface::InterfaceTypeEnum emType, LPCWSTR pwszMessage, UINT uType = MB_OK, int nID = -1, CEtUICallback *pCall = NULL, bool bTopMost = false, bool bSmartMoveNoButton = false, bool bNoBtn = false, bool bNoOverwrite = false );
	virtual void OnOpenHyperLink( const std::wstring& argString );

public:
	void AddMessage( LPCWSTR pwszMessage ) { if( m_pTextMessage ) m_pTextMessage->AddText( pwszMessage ); }

//---- PRE_ADD_JELLYCOUNT
	void AddMessage( LPCWSTR pwszMessage, const D3DCOLOR TextColor, DWORD dwFormat = UITEXT_NONE, const D3DCOLOR BgColor = 0 ){ 
		if( m_pTextMessage ) 
			m_pTextMessage->AddText( pwszMessage, TextColor, dwFormat, BgColor ); 
	}

	void ClearText(){
		m_pTextMessage->ClearText();
	}
// -----//

};
