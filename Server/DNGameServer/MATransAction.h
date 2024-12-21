#pragma once

#include "DnBaseTransAction.h"
class CClientSession;
class CMemoryStream;
class CDNUserSession;
class CDnActor;

class MATransAction : public CDnBaseTransAction {
public:
	MATransAction();
	virtual ~MATransAction();

protected:
	CDnActor *m_pActor;
	bool m_bIsValidActor;

protected:
	bool IsValidActor();

public:
	virtual void Send( DWORD dwActorProtocol, DnActorHandle hActor, CMemoryStream *pStream, GlobalEnum::ClientSessionTypeEnum SessionType = GlobalEnum::ClientSessionTypeEnum::Auto );
	virtual void Send( DWORD dwActorProtocol, CMemoryStream *pStream, GlobalEnum::ClientSessionTypeEnum SessionType = GlobalEnum::ClientSessionTypeEnum::Auto, bool bImmediate = true );
	virtual void Send( DWORD dwActorProtocol, UINT nExceptSessionID, CMemoryStream *pStream, GlobalEnum::ClientSessionTypeEnum SessionType = GlobalEnum::ClientSessionTypeEnum::Auto );
	virtual void Send( DWORD dwActorProtocol, DWORD dwUniqueID, CMemoryStream *pStream );
	virtual void Send( CDNUserSession* pGameSession, DWORD dwActorProtocol, DWORD dwUniqueID, CMemoryStream *pStream );
	virtual void OnDispatchMessage( CDNUserSession *pSession, DWORD dwActorProtocol, BYTE *pPacket ) {}

private:

	int	_GetPrior( DWORD dwActorProtocol );
};