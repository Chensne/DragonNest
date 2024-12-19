#include "StdAfx.h"
#include "DnHardCoreModeTimeAttackDlg.h"
#include "DnPartyTask.h"
#include "Timeset.h"



#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnHardCoreModeTimeAttackDlg::CDnHardCoreModeTimeAttackDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, false )
, m_pStaticMinute( NULL )
, m_pStaticSecond( NULL )
, m_pStaticMilliSecond( NULL )
, m_pBaseTexture( NULL )
, m_pScaleTexture( NULL )
, m_nMin( 0 )
, m_nSec( 0 )
, m_tAxisTime( 0 )
, m_tMSec( 0 )
, m_fRemainSec( 0.0f )
, m_fOriginSec( 0.0f )
, m_fHurryUpSec( 0.0f )
, m_fTotalSec( 0.0f )
, m_bStartTimer( false )
, m_bScaleProcess( false )
, m_fScaleAfterShowTime( 1.0f )
, m_fScaleAfterShowElapsedTime( 0.0f )
, m_fScaleShowTime( 0.1f )
, m_fScaleTime( 0.0f )
, m_fScaleAccel( 0.0f )
, m_fScaleInit( 1.5f )
, m_fHalfTime( 0.0f )
, m_fHalfShowTime( 0.0f )
, m_bContinue( false )
{
}

CDnHardCoreModeTimeAttackDlg::~CDnHardCoreModeTimeAttackDlg()
{
}

void CDnHardCoreModeTimeAttackDlg::InitialUpdate()
{
	m_pStaticMinute = GetControl<CEtUIStatic>( "ID_TEXT_MINUTE" );
	m_pStaticSecond = GetControl<CEtUIStatic>( "ID_TEXT_SECOND" );
	m_pStaticMilliSecond = GetControl<CEtUIStatic>( "ID_TEXT_MILISECOND" );

	m_pBaseTexture = GetControl<CEtUITextureControl>("ID_TEXTUREL_HURRYUP0");
	m_pScaleTexture = GetControl<CEtUITextureControl>("ID_TEXTUREL_HURRYUP1");

	m_fScaleShowTime = 0.0f;
	m_fScaleAccel = 1.0f;
	m_fScaleInit = 1.5f;
	m_fScaleAfterShowTime = 1.5f;
	m_fHalfShowTime = 0.0f;
	m_bContinue = true;

	m_pStaticMinute->SetText( L"00" );
	m_pStaticSecond->SetText( L"00" );
	m_pStaticMilliSecond->SetText( L"00" );

	m_pBaseTexture->Show( false );
	m_pScaleTexture->Show( false );
}

void CDnHardCoreModeTimeAttackDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "HardMoveTimerDlg.ui" ).c_str(), bShow );
}

void CDnHardCoreModeTimeAttackDlg::Show( bool bShow )
{
	if( m_bShow == bShow ) return;
	CDnCustomDlg::Show( bShow );
}

void CDnHardCoreModeTimeAttackDlg::ResetTimer()
{
	m_fRemainSec = 0.0f;
	m_fHurryUpSec = 0.0f;
	m_fTotalSec = 0.0f;
	m_bStartTimer = false;

	m_pStaticMinute->SetText( L"00" );
	m_pStaticSecond->SetText( L"00" );
	m_pStaticMilliSecond->SetText( L"00" );

	m_pBaseTexture->Show( false );
	m_pScaleTexture->Show( false );
}

void CDnHardCoreModeTimeAttackDlg::SetRemainTime( int nOriginTime, int nRemainTime )
{
	m_fRemainSec = float(nRemainTime);
	m_fOriginSec = float(nOriginTime);

	if( 0.0f == m_fHurryUpSec )
		m_fHurryUpSec = m_fOriginSec * 0.15f;

	m_fTotalSec = m_fRemainSec-(float)CTimeSet().GetMilliseconds() / 1000; // sMSec 짜투리 밀리세컨
	m_tAxisTime = CTimeSet().GetTimeT64_GM();
	m_tMSec = (float)CTimeSet().GetMilliseconds() / 1000;
	UpdateTimer( true );
}

void CDnHardCoreModeTimeAttackDlg::SetHurryTime( int nTime )
{
	m_fHurryUpSec = (float)nTime;
}

void CDnHardCoreModeTimeAttackDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( !m_bStartTimer )
	{
		if( CDnPartyTask::IsActive() && CDnPartyTask::GetInstance().IsSyncComplete() )
		{
			m_bScaleProcess = true;
			m_fScaleTime = 0.0f;
			m_fScaleAfterShowElapsedTime = 0.0f;
			m_fHalfTime = 0.0f;
			m_bStartTimer = true;
		}
		else
			m_bStartTimer = false;
	}

	bool bShowHurryUp = false;
	if( m_bStartTimer && m_fRemainSec > 0.0f )
	{
		m_fRemainSec = (m_fTotalSec - (float)((CTimeSet().GetTimeT64_GM() - (float)m_tAxisTime)));

		if( m_fRemainSec < m_fHurryUpSec )
			bShowHurryUp = true;

		UpdateTimer( !bShowHurryUp );
	}
	else if( !m_bStartTimer )
	{
		m_tAxisTime = CTimeSet().GetTimeT64_GM();
		m_tMSec = (float)CTimeSet().GetMilliseconds()/1000;
	}

	if( IsShow() )
	{
		if( bShowHurryUp )
		{
			ProcessTimerEffect( fElapsedTime );
		}
		else
		{
			m_pBaseTexture->Show( false );
			m_pScaleTexture->Show( false );
		}
	}
}

