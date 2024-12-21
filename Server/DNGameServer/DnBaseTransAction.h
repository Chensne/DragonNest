#pragma once

class CMemoryStream;
class CDNGameRoom;
class CDNUserSession;

class CDnBaseTransAction {
public:
	CDnBaseTransAction() { m_pRoom = NULL; }
	virtual ~CDnBaseTransAction() {}

protected:
	CDNGameRoom *m_pRoom;

public:
	virtual void InitializeRoom( CDNGameRoom * pRoom) { m_pRoom = pRoom; }
	CDNGameRoom *GetGameRoom() { return m_pRoom; }

	/*CDnBaseTransAction() { m_pSession = NULL; }
	virtual ~CDnBaseTransAction() {}

protected:
	CDNSession * m_pSession;

public:
	virtual void InitializeRoom( CDNSession *pSession ) { 
		m_pSession = pSession;
	}
	CDNSession *GetSession() { return m_pSession; }*/

	virtual void Send( DWORD dwProtocol, DnActorHandle hActor, CMemoryStream *pStream, GlobalEnum::ClientSessionTypeEnum SessionType = GlobalEnum::ClientSessionTypeEnum::Auto ) = 0;
	virtual void Send( DWORD dwProtocol, CMemoryStream *pStream, GlobalEnum::ClientSessionTypeEnum SessionType = GlobalEnum::ClientSessionTypeEnum::Auto, bool bImmediate = true ) = 0;
	virtual void OnDispatchMessage( CDNUserSession *pSession, DWORD dwProtocol, BYTE *pPacket ) = 0;
};
