#include "StdAfx.h"
#include "DnSkillMovieDlg.h"
#include "DnCashShopTask.h"
#include "DnMainFrame.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnSkillMovieDlg::CDnSkillMovieDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
				: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback, false)
{
	m_pMovieCtrl	= NULL;
	m_pSkillName	= NULL;
	m_pClose		= NULL;
	m_pNoMovieStatic = NULL;
}

void CDnSkillMovieDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize(CEtResourceMng::GetInstance().GetFullName( "SkillTreeAVIDlg.ui" ).c_str(), bShow);
}

void CDnSkillMovieDlg::InitialUpdate()
{
	m_pSkillName	= GetControl<CEtUIStatic>("ID_TEXT_SKILLNAME");
	m_pClose		= GetControl<CEtUIButton>("ID_BT_CLOSE");
	m_pNoMovieStatic = GetControl<CEtUIStatic>("ID_STATIC_AVI");
	if (m_pNoMovieStatic)
		m_pNoMovieStatic->Show(false);
}

void CDnSkillMovieDlg::InitCustomControl( CEtUIControl *pControl )
{
	m_pMovieCtrl	= GetControl<CDnMovieControl>("ID_MOVIE_SKILL");
}

void CDnSkillMovieDlg::Show(bool bShow)
{
	if (bShow == m_bShow)
		return;

	m_pMovieCtrl->Show(bShow);

	if (bShow == false)
		Stop();

	CDnCustomDlg::Show(bShow);
}

void CDnSkillMovieDlg::Process(float fElapsedTime)
{
	CDnCustomDlg::Process(fElapsedTime);
	MoveToTail( m_pMovieCtrl );
}

void CDnSkillMovieDlg::Render(float fElapsedTime)
{
	CDnCustomDlg::Render(fElapsedTime);
}

void CDnSkillMovieDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg /* = 0 */)
{
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl("ID_BT_CLOSE"))
			Show(false);
	}

	CDnCustomDlg::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnSkillMovieDlg::Play(const std::wstring& skillName, const std::string& fileName)
{
	if (m_pMovieCtrl)
	{
		m_pSkillName->ClearText();
		if (m_pNoMovieStatic)
			m_pNoMovieStatic->Show(fileName.empty());

		if (fileName.empty() == false && skillName.empty() == false)
		{
			m_pMovieCtrl->Play(fileName.c_str());
			if (m_pSkillName)
				m_pSkillName->SetText(skillName.c_str());
		}
		else
		{
			m_pMovieCtrl->Stop();
		}
	}
}

void CDnSkillMovieDlg::Stop()
{
	if (m_pMovieCtrl)
		m_pMovieCtrl->Stop();
}