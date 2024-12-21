#pragma once

#include "DnParts.h"

class CDnPartsWing : public CDnParts 
{
public:
	CDnPartsWing();
	virtual ~CDnPartsWing();

	enum WingType {
		None,
		GuildFlag,
	};

protected:
	WingType m_WingType;
	int m_nGuildMarkSubMeshIndex;
	int m_nTextureIndex;

public:
	virtual bool CreateObject( DnActorHandle hActor, const char *szAniName );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
};