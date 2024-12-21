#include "StdAfx.h"
#include "DnCpMinusScoreDlg.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCpMinusScoreDlg::CDnCpMinusScoreDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  ),
	m_CpScoreStatic(NULL),
	m_CpScoreColorConst(textcolor::RED),
	m_CpScoreShadowColorConst(textcolor::BLACK),
	m_CpScoreShowTime(0.1f),
	m_ShowWaitingTime(0.f),
	m_HideDistConst(0.1f),
	m_HideYOffsetCoeffConst(0.001f),
	m_bStart(false),
	m_bHide(true),
	m_bClosed(true),
	m_bBlink(false),
	m_bBlinker(false),
	m_CurAlpha(0)
{
}

void CDnCpMinusScoreDlg::Initialize( bool bShow )
{
	if( m_CpScoreStatic == NULL)
	{
		CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CpDlg.ui" ).c_str(), bShow );
	}
	InitialUpdate();
}

void CDnCpMinusScoreDlg::InitialUpdate()
{
	m_CpScoreStatic= GetControl<CEtUIStatic>("ID_CP");
	m_CpScoreStatic->Show(false);
}

void CDnCpMinusScoreDlg::ShowCpMinusScore(int score, float posXRatio, float posYRatio, bool bBlink)
{
	SUICoord dlgCoord;
	m_CpScoreStatic->GetUICoord(dlgCoord);
	dlgCoord.SetPosition(posXRatio, posYRatio);
	SetDlgCoord(dlgCoord);

	m_HideLimitY = posYRatio + m_HideDistConst;

	m_ShowWaitingTime	= 0.f;
	m_CurAlpha			= 0;
	m_bStart			= true;
	m_bClosed			= false;
	m_bHide				= false;
	m_bBlink			= bBlink;

	Show(true);
}

void CDnCpMinusScoreDlg::Process(float fElapsedTime)
{
	if (m_bClosed == false)
	{
		SUICoord uiCoord;
		GetDlgCoord(uiCoord);
		uiCoord.fY += m_HideYOffsetCoeffConst;
		SetDlgCoord(uiCoord);
	}

	if (m_bStart)
	{
		m_ShowWaitingTime += fElapsedTime;

		if (UpdateShowAni())
			Close();
	}
}

void CDnCpMinusScoreDlg::Render( float fElapsedTime )
{
	if( GetInterface().IsOpenBlind())
		return;

	CEtUIDialog::Render( fElapsedTime );
}

float CDnCpMinusScoreDlg::GetDelayTime() const
{
	return 1.f;
}

bool CDnCpMinusScoreDlg::IsShowCount()
{
	return (m_bClosed == false);
}

void CDnCpMinusScoreDlg::ShowCount( bool bShow )
{
	Show(bShow);
}

int CDnCpMinusScoreDlg::CalcAlpha(bool bShowType, const SUICoord& uiCoord, float delta)
{
	int alpha = 255;
	if (bShowType)
	{
		alpha = int(255 * m_ShowWaitingTime / m_CpScoreShowTime);
		if (alpha > 255)
			alpha = 255;
	}
	else
	{
		alpha = int(255 * (m_HideLimitY - uiCoord.fY) / m_HideDistConst);
		if (alpha <= 0)
			alpha = 0;
	}

	if (m_bBlink)
	{
		alpha = m_bBlinker ? int(alpha * 0.1f) : alpha;
		m_bBlinker = !m_bBlinker;
	}

	return alpha;
}

bool CDnCpMinusScoreDlg::UpdateShowAni()
{
	SUICoord uiCoord;
	GetDlgCoord(uiCoord);

	if (m_CurAlpha >= 255)
		m_bHide = true;
	m_CurAlpha = CalcAlpha(!m_bHide, uiCoord, m_ShowWaitingTime);
	DWORD color = (0x00ffffff & m_CpScoreStatic->GetTextureColor()) | (m_CurAlpha & 0xff) << 24;
	m_CpScoreStatic->SetTextureColor(color);

	return (m_CurAlpha <= 0 || uiCoord.fY >= m_HideLimitY);
}

void CDnCpMinusScoreDlg::Close()
{
	m_bClosed = true;
	m_bStart = false;

	Show(false);
}