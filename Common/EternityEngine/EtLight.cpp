#include "StdAfx.h"
#include "EtLight.h"
#include "EtOptionController.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif

DECL_SMART_PTR_STATIC( CEtLight, 5 )

EtColor CEtLight::s_GlobalAmbient = EtColor( 1.0f, 1.0f, 1.0f, 1.0f );

std::vector< CEtLight * > CEtLight::s_vecDirectionalLight;
std::vector< CEtLight * > CEtLight::s_vecPointLight;
std::vector< CEtLight * > CEtLight::s_vecSpotLight;

std::vector< CEtLight * > CEtLight::s_vecInfluencePointLight;
std::vector< CEtLight * > CEtLight::s_vecInfluenceSpotLight;

float CEtLight::s_fDirLightAttenuation = 1.0f;

CEtOctree< CEtLight * > *CEtLight::s_pLightOctree = NULL;
float CEtOctreeNode< CEtLight * >::s_fMinRadius = 1000.0f;

CEtLight::CEtLight(void)
{
	m_bEnable = true;
	m_pCurOctreeNode = NULL;
}

CEtLight::~CEtLight(void)
{
	std::vector< CEtLight * > *pvecLight;
	std::vector< CEtLight * >::iterator Iter;

	if( m_pCurOctreeNode )
	{
		s_pLightOctree->Remove( this, m_pCurOctreeNode );
	}

	switch( m_LightInfo.Type )
	{
		case LT_DIRECTIONAL:
			pvecLight = &s_vecDirectionalLight;
			break;
		case LT_POINT:
			pvecLight = &s_vecPointLight;
			break;
		case LT_SPOT:
			pvecLight = &s_vecSpotLight;
			break;
	}

	Iter =  std::find( pvecLight->begin(), pvecLight->end(), this );
	if( Iter != pvecLight->end() )
	{
		pvecLight->erase( Iter );
	}	
	SAFE_RELEASE_SPTR( m_hPointLightDecal );
}

void CEtLight::InitializeClass()
{
	s_pLightOctree = new CEtOctree< CEtLight * >;
}

void CEtLight::FinalizeClass()
{
	SAFE_DELETE( s_pLightOctree );
}

void CEtLight::Initialize( SLightInfo *pSLightInfo, EtObjectHandle hCastLightObject )
{
	m_LightInfo = *pSLightInfo;
	m_hLightCastObject = hCastLightObject;

	switch( m_LightInfo.Type )
	{
		case LT_DIRECTIONAL:
			s_vecDirectionalLight.push_back( this );
			break;
		case LT_POINT:
			{
				SSphere BoundingSphere;

				if( !m_hLightCastObject )
				{
					AddDecal();
				}
				s_vecPointLight.push_back( this );
				BoundingSphere.Center = m_LightInfo.Position;
				BoundingSphere.fRadius = m_LightInfo.fRange;
				m_pCurOctreeNode = s_pLightOctree->Insert( this, BoundingSphere );
				if( hCastLightObject )
				{
					hCastLightObject->CalcLightInfluence();
				}
				else
				{
					CEtObject::CalcLightInfluenceList();
				}
			}
			break;
		case LT_SPOT:
			{
				SSphere BoundingSphere;

				s_vecSpotLight.push_back( this );
				BoundingSphere.Center = m_LightInfo.Position;
				BoundingSphere.fRadius = m_LightInfo.fRange;
				m_pCurOctreeNode = s_pLightOctree->Insert( this, BoundingSphere );
				if( hCastLightObject )
				{
					hCastLightObject->CalcLightInfluence();
				}
				else
				{
					CEtObject::CalcLightInfluenceList();
				}
			}
			break;
	}
}

void CEtLight::SetLightInfo( SLightInfo *pSLightInfo )
{
	if( m_LightInfo.Type != pSLightInfo->Type )
	{
		return;
	}

	m_LightInfo = *pSLightInfo;
	if( ( m_LightInfo.Type == LT_POINT ) && ( !m_hLightCastObject ) )
	{
		SAFE_RELEASE_SPTR( m_hPointLightDecal );
		AddDecal();
	}
}

void CEtLight::GetBoundingSphere( SSphere &Sphere, bool bActorSize/* = false*/ )
{
	Sphere.Center = m_LightInfo.Position;
	Sphere.fRadius = m_LightInfo.fRange;
}

void CEtLight::AddDecal()
{
	EtTerrainHandle hTerrainArea;
	float fLightHeight, fRadius;

	m_LightInfo.Diffuse.a = 1.0f;
	hTerrainArea = CEtTerrainArea::GetTerrainArea( m_LightInfo.Position.x, m_LightInfo.Position.z );
	if( !hTerrainArea )
	{
		return;
	}
	fLightHeight = m_LightInfo.Position.y - hTerrainArea->GetLandHeight( m_LightInfo.Position.x, m_LightInfo.Position.z, NULL );
	if( fLightHeight < 0.0f )
	{
		return;
	}
	fRadius = m_LightInfo.fRange * m_LightInfo.fRange - fLightHeight * fLightHeight;
	if( fRadius < 0.0f )
	{
		return;
	}

	CEtPointLightDecal *pDecal;

	pDecal = new CEtPointLightDecal();
	pDecal->Initialize( m_LightInfo, sqrtf( fRadius ), FLT_MAX );
	m_hPointLightDecal = pDecal->GetMySmartPtr();
}

SLightInfo *CEtLight::GetShadowCastDirLightInfo()
{
	//ASSERT( !s_vecDirectionalLight.empty() );
	if( s_vecDirectionalLight.empty() )
	{
		return NULL;
	}

	int i;

	for( i = 0; i < ( int )s_vecDirectionalLight.size(); i++ )
	{
		if( ( s_vecDirectionalLight[ i ]->m_LightInfo.bCastShadow ) && ( s_vecDirectionalLight[ i ]->IsEnable() ) )
		{
			return s_vecDirectionalLight[ i ]->GetLightInfo();
		}
	}

	return s_vecDirectionalLight[ 0 ]->GetLightInfo();
}

void CEtLight::SetWorldSize( EtVector3 &WorldCenter, float fRadius )
{
	s_pLightOctree->Initialize( WorldCenter, fRadius );
}

void CEtLight::CalcLightInfluence( SSphere &Sphere, DNVector(CEtLight *) &vecInfluenceLight, EtObjectHandle CalcLightObject )
{
	s_pLightOctree->Pick( Sphere, vecInfluenceLight );
	DNVector( CEtLight * )::iterator it;
	for( it = vecInfluenceLight.begin(); it != vecInfluenceLight.end(); )
	{
		if( ( *it )->m_hLightCastObject )
		{
			if( ( *it )->m_hLightCastObject != CalcLightObject )
			{
				it = vecInfluenceLight.erase( it );
				continue;
			}
		}
		++it;
	}
}

void CEtLight::SetInfluenceLight( CEtLight **ppLight )
{
	int nIndex;

	s_vecInfluencePointLight.clear();
	s_vecInfluenceSpotLight.clear();
	if( ppLight == NULL ) return;
	nIndex = 0;
	while( ppLight[ nIndex ] )
	{
		switch( ppLight[ nIndex ]->m_LightInfo.Type )
		{
			case LT_POINT:
				s_vecInfluencePointLight.push_back( ppLight[ nIndex ] );
				break;
			case LT_SPOT:
				s_vecInfluenceSpotLight.push_back( ppLight[ nIndex ] );
				break;
		}
		nIndex++;
	}
}
