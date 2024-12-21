
#pragma once

class IPvPGameModeChecker
{
public:

	IPvPGameModeChecker( CPvPGameMode* pGameMode ):m_pPvPGameMode(pGameMode){}
	virtual ~IPvPGameModeChecker(){}

	virtual void Check() = 0;

	// Get
	CDNGameRoom*	GetGameRoom(){ return m_pPvPGameMode->GetGameRoom(); }

protected:

	CPvPGameMode* m_pPvPGameMode;
};
