
#include "StdAfx.h"
#include "DnPvPPartyTask.h"
#include "DNUserSession.h"

CDnPvPPartyTask::CDnPvPPartyTask( CDNGameRoom* pRoom )
: CDnPartyTask( pRoom )
{
	//g_Log.LogA( "CDnPvPPartyTask »ý¼º!!!\r\n" );
}

CDnPvPPartyTask::~CDnPvPPartyTask()
{
	//g_Log.LogA( "CDnPvPPartyTask ÆÄ±«!!!\r\n" );
}

void CDnPvPPartyTask::OutPartyMember( UINT nOutPartyMemberUID, UINT nNewLeaderUID, char cKickKind )
{
	for( DWORD i=0; i<GetUserCount(); i++ ) 
	{
		CDNUserSession* pSession = GetUserData(i);
		if( pSession )
			pSession->SendPartyOut(ERROR_NONE, cKickKind, nOutPartyMemberUID);
			//pSession->SendDelPartyMember( nOutPartyMemberUID, bIsUnintended );
	}
}
