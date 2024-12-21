#include "Stdafx.h"
#include "DnWorldKeepOperationProp.h"
#include "DnPropStateTrigger.h"
#include "DNUserSession.h"
#include "DNGameRoom.h"
#include "DnWorld.h"

CDnWorldKeepOperationProp::CDnWorldKeepOperationProp(CMultiRoom * pRoom) : CDnWorldOperationProp(pRoom)
{
	_ClearClickedInfo( NULL );
}

CDnWorldKeepOperationProp::~CDnWorldKeepOperationProp()
{
}

bool CDnWorldKeepOperationProp::Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale )
{
	return CDnWorldOperationProp::Initialize( pParentSector, szPropName, vPos, vRotate, vScale );
}

void CDnWorldKeepOperationProp::Process(LOCAL_TIME LocalTime, float fDelta)
{
	_CheckKeepProp(LocalTime);
	CDnWorldOperationProp::Process(LocalTime, fDelta);
}

void CDnWorldKeepOperationProp::OnDispatchMessage( CDNUserSession *pSession, DWORD dwProtocol, BYTE *pPacket )
{
	switch( dwProtocol ) 
	{
		case eProp::CS_CMDKEEPCLICKCANCLE:
			OnCancelKeepOperationProp( pSession );
			break;
	}

	CDnWorldOperationProp::OnDispatchMessage( pSession, dwProtocol, pPacket );
}

bool CDnWorldKeepOperationProp::OnClickKeepProp( CDNUserSession * pSession, int nOperationTime, int nStringID, const char * szActionName )
{
	if (m_bClicked && m_nClickedUniqueID > 0)
	{
		_DANGER_POINT_MSG(L"두번은 못 눌러요");
		return false;
	}

	if (pSession)
	{
		m_bClicked = true;
		m_nPropKeepClickTick = nOperationTime;
		m_nClickedSessionID = pSession->GetSessionID();
		m_nClickedUniqueID = pSession->GetActorHandle() ? pSession->GetActorHandle()->GetUniqueID() : -1;
		m_PropClickedTime = GetLocalTime();

		pSession->SetKeepOperationID(GetUniqueID());

		SendKeepOperationStart( pSession->GetActorHandle(), nOperationTime, nStringID, szActionName );
		return true;
	}
	return false;
}

void CDnWorldKeepOperationProp::OnCancelKeepOperationProp( CDNUserSession * pSession )
{
	_ClearClickedInfo( pSession );

	CDnWorld::GetInstance(GetRoom()).OnTriggerEventCallback( "CDnWorldProp::OnKeepOperationPropFail", GetLocalTime(), 0.f );

	if( pSession )
		SendKeepOperationResult( pSession->GetActorHandle(), false );
}

void CDnWorldKeepOperationProp::_CheckKeepProp(LOCAL_TIME LocalTime)
{
	if ( m_bClicked && m_nClickedUniqueID > 0 && m_PropClickedTime + m_nPropKeepClickTick <= LocalTime)
	{
		CDNUserSession * pSession = GetGameRoom()->GetUserSession(m_nClickedSessionID);
		if (pSession == NULL)
		{
			_DANGER_POINT_MSG(L"이러면 클릭은 해두셨는데 중간에 나가셨거나.....하는 경우이다");
			_ClearClickedInfo( NULL );
			return;
		}

		_ClearClickedInfo( pSession );

		CDnWorld::GetInstance(GetRoom()).OnTriggerEventCallback( "CDnWorldProp::OnKeepOperationPropSuccess", GetLocalTime(), 0.f );
		SendKeepOperationResult( pSession->GetActorHandle(), true );
		return;
	}
}

void CDnWorldKeepOperationProp::_ClearClickedInfo( CDNUserSession * pSession )
{
	m_bClicked = false;

	m_nClickedSessionID = 0;
	m_nClickedUniqueID = 0;
	m_PropClickedTime = 0;
	m_nPropKeepClickTick = 0;

	if( pSession )
		pSession->SetKeepOperationID( 0 );
}

void CDnWorldKeepOperationProp::SendKeepOperationStart( DnActorHandle hActor, int nOperationTime, int nStringID, const char * szActionName )
{
	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	int nActorHandle = hActor->GetUniqueID();
	int nActionIndex = hActor->GetElementIndex( szActionName );
	Stream.Write( &nActionIndex, sizeof(int) );
	Stream.Write( &nActorHandle, sizeof(int) );
	Stream.Write( &nOperationTime, sizeof(int) );
	Stream.Write( &nStringID, sizeof(int) );

	Send( eProp::SC_CMDKEEPCLICKSTART, hActor, &Stream );
}

void CDnWorldKeepOperationProp::SendKeepOperationResult(DnActorHandle hActor, bool bRsult)
{
	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	Stream.Write( &bRsult, sizeof(bool) );

	Send( eProp::SC_CMDKEEPCLICK, hActor, &Stream );
}