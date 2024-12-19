#include "StdAfx.h"
#include "DnStageClearRewardGoldDlg.h"

CDnStageClearRewardGoldDlg::CDnStageClearRewardGoldDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
{
	m_nRewardGold = 0;
	m_pGold = m_pSilver = m_pCopper = NULL;
	
	m_pStaticGold = m_pStaticSilver = m_pStaticCopper = NULL;
	m_fCountDelta = 0.0f;
}

CDnStageClearRewardGoldDlg::~CDnStageClearRewardGoldDlg()
{
}

void CDnStageClearRewardGoldDlg::InitialUpdate()
{
	m_pStaticGold = GetControl<CEtUIStatic>( "ID_GOLD0" );
	m_pStaticSilver = GetControl<CEtUIStatic>( "ID_SIOVER0" );
	m_pStaticCopper = GetControl<CEtUIStatic>( "ID_COPPER0" );

	m_pGold = GetControl<CDnDamageCount>( "ID_GOLD1" );
	m_pSilver = GetControl<CDnDamageCount>( "ID_SIOVER1" );
	m_pCopper = GetControl<CDnDamageCount>( "ID_COPPER1" );

	m_pGold->Init();
	m_pSilver->Init();
	m_pCopper->Init();

	m_pGold->SetRightAlign( true );
	m_pSilver->SetRightAlign( true );
	m_pCopper->SetRightAlign( true );
}

void CDnStageClearRewardGoldDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "StageGoldCountDlg.ui" ).c_str(), bShow );
}

void CDnStageClearRewardGoldDlg::Process( float fElapsedTime )
{
	CDnCustomDlg::Process(fElapsedTime);

	if( m_fCountDelta > 0.f ) {
		m_fCountDelta -= fElapsedTime;
		if( m_fCountDelta <= 0.f ) m_fCountDelta = 0.f;

		int nValue = static_cast<int>(m_nRewardGold * ( 1.f - m_fCountDelta ));
		RefreshValue( nValue );
	}
}

void CDnStageClearRewardGoldDlg::Render( float fElapsedTime )
{
	CDnCustomDlg::Render( fElapsedTime );
}

void CDnStageClearRewardGoldDlg::Set( int nValue )
{
	m_nRewardGold = nValue;

	m_fCountDelta = 1.f;
	RefreshValue( 0 );
}


void CDnStageClearRewardGoldDlg::RefreshValue( int nValue )
{
	// 일단 전부 하이드
	m_pStaticGold->Show( false );
	m_pStaticSilver->Show( false );
	m_pStaticCopper->Show( false );

	m_pGold->Show( false );
	m_pSilver->Show( false );
	m_pCopper->Show( false );
	m_pGold->SetFontSize( 1.f );
	m_pSilver->SetFontSize( 1.f );
	m_pCopper->SetFontSize( 1.f );

	// 계산
	int nG = nValue / 10000;
	int nS = (nValue % 10000) / 100;
	int nC = nValue % 100;

	m_pStaticCopper->Show( true );
	m_pCopper->Show( true );
	m_pCopper->SetValue( nC );
	if( nS > 0 ) {
		m_pStaticSilver->Show( true );
		m_pSilver->Show( true );
		m_pSilver->SetValue( nS );
	}
	if( nG > 0 ) {
		m_pStaticGold->Show( true );
		m_pGold->Show( true );
		m_pGold->SetValue( nG );
	}
}