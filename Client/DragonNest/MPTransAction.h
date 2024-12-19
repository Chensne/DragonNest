#pragma once

#include "DnBaseTransAction.h"

class CMemoryStream;
class MPTransAction : public CDnBaseTransAction {
public:
	MPTransAction();
	virtual ~MPTransAction();

	/*
	enum PROP_PROTOCOL {
		PPT_ONDAMAGE = 0,
		PPT_CMDACTION = 1,
	};
	*/

protected:

public:

public:
	// CDnBaseTransAction Message
	virtual void Send( DWORD dwProtocol, DnActorHandle hActor, CMemoryStream *pStream, ClientSessionTypeEnum SessionType = ClientSessionTypeEnum::Auto );
	virtual void Send( DWORD dwProtocol, CMemoryStream *pStream, ClientSessionTypeEnum SessionType = ClientSessionTypeEnum::Auto, bool bImmediate = true );
	virtual void OnDispatchMessage( DWORD dwProtocol, BYTE *pPacket ) {}

};