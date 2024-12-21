#pragma once

#include "DnBaseTransAction.h"

class CMemoryStream;
class CDNUserSession;

class MPTransAction : public CDnBaseTransAction {
public:
	MPTransAction();
	virtual ~MPTransAction();

protected:
	CDnWorldProp *m_pProp;
	bool m_bIsValidProp;
	bool IsValidProp();

public:

public:
	// CDnBaseTransAction Message
	virtual void Send( DWORD dwProtocol, DnActorHandle hActor, CMemoryStream *pStream, GlobalEnum::ClientSessionTypeEnum SessionType = GlobalEnum::ClientSessionTypeEnum::Auto );
	virtual void Send( DWORD dwProtocol, CMemoryStream *pStream, GlobalEnum::ClientSessionTypeEnum SessionType = GlobalEnum::ClientSessionTypeEnum::Auto, bool bImmediate = true );
	virtual void Send( DWORD dwProtocol, CMemoryStream *pStream, CDNUserSession* pSession, GlobalEnum::ClientSessionTypeEnum SessionType = GlobalEnum::ClientSessionTypeEnum::Auto, bool bImmediate = true );
	virtual void OnDispatchMessage( CDNUserSession *pSession, DWORD dwProtocol, BYTE *pPacket ) {}

};