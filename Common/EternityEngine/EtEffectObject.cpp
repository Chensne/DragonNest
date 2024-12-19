#include "StdAfx.h"
#include "EtEffectObject.h"
#include "EtEffectData.h"
#include "EtEffectElement.h"
#include "EtMatrixEx.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

DECL_SMART_PTR_STATIC( CEtEffectObject, 500 )

CEtEffectObject::CEtEffectObject()
{
	m_bDelete = false;
	m_nPrevTick = 0;
	m_nEffectTick = 0;
	m_fFloatTick = 0.0f;
	m_bLoop = false;
	m_fScale = 1.f;
	m_bLink = false;
	m_bShow = true;
	EtMatrixIdentity( &m_WorldMat );
	m_RenderType = RT_SCREEN;
	m_LinkBoneIndex = 0;
	m_nLinkType = 0;
}

CEtEffectObject::~CEtEffectObject()
{
	SAFE_RELEASE_SPTR( m_hEffectData );
	SAFE_DELETE_PVEC( m_vecEffectElement );
}

void CEtEffectObject::Initialize( EtEffectDataHandle hEffectData, EtMatrix *pInitMat )
{
	SAFE_RELEASE_SPTR( m_hEffectData );

	m_hEffectData = hEffectData;
	hEffectData->AddRef();
	if( pInitMat )
	{
		m_WorldMat = *pInitMat;
	}

	int i, nElementCount;
	CEtEffectElement *pElement = NULL;

	nElementCount = hEffectData->GetElementCount();
	for( i = 0; i < nElementCount; i++ )
	{
		switch( hEffectData->GetElementType( i ) )
		{
		case ERT_MESH:
			pElement = new CEtEffectMeshElement();
			break;
		case ERT_PARTICLE:
			pElement = new CEtEffectBillboardEffectElement();
			break;
		case ERT_POINT_LIGHT:
			pElement = new CEtEffectPointLightElement();
			break;
		default:
			ASSERT( 0 );
			break;
		}
		pElement->Initialize( hEffectData->GetElementData( i ) );
		m_vecEffectElement.push_back( pElement );
	}
}

void CEtEffectObject::ProcessLink( EtMatrix &WorldMat )
{
	WorldMat = m_WorldMat;
	if( m_bLink ) 
	{
		MatrixEx Cross;
		Cross = m_LinkWorldMat;
		MatrixEx CrossOrigin = Cross;
		if( m_LinkBoneIndex != -1 ) 
		{
			EtMatrix MatParent;
			EtMatrixIdentity(&MatParent);
			if( m_hLinkParent ) 
			{
				if( m_nLinkType == CEtObject::LT_BONE )
				{
					EtMatrixMultiply( &MatParent, m_hLinkParent->GetBoneTransMat( m_LinkBoneIndex ), m_hLinkParent->GetWorldMat() );
				}
				else
				if( m_nLinkType == CEtObject::LT_MESH_DUMMY )
				{
					EtMatrixMultiply( &MatParent, m_hLinkParent->GetDummyTransMat( m_LinkBoneIndex ), m_hLinkParent->GetWorldMat() );
				}
			}
			memcpy( &Cross.m_vXAxis, &MatParent._11, sizeof(EtVector3) );
			memcpy( &Cross.m_vYAxis, &MatParent._21, sizeof(EtVector3) );
			memcpy( &Cross.m_vZAxis, &MatParent._31, sizeof(EtVector3) );
			Cross.m_vPosition += *(EtVector3*)&MatParent._41;
			Cross.m_vPosition -= *(EtVector3*)&m_LinkWorldMat._41;
		}
		else
		{
			if( m_nLinkType == CEtObject::LT_OBJECT )
			{
				
			}
		}

		Cross.MoveLocalZAxis( m_LinkPos.z );
		Cross.MoveLocalXAxis( m_LinkPos.x );
		Cross.MoveLocalYAxis( m_LinkPos.y );

		Cross.RotateYaw( m_LinkRotate.y );
		Cross.RotateRoll( m_LinkRotate.z );
		Cross.RotatePitch( m_LinkRotate.x );

		WorldMat = Cross;
	}
}

void CEtEffectObject::Render( float fElapsedTime )
{
	if( !m_bShow )
	{
		return;
	}

	EtMatrix WorldMat;
	ProcessLink( WorldMat );

	int i;
	for( i = 0; i < ( int )m_vecEffectElement.size(); i++ )
	{
		m_vecEffectElement[ i ]->Process( WorldMat, m_nEffectTick, m_fScale );
		m_vecEffectElement[ i ]->Render();
	}
}

void CEtEffectObject::RenderImmediate( float fElapsedTime )
{
	if( !m_bShow )
	{
		return;
	}

	EtMatrix WorldMat;
	ProcessLink( WorldMat );

	int i;
	for( i = 0; i < ( int )m_vecEffectElement.size(); i++ )
	{
		m_vecEffectElement[ i ]->Process( WorldMat, m_nEffectTick, m_fScale );
		m_vecEffectElement[ i ]->RenderImmediate();
	}
}

