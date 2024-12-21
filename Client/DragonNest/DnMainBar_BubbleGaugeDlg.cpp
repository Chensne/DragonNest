//
#include "StdAfx.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnMainDlg.h"
#include "DnTimeEventTask.h"
#include "DnSystemDlg.h"
#include "DnFadeInOutDlg.h"
#include "DnMainBar_BubbleGaugeDlg.h"
#include "DnMinimap.h"
#include "GameOption.h"
#include "DnVillageTask.h"
#include "TaskManager.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnMainBar_BubbleGaugeDlg::CDnMainBar_BubbleGaugeDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	: CEtUIDialog(dialogType, pParentDialog, nID, pCallback)
{
	m_pBubbleCount = NULL;
	m_pProgressBarCount = NULL;
	m_pProgressBarTime = NULL;
	m_nMaxBubbles = 0;
}

CDnMainBar_BubbleGaugeDlg::~CDnMainBar_BubbleGaugeDlg(void)
{
}



void CDnMainBar_BubbleGaugeDlg::InitialUpdate()
{
	m_pBubbleCount = GetControl<CEtUIStatic>("ID_TEXT_COUNT");
	m_pBubbleCount->SetText(L"");

	m_pProgressBarCount = GetControl<CEtUIProgressBar>("ID_PROGRESSBAR_COUNT");
	m_pProgressBarCount->SetProgress(0);

	m_pProgressBarTime = GetControl<CEtUIProgressBar>("ID_PROGRESSBAR_TIME");
	m_pProgressBarTime->SetProgress(0);

}

void CDnMainBar_BubbleGaugeDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("bubbledlg.ui").c_str(), bShow);
}

void CDnMainBar_BubbleGaugeDlg::Show(bool bShow)
{
	CEtUIDialog::Show(bShow);
}

void CDnMainBar_BubbleGaugeDlg::Render(float fElapsedTime)
{
	CEtUIDialog::Render(fElapsedTime);
}


void CDnMainBar_BubbleGaugeDlg::SetBubble(int Count, float fExpireTime, float fTotalTime)
{
	UpdateBubble(Count);
	UpdateExpireTime(fExpireTime);

	int	nCount = GetBubbleCount();
	float fExpire = GetExpireTime();

	if (nCount < 0)
		return;

	if (nCount == 0 || fExpire <= 1.0f || fTotalTime <= 1.0f)
	{
		ResetBubbles();
		return;
	}

	m_pBubbleCount->SetText(FormatW(L"%d", Count));
	//rlkt_test -- seems ok.
	m_pProgressBarCount->SetProgress((Count / 10.0f) * 100.0f); //10 test.
	m_pProgressBarTime->SetProgress((fExpireTime / fTotalTime) * 100.0f);

}

void CDnMainBar_BubbleGaugeDlg::ResetBubbles()
{
	m_pBubbleCount->SetText(L"");
	m_pProgressBarCount->SetProgress(0);
	m_pProgressBarTime->SetProgress(0);

}

bool CDnMainBar_BubbleGaugeDlg::IsValidClass(int ClassID)
{
	switch (ClassID)
	{
	case 3://sorceres
		return false;
		break;

	default:
		return true;
		break;
	}
}