#include "StdAfx.h"
#include "ObjectLightPoint.h"
#include "RenderBase.h"

#include "resource.h"
#include "MainFrm.h"
#include "EtViewerDoc.h"
#include "EtViewerView.h"
#include "UserMessage.h"
#include "PaneDefine.h"

CObjectLightPoint::CObjectLightPoint()
{
	m_Type = CObjectBase::LIGHTPOINT;
	m_nMouseFlag = 0;
}

CObjectLightPoint::~CObjectLightPoint()
{
	SAFE_RELEASE_SPTR( m_LightHandle );
}

void CObjectLightPoint::Activate()
{
	SLightInfo LightInfo;

	LightInfo.Type = LT_POINT;
	LightInfo.Diffuse = EtColor( 0.5f, 0.5f, 0.5f, 1.f );
	LightInfo.Specular = EtColor( 1.f, 1.f, 1.f, 1.f );
	LightInfo.Position = EtVector3( 0.f, 0.f, 0.f );
	LightInfo.fRange = 500.f;

	m_LightHandle = EternityEngine::CreateLight( &LightInfo );

	InitPropertyInfo();
}

void CObjectLightPoint::InitPropertyInfo()
{
	SAFE_DELETE_PVEC( m_pVecPropertyList );

	PropertyGridBaseDefine Default[] = {
		{ "Setting", "Diffuse", CUnionValueProperty::Vector4Color, "Diffuse Color", TRUE },
		{ "Setting", "Specular", CUnionValueProperty::Vector4Color, "Specular Color", TRUE },
		{ "Setting", "Position", CUnionValueProperty::Vector3, "Position", TRUE },
		{ "Setting", "Range", CUnionValueProperty::Float, "Range", TRUE },
		{ "Setting", "CastShadow", CUnionValueProperty::Boolean, "Cast Shadow", TRUE },
		NULL,
	};

	AddPropertyInfo( Default );
}

void CObjectLightPoint::OnSetPropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable )
{
	switch( dwIndex ) {
		case 0:
			pVariable->SetVariable( (D3DXVECTOR4)m_BackupInfo.Diffuse );
			break;
		case 1:
			pVariable->SetVariable( (D3DXVECTOR4)m_BackupInfo.Specular );
			break;
		case 2:
			pVariable->SetVariable( m_BackupInfo.Position );
			break;
		case 3:
			pVariable->SetVariable( m_BackupInfo.fRange );
			break;
		case 4:
			pVariable->SetVariable( m_BackupInfo.bCastShadow );

	}
}

void CObjectLightPoint::OnChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable )
{
	switch( dwIndex ) {
		case 0:
			m_BackupInfo.Diffuse = (EtColor)pVariable->GetVariableVector4();
			break;
		case 1:
			m_BackupInfo.Specular = (EtColor)pVariable->GetVariableVector4();
			break;
		case 2:
			m_BackupInfo.Position = (EtVector3)pVariable->GetVariableVector3();
			break;
		case 3:
			m_BackupInfo.fRange = pVariable->GetVariableFloat();
			break;
		case 4:
			m_BackupInfo.bCastShadow = pVariable->GetVariableBool();
			break;
	}
	RefreshLight();
}


void CObjectLightPoint::Process( LOCAL_TIME LocalTime )
{
	if( m_bFocus == true ) {
		CRenderBase::GetInstance().UpdateLightGrid( &m_BackupInfo );
	}
}


void CObjectLightPoint::Show( bool bShow )
{
	if( m_bShow == bShow ) return;
	CObjectBase::Show( bShow );

	if( m_bShow == true ) {
		m_LightHandle = EternityEngine::CreateLight( &m_BackupInfo );
	}
	else {
		SAFE_RELEASE_SPTR( m_LightHandle );
	}
}


