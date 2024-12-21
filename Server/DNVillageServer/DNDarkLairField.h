
#pragma once

#include "DNField.h"

// 다크레어 마을
class CDNDarkLairField : public CDNField
{
public:

	CDNDarkLairField();
	virtual ~CDNDarkLairField();

	virtual bool bIsPvPVillage(){ return false; }
	virtual bool bIsPvPLobby(){ return false; }
	virtual bool bIsDarkLairVillage(){ return true; }
	virtual GameTaskType::eType GetGameTaskType(){ return GameTaskType::DarkLair; }

private:

};
