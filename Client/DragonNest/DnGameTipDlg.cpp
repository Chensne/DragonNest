#include "StdAfx.h"
#include "DnGameTipDlg.h"
#include "DNClientScriptAPI.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGameTipDlg::CDnGameTipDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnSlideCaptionDlg( dialogType, pParentDialog, nID, pCallback )
{
}

CDnGameTipDlg::~CDnGameTipDlg(void)
{
}

void CDnGameTipDlg::InitialUpdate()
{
	CDnSlideCaptionDlg::InitialUpdate();

	// 위치 이동 할필요 없다. 블라인드에 맞게 process에서 알아서 움직이기 때문.
	//m_pCation->SetPosition();
}

void CDnGameTipDlg::UpdateBlind()
{
	m_uiBlind.fWidth = GetScreenWidthRatio();
	m_uiBlind.fHeight = m_pCation->GetUICoord().fHeight;
	m_uiBlind.fX = -GetScreenWidthBorderSize();
	m_uiBlind.fY = GetScreenHeightRatio() - GetScreenHeightBorderSize() - m_uiBlind.fHeight;
	// 현재해상도를 디폴트해상도로 나눈 값에 스케일곱한 값 - 왼쪽값에서 1.0뺀 후 반으로 나눈 값 = 이게 현재 화면 최하단 float값.
	// 여기서 Height만큼 빼면 바닥에 붙어서 나온다.
}

void CDnGameTipDlg::Render( float fElapsedTime )
{
	if( CGlobalInfo::GetInstance().IsPlayingCutScene() ) return;
	if( IsPlayingTriggerCamera() ) return;

	CDnSlideCaptionDlg::Render( fElapsedTime );
}