void CObjectLightPoint::OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime )
{
	if( nReceiverState == -1 ) {
		m_nMouseFlag = 0;
		return;
	}

	if( nReceiverState & CInputReceiver::IR_MOUSE_MOVE ) {
		char cUpdate = -1;
		if( m_nMouseFlag & CRenderBase::LB_DOWN ) {
			MatrixEx *pCamera = CRenderBase::GetInstance().GetCameraCross();
			m_Cross.m_vXAxis = pCamera->m_vXAxis;
			m_Cross.m_vYAxis = EtVector3( 0.f, 1.f, 0.f );
			EtVec3Cross( &m_Cross.m_vZAxis, &m_Cross.m_vXAxis, &m_Cross.m_vYAxis );
			m_Cross.MoveLocalXAxis( CRenderBase::GetInstance().GetMouseVariation().x );
			m_Cross.MoveLocalZAxis( -CRenderBase::GetInstance().GetMouseVariation().y );
			m_BackupInfo.Position = m_Cross.m_vPosition;
			cUpdate = 0;
		}
		if( m_nMouseFlag & CRenderBase::RB_DOWN ) {
			m_Cross.MoveLocalYAxis( -CRenderBase::GetInstance().GetMouseVariation().y );
			m_BackupInfo.Position = m_Cross.m_vPosition;
			cUpdate = 0;
		}
		if( m_nMouseFlag & CRenderBase::WB_DOWN ) {
			m_BackupInfo.fRange -= CRenderBase::GetInstance().GetMouseVariation().y;
			if( m_BackupInfo.fRange <= 0.f ) m_BackupInfo.fRange = 0.f;
			cUpdate = 1;
		}

		if( cUpdate != -1 ) {
			if( cUpdate == 0 ) {
				CUnionValueProperty Variable( CUnionValueProperty::Vector3 );
				Variable.SetVariable(m_BackupInfo.Position);

				CWnd *pWnd = GetPaneWnd( PROP_SHADER_PANE );
				pWnd->SendMessage( UM_SHADERPROP_MODIFY_ITEM, 2, (LPARAM)&Variable );
			}
			else if( cUpdate == 1 ) {
				CUnionValueProperty Variable( CUnionValueProperty::Float );
				Variable.SetVariable(m_BackupInfo.fRange);

				CWnd *pWnd = GetPaneWnd( PROP_SHADER_PANE );
				pWnd->SendMessage( UM_SHADERPROP_MODIFY_ITEM, 3, (LPARAM)&Variable );
			}
		}
	}
	if( nReceiverState & CInputReceiver::IR_MOUSE_LB_DOWN ) {
		if( CRenderBase::GetInstance().IsInMouseRect( CGlobalValue::GetInstance().GetView() ) ) {
			m_nMouseFlag |= CRenderBase::LB_DOWN;

			m_Cross.Identity();
			m_Cross.m_vPosition = m_BackupInfo.Position;
		}
	}
	if( nReceiverState & CInputReceiver::IR_MOUSE_LB_UP ) {
		m_nMouseFlag &= ~CRenderBase::LB_DOWN;
	}
	if( nReceiverState & CInputReceiver::IR_MOUSE_RB_DOWN ) {
		if( CRenderBase::GetInstance().IsInMouseRect( CGlobalValue::GetInstance().GetView() ) ) {
			m_nMouseFlag |= CRenderBase::RB_DOWN;

			m_Cross.Identity();
			m_Cross.m_vPosition = m_BackupInfo.Position;
		}
	}
	if( nReceiverState & CInputReceiver::IR_MOUSE_RB_UP ) {
		m_nMouseFlag &= ~CRenderBase::RB_DOWN;
	}
	if( nReceiverState & CInputReceiver::IR_MOUSE_WB_DOWN ) {
		if( CRenderBase::GetInstance().IsInMouseRect( CGlobalValue::GetInstance().GetView() ) )
			m_nMouseFlag |= CRenderBase::WB_DOWN;
	}
	if( nReceiverState & CInputReceiver::IR_MOUSE_WB_UP ) {
		m_nMouseFlag &= ~CRenderBase::WB_DOWN;
	}
	RefreshLight();
}

void CObjectLightPoint::RefreshLight()
{
	if( !m_LightHandle ) return;
	SLightInfo Info = m_BackupInfo;
	Info.Diffuse.r *= Info.Diffuse.a;
	Info.Diffuse.g *= Info.Diffuse.a;
	Info.Diffuse.b *= Info.Diffuse.a;
	Info.Specular.r *= Info.Specular.a;
	Info.Specular.g *= Info.Specular.a;
	Info.Specular.b *= Info.Specular.a;
	m_LightHandle->SetLightInfo( &Info );
}

SLightInfo CObjectLightPoint::GetLightInfo()
{
	return m_BackupInfo;
}

void CObjectLightPoint::SetLightInfo( SLightInfo &Info )
{
	m_BackupInfo = Info;
	if( m_LightHandle ) RefreshLight();
}


void CObjectLightPoint::ExportObject( FILE *fp, int &nCount )
{
	fwrite( &m_Type, sizeof(int), 1, fp );

	SLightInfo Info = GetLightInfo();
	fwrite( &Info, sizeof(SLightInfo), 1, fp );

	nCount++;
}

void CObjectLightPoint::ImportObject( FILE *fp )
{
	SLightInfo Info;
	fread( &Info, sizeof(SLightInfo), 1, fp );

	SetLightInfo( Info );
}