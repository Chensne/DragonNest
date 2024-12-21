#include "StdAfx.h"
#include "DnProgressDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnProgressDlg::CDnProgressDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
	, m_pStaticText(NULL)
	, m_pProgressBarTime(NULL)
	, m_fProgressBarTime(0.0f)
	, m_RemainTime(0.0f)
{
	m_bManualTimeCtrl	= false;
	m_bManualClose		= false;
}

CDnProgressDlg::~CDnProgressDlg(void)
{
}

void CDnProgressDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "ProgressDlg.ui" ).c_str(), bShow );
}

void CDnProgressDlg::InitialUpdate()
{
	m_pStaticText = GetControl<CEtUIStatic>("ID_TEXT");
	m_pProgressBarTime = GetControl<CEtUIProgressBar>("ID_PROGRESSBAR_TIME");
}

void CDnProgressDlg::Process(float fElapsedTime)
{
	CEtUIDialog::Process(fElapsedTime);

	if (IsShow() == false)
		return;

	if (m_RemainTime <= 0.0f)
	{
		ManualClose();
	}
	else
	{
		if (m_bManualTimeCtrl == false)
			m_RemainTime -= fElapsedTime;

		if (m_fProgressBarTime != 0.f)
			m_pProgressBarTime->SetProgress((m_RemainTime / m_fProgressBarTime) * 100.0f);
		else
			m_pProgressBarTime->SetProgress(0.0f);
	}
}

void CDnProgressDlg::ManualClose()
{
	SetDialogID(-1);
	SetCallback(NULL);
	m_fProgressBarTime = 0.0f;
	m_RemainTime = 0.0f;

	if (m_pCallback)
		m_pCallback->OnUICallbackProc(GetDialogID(), EVENT_DIALOG_HIDE, NULL);

	m_bShow = false;
	CEtUIDialog::Show(false);
}

void CDnProgressDlg::Show(bool bShow)
{ 
	if (m_bManualClose && bShow == false)
		return;

	if (bShow == m_bShow)
		return;

	if (bShow == false)
	{
		SetDialogID(-1);
		SetCallback(NULL);
		m_fProgressBarTime = 0.0f;
		m_RemainTime = 0.0f;
	}
	else
	{
		m_pProgressBarTime->SetProgress(100.0f);
	}

	if (m_pCallback)
	{
		m_pCallback->OnUICallbackProc(GetDialogID(), bShow?EVENT_DIALOG_SHOW:EVENT_DIALOG_HIDE, NULL);
	}

	CEtUIDialog::Show(bShow);
	//CDnMouseCursor::GetInstance().ShowCursor( bShow );
}

void CDnProgressDlg::SetInfo(LPCWSTR pwszMessage, float fTotalTime, float fRemainTime, bool bManualTimeCtrl, bool bManualClose, int nID, CEtUICallback *pCall)
{
	m_nDialogID			= nID;
	m_pCallback			= pCall;
	m_fProgressBarTime	= fTotalTime;
	m_RemainTime		= fRemainTime;
	m_bManualTimeCtrl	= bManualTimeCtrl;
	m_bManualClose		= bManualClose;

	m_pStaticText->SetText(pwszMessage);
}

void CDnProgressDlg::SetElapsedTime(float fElapsedTime)
{
	fElapsedTime = min(fElapsedTime, m_fProgressBarTime);
	fElapsedTime = max(fElapsedTime, 0.0f);
	m_RemainTime = fElapsedTime;
}