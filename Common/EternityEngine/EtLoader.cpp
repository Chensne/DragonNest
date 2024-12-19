#include "Stdafx.h"
#include "EtLoader.h"
#include "EtResource.h"
#include "EtAni.h"
#include "EtSkin.h"
#include "EtMesh.h"
#include "EtBillboardEffect.h"
#include "EtEffectData.h"
#include "EtLoader.h"
#include "PerfCheck.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

std::vector< std::string > g_vecResourceDir;

/*
bool FindFile( const char *pFileName, char *pFullName )
{
int i;

for( i = 0; i < ( int )g_vecResourceDir.size(); i++ )
{
if( FindFileInDirectory( g_vecResourceDir[ i ].c_str(), pFileName, pFullName ) )
{
return true;
}
}

return false;
}
*/

#ifdef PRE_CRASH_CHECK_BACKUP
char g_LoadResource[ _MAX_PATH ];
#endif
EtResourceHandle LoadResource( const char *pFileName, ResourceType Type, bool bLoadImmediate )
{
	EtResourceHandle Handle;

	if( pFileName[ 0 ] == 0 )
	{
		return CEtResource::Identity();
	}

	Handle = CEtResource::GetResource( pFileName, true );
	if( !Handle )
	{
		CEtResource *pResource;

		//		CStream *pStream = CEtResourceMng::GetInstance().GetStream( pFileName );
		// pStream을 포인터로 쓴 이유는 로드 함수 내부에서 지워줘야 하기땜에 그렇다.
		// 지금은 쓰지않고 있는 백그라운드 로더 때문에 포인터 받아서 내부에서 지워줘야 한다
		CResMngStream *pStream = new CResMngStream( pFileName, true, true );
		bool bDeleteImmeidate = false;

		if( pStream->IsValid() == false )
		{
			if( ( Type != RT_SHADER ) || ( !CEtMaterial::IsLoadCompiledShaders() ) )
			{
				SAFE_DELETE( pStream );
				OutputDebug( "%s File Not Found!!!\n", pFileName );
				return CEtResource::Identity();
			}
		}

		switch( Type )
		{
			case RT_ANI:
				pResource = new CEtAni();
				break;
			case RT_SKIN:
				bDeleteImmeidate = true;
				pResource = new CEtSkin();
				break;
			case RT_MESH:
				pResource = new CEtMesh();
				break;
			case RT_SHADER:
//				bLoadImmediate = true;
				pResource = new CEtMaterial();
				break;
			case RT_TEXTURE:
				pResource = new CEtTexture();
				break;
			case RT_PARTICLE:
				bDeleteImmeidate = true;
				pResource = new CEtBillboardEffectData();
				break;
			case RT_EFFECT:
				bDeleteImmeidate = true;
				pResource = new CEtEffectData();
				break;
			default:
				SAFE_DELETE( pStream );
				ASSERT( 0 && "Invalid Resource Type!!!" );
				return Handle;
		}

		Handle = pResource->GetMySmartPtr();
		pResource->SetResourceType( Type );
		pResource->SetDeleteImmediate( bDeleteImmeidate );
		pResource->SetReady( false );
		if( pResource->Load( pStream ) != ET_OK )
		{
			delete pResource;
			return CEtResource::Identity();
		}
		pResource->SetReady( true );
	}

	return Handle;
}
