#include "stdafx.h"
#include "DnLifeTooltipDlg.h"


CDnLifeTooltipDlg::CDnLifeTooltipDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pStaticName( NULL )
, m_pStaticState( NULL )
, m_pStaticTime( NULL )
, m_pStaticWater( NULL )
, m_pStaticWaterCount( NULL )
, m_pStaticWaterTime( NULL )
, m_pStaticCharName( NULL )
, m_nRenderCount( 0 )
{

}

CDnLifeTooltipDlg::~CDnLifeTooltipDlg()
{

}

void CDnLifeTooltipDlg::Initialize(bool bShow)
{
#if defined(PRE_REMOVE_FARM_WATER)
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "LifeTooltipDlg.ui" ).c_str(), bShow );
#else
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "LifeTooltip_WDlg.ui" ).c_str(), bShow );
#endif	// #if defined(PRE_REMOVE_FARM_WATER)
}

void CDnLifeTooltipDlg::InitialUpdate()
{
	m_pStaticName = GetControl<CEtUIStatic>( "ID_TEXT_MANE" );
	m_pStaticState = GetControl<CEtUIStatic>( "ID_TEXT_CONDITION" );
	m_pStaticTime = GetControl<CEtUIStatic>( "ID_TEXT_TIME" );
	m_pStaticWater = GetControl<CEtUIStatic>( "ID_TEXT_COUNT0" );
	m_pStaticWaterCount = GetControl<CEtUIStatic>( "ID_TEXT_COUNT1" );
	m_pStaticWaterTime = GetControl<CEtUIStatic>( "ID_TEXT_WATERCOUNT" );
	m_pStaticCharName = GetControl<CEtUIStatic>( "ID_TEXT_MASTERNAME" );
}

void CDnLifeTooltipDlg::SetInfo( CDnLifeSkillPlantTask::SPlantTooltip & info )
{
	m_pStaticName->SetText( info.m_wszName );
	m_pStaticState->SetText( info.m_wszState );

	if( Farm::AreaState::GROWING != info.m_eState )
	{
		m_pStaticTime->SetText( L"--:--" );
		m_pStaticWater->SetText( L"--:--" );
		m_pStaticWaterCount->SetText( L"--:--" );
		m_pStaticWaterTime->SetText( L"" );
	}
	else
	{
		m_pStaticTime->SetText( info.m_wszTime );
		m_pStaticWater->SetText( info.m_wszWater );
		m_pStaticWaterCount->SetText( info.m_wszWaterCount );
		m_pStaticWaterTime->SetText( info.m_wszWaterTime );
	}

#if defined(PRE_REMOVE_FARM_WATER)
	m_pStaticWater->Show( false );
	m_pStaticWaterCount->Show( false );
	m_pStaticWaterTime->Show( false );
#endif	// PRE_REMOVE_FARM_WATER

	if( Farm::AreaState::GROWING == info.m_eState || Farm::AreaState::COMPLETED == info.m_eState)
		m_pStaticCharName->SetText( info.m_wszPlayerName);
	else
		m_pStaticCharName->SetText(L"--");
}

void CDnLifeTooltipDlg::Render(float fElapsedTime)
{
	if( 0 == m_nRenderCount )
		return;

	m_nRenderCount = 0;

	CEtUIDialog::Render(fElapsedTime);
}

void CDnLifeTooltipDlg::CenterTooltip()
{
	float fWidth = 0.5f * GetScreenWidthRatio();
	float fHeight = 0.5f * GetScreenHeightRatio();

	SetPosition( fWidth, fHeight );
}

void CDnLifeTooltipDlg::UpdateTooltip( CEtUIDialog * pParent, float fX, float fY )
{
	SUICoord sDlgCoord;

	float fParentX, fParentY;
	fParentX = pParent->GetXCoord();
	fParentY = pParent->GetYCoord();

	GetDlgCoord( sDlgCoord );
	sDlgCoord.fX = fX + fParentX;
	sDlgCoord.fY = fY + fParentY;

	SetDlgCoord( sDlgCoord );
}

