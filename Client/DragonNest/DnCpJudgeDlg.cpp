#include "StdAfx.h"
#include "DnCpJudgeDlg.h"
#include "DnInterface.h"
#include "DnCpJudgeControl.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCpJudgeDlg::CDnCpJudgeDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback)
	, m_bAction(false)
	, m_bHide(false)
	, m_fHideElapsedTime(0.f)
	, m_pJudgeCtrl(NULL)
	, m_bClosed(true)
	, m_HideYOffsetCoeff(-0.005f)
	, m_HideXOffsetCoeff(-0.02f)
	, m_HideXOffsetCoeff2(-0.01f)
	, m_ShadowColorValue(0xff000000)
	, m_fHideWaitingTime(0.f)
	, m_fHideLimitY(0.3f)
	, m_HideDist(0.f)
{
	m_Color = 0xffffffff;
}

void CDnCpJudgeDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CpJudgeDlg.ui" ).c_str(), bShow );
}

void CDnCpJudgeDlg::InitCustomControl(CEtUIControl *pControl)
{
	if (pControl == NULL)
	{
		_ASSERT(0);
		return;
	}

	m_pJudgeCtrl = static_cast<CDnCpJudgeControl*>(pControl);
}

void CDnCpJudgeDlg::Process(float fElapsedTime)
{
	if (IsClosed())
		return;

	if (m_bAction)
	{
		if (UpdateShowAni())
			m_bAction = false;
	}

	if (m_bHide)
	{
		m_fHideWaitingTime -= fElapsedTime;

		if (m_fHideWaitingTime <= 0.f)
		{
			m_fHideElapsedTime += fElapsedTime;

			if (UpdateHideAni())
				Close();
		}
	}
}

void CDnCpJudgeDlg::Render(float fElapsedTime)
{
	if (GetInterface().IsOpenBlind() || IsClosed())
		return;

	CEtUIDialog::Render(fElapsedTime);
}

void CDnCpJudgeDlg::SetCoords(SUICoord& coord)
{
	SetDlgCoord(coord);
}

void CDnCpJudgeDlg::SetCpJudge(CDnInterface::eCpJudgeType type, float relativeX, float relativeY, float hideTargetY, float delay)
{
	m_fHideElapsedTime	= 0.f;
	m_bHide				= false;

// 	JUDGE_AERIALCOMBO,
// 		JUDGE_SKILLCOMBO,
// 		JUDGE_CRITICAL,
// 		JUDGE_DOWNATTACK,
// 		JUDGE_FINISHATTACK,
// 		JUDGE_GENOCIDE,
// 		JUDGE_SHIELDBREAK,
// 		JUDGE_STUN,
// 		JUDGE_SUPERARMORBREAK,

	CDnCpJudgeControl::eCpJudgeType presentType = CDnCpJudgeControl::JUDGE_NONE;
	if (type == CDnInterface::CpType_Critical)					{	presentType = CDnCpJudgeControl::JUDGE_CRITICAL;		}
	else if (type == CDnInterface::CpType_Stun)					{	presentType = CDnCpJudgeControl::JUDGE_STUN;			}
	else if (type == CDnInterface::CpType_SuperArmorAttack)		{	presentType = CDnCpJudgeControl::JUDGE_SUPERARMORBREAK;	}
	else if (type == CDnInterface::CpType_BrokenShield)			{	presentType = CDnCpJudgeControl::JUDGE_SHIELDBREAK;		}
	else if (type == CDnInterface::CpType_Genocide)				{	presentType = CDnCpJudgeControl::JUDGE_GENOCIDE;		}
	else if (type == CDnInterface::CpType_AirCombo)				{	presentType = CDnCpJudgeControl::JUDGE_AERIALCOMBO;		}
	else if (type == CDnInterface::CpType_FinishAttack)			{	presentType = CDnCpJudgeControl::JUDGE_FINISHATTACK;	}
	else if (type == CDnInterface::CpType_SkillCombo)			{	presentType = CDnCpJudgeControl::JUDGE_SKILLCOMBO;		}
	else if (type == CDnInterface::CpType_Rescue)				{	presentType = CDnCpJudgeControl::JUDGE_RESCUE;			}
	else if (type == CDnInterface::CpType_PropBreak)			{	presentType = CDnCpJudgeControl::JUDGE_PROPBREAK;		}
	else if (type == CDnInterface::CpType_GetItem)				{	presentType = CDnCpJudgeControl::JUDGE_GETITEM;			}
	else 
		return;

	m_pJudgeCtrl->SetJudge(presentType);

	SUICoord dlgCoord, ctrlCoord;
	m_pJudgeCtrl->GetUICoord(ctrlCoord);
	GetDlgCoord(dlgCoord);
	m_TargetCoord.fX = dlgCoord.fX;
	dlgCoord.fX = GetScreenWidthRatio();
	dlgCoord.fY = relativeY;
	dlgCoord.fWidth = ctrlCoord.fWidth;
	dlgCoord.fHeight = ctrlCoord.fHeight;
	SetCoords(dlgCoord);

	m_TargetCoord.fY = relativeY;
	m_fHideLimitY = hideTargetY;

	m_bAction	= true;
	m_bClosed	= false;
}

bool CDnCpJudgeDlg::UpdateShowAni()
{
	SUICoord uiCoord;
	GetDlgCoord(uiCoord);
	uiCoord.fX += m_HideXOffsetCoeff;
	SetCoords(uiCoord);

	return (uiCoord.fX <= m_TargetCoord.fX);
}

#define _HIDE_ALPHA_DURATION 0.1f

bool CDnCpJudgeDlg::UpdateHideAni()
{
	SUICoord uiCoord;
	GetDlgCoord( uiCoord );
	uiCoord.fX -= m_HideXOffsetCoeff2;
	//uiCoord.fY += m_HideYOffsetCoeff;
	SetCoords(uiCoord);

	//OutputDebug("uicoord.y:%f m_fHideLimitY:%f\n", uiCoord.fY, m_fHideLimitY);

	//int alpha = int(255 * (uiCoord.fY - m_fHideLimitY) / m_HideDist);
	int alpha = int(255 * (1.f - (m_fHideElapsedTime / _HIDE_ALPHA_DURATION)));
	if (alpha <= 0)
		alpha = 0;
//	OutputDebug("JudgeDlg:%d m_fHideElapsedTime:%f\n", alpha, m_fHideElapsedTime);
	m_Color = (0x00ffffff & m_Color) | (alpha & 0xff) << 24;
	m_pJudgeCtrl->SetColor(m_Color);
	
	//return (uiCoord.fY <= m_fHideLimitY || alpha <= 0);
	//return (alpha <= 0);
	return (uiCoord.fX > GetScreenWidthRatio());
}

void CDnCpJudgeDlg::Close()
{
	m_pJudgeCtrl->SetColor(0x0);
	m_bClosed = true;
}

void CDnCpJudgeDlg::ShowCount(bool bShow)
{
	m_pJudgeCtrl->Show(bShow);
}

bool CDnCpJudgeDlg::IsShowCount()
{
	return (m_pJudgeCtrl->GetColor() != 0);
}

void CDnCpJudgeDlg::Hide(float waitingTime)
{
	SUICoord uiCoord;
	GetDlgCoord(uiCoord);

	m_fHideWaitingTime	= waitingTime;
	m_HideDist			= uiCoord.fY - m_fHideLimitY;
	m_bHide				= true;
}

float CDnCpJudgeDlg::GetDelayTime() const
{
	float ret = 1.f;
	if (m_bClosed == true) 
		ret = -1.f;

	return ret;
}