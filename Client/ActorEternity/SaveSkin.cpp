#include "StdAfx.h"
#include ".\saveskin.h"
#include "IDXMaterial.h"
#include "RTMax.h"

CSaveSkin::CSaveSkin(void)
{
}

CSaveSkin::~CSaveSkin(void)
{
}

void CSaveSkin::SaveSkin( FILE *fp, Mtl *pMtl )
{
	int i;
	IParamBlock2 *pParamBlock;
	ParamDef Def;
	SParamStruct Param;

	if( CheckDirectX9ShaderClass( pMtl ) )
	{
		IDxMaterial3 *pDXMtl3;

		m_VecParamList.clear();
		pDXMtl3 = ( IDxMaterial3 * )pMtl->GetInterface( IDXMATERIAL3_INTERFACE );

		pParamBlock = pMtl->GetParamBlock( 0 );
		for( i = 0; i < pParamBlock->NumParams(); i++ )
		{
			Def = pParamBlock->GetParamDef( i );
			Param.Type = PT_UNKNOWN;
			strcpy( Param.szParamName, Def.int_name );

			if( stricmp( Param.szParamName, "g_LightDir" ) == 0 )
			{
				continue;
			}
			if( stricmp( Param.szParamName, "g_EnvTex" ) == 0 )
			{
				continue;
			}

			switch( Def.type )
			{
				case TYPE_INT:
					Param.Type = PT_INT;
					Param.nInt = pParamBlock->GetInt( i );
					break;
				case TYPE_FLOAT:
					Param.Type = PT_FLOAT;
					Param.fFloat = pParamBlock->GetFloat( i );
					break;
				case TYPE_BITMAP:
				case TYPE_TEXMAP:
				{
					Texmap *pTex = pParamBlock->GetTexmap( i );
					PBBitmap *pBitmap = pParamBlock->GetBitmap( i );
					Param.Type = PT_TEX;
					if( pBitmap )
					{
						Param.szStr = ( TCHAR * )pBitmap->bi.Name();
					}
					else
					{
						Param.szStr = _T( "" );
					}
					break;
				}
				case TYPE_RGBA:
				case TYPE_FRGBA:
					Param.Type = PT_FLOAT4;
					Param.fFloat4[0] = pParamBlock->GetAColor( i ).r;
					Param.fFloat4[1] = pParamBlock->GetAColor( i ).g;
					Param.fFloat4[2] = pParamBlock->GetAColor( i ).b;
					Param.fFloat4[3] = pParamBlock->GetAColor( i ).a;
					break;
				case TYPE_POINT4:
					Param.Type = PT_FLOAT4;
					Param.fFloat4[0] = pParamBlock->GetPoint4( i ).x;
					Param.fFloat4[1] = pParamBlock->GetPoint4( i ).y;
					Param.fFloat4[2] = pParamBlock->GetPoint4( i ).z;
					Param.fFloat4[3] = pParamBlock->GetPoint4( i ).w;
					break;
				default:
					assert( 0 );
					break;
			}
			if( Param.Type != PT_UNKNOWN )
			{
				m_VecParamList.push_back( Param );
			}
		}
	}
//	else	// Default.fx �� �ʿ��� �Ķ���� ���� �ʿ��ϴ�.
	{
	}

	int nCount;
	SSubSkinHeader SubSkinHeader;
	char szName[ 256 ], szTemp[ 256 ], *pFindPtr;
	IDxMaterial *pDXMtl;

	memset( &SubSkinHeader, 0, sizeof( SSubSkinHeader ) );
	SubSkinHeader.bEnableAlpha = false;
	SubSkinHeader.fAlphaValue = 1.0f;
	strcpy( SubSkinHeader.szMaterialName, pMtl->GetName() );

	pDXMtl = ( IDxMaterial * )pMtl->GetInterface( IDXMATERIAL_INTERFACE );
	if( pDXMtl )
	{
		strcpy( szTemp, pDXMtl->GetEffectFilename() );
	}
	else
	{
		strcpy( szTemp, "diffuse.fx" );
	}
	pFindPtr = strstr( szTemp, ".fxo" );
	if( pFindPtr )
	{
		strcpy( pFindPtr, ".fx" );
	}

	pFindPtr = strrchr( szTemp, '\\' );		// �н������ϰ� �����̸��� ����..
	if( pFindPtr )
	{
		strcpy( SubSkinHeader.szEffectName, pFindPtr + 1 );
	}
	else
	{
		strcpy( SubSkinHeader.szEffectName, szTemp );
	}
	fwrite( &SubSkinHeader, sizeof( SSubSkinHeader ), 1, fp );

	nCount = m_VecParamList.size();
	fwrite( &nCount, sizeof( int ), 1, fp );
	for( i = 0; i < nCount; i++ )
	{
		WriteString( fp, m_VecParamList[ i ].szParamName );
		fwrite( &m_VecParamList[ i ].Type, sizeof( ExportParamType ), 1, fp );
		switch( m_VecParamList[ i ].Type )
		{
			case PT_INT:
				fwrite( &m_VecParamList[ i ].nInt, sizeof( int ), 1, fp );
				break;
			case PT_FLOAT:
				fwrite( &m_VecParamList[ i ].fFloat, sizeof( float ), 1, fp );
				break;
			case PT_FLOAT4:
				fwrite( m_VecParamList[ i ].fFloat4, sizeof( float ) * 4, 1, fp );
				break;
			case PT_TEX:
				memset( szName, 0, 256 );
				pFindPtr = strrchr( m_VecParamList[ i ].szStr, '\\' );
				if( pFindPtr )
				{
					strcpy( szName, pFindPtr + 1 );
				}
				else
				{
					strcpy( szName, m_VecParamList[ i ].szStr );
				}
				WriteString( fp, szName );
				break;
		}
	}
}

bool CSaveSkin::CheckDirectX9ShaderClass( Mtl *pMtl )
{
	TSTR Str;

	pMtl->GetClassName( Str );
	if( !( strstr( Str, "DirectX" ) && strstr( Str, "Shader" ) ) )
	{
		return false;
	}

	IDxMaterial *idxm = ( IDxMaterial * )pMtl->GetInterface( IDXMATERIAL_INTERFACE );
	if( idxm == NULL )
	{
		return false;
	}
	return true;
}

void CSaveSkin::WriteString( FILE *fp, const char *pString )
{
	int nLength;
	char szName[ 2048 ];

	nLength = strlen( pString ) + 1;
	fwrite( &nLength, sizeof( int ), 1, fp );
	strcpy( szName, pString );
	fwrite( szName, nLength, 1, fp );
}
