#include "StdAfx.h"
#include "DnFadeInOutDlg.h"
#include "DnLocalPlayerActor.h"
#include "DnMessageBox.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnFadeInOutDlg::CDnFadeInOutDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
	, m_FadeMode(modeEnd)
	, m_fConst(1.0f)
	, m_fFadeTime(0.0f)
	, m_srcColor((DWORD)0x00000000)
	, m_renderColor((DWORD)0x00000000)
	, m_destColor((DWORD)0x00000000)
	, m_bCallFunc(true)
{
}

CDnFadeInOutDlg::~CDnFadeInOutDlg(void)
{
}

void CDnFadeInOutDlg::InitialUpdate()
{
	POINT pt;
	pt.x = 0; pt.y = 0;
	PointToFloat( pt, m_ScreenCoord.fX, m_ScreenCoord.fY );
	m_ScreenCoord.fWidth = GetScreenWidthRatio();
	m_ScreenCoord.fHeight = GetScreenHeightRatio();
}

void CDnFadeInOutDlg::CheckFadeComplete()
{
	emFADE_MODE mode = GetFadeMode();
	if( mode == modeEnd )
	{
		if( m_pCallback && !m_bCallFunc )
		{
			m_pCallback->OnUICallbackProc( GetDialogID(), EVENT_FADE_COMPLETE, NULL );
//			m_pCallback = NULL;
			m_bCallFunc = true;
		}
	}
}

void CDnFadeInOutDlg::Process( float fElapsedTime )
{
	if( GetFadeMode() == modeEnd )
		return;

	if( m_fFadeTime > 0.0f )
	{
		EtColorLerp( &m_renderColor, &m_srcColor, &m_destColor, 1.0f-(1/powf(m_fConst,2)*powf(m_fFadeTime,2) ) );
		m_fFadeTime -= fElapsedTime;
		CDnLocalPlayerActor::ShowCrosshair( false );
	}
	else
	{
		SetFadeMode(modeEnd);
		m_renderColor = m_destColor;
		CheckFadeComplete();
		if( m_renderColor.a == 0.f )
			CDnLocalPlayerActor::ShowCrosshair( true );
	}
}

void CDnFadeInOutDlg::Render( float fElapsedTime )
{
	if( m_renderColor.a == 0.0f )
		return;

	DrawRect( m_ScreenCoord, m_renderColor );


	if( CDnInterface::IsActive() ) {
		CDnMessageBox *pMessageBox = GetInterface().GetMessageBox();
		if( pMessageBox && pMessageBox->IsShow() && pMessageBox->IsTopMost() )
			GetInterface().GetMessageBox()->Render( fElapsedTime );
	}
}

void CDnFadeInOutDlg::OnChangeResolution()
{
	CEtUIDialog::OnChangeResolution();

	POINT pt;
	pt.x = 0; pt.y = 0;
	PointToFloat( pt, m_ScreenCoord.fX, m_ScreenCoord.fY );
	m_ScreenCoord.fWidth = GetScreenWidthRatio();
	m_ScreenCoord.fHeight = GetScreenHeightRatio();
}

void CDnFadeInOutDlg::SetFadeColor( DWORD srcColor, DWORD destColor, bool bForceApplySrcColor )
{
//	m_srcColor = srcColor; 
//	m_renderColor = m_srcColor;
	if( bForceApplySrcColor ) {
		m_renderColor = srcColor;
		m_srcColor = srcColor;
	}
	else m_srcColor = m_renderColor;
	m_destColor = destColor;
}

void CDnFadeInOutDlg::SetFadeTime( float fFadeTime ) 
{ 
	m_fConst = fFadeTime;
	m_fFadeTime = fFadeTime;
}

void CDnFadeInOutDlg::SetFadeMode( emFADE_MODE emFadeMode ) 
{ 
	m_FadeMode = emFadeMode;
	m_bCallFunc = false; 
}

CDnFadeForBlowDlg::CDnFadeForBlowDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CDnFadeInOutDlg( dialogType, pParentDialog, nID, pCallback )
	, m_nStep(0)
	, m_fFadeTimeForBlow( 0.f )
	, m_bUseInvert( true )
{
}

CDnFadeForBlowDlg::~CDnFadeForBlowDlg(void)
{
}

void CDnFadeForBlowDlg::Process( float fElapsedTime )
{
	if( m_nStep == 0 )
		return;

	CDnFadeInOutDlg::Process( fElapsedTime );

	if( m_nStep == 1 )
	{
		if( m_fFadeTime <= 0.0f )
			m_nStep = 2;
	}
	else if( m_nStep == 2 )
	{
		m_fFadeTimeForBlow -= fElapsedTime;
		if( m_fFadeTimeForBlow <= 0.0f )
		{
			m_destColor.a = 0.0f;
			SetFadeColor( m_srcColor, m_destColor, false );
			SetFadeTime( 1.0f );
			SetFadeMode( CDnFadeInOutDlg::modeBegin );
			m_nStep = 3;
		}
	}
	else if( m_nStep == 3 )
	{
		if( m_fFadeTime <= 0.0f && GetFadeMode() == modeEnd )
			m_nStep = 0;
	}
}

void CDnFadeForBlowDlg::Render( float fElapsedTime )
{
	if( !IsShow() )
		return;

	if( m_bUseInvert )
	{
		if( m_nStep == 1 )
		{
			CEtSprite::GetInstance().End();
			CEtSprite::GetInstance().Begin( D3DXSPRITE_ALPHABLEND );
			CEtDevice *pEtDevice = GetEtDevice();
			IDirect3DDevice9 *pd3dDevice = (IDirect3DDevice9*)pEtDevice->GetDevicePtr();
			GetEtDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVDESTCOLOR );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVDESTCOLOR );
		}
	}

	DrawRect( m_ScreenCoord, m_renderColor );

	if( m_bUseInvert )
	{
		if( m_nStep == 1 )
		{
			CEtSprite::GetInstance().End();
			CEtSprite::GetInstance().Begin( D3DXSPRITE_DONOTSAVESTATE );
		}
	}
}

bool CDnFadeForBlowDlg::IsShow() const
{
	if( m_nStep == 0 )
		return false;

	if( m_renderColor.a == 0.0f )
		return false;

	return true;
}

void CDnFadeForBlowDlg::SetFadeForBlow( float fFadeTime, DWORD dwColor )
{
	if( m_nStep == 0 || m_nStep == 3 )
	{
		m_fFadeTimeForBlow = fFadeTime;
		m_srcColor = m_destColor = dwColor;
		m_srcColor.a = 0.0f;

		if( m_bUseInvert )
			SetFadeTime( 0.5f );
		else
			SetFadeTime( 0.1f );
		SetFadeMode( CDnFadeInOutDlg::modeBegin );

		m_nStep = 1;
	}
	else if( m_nStep == 1 || m_nStep == 2 )
	{
		m_fFadeTimeForBlow = max( m_fFadeTimeForBlow, fFadeTime );
	}
}