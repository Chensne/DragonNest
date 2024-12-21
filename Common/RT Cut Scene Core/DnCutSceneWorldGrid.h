#pragma once

#include "EtWorldGrid.h"


class CEtWorld;

class CDnCutSceneWorldGrid : public CEtWorldGrid
{
private:
	

protected:
	// from CEtWorld
	CEtWorldSector* AllocSector( void );


public:
	CDnCutSceneWorldGrid( CEtWorld* pWorld );
	virtual ~CDnCutSceneWorldGrid(void);
};
