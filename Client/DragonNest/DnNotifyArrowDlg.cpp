#include "stdafx.h"
#include "DnNotifyArrowDlg.h"
#include "DnActor.h"
#include "DnWorld.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnNotifyArrowDialog::CDnNotifyArrowDialog( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
{
	m_pStaticArrow = NULL;
	m_nViewTime = 0;
	m_nSetTime = 0;
}

CDnNotifyArrowDialog::~CDnNotifyArrowDialog()
{

}

void
CDnNotifyArrowDialog::InitialUpdate()
{
	m_pStaticArrow = GetControl<CEtUIStatic>("ID_NOTIFY_ARROW");
	if ( m_pStaticArrow )
		m_pStaticArrow->Show(false);
	
#ifdef PRE_ADD_STAMPSYSTEM
	CEtUIStatic * pStatic = GetControl<CEtUIStatic>("ID_EVENT_ARROW");
	if( pStatic )
		pStatic->Show( false );
#endif // PRE_ADD_STAMPSYSTEM

}

void
CDnNotifyArrowDialog::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "NotifyArrow.ui" ).c_str(), bShow );
}

void
CDnNotifyArrowDialog::Render( float fElapsedTime )
{
	CEtUIDialog::Render( fElapsedTime );

	if( !IsShow() || !m_pStaticArrow )
		return;

	const EtVector3 *pPlayerPos(NULL);
	DnActorHandle hPlayer = CDnActor::s_hLocalActor;

	if( hPlayer )
		pPlayerPos = hPlayer->GetPosition();
	else 
		pPlayerPos = CDnWorld::GetInstance().GetUpdatePosition();

	EtVector3 vZDir, vCross, vTemp;
	vTemp = m_vTargetPos - (*pPlayerPos);
	vTemp.y = 0.0f;
	EtVec3Normalize( &vTemp, &vTemp );

	DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
	if( hCamera )
	{
		vZDir = hCamera->GetMatEx()->m_vZAxis;
		vZDir.y = 0.0f;
	}

	EtVec3Normalize( &vZDir, &vZDir );
	float fDegree = EtAcos( EtVec3Dot( &vTemp, &vZDir ) );
	EtVec3Cross( &vCross, &vTemp, &vZDir );
	if( vCross.y > 0.0f ) 
	{
		fDegree = -fDegree;
	}

	SUICoord uiCoord;
	m_pStaticArrow->GetUICoord(uiCoord);
	SUIElement *pElement = m_pStaticArrow->GetElement(0);

	LOCAL_TIME time = CGlobalInfo::GetInstance().m_LocalTime;

	/*
	DWORD TIME_ = 350;
	DWORD n = (DWORD)time%(TIME_+TIME_);
	if ( n > TIME_ )
		n = 255;
	else
		n = 0;
	*/
	int nAlpha = 255;
	int BLEND_TIME = 500;
	float BLEND_RANGE  = 1.0f;
	int nTemp = (int)time%(BLEND_TIME+BLEND_TIME);
	if( nTemp < BLEND_TIME ) 
	{
		nAlpha = int(( BLEND_RANGE / (float)BLEND_TIME ) * nTemp * 255.0f);
	}
	else 
	{
		nAlpha = int(( BLEND_RANGE / (float)BLEND_TIME ) * ( (BLEND_TIME+BLEND_TIME) - nTemp ) *  255.0f);
	}

	nAlpha = min(nAlpha, 255);

	if( m_pStaticArrow->GetTemplate().m_hTemplateTexture )
		DrawSprite( m_pStaticArrow->GetTemplate().m_hTemplateTexture, pElement->TemplateUVCoord, D3DCOLOR_ARGB(nAlpha,255,255,255), uiCoord, EtToDegree(fDegree) );
	else
		DrawSprite( pElement->UVCoord, D3DCOLOR_ARGB(nAlpha,255,255,255), uiCoord, EtToDegree(fDegree) );

	if ( CGlobalInfo::GetInstance().m_LocalTime - m_nSetTime >= m_nViewTime )
	{
		Show(false);
	}
}

void
CDnNotifyArrowDialog::SetDirection(EtVector3 vTarget, LOCAL_TIME nTime)
{
	m_vTargetPos = vTarget;
	m_nViewTime = nTime;
	m_nSetTime = CGlobalInfo::GetInstance().m_LocalTime;

}

void
CDnNotifyArrowDialog::OnBlindClosed()
{
	this->Show(true);
	m_nSetTime = CGlobalInfo::GetInstance().m_LocalTime;

}