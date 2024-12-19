
#include "StdAfx.h"

#ifdef PRE_ADD_CRAZYDUC_UI

#include "DnScoreDlg.h"
#include "DnCamera.h"
#include "VelocityFunc.h"
#include "DnActor.h"
#include "DnWorld.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnScoreDlg::CDnScoreDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CDnCountDlg( dialogType, pParentDialog, nID, pCallback  )
, m_fXGap(0.0f)
, m_fYGap(0.0f)
, m_fTotalElapsedTime(0.0f)
, m_fDistanceFromCamera(0.f)
, m_ColorType(CT_NONE)
{	

}

CDnScoreDlg::~CDnScoreDlg(void)
{

}

void CDnScoreDlg::Initialize( bool bShow )
{
	if ( ! m_pCountCtl )
	{
		CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "DamageDlg.ui" ).c_str(), bShow );
	}
	else if ( ! m_hTexture )
	{
		LoadDialogTexture();
	}

	InitialUpdate();

	m_bShow = bShow;
}

void CDnScoreDlg::InitialUpdate()
{	
	// 임시, 새로운 UI 가 제작되면은 컨트롤 모두 바꿈. 
	switch( m_ColorType ) {
		case CT_PARTYATK :
			m_pCountCtl = GetControl<CDnDamageCount>("ID_PARTYATK");
			break;
		case CT_MYATK :
			m_pCountCtl = GetControl<CDnDamageCount>("ID_MYATK");
			break;
		case CT_MYCRITICAL :
			m_pCountCtl = GetControl<CDnDamageCount>("ID_MYCRITICAL");
			break;
		case CT_MYDAMAGE :
			m_pCountCtl = GetControl<CDnDamageCount>("ID_MYDAMAGE");
			break;
		case CT_MYMANA : 
			m_pCountCtl = GetControl<CDnDamageCount>("ID_MYMANA");
			break;
		case CT_MYHEAL :
			m_pCountCtl = GetControl<CDnDamageCount>("ID_MYHEAL");
			break;
		case CT_PARTYHEAL : 
			m_pCountCtl = GetControl<CDnDamageCount>("ID_PARTYHEAL");
			break;
		case CT_PARTYDAMAGE :
			m_pCountCtl = GetControl<CDnDamageCount>("ID_PARTYDAMAGE");
			break;
		case CT_BLUE1 :
			m_pCountCtl = GetControl<CDnDamageCount>("ID_BLUE1");
			break;
		case CT_RED1 :
			m_pCountCtl = GetControl<CDnDamageCount>("ID_RED1");
			break;
		case CT_FIRE:
			m_pCountCtl = GetControl<CDnDamageCount>("ID_FIRE");
			break;
		case CT_WATER:
			m_pCountCtl = GetControl<CDnDamageCount>("ID_WATER");
			break;
		case CT_LIGHT:
			m_pCountCtl = GetControl<CDnDamageCount>("ID_LIGHT");
			break;
		case CT_DARK:
			m_pCountCtl = GetControl<CDnDamageCount>("ID_DARK");
			break;
		case CT_STAGELIMIT:
			m_pCountCtl = GetControl<CDnDamageCount>("ID_LIMIT");
			break;
		default:
			_ASSERT( false );
			break;
	}

	if( m_pCountCtl ) 
	{
		m_pCountCtl->Show(false);
		m_pCountCtl->Init();
		m_pCountCtl->UseCustomColor( true );
	}
}

void CDnScoreDlg::SetSign( bool bSign )
{
	if( bSign )
		m_pCountCtl->SetPlus( true );
	else 
		m_pCountCtl->SetMinus( true );
}

void CDnScoreDlg::Process( float fElapsedTime )
{
	if( m_fDelayTime > 0.0f )
	{
		m_fTotalElapsedTime += fElapsedTime;

		float fCurFrame = m_fTotalElapsedTime*100.0f;
		if (m_hAni)
		{
			if( m_hAni->GetAniLength( 0 ) < m_fTotalElapsedTime*70.f )
			{
				m_fDelayTime = 0.0f;
			}
			else
			{
				m_hAni->CalcAniDistance( 0, fCurFrame, 0.0f, m_vAniPos );
				UpdateCount();
			}
		}
		else
		{
			_ASSERT(0);
			m_fDelayTime = 0.0f;
		}	
	}

	CEtUIDialog::Process( fElapsedTime );
}

