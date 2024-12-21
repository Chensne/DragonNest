#include "StdAfx.h"
#include "ObjectLightDir.h"
#include "RenderBase.h"
#include "InputReceiver.h"
#include "resource.h"
#include "MainFrm.h"
#include "EtActionToolDoc.h"
#include "EtActionToolView.h"
#include "UserMessage.h"
#include "PaneDefine.h"

CObjectLightDir::CObjectLightDir()
{
	m_Type = CActionBase::LIGHTDIR;
	m_nMouseFlag = 0;
}

CObjectLightDir::~CObjectLightDir()
{
	SAFE_RELEASE_SPTR( m_LightHandle );
}

void CObjectLightDir::Activate()
{
	SLightInfo LightInfo;

	LightInfo.Type = LT_DIRECTIONAL;
	LightInfo.Diffuse = EtColor( 0.5f, 0.5f, 0.5f, 1.f );
	LightInfo.Specular = EtColor( 1.f, 1.f, 1.f, 1.f );
	LightInfo.Direction = EtVector3( -1.f, -1.f, -1.f );
	EtVec3Normalize( &LightInfo.Direction, &LightInfo.Direction );

	m_LightHandle = EternityEngine::CreateLight( &LightInfo );

	InitPropertyInfo();
}

void CObjectLightDir::InitPropertyInfo()
{
	SAFE_DELETE_PVEC( m_pVecPropertyList );

	PropertyGridBaseDefine Default[] = {
		{ "Setting", "Diffuse", CUnionValueProperty::Vector4Color, "Diffuse Color", TRUE },
		{ "Setting", "Specular", CUnionValueProperty::Vector4Color, "Specular Color", TRUE },
		{ "Setting", "Direction", CUnionValueProperty::Vector3, "Direction!", TRUE },
		{ "Setting", "CastShadow", CUnionValueProperty::Boolean, "Cast Shadow", TRUE },
		NULL,
	};

	AddPropertyInfo( Default );
}

void CObjectLightDir::OnSetPropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable )
{
	switch( dwIndex ) {
		case 0:
			pVariable->SetVariable( (D3DXVECTOR4)m_BackupInfo.Diffuse );
			break;
		case 1:
			pVariable->SetVariable( (D3DXVECTOR4)m_BackupInfo.Specular );
			break;
		case 2:
			pVariable->SetVariable( m_BackupInfo.Direction );
			break;
		case 3:
			pVariable->SetVariable( m_BackupInfo.bCastShadow );
			break;
	}
}

void CObjectLightDir::OnChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable )
{
	switch( dwIndex ) {
		case 0:
			m_BackupInfo.Diffuse = (EtColor)pVariable->GetVariableVector4();
			break;
		case 1:
			m_BackupInfo.Specular = (EtColor)pVariable->GetVariableVector4();
			break;
		case 2:
			m_BackupInfo.Direction = (EtVector3)pVariable->GetVariableVector3();
			break;
		case 3:
			m_BackupInfo.bCastShadow = pVariable->GetVariableBool();
			break;

	}

	RefreshLight();
}


void CObjectLightDir::Process( LOCAL_TIME LocalTime )
{
	if( m_bFocus == true ) {
		CRenderBase::GetInstance().UpdateLightGrid( &m_BackupInfo );
	}
}


void CObjectLightDir::Show( bool bShow )
{
	if( m_bShow == bShow ) return;
	CActionBase::Show( bShow );

	if( m_bShow == true ) {
		m_LightHandle = EternityEngine::CreateLight( &m_BackupInfo );
		SetLightInfo( m_BackupInfo );
	}
	else {
		SAFE_RELEASE_SPTR( m_LightHandle );
	}
}

void CObjectLightDir::OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime )
{
	if( nReceiverState == -1 ) {
		m_nMouseFlag = 0;
		return;
	}

	if( nReceiverState & CInputReceiver::IR_MOUSE_MOVE ) {
		if( m_nMouseFlag & CRenderBase::LB_DOWN ) {
			m_matExWorld.RotateYAxis( CRenderBase::GetInstance().GetMouseVariation().x / 2.f );
			m_matExWorld.RotatePitch( CRenderBase::GetInstance().GetMouseVariation().y / 2.f );

			m_BackupInfo.Direction = m_matExWorld.m_vZAxis;
			static CUnionValueProperty Variable( CUnionValueProperty::Vector3 );
			Variable.SetVariable(m_BackupInfo.Direction);

			CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
			pWnd->SendMessage( UM_PROPERTY_PANE_SET_MODIFY, 2, (LPARAM)&Variable );
		}
	}
	if( nReceiverState & CInputReceiver::IR_MOUSE_LB_DOWN ) {
		if( CRenderBase::GetInstance().IsInMouseRect( CGlobalValue::GetInstance().GetView() ) ) {
			m_nMouseFlag |= CRenderBase::LB_DOWN;

			m_matExWorld.Identity();
			m_matExWorld.m_vZAxis = m_BackupInfo.Direction;
			m_matExWorld.MakeUpCartesianByZAxis();
		}
	}
	if( nReceiverState & CInputReceiver::IR_MOUSE_LB_UP ) {
		m_nMouseFlag &= ~CRenderBase::LB_DOWN;
	}
	RefreshLight();
}

SLightInfo CObjectLightDir::GetLightInfo()
{
	return m_BackupInfo;
}

void CObjectLightDir::SetLightInfo( SLightInfo &Info )
{
	m_BackupInfo = Info;
	RefreshLight();
}

void CObjectLightDir::RefreshLight()
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