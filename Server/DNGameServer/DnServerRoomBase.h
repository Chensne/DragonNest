#pragma once

class CDNGameRoom;
class CTask;
class CDnServerRoomBase {
public:
	CDnServerRoomBase() { m_pBaseRoom = NULL; }
	CDnServerRoomBase( CDNGameRoom *pRoom ) { Initialize( pRoom ); }
	virtual ~CDnServerRoomBase() {}

protected:
	CDNGameRoom *m_pBaseRoom;

public:
	void Initialize( CDNGameRoom *pRoom ) { m_pBaseRoom = pRoom; }

	CDNGameRoom *GetRoom() { return m_pBaseRoom; }
};
