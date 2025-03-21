#include "StdAfx.h"
#include "DnProgressSimpleMessageBox.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnProgressSimpleMessageBox::CDnProgressSimpleMessageBox(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	: CEtUIDialog(dialogType, pParentDialog, nID, pCallback, true)
{
	m_pStaticMessage	= NULL;
	m_pStaticTitle		= NULL;
	m_HeartBeatSec		= 0.f;
	m_HeartBeatCounter	= 0.f;
	m_pLoadingAni		= NULL;
}

CDnProgressSimpleMessageBox::~CDnProgressSimpleMessageBox(void)
{
}

void CDnProgressSimpleMessageBox::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MessageBox_Ani.ui" ).c_str(), bShow );
}

void CDnProgressSimpleMessageBox::InitialUpdate()
{
	m_pButton[eBT_OK]		= GetControl<CEtUIButton>("ID_OK");
	m_pButton[eBT_CANCEL]	= GetControl<CEtUIButton>("ID_CANCEL");
	m_pButton[eBT_RETRY]	= GetControl<CEtUIButton>("ID_RETRY");
	m_pButton[eBT_YES]		= GetControl<CEtUIButton>("ID_YES");
	m_pButton[eBT_NO]		= GetControl<CEtUIButton>("ID_NO");
	m_pButton[eBT_AUTHPW]	= GetControl<CEtUIButton>("ID_BTN_AUTH_PW");
	m_pButton[eBT_BUCKET]	= GetControl<CEtUIButton>("ID_BT_BUCKET");
	m_pButton[eBT_USEPOT]	= GetControl<CEtUIButton>("ID_BT_USEPOT");

	m_pStaticMessage		 = GetControl<CEtUIStatic>("ID_MESSAGE");
	m_pStaticTitle			= GetControl<CEtUIStatic>("ID_TITLE");
	//m_pLoadingBtn			= GetControl<CDnLoadingButton>("ID_AIN_LOADING");
	m_pLoadingAni			= GetControl<CEtUIAnimation>("ID_ANI_LOADING");

	int i = 0;
	for (; i < eBT_MAX; ++i)
	{
		m_pButton[i]->Show(false);
	}
}

void CDnProgressSimpleMessageBox::SetMessageBox(LPCWSTR pwszMessage, float heartBeatMsgSec, bool bSetLoadingAni, int nID, CEtUICallback *pCall)
{
	m_nDialogID = nID;
	m_pCallback = pCall;
	m_MessageString = pwszMessage;
	m_pStaticMessage->SetText(pwszMessage);

	SetHeartBeat(heartBeatMsgSec);
	m_pLoadingAni->Play();
	//m_pLoadingBtn->SetAnimation(bSetLoadingAni);
}

void CDnProgressSimpleMessageBox::SetMessageBox(LPCWSTR pwszMessage, float heartBeatMsgSec, float loadingAniSecForRound, int nID, CEtUICallback *pCall)
{
	m_nDialogID = nID;
	m_pCallback = pCall;
	m_MessageString = pwszMessage;
	m_pStaticMessage->SetText(pwszMessage);

	SetHeartBeat(heartBeatMsgSec);
	//m_pLoadingBtn->SetAnimation((loadingAniSecForRound > 0.f), loadingAniSecForRound);
}

void CDnProgressSimpleMessageBox::Show( bool bShow )
{ 
	if (bShow == m_bShow)
		return;

	if (bShow == false)
	{
		SetDialogID(-1);
		SetCallback(NULL);
		m_HeartBeatSec = 0.f;
		m_HeartBeatCounter = 0.f;
	}

	CEtUIDialog::Show( bShow );
}

void CDnProgressSimpleMessageBox::Process(float fElapsedTime)
{
	CEtUIDialog::Process(fElapsedTime);

	if (!IsShow())
		return;

	if (m_HeartBeatSec > 0.f)
	{
		if (m_HeartBeatCounter >= m_HeartBeatSec)
		{
			if (m_pCallback)
				m_pCallback->OnUICallbackProc(GetDialogID(), EVENT_BUTTON_CLICKED, NULL);
			m_HeartBeatCounter = 0.f;
		}
		else
		{
			m_HeartBeatCounter += fElapsedTime;
		}
	}
}

bool CDnProgressSimpleMessageBox::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnProgressSimpleMessageBox::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg /* = 0 */ )
{
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
// 		if (IsCmdControl("ID_AIN_LOADING") && m_pLoadingBtn->IsAnimating())
// 		{
// 		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnProgressSimpleMessageBox::SetHeartBeat(float heartBeatMsgSec)
{
	m_HeartBeatSec = heartBeatMsgSec;
	m_HeartBeatCounter = 0.f;
}
