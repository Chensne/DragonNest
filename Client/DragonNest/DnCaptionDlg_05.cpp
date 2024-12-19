#include "StdAfx.h"
#include "DnCaptionDlg_05.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#define SHOW_MSG_BEFORE_CLOSE_TIME	100.f
#define POINT_LETTER_ANIM_TIME		1.f

CDnCaptionDlg_05::CDnCaptionDlg_05( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
{
	m_pStaticMessage	 = NULL;
	m_CurPointIdx		 = 0;
	m_PointLetterAnimTerm = 0.f;
}

CDnCaptionDlg_05::~CDnCaptionDlg_05(void)
{
}

void CDnCaptionDlg_05::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CaptionDlg_05.ui" ).c_str(), bShow );
}

void CDnCaptionDlg_05::InitialUpdate()
{
	m_pStaticMessage = GetControl<CDnMessageStatic>("ID_CAPTION0");
}

void CDnCaptionDlg_05::AddCaption(LPCWSTR wszMsg, DWORD dwColor)
{
	if (m_pStaticMessage == NULL)
	{
		_ASSERT(0);
		return;
	}

	m_Msg = wszMsg;
	m_pStaticMessage->SetMsgTime(SHOW_MSG_BEFORE_CLOSE_TIME);
	m_pStaticMessage->SetTextColor(dwColor);
	m_pStaticMessage->SetText(m_Msg.c_str());
	m_pStaticMessage->Show(true);

	m_CurPointIdx = 0;
	m_PointLetterAnimTerm = 0.f;

	m_bShow = true;
}

void CDnCaptionDlg_05::Process(float fElapsedTime)
{
	if (IsShow() == false || m_Msg.empty())
		return;

	m_pStaticMessage->SetMsgTime(SHOW_MSG_BEFORE_CLOSE_TIME);

	m_PointLetterAnimTerm += fElapsedTime;

	if (m_PointLetterAnimTerm >= POINT_LETTER_ANIM_TIME)
	{
		std::wstring str[3] = { L"", L".", L".." };
		m_CurPointIdx = (m_CurPointIdx + 1) % 3;
		m_pStaticMessage->SetText(std::wstring(m_Msg + str[m_CurPointIdx]));
		m_PointLetterAnimTerm = 0.f;
	}

	CDnCustomDlg::Process(fElapsedTime);
}

void CDnCaptionDlg_05::CloseCaption()
{
	if  (m_pStaticMessage->IsShow())
	{
		m_pStaticMessage->SetMsgTime(0.0f);
		m_pStaticMessage->SetText(L"");
		m_pStaticMessage->Show(false);
	}
}