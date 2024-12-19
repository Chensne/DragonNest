#include "StdAfx.h"
#include "DnCutSceneWorldGrid.h"
#include "DnCutSceneWorldSector.h"
#include "DnCutSceneWorld.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnCutSceneWorldGrid::CDnCutSceneWorldGrid( CEtWorld* pWorld ) : CEtWorldGrid( pWorld )
{

}

CDnCutSceneWorldGrid::~CDnCutSceneWorldGrid(void)
{

}


CEtWorldSector* CDnCutSceneWorldGrid::AllocSector( void )
{
	CDnCutSceneWorldSector* pNewSector = new CDnCutSceneWorldSector;
	pNewSector->SetResPathFinder( static_cast<CDnCutSceneWorld*>(m_pWorld)->GetResPathFinder() );
	return pNewSector;
}