void CDnScoreDlg::Render( float fElapsedTime )
{
	if( GetInterface().IsOpenBlind() )
		return;

	CEtUIDialog::Render( fElapsedTime );
}

void CDnScoreDlg::SetScore( EtVector3 vPos, int nScore, EtAniHandle hAni, float fFontSize )
{
	SetSign( nScore >= 0 );
	m_pCountCtl->SetValue( nScore );
	m_pCountCtl->SetFontSize( fFontSize );		
	SetPosition( vPos );
	m_hAni = hAni;
}

void CDnScoreDlg::SetPosition( EtVector3 vPos )
{
	m_fTotalElapsedTime = 0.0f;
	m_fDelayTime = 0.8f;
	m_vAniPos.y = 0.0f;
	m_vActorPos = vPos;

	UpdateScorePos();

	if( CDnCamera::GetActiveCamera() )
	{
		EtVector3 vDist = m_vActorPos - CDnCamera::GetActiveCamera()->GetMatEx()->GetPosition();
		m_fDistanceFromCamera = EtVec3Length( &vDist );		
	}

	SUICoord dlgCoord;
	GetDlgCoord( dlgCoord );
	dlgCoord.SetSize( m_pCountCtl->GetControlWidth(), m_pCountCtl->GetControlHeight() );
	m_fXGap = (dlgCoord.fWidth * 0.5f );
	dlgCoord.SetPosition( m_vScorePos.x + m_fXGap, m_vScorePos.y );
	SetDlgCoord( dlgCoord );
}

void CDnScoreDlg::UpdateCount()
{
	UpdateScorePos();

	SUICoord dlgCoord;
	GetDlgCoord( dlgCoord );

	float fScale = 1.f;
	BYTE cAlpha = 255;
	if( m_fTotalElapsedTime < 0.5f ) {
		fScale = 1.f + (0.5f-m_fTotalElapsedTime)*2.f;
		cAlpha = (BYTE)min(255, 55 + (200 * (m_fTotalElapsedTime / 0.5f)) );
	}

	m_pCountCtl->SetAlpha( cAlpha );

	m_fXGap = (dlgCoord.fWidth * 0.5f) * fScale;
	m_pCountCtl->SetScale( fScale ) ;

	dlgCoord.SetPosition( m_vScorePos.x + m_fXGap, m_vScorePos.y );	
	SetDlgCoord( dlgCoord );
}

void CDnScoreDlg::UpdateScorePos()
{
	const EtVector3 *pPlayerPos(NULL);

	DnActorHandle hPlayer = CDnActor::s_hLocalActor;
	if( hPlayer )	pPlayerPos = hPlayer->GetPosition();
	else			pPlayerPos = CDnWorld::GetInstance().GetUpdatePosition();

	if( !pPlayerPos )
	{
		m_fDelayTime = 0.0f;
		m_pCountCtl->Show( false );
		Show( false );
		return;
	}

	EtMatrix matViewProj;
	DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
	if( hCamera )
	{
		matViewProj = *hCamera->GetCameraHandle()->GetViewProjMat();
	}
	else 
	{
		EtCameraHandle hCam = CEtCamera::GetActiveCamera();
		if( !hCam )
		{
			HideScore();
			return;
		}

		matViewProj = *CEtCamera::GetActiveCamera()->GetViewProjMat();
	}

	EtVector3 vEnermyPos;
	vEnermyPos = m_vActorPos;
	EtVec3TransformCoord( &m_vScorePos, &vEnermyPos, &matViewProj );
	
	float fScaleY = 130.f * min(1.7f, max(1.2f, 1200.f / m_fDistanceFromCamera )); 
	m_vScorePos.y += m_vAniPos.y / fScaleY;

	m_vScorePos.x = ( m_vScorePos.x * 0.5f ) + 0.5f;
	m_vScorePos.y = -( m_vScorePos.y * 0.7f ) + 0.4f;
	
	if( m_vScorePos.z >= 1.0f )
	{
		HideScore();
		return;
	}
}

void CDnScoreDlg::HideScore()
{
	m_fDelayTime = 0.0f;
	m_pCountCtl->Show( false );
	Show( false );
	m_bShow = false;
}

void CDnScoreDlg::Show( bool bShow )
{
	CDnCountDlg::ShowCount( bShow );
}

#endif // PRE_ADD_CRAZYDUC_UI