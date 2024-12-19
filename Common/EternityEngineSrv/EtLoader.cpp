#include "Stdafx.h"
#include "EtLoader.h"
#include "EtResource.h"
#include "EtAni.h"
#include "EtSkin.h"
#include "EtMesh.h"
#include "CriticalSection.h"
#include "MultiCommon.h"
#include "MultiRoom.h"

std::vector< std::string > g_vecResourceDir;
CSyncLock g_ResourceSyncLock;

EtResourceHandle LoadResource( CMultiRoom *pRoom, const char *pFileName, ResourceType Type )
{
	EtResourceHandle Handle;
	if( pFileName[ 0 ] == 0 )
	{
		return CEtResource::Identity();
	}

	Handle = CEtResource::GetResource( pRoom, pFileName );
	if( !Handle )
	{
		CEtResource *pResource;

		CEtResourceAccess Access( pFileName );

		if( Access.IsValid() == false ) {
			OutputDebug( "%s File Not Found!!!\n", pFileName );
			return CEtResource::Identity();
		}

		switch( Type )
		{
			case RT_ANI:
				pResource = new CEtAni( pRoom );
				break;
			case RT_SKIN:
				pResource = new CEtSkin( pRoom );
				break;
			case RT_MESH:
				pResource = new CEtMesh( pRoom );
				break;
			default:
				ASSERT( 0 && "Invalid Resource Type!!!" );
				return Handle;
		}

		Handle = pResource->GetMySmartPtr();
		Handle->AddRef();
		pResource->SetResourceType( Type );
		pResource->Load( Access.GetFullName() );
	}
	else {
		Handle->AddRef();
	}

	return Handle;
}
