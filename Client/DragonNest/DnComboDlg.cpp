#include "StdAfx.h"
#include "DnComboDlg.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnComboDlg::CDnComboDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CDnCountDlg( dialogType, pParentDialog, nID, pCallback  )
	, m_bAction(false)
	, m_bShake(false)
	, m_bHide(false)
	, m_nShakeCount(0)
{
}

CDnComboDlg::~CDnComboDlg(void)
{
}

void CDnComboDlg::Initialize( bool bShow )
{
	if( !m_pCountCtl )
	{
		CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "ComboDlg.ui" ).c_str(), bShow );
	}
}

void CDnComboDlg::InitialUpdate()
{
	m_pCountCtl = GetControl<CDnComboCount>("ID_COMBO_COUNT");
	m_pCountCtl->Show(false);
}

void CDnComboDlg::Process( float fElapsedTime )
{
	if( m_bAction )
	{
		if( UpdateShowAni() )
		{
			m_bShake = true;
			m_bAction = false;
		}
	}

	if( !m_bAction && m_bShake )
	{
		SUICoord uiCoord;
		GetDlgCoord( uiCoord );
		if( m_nShakeCount%2 )
		{
			uiCoord.fX += 0.02f;
		}
		else
		{
			uiCoord.fX -= 0.01f;
		}
		SetDlgCoord( uiCoord );

		m_nShakeCount--;

		if( m_nShakeCount <= 0 )
		{
			m_bShake = false;
			m_bHide = true;
		}
	}

	if( !m_bAction && !m_bShake && (m_fDelayTime > 0.0f) )
	{
		m_fDelayTime -= fElapsedTime;
	}

	if( m_bHide && (m_fDelayTime <= 0.0f) )
	{
		if( UpdateHideAni() )
		{
			m_bHide = false;
		}
	}
}

void CDnComboDlg::Render( float fElapsedTime )
{
	if( GetInterface().IsOpenBlind() )
		return;

	CEtUIDialog::Render( fElapsedTime );
}

void CDnComboDlg::SetCombo( int nCombo, int nDelay )
{
	m_pCountCtl->SetValue( nCombo );
	m_fDelayTime = nDelay*0.001f;
	m_nShakeCount = 5;

	SUICoord dlgCoord;
	GetDlgCoord( dlgCoord );
	dlgCoord.fWidth = m_pCountCtl->GetControlWidth();
	dlgCoord.fHeight = m_pCountCtl->GetControlHeight();
	dlgCoord.fX = GetScreenWidthRatio() * 0.75f;

	//if( nCombo > 1 )
	{
		dlgCoord.fX -= (dlgCoord.fWidth+0.1f);
		m_bShake = true;
		m_bAction = false;
	}
	//else
	//{
	//	m_bShake = false;
	//	m_bAction = true;
	//}

	SetDlgCoord( dlgCoord );
}

bool CDnComboDlg::UpdateShowAni()
{
	SUICoord uiCoord;
	GetDlgCoord( uiCoord );
	uiCoord.fX -= 0.02f;
	SetDlgCoord( uiCoord );

	if( (GetScreenWidthRatio()-uiCoord.fX) > (uiCoord.fWidth+0.1f) )
	{
		return true;
	}

	return false;
}

bool CDnComboDlg::UpdateHideAni()
{
	SUICoord uiCoord;
	GetDlgCoord( uiCoord );
	uiCoord.fX += 0.02f;
	SetDlgCoord( uiCoord );

	if( GetScreenWidthRatio() <= uiCoord.fX )
	{
		return true;
	}

	return false;
}