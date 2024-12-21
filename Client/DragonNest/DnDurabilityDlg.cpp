#include "StdAfx.h"
#include "DnDurabilityDlg.h"
#include "DnInterface.h"
#include "DnActor.h"
#include "DnStateBlow.h"

#include "DnFreezingPrisonBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnDurabilityDlg::CDnDurabilityDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( UI_TYPE_BOTTOM, pParentDialog, nID, pCallback  )
{
	m_pDurabilityBar = NULL;
	m_pIcon = NULL;

	m_fDelayTime = 0.0f;
	m_nStateBlowID = -1;
	m_fRate = 0.0f;
}

CDnDurabilityDlg::~CDnDurabilityDlg(void)
{
}

void CDnDurabilityDlg::Initialize( bool bShow )
{
	if( !m_pDurabilityBar )
	{
		CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "Durability.ui" ).c_str(), bShow );
	}
}

void CDnDurabilityDlg::InitialUpdate()
{
	m_pDurabilityBar = GetControl< CEtUIProgressBar >( "ID_DURABILITY_GAUGE" );
	m_pDurabilityBar->SetProgress(0.0f);
	m_pDurabilityBar->Show(true);

	m_pIcon = GetControl<CEtUIStatic>("ID_DURABILITY_ICON");
	m_pIcon->Show(true);
	

	SetPosition( -100.0f, -100.0f );
}

void CDnDurabilityDlg::Process( float fElapsedTime )
{
	if (!m_hActor || m_hActor->IsDie() || !m_hActor->IsShow())
	{
		Show(false);
		return;
	}

	CEtUIDialog::Process( fElapsedTime );

// 	if( !m_hActor )
// 		return;
	
	UpdateDurability();
	UpdateGaugePos();
}

void CDnDurabilityDlg::Render( float fElapsedTime )
{
	if( GetInterface().IsOpenBlind() )
		return;

	CEtUIDialog::Render( fElapsedTime );
}

void CDnDurabilityDlg::Show( bool bShow )
{ 	
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );
}

void CDnDurabilityDlg::SetDurability(float fDurability)
{
	m_fRate = fDurability;

	if (m_fRate <= 0.0f)
		Show(false);

	OutputDebug("ActorHandle : 0x%x, Durability : %f\n", m_hActor.GetPointer(), m_fRate);
}

void CDnDurabilityDlg::UpdateDurability()
{
	m_pDurabilityBar->SetProgress( m_fRate * 100.0f );
}

void CDnDurabilityDlg::SetActorInfo( DnActorHandle hActor, int nStateBlowID ) 
{ 
	m_hActor = hActor;
	m_nStateBlowID = nStateBlowID;
}

EtVector3 CDnDurabilityDlg::GetGaugePos()
{
	EtVector3 vPos = *m_hActor->GetPosition();

	DnBlowHandle hBlow = m_hActor->GetStateBlow()->GetStateBlowFromServerID(m_nStateBlowID);
	// hBlow가 정상인지 확인 코드 추가 [2010/12/14 semozz]
	CDnFreezingPrisonBlow* pFreezingPrison = NULL;
	if (hBlow)
		pFreezingPrison = dynamic_cast<CDnFreezingPrisonBlow*>(hBlow.GetPointer());

	if (pFreezingPrison)
		vPos = pFreezingPrison->GetGaugePos();
	else
	{
		// 만약에 상태 효과가 없다면 위치값이 순간 바닥으로 내려 가는경우 발생
		// 그래서 여기서 Dialog를 Hide시킨다.
		Show(false);
	}

	return vPos;
}

void CDnDurabilityDlg::UpdateGaugePos()
{
	SAABox box;
	m_hActor->GetBoundingBox(box);
	float fHeight = box.Max.y - box.Min.y;

	float fDepth = box.Max.z - box.Min.z;
	//fHeight *= m_hActor->GetScale()->y;
	//EtVector3 vPos = m_hActor->GetMatEx()->m_vPosition;
	//vPos.y += fHeight * 0.95f;	//머리높이 보다 약간 낮게


	EtVector3 vPos = GetGaugePos();

	if( !CEtCamera::GetActiveCamera() ) {
		return;
	}

	EtVector4 vDamagePos, vDamagePosDepth;
	EtVec4Transform( &vDamagePos, &EtVector4(vPos, 1.0f), CEtCamera::GetActiveCamera()->GetViewMat() );
	if( vDamagePos.z < 0.0f ) {
		SetPosition( -100.0f, -100.0f );
		return;
	}

	vDamagePosDepth = vDamagePos;
	vDamagePosDepth.z -= (fDepth+10.0f);
	EtVec4Transform( &vDamagePos, &vDamagePos, CEtCamera::GetActiveCamera()->GetProjMat() );
	EtVec4Transform( &vDamagePosDepth, &vDamagePosDepth, CEtCamera::GetActiveCamera()->GetProjMat() );
	vDamagePos.x /= vDamagePos.w;
	vDamagePos.y /= vDamagePos.w;
	vDamagePos.z = vDamagePosDepth.z / vDamagePosDepth.w;

	m_pDurabilityBar->SetZValue( vDamagePos.z );
	m_pIcon->SetZValue( vDamagePos.z );
	

	vDamagePos.x *= GetScreenWidthRatio();
	vDamagePos.x = (vDamagePos.x*0.5f)+0.5f;
	vDamagePos.y *= GetScreenHeightRatio();
	vDamagePos.y = (vDamagePos.y*0.5f)+0.5f;
	vDamagePos.y = 1.0f - vDamagePos.y;

	static float fUpVal = 0.035f;
	vDamagePos.y -= fUpVal;

	static float fMinY = 0.025f * DEFAULT_UI_SCREEN_HEIGHT / GetEtDevice()->Height();
	static float fMinBallonY = -0.1f * DEFAULT_UI_SCREEN_HEIGHT / GetEtDevice()->Height();
	if( vDamagePos.y < fMinY ) {
		vDamagePos.y = fMinY;				
	}
	
	//if( ( abs( vDamagePos.x - m_vDamageOld.x ) > 0.001f ) || ( abs( vDamagePos.y - m_vDamageOld.y ) > 0.001f ) )
	{
		SUICoord dlgCoord;
		GetDlgCoord( dlgCoord );
		dlgCoord.SetPosition( vDamagePos.x-(dlgCoord.fWidth * 0.5f), vDamagePos.y );
		SetDlgCoord( dlgCoord );

		//m_vDamageOld = vDamagePos;
	}
}

/*
void CDnDurabilityDlg::ResetDelayTime()
{
	m_fDelayTime = 2.0f;
}
*/