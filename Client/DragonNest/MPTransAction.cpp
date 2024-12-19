#include "StdAfx.h"
#include "MPTransAction.h"

#include "DNPacket.h"
#include "DNProtocol.h"
#include "VillageSendPacket.h"

#include "GameSendPacket.h"

#include "GameClientSession.h"
#include "ClientSessionManager.h"

#include "DnWorldProp.h"
#include "TaskManager.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

MPTransAction::MPTransAction()
{
}

MPTransAction::~MPTransAction()
{
}

void MPTransAction::Send( DWORD dwProtocol, DnActorHandle hActor, CMemoryStream *pStream, ClientSessionTypeEnum SessionType )
{
	if( SessionType == ClientSessionTypeEnum::Auto ) SessionType = m_DefaultSession;
	if( SessionType == ClientSessionTypeEnum::Unknown ) return;

	CDnWorldProp *pProp = dynamic_cast<CDnWorldProp *>(this);
	if( !pProp ) return;
	DWORD dwUniqueID = pProp->GetUniqueID();

	switch( SessionType ) {
		case ClientSessionTypeEnum::Tcp:
			break;
		case ClientSessionTypeEnum::Udp:
			{
				CGameClientSession *pSession = dynamic_cast<CGameClientSession *>(m_pTask);
				SendGamePropMsg(dwUniqueID, (USHORT)dwProtocol, ( pStream ) ? (BYTE*)pStream->GetBuffer() : NULL, ( pStream ) ? (int)pStream->Tell() : 0, _RELIABLE);
			}
			break;
	}
}

void MPTransAction::Send( DWORD dwProtocol, CMemoryStream *pStream, ClientSessionTypeEnum SessionType, bool bImmediate )
{
	if( SessionType == ClientSessionTypeEnum::Auto ) SessionType = m_DefaultSession;
	if( SessionType == ClientSessionTypeEnum::Unknown ) return;

	CDnWorldProp *pProp = dynamic_cast<CDnWorldProp *>(this);
	if( !pProp ) return;
	DWORD dwUniqueID = pProp->GetUniqueID();

	int nUserCount = 0;
	switch( SessionType ) {
		case ClientSessionTypeEnum::Tcp:
			break;
		case ClientSessionTypeEnum::Udp:
			{
				CGameClientSession *pSession = dynamic_cast<CGameClientSession *>(m_pTask);
				SendGamePropMsg(dwUniqueID, (USHORT)dwProtocol, ( pStream ) ? (BYTE*)pStream->GetBuffer() : NULL, ( pStream ) ? pStream->Tell() : 0, _RELIABLE);
			}
			break;
	}
}