void CDnHardCoreModeTimeAttackDlg::ProcessTimerEffect( float fElapsedTime )
{
	if( !m_bScaleProcess )
		return;

	if( m_fScaleTime < m_fScaleShowTime )
	{
		if( 0.f == m_fScaleTime )
		{
			m_pScaleTexture->SetTextureColor(m_pScaleTexture->GetTextureColor());
			m_pScaleTexture->SetManualControlColor(true);
			m_pScaleTexture->Show( true );
		}

		m_fScaleTime += fElapsedTime;

		float ratio = 1.f - ((m_fScaleShowTime - m_fScaleTime) / m_fScaleShowTime);

		float scale = m_fScaleInit - ((m_fScaleInit - 1.f) * ratio) * m_fScaleAccel;
		m_fScaleAccel += 0.2f;
		if (scale < 1.f)
			scale = 1.f;
		m_pScaleTexture->SetScale(scale);

		EtColor color(m_pScaleTexture->GetTextureColor());
		color.a = float(pow(4 * m_fScaleTime, 2));
		if (color.a > 1.f)
			color.a = 1.f;
		m_pScaleTexture->SetTextureColor(color);

		EtVector2 prev, cur, result;
		GetUICtrlCenter(prev, m_pScaleTexture, 1.f);
		GetUICtrlCenter(cur, m_pScaleTexture, scale);
		result = prev - cur;

		SUICoord curCoord;
		m_pScaleTexture->GetUICoord(curCoord);
		m_pScaleTexture->SetTexturePosition(curCoord.fX + result.x, curCoord.fY + result.y);
	}
	else if( m_fHalfTime < m_fHalfShowTime )
	{
		m_fHalfTime += fElapsedTime;
	}
	else if( m_fScaleAfterShowElapsedTime < m_fScaleAfterShowTime )
	{
		if (m_fScaleAfterShowElapsedTime == 0.f)
		{
			m_pScaleTexture->SetScale(1.f);
			m_pScaleTexture->Show( m_bContinue );
			m_pBaseTexture->SetTextureColor(m_pBaseTexture->GetTextureColor());
			m_pBaseTexture->SetManualControlColor(true);
			m_pBaseTexture->Show(true);
			m_fScaleAccel = 1.f;
		}

		m_fScaleAfterShowElapsedTime += fElapsedTime;
		float ratio = (m_fScaleAfterShowTime - m_fScaleAfterShowElapsedTime) / m_fScaleAfterShowTime;
		EtColor color(m_pBaseTexture->GetTextureColor());
		color.a = 0.7f - float(pow(4 * m_fScaleAfterShowElapsedTime, 2));
		if (color.a < 0.f)
			color.a = 0.f;
		m_pBaseTexture->SetTextureColor(color);
		float scale = 1.f + log(10.f * m_fScaleAfterShowElapsedTime + 1);
		m_pBaseTexture->SetScale(scale);

		EtVector2 prev, cur, result;
		GetUICtrlCenter(prev, m_pBaseTexture, 1.f);
		GetUICtrlCenter(cur, m_pBaseTexture, scale);
		result = prev - cur;

		SUICoord curCoord;
		m_pBaseTexture->GetUICoord(curCoord);
		m_pBaseTexture->SetTexturePosition(curCoord.fX + result.x, curCoord.fY + result.y);
	}
	else
	{
		if( m_bContinue )
		{
			m_fScaleTime = 0.f;
			m_fScaleAfterShowElapsedTime = 0.f;
			m_fHalfTime = 0.f;
		}
		else
		{
			m_pBaseTexture->Show( m_bContinue );
			m_pScaleTexture->Show( m_bContinue );
			m_bScaleProcess = false;
		}
	}
}

void CDnHardCoreModeTimeAttackDlg::GetUICtrlCenter( EtVector2& result, CEtUIControl* ctrl, float scale )
{
	if( ctrl == NULL )
	{
		_ASSERT(0);
		return;
	}
	SUICoord curCoord;
	ctrl->GetUICoord(curCoord);
	curCoord.fWidth *= scale;
	curCoord.fHeight *= scale;
	result.x = (curCoord.fX + curCoord.fWidth) * 0.5f;
	result.y = (curCoord.fY + curCoord.fHeight) * 0.5f;
}

void CDnHardCoreModeTimeAttackDlg::UpdateTimer( bool bShowTimer )
{
	m_pStaticMinute->Show( bShowTimer );
	m_pStaticSecond->Show( bShowTimer );
	m_pStaticMilliSecond->Show( bShowTimer );

	CEtUIStatic* pStatic = GetControl<CEtUIStatic>( "ID_TEXT0" );
	if( pStatic )
		pStatic->Show( bShowTimer );
	pStatic = GetControl<CEtUIStatic>( "ID_TEXT1" );
	if( pStatic )
		pStatic->Show( bShowTimer );
	pStatic = GetControl<CEtUIStatic>( "ID_STATIC0" );
	if( pStatic )
		pStatic->Show( bShowTimer );

	int CurSec = static_cast<int>(ceil(m_fRemainSec)) % 60;
	int CurMin = static_cast<int>(ceil(m_fRemainSec)) / 60;
	int nMSec = 100 - (CTimeSet().GetMilliseconds() / 10);

	WCHAR wszTime[10];
	if( CurSec < 0 )
		CurSec = 0;
	m_nSec  =  CurSec;
	wsprintf( wszTime, L"%.2d", m_nSec );
	m_pStaticSecond->SetText( wszTime );

	if( m_nMin < 0 )
		m_nMin = 0;

	m_nMin = CurMin;
	wsprintf( wszTime, L"%.2d", m_nMin );
	m_pStaticMinute->SetText( wszTime );

	wsprintf( wszTime, L"%.2d", nMSec );
	m_pStaticMilliSecond->SetText( wszTime );

	if( 0 >= m_fRemainSec )
		m_pStaticMilliSecond->SetText( L"00" );
}

