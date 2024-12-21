#include "StdAfx.h"
#include "DnStageClearCounterDlg.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnStageClearCounterDlg::CDnStageClearCounterDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CDnCountDlg( dialogType, pParentDialog, nID, pCallback  )
	, m_ColorType(COLOR_NONE)
	, m_bWithSign(false)
{
}

CDnStageClearCounterDlg::~CDnStageClearCounterDlg(void)
{
}

void CDnStageClearCounterDlg::Initialize( bool bShow )
{
	if( !m_pCountCtl )
	{
		CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "StageClearCountDlg.ui" ).c_str(), bShow );
	}
}

void CDnStageClearCounterDlg::InitialUpdate()
{
	bool bUseCustomColor = true;
	switch( m_ColorType ) {
		case COLOR_YELLOW:
			m_pCountCtl = GetControl<CDnDamageCount>("ID_YELLOW");
			break;
		case COLOR_BLUE1:
			m_pCountCtl = GetControl<CDnDamageCount>("ID_BLUE1");
			break;
		default:
			break;
	}

	if( m_pCountCtl )
	{
		m_pCountCtl->Show(false);
		m_pCountCtl->Init();
		m_pCountCtl->UseCustomColor( bUseCustomColor );
	}
}

void CDnStageClearCounterDlg::Process( float fElapsedTime )
{
	CDnCountDlg::Process(fElapsedTime);
}

void CDnStageClearCounterDlg::Render( float fElapsedTime )
{
	CDnCountDlg::Render( fElapsedTime );
}

void CDnStageClearCounterDlg::CalcAndSetPos()
{
	SUICoord coord;
	GetDlgCoord(coord);
	coord.fWidth = m_pCountCtl->GetControlWidth();
	SetDlgCoord(coord);
}

void CDnStageClearCounterDlg::Set(int val, COLOR_TYPE color)
{
	m_ColorType = color;
	InitialUpdate();

	if (m_pCountCtl == NULL)
		return;

	if (m_bWithSign)
		m_pCountCtl->SetPlus(true);

	m_pCountCtl->SetValue(val);
	m_pCountCtl->SetFontSize( 0.9f );
	m_pCountCtl->SetAlpha(255);

	CalcAndSetPos();

	ShowCount(true, true);
	Show(true);
}

void CDnStageClearCounterDlg::ShowPlusSign(bool bEnable)
{
	m_bWithSign = bEnable;
}

void CDnStageClearCounterDlg::Hide()
{
	SetDelayTime(0.f);
	
	CDnCountDlg::Hide();
}