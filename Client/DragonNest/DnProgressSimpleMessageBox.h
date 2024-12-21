#pragma once
#include "DnCustomDlg.h"
#include "DnInterface.h"
#include "DnSmartMoveCursor.h"
#include "DnLoadingButton.h"

class CDnProgressSimpleMessageBox : public CEtUIDialog
{
public:
	CDnProgressSimpleMessageBox(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnProgressSimpleMessageBox(void);

protected:
	enum
	{
		eBT_OK,
		eBT_CANCEL,
		eBT_RETRY,
		eBT_YES,
		eBT_NO,
		eBT_AUTHPW,
		eBT_BUCKET,
		eBT_USEPOT,
		eBT_MAX
	};

	CEtUIButton*		m_pButton[eBT_MAX];
	CEtUIStatic*		m_pStaticMessage;
	CEtUIStatic*		m_pStaticTitle;
	//CDnLoadingButton*	m_pLoadingBtn;
	CEtUIAnimation*		m_pLoadingAni;

	float				m_HeartBeatSec;
	float				m_HeartBeatCounter;
	std::wstring		m_MessageString;

protected:
	void ShowControl(bool bShow);

public:
	void SetTitle(LPCWSTR wszTitle, DWORD dwColor = EtInterface::textcolor::WHITE);
	void SetMessageBox(LPCWSTR pwszMessage, float heartBeatMsgSec, bool bbSetLoadingAni, int nID = -1, CEtUICallback *pCall = NULL);
	void SetMessageBox(LPCWSTR pwszMessage, float heartBeatMsgSec, float loadingAniSecForRound, int nID = -1, CEtUICallback *pCall = NULL);
	void SetHeartBeat(float heartBeatMsgSec);

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void Process(float fElapsedTime);
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg /* = 0 */ );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};
