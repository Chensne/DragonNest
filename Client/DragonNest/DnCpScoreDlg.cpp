#include "StdAfx.h"
#include "DnCpScoreDlg.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCpScoreDlg::CDnCpScoreDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
{
	m_pBg = NULL;
}

void CDnCpScoreDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CpBackground.ui" ).c_str(), bShow );

	GetDlgCoord(m_BaseUICoord);
}

void CDnCpScoreDlg::InitialUpdate()
{
	m_pBg = GetControl<CEtUIStatic>("ID_SCORE_BACKGROUND");
}

void CDnCpScoreDlg::Process(float fElapsedTime)
{
}

void CDnCpScoreDlg::Render( float fElapsedTime )
{
	if( GetInterface().IsOpenBlind())
		return;

	CEtUIDialog::Render( fElapsedTime );
}

#define SCALE_RESERVE_RATIO 1.2f

void CDnCpScoreDlg::ShowDlg(const SUICoord& standard)
{
	SUICoord bgCoord, dlgCoord;
	m_pBg->GetUICoord(bgCoord);
	float scaleValue = (standard.fWidth / bgCoord.fWidth) * SCALE_RESERVE_RATIO;
	m_pBg->SetTextureWidthScale(scaleValue);

	dlgCoord = standard;
	dlgCoord.fX = standard.fX - (bgCoord.fWidth * scaleValue - standard.fWidth) * 0.5f;

	SetDlgCoord(dlgCoord);
}

SUICoord CDnCpScoreDlg::GetBaseBGCoord() const
{
	return m_BaseUICoord;
}