#include "StdAfx.h"
#include "EtCustomParam.h"

CEtCustomParam::CEtCustomParam(void)
{
}

CEtCustomParam::~CEtCustomParam(void)
{
}

void CEtCustomParam::Initialize( EtSkinHandle hSkin )
{
	m_hSkin = hSkin;
	m_vecCustomParams.resize( m_hSkin->GetMeshHandle()->GetSubMeshCount() );
}

int CEtCustomParam::AddCustomParam( const char *pParamName, EffectParamType Type )
{
	int i, j;
	EtMaterialHandle hMaterial;
	EtParameterHandle hParameter, CheckParameter;

	for( i = 0; i < ( int )m_vecCustomParams.size(); i++ )
	{
		hMaterial = m_hSkin->GetMaterialHandle( i );
		hParameter = hMaterial->GetParameterByName( pParamName );
		for( j = 0; j < ( int )m_vecCustomParams[ i ].size(); j++ )
		{
			if( m_vecCustomParams[ i ][ j ].hParamHandle == hParameter )
			{
				return j;
			}
		}
	}

	SCustomParam Param;

	CheckParameter = -1;
	for( i = 0; i < ( int )m_vecCustomParams.size(); i++ )
	{
		hMaterial = m_hSkin->GetMaterialHandle( i );
		hParameter = hMaterial->GetParameterByName( pParamName );
		Param.hParamHandle = hParameter;
		if( hParameter != -1 )
		{
			CheckParameter = hParameter;
		}
		Param.Type = Type;
		Param.nVariableCount = 1;
		m_vecCustomParams[ i ].push_back( Param );
	}
	ASSERT( CheckParameter != -1 );

	return ( int )m_vecCustomParams[ 0 ].size() - 1;
}

void CEtCustomParam::SetCustomParam( int nParamIndex, EffectParamType Type, void *pValue, int nSubMeshIndex )
{
	ASSERT( ( int )m_vecCustomParams[ 0 ].size() > nParamIndex );

	int i;

	switch( Type )
	{
	case EPT_INT:
		for( i = 0; i < ( int )m_vecCustomParams.size(); i++ )
		{
			if( ( nSubMeshIndex == -1 ) || ( nSubMeshIndex == i ) )
			{
				m_vecCustomParams[ i ][ nParamIndex ].nInt = *( int * )pValue;
			}
		}
		break;
	case EPT_FLOAT:
		for( i = 0; i < ( int )m_vecCustomParams.size(); i++ )
		{
			if( ( nSubMeshIndex == -1 ) || ( nSubMeshIndex == i ) )
			{
				m_vecCustomParams[ i ][ nParamIndex ].fFloat = *( float * )pValue;
			}
		}
		break;
	case EPT_VECTOR:
		for( i = 0; i < ( int )m_vecCustomParams.size(); i++ )
		{
			if( ( nSubMeshIndex == -1 ) || ( nSubMeshIndex == i ) )
			{
				memcpy( m_vecCustomParams[ i ][ nParamIndex ].fFloat4, pValue, sizeof( float ) * 4 );
			}
		}
		break;
	case EPT_TEX:
		for( i = 0; i < ( int )m_vecCustomParams.size(); i++ )
		{
			if( ( nSubMeshIndex == -1 ) || ( nSubMeshIndex == i ) )
			{
				m_vecCustomParams[ i ][ nParamIndex ].nTextureIndex = *( int * )pValue;
			}
		}
		break;
	default:
		ASSERT( 0 );
		break;
	}
}