bool CEtEffectObject::CalcTick( float fElapsedTime )
{
	if( !m_hEffectData ) {
		m_bDelete = true;
		return true;
	}
	int nTickAdd;

	m_nPrevTick = m_nEffectTick;
	m_fFloatTick += ( fElapsedTime * 60 );
	nTickAdd = ( int )( m_fFloatTick );
	m_nEffectTick += nTickAdd;
	m_fFloatTick -= nTickAdd;
	if( m_nEffectTick >= ( int )m_hEffectData->GetEffectLength() )
	{
		if( m_bLoop ) {
			m_nEffectTick -= ( int )m_hEffectData->GetEffectLength();
		}
		else {
			m_bDelete = true;
			return true;
		}
	}

	return false;
}

void CEtEffectObject::RenderEffectObjectList( float fElapsedTime )
{
	ScopeLock<CSyncLock> Lock( s_SmartPtrLock );

	int i, nCount;
	EtEffectObjectHandle hHandle;
	std::vector< EtEffectObjectHandle > vecDeleteList;

	nCount = GetItemCount();
	for( i = 0; i < nCount; i++ )
	{
		hHandle = GetItem( i );
		if( hHandle->m_bDelete )
		{
			vecDeleteList.push_back( hHandle );
			continue;
		}
		hHandle->CalcTick( fElapsedTime );
		if( hHandle->GetRenderType() == RT_SCREEN )
		{
			hHandle->Render( fElapsedTime );
		}
	}

	nCount = ( int )vecDeleteList.size();
	for( i = 0; i < nCount; i++ )
	{
		SAFE_RELEASE_SPTR( vecDeleteList[ i ] );
	}
}

void CEtEffectObject::ProcessDeleteEffectObjectList( float fElapsedTime )
{
	ScopeLock<CSyncLock> Lock( s_SmartPtrLock );

	int i, nCount;
	EtEffectObjectHandle hHandle;
	std::vector< EtEffectObjectHandle > vecDeleteList;

	nCount = GetItemCount();
	for( i = 0; i < nCount; i++ )
	{
		hHandle = GetItem( i );
		if( hHandle->m_bDelete )
		{
			vecDeleteList.push_back( hHandle );
			continue;
		}
		hHandle->CalcTick( fElapsedTime );
	}

	nCount = ( int )vecDeleteList.size();
	for( i = 0; i < nCount; i++ )
	{
		SAFE_RELEASE_SPTR( vecDeleteList[ i ] );
	}
}

void CEtEffectObject::SetWorldMat( EtMatrix *mat )
{
	m_WorldMat = *mat;
}

void CEtEffectObject::SetScale( float fScale )
{
	m_fScale = fScale;
}

void CEtEffectObject::SetAlpha( float fAlpha )
{
	for( DWORD i=0; i<m_vecEffectElement.size(); i++ ) {
		m_vecEffectElement[ i ]->SetAlpha( fAlpha );
	}
}

RenderType CEtEffectObject::SetRenderType( RenderType Type )
{
	int i;
	for( i = 0; i < ( int )m_vecEffectElement.size(); i++ )
	{
		m_vecEffectElement[ i ]->SetRenderType( Type );
	}
	RenderType Ret = m_RenderType;
	m_RenderType = Type;
	return Ret;
}

void CEtEffectObject::SetLink( EtMatrix WorldMat, char *szBoneName, EtVector3 vPos, EtVector3 vRotate, EtAniObjectHandle hParent )
{
	m_LinkWorldMat = WorldMat;
	m_LinkBoneIndex = -1;
	m_nLinkType = CEtObject::LT_OBJECT;
	
	if( hParent && szBoneName && strlen(szBoneName) > 0 )
	{
		EtMeshHandle hMesh = hParent->GetMesh();
		if( hMesh )
		{
			m_LinkBoneIndex = hMesh->FindDummy( szBoneName );
			if( m_LinkBoneIndex != -1 )
			{
				m_nLinkType = CEtObject::LT_MESH_DUMMY;
			}
			else
			{
				m_LinkBoneIndex = hMesh->FindBone( szBoneName );
				m_nLinkType = CEtObject::LT_BONE;
			}
		}
	}

	m_LinkPos = vPos;
	m_LinkRotate = vRotate;
	m_hLinkParent = hParent;
	m_bLink = true;
}

const char *CEtEffectObject::GetEffectFileName()
{
	if( !m_hEffectData ) return "";
	return m_hEffectData->GetFileName();
}

void CEtEffectObject::EnableCull( bool bEnable )
{
	for( DWORD i=0; i<m_vecEffectElement.size(); i++ ) {
		m_vecEffectElement[i]->EnableCull( bEnable );
	}
}