
#pragma once

#include "DNField.h"

// PvP ����
class CDNPvPField:public CDNField
{
public:

	CDNPvPField();
	virtual ~CDNPvPField();

	virtual bool bIsPvPVillage(){ return true; }
	virtual bool bIsPvPLobby(){ return false; }

private:

};
