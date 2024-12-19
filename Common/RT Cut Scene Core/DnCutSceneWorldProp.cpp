#include "StdAfx.h"
#include "DnCutSceneWorldProp.h"
#include "EternityEngine.h"
#include "EtWorldSector.h"
#include "DnCutSceneTable.h"
#include "PropHeader.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCutSceneWorldProp::CDnCutSceneWorldProp(void)
{
	EtMatrixIdentity( &m_matWorld );

}

CDnCutSceneWorldProp::~CDnCutSceneWorldProp(void)
{
	SAFE_RELEASE_SPTR( m_Handle );
}


bool CDnCutSceneWorldProp::Initialize( CEtWorldSector *pParentSector, const char *szPropName, 
									   EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale )
{
	bool bResult = CEtWorldProp::Initialize( pParentSector, szPropName, vPos, vRotate, vScale );
	if( false == bResult )
		return false;

	DNTableFileFormat*  pPropTable = CDnCutSceneTable::GetInstance().GetTable( CDnCutSceneTable::PROP_TABLE );
	if( NULL == pPropTable )
		return false;
	int iTableID = pPropTable->GetItemIDFromField( "_Name", szPropName );
	
	if( -1 != iTableID )
	{
		bResult = InitializeTable( pPropTable, iTableID );
		if( false == bResult )
			return false;
	}

	bool bSuccess = _CreateObject();
	if( false == bSuccess )
		return false;

	m_Handle->SetCollisionScale( max( max( vScale.x, vScale.y ), vScale.z ) );
	UpdateMatrix();

	// 카메라 메시는 안보이게 요청 들어와서 수정. 이름이 "Camera.skn" 이면 실패로.
	if( strcmp( szPropName, "Camera.skn" ) == 0 )
		return false;

	return true;
}



bool CDnCutSceneWorldProp::InitializeTable( DNTableFileFormat*  pPropTable, int iTableID )
{
	EnableCastShadow( (pPropTable->GetFieldFromLablePtr( iTableID, "_IsCastLightmap" )->GetInteger() == TRUE) ? true : false );
	EnableLightmapInfluence( (pPropTable->GetFieldFromLablePtr( iTableID, "_IsLightmapInfluence" )->GetInteger() == TRUE ) ? true : false );

	return true;
}



bool CDnCutSceneWorldProp::_CreateObject( void )
{
	m_Handle = EternityEngine::CreateStaticObject( GetPropName() );
	if( !m_Handle )
		return false;
	m_Handle->GetSkinInstance()->SetRenderAlphaTwoPass( true );

	return true;
}


void CDnCutSceneWorldProp::UpdateMatrix( void )
{
	EtVector3 vOffset = *m_pParentSector->GetOffset();
	vOffset.x -= (m_pParentSector->GetTileWidthCount() * m_pParentSector->GetTileSize() ) / 2.0f;
	vOffset.z -= (m_pParentSector->GetTileHeightCount() * m_pParentSector->GetTileSize() ) / 2.0f;
	vOffset.y = 0.0f;

	m_matExWorld.m_vPosition = m_vPosition + vOffset;
	m_matExWorld.RotatePitch( m_vRotation.x );
	m_matExWorld.RotateRoll( m_vRotation.z );
	m_matExWorld.RotateYaw( m_vRotation.y );

	EtMatrix matScale;
	m_matWorld = *m_matExWorld;
	EtMatrixScaling( &matScale, m_vScale.x, m_vScale.y, m_vScale.z );
	EtMatrixMultiply( &m_matWorld, &matScale, &m_matWorld );

	m_Handle->Update( &m_matWorld );
}


void CDnCutSceneWorldProp::EnableCastShadow( bool bEnable )
{
	CEtWorldProp::EnableCastShadow( bEnable );

	if( m_Handle )
	{
		m_Handle->EnableShadowCast( bEnable );

		if( !IsCastShadow() && !IsReceiveShadow() )
		{
			m_Handle->EnableLightMapCast( true );
			m_Handle->EnableLightMapInfluence( false );
		}
		else
		{
			m_Handle->EnableLightMapCast( false );
			m_Handle->EnableLightMapInfluence( true );
		}
	}
}


void CDnCutSceneWorldProp::EnableReceiveShadow( bool bEnable )
{
	CEtWorldProp::EnableReceiveShadow( bEnable );

	if( m_Handle )
	{
		m_Handle->EnableShadowReceive( bEnable );

		if( !IsCastShadow() && !IsReceiveShadow() )
		{
			m_Handle->EnableLightMapCast( true );
			m_Handle->EnableLightMapInfluence( false );
		}
		else
		{
			m_Handle->EnableLightMapCast( false );
			m_Handle->EnableLightMapInfluence( true );
		}
	}
}

void* CDnCutSceneWorldProp::AllocPropData(int& usingCount)
{
	//usingCount = GetPropSignalDataUsingCount(GetClassID());
	//if (usingCount > 0)
	//	return new int[usingCount];
	//return NULL;

	// 컷신툴에서는 강제로 다른 타입의 프랍으로 생성될 때도 있으므로 그냥 넘어온 양만큼 생성해준다.
	if( 0 < usingCount )
		return new int[usingCount];
	else
		return NULL;
}
