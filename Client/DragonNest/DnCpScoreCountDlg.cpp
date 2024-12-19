#include "StdAfx.h"
#include "DnCpScoreCountDlg.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCpScoreCountDlg::CDnCpScoreCountDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	: CDnCountDlg(dialogType, pParentDialog, nID, pCallback),
	m_fXGapConst(-0.005f),
	m_fYGapConst(0.007f),//-0.003f),
	m_fShowActionYGap(0.005f),
	m_ShowActionOffset(0.001f),
	m_bShowAction(false),
	m_DlgTargetPosY(0.0f)
{
}

void CDnCpScoreCountDlg::Initialize( bool bShow )
{
	if( !m_pCountCtl )
		CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CpPointDlg.ui" ).c_str(), bShow );
}

void CDnCpScoreCountDlg::InitialUpdate()
{
	m_pCountCtl = GetControl<CDnCpCount>("ID_CP_COUNT");
	m_pCountCtl->Show(false);
}

void CDnCpScoreCountDlg::ShowCpScoreCount(int cp, float rightX, bool bShowAction)
{
	CDnCpCount* countCtl = static_cast<CDnCpCount*>(m_pCountCtl);
	if (countCtl == NULL)
	{
		_ASSERT(0);
		return;
	}

	countCtl->SetValue( cp );
	countCtl->SetStaticIdx(10);

	SUICoord dlgCoord;
	GetDlgCoord( dlgCoord );
	dlgCoord.fWidth = m_pCountCtl->GetControlWidth();
	dlgCoord.fHeight = m_pCountCtl->GetControlHeight();
	//dlgCoord.fX = rightX - dlgCoord.fWidth;

	SetDlgCoord( dlgCoord );

	ShowCount(true, true);
}

float CDnCpScoreCountDlg::GetCpPointStaticWidth() const
{
	CDnCpCount* countCtl = static_cast<CDnCpCount*>(m_pCountCtl);
	if (countCtl == NULL)
	{
		_ASSERT(0);
		return -1.f;
	}

	SUIElement* pPointStatic = countCtl->GetElement(10);
	if (pPointStatic)
		return pPointStatic->fTextureWidth;

	return -1.f;
}

bool CDnCpScoreCountDlg::ProcessShowAction()
{
	SUICoord uiCoord;
	GetDlgCoord(uiCoord);
	uiCoord.fY += m_ShowActionOffset;
	SetDlgCoord(uiCoord);

	//OutputDebug("JudgeDlg:%d m_fHideElapsedTime:%f\n", alpha, m_fHideElapsedTime);
	int alpha = int(255 * (1.f - (m_DlgTargetPosY - uiCoord.fY) / m_fShowActionYGap));
	if (alpha > 255)
		alpha = 255;
	m_pCountCtl->SetAlpha(alpha);

	return (uiCoord.fY >= m_DlgTargetPosY || alpha >= 255);
}

void CDnCpScoreCountDlg::Process( float fElapsedTime )
{
	if (m_bShowAction)
	{
		if (ProcessShowAction())
			m_bShowAction = false;
	}
	
	CDnCountDlg::Process(fElapsedTime);
}

void CDnCpScoreCountDlg::Render( float fElapsedTime )
{
	if (GetInterface().IsOpenBlind())
		return;

	CEtUIDialog::Render( fElapsedTime );
}

void CDnCpScoreCountDlg::Hide()
{
	m_pCountCtl->SetAlpha(0);
	SetDelayTime(0.f);
}

void CDnCpScoreCountDlg::Shake(float offsetX)
{
	SUICoord dlgCoord;
	GetDlgCoord(dlgCoord);
	dlgCoord.SetPosition(dlgCoord.fX + offsetX, dlgCoord.fY);
	SetDlgCoord(dlgCoord);
}