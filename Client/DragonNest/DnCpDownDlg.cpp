#include "StdAfx.h"
#include "DnCpDownDlg.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCpDownDlg::CDnCpDownDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CDnCountDlg( dialogType, pParentDialog, nID, pCallback  ),
	m_CpScoreShowTime(0.1f),
	m_ShowTime(0.f),
	m_HideDistConst(0.1f),
	m_HideYOffsetCoeffConst(-0.0005f),
	m_bStart(false),
	m_bHide(true),
	m_bClosed(true),
	m_CurAlpha(0),
	m_HideLimitY(0.0f)
{
}

void CDnCpDownDlg::Initialize( bool bShow )
{
	if( !m_pCountCtl )
		CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CpPointDlg.ui" ).c_str(), bShow );
}

void CDnCpDownDlg::InitialUpdate()
{
	m_pCountCtl = GetControl<CDnCpCount>("ID_CP_COUNT");
	m_pCountCtl->Show(false);
}

void CDnCpDownDlg::ShowCpDownCount(int score, float rightX, float posY)
{
	CDnCpCount* countCtl = static_cast<CDnCpCount*>(m_pCountCtl);
	//score = (score < 0.f) ? -score : score; // temp
	countCtl->SetValue(score);
	countCtl->SetStaticIdx(-1);
	countCtl->EnableCustomColor(0xffff0000);

	SUICoord dlgCoord;
	GetDlgCoord( dlgCoord );
	dlgCoord.fWidth = countCtl->GetControlWidth();
	dlgCoord.fHeight = countCtl->GetControlHeight();
	dlgCoord.fY = posY;

	SetDlgCoord( dlgCoord );

	m_HideLimitY = dlgCoord.fY + m_HideDistConst;

	m_bStart = true;
	m_bClosed = false;

	SetDelayTime(1000.f);
	m_CurAlpha = 255;

	m_ShowTime = 0.f;

	ShowCount(true);
}

void CDnCpDownDlg::Process(float fElapsedTime)
{
	if (m_bClosed == false)
	{
		SUICoord uiCoord;
		GetDlgCoord(uiCoord);
		uiCoord.fY += m_HideYOffsetCoeffConst;
		SetDlgCoord(uiCoord);
		SetDelayTime(1000.f);
	}

	if (m_bStart)
	{
		m_ShowTime += fElapsedTime;

		if (UpdateShowAni())
			Close();
	}
}

void CDnCpDownDlg::Render( float fElapsedTime )
{
	if( GetInterface().IsOpenBlind() || IsShow() == false)
		return;

	CEtUIDialog::Render( fElapsedTime );
}

#define _HIDE_ALPHA_DURATION 0.5f

int CDnCpDownDlg::CalcAlpha(bool bShowType, const SUICoord& uiCoord, float delta)
{
//	int alpha = 255;
// 	if (bShowType)
// 	{
// 		alpha = int(255 * m_ShowTime / m_CpScoreShowTime);
// 		if (alpha > 255)
// 			alpha = 255;
// 	}
// 	else
// 	{
		//alpha = int(255 * (m_HideLimitY - uiCoord.fY) / m_HideDistConst);
	int alpha = int(255 * (1.f - (m_ShowTime / _HIDE_ALPHA_DURATION)));
	if (alpha <= 0)
		alpha = 0;
// 	}

	return alpha;
}

bool CDnCpDownDlg::UpdateShowAni()
{
	SUICoord uiCoord;
	GetDlgCoord(uiCoord);

// 	if (m_CurAlpha >= 255)
// 		m_bHide = true;
	m_CurAlpha = CalcAlpha(!m_bHide, uiCoord, m_ShowTime);
	m_pCountCtl->SetAlpha(BYTE(m_CurAlpha));

	return (m_CurAlpha <= 0);
}

void CDnCpDownDlg::Close()
{
	m_bClosed = true;
	m_bStart = false;
	SetDelayTime(0.f);

	Show(false);
}