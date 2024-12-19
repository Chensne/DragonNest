#include "StdAfx.h"
#include "DNPropNpcObject.h"
#include "EtWorldProp.h"
#include "EtWorldSector.h"
#include "EtWorldGrid.h"

CDNPropNpcObject::CDNPropNpcObject(void)
{
	m_NpcType = PropNpc;
	m_nPropCreateUniqueID = -1;
}

CDNPropNpcObject::~CDNPropNpcObject(void)
{
}

bool CDNPropNpcObject::CreateNpc( TNpcData *pNpcData, UINT nUID, int nChannelID, int nMapIndex, CEtWorldProp *pProp )
{
	memset(&m_NpcData, 0, sizeof(TNpcData));
	memcpy(&m_NpcData, pNpcData, sizeof(TNpcData));

	// 포지션 구하고.
	TPosition Pos;
	EtVector3 vPos = *pProp->GetSector()->GetOffset();
	vPos.x -= ( pProp->GetSector()->GetTileWidthCount() * pProp->GetSector()->GetTileSize() ) / 2.f;
	vPos.z -= ( pProp->GetSector()->GetTileHeightCount() * pProp->GetSector()->GetTileSize() ) / 2.f;
	vPos.y = 0.f;

	vPos += *pProp->GetPosition();

	Pos.nX = (int)( vPos.x * 1000.f );
	Pos.nY = (int)( vPos.y * 1000.f );
	Pos.nZ = (int)( vPos.z * 1000.f );

	InitObject(NULL, nUID, nChannelID, nMapIndex, &Pos);

	m_BaseData.fRotate = -pProp->GetRotation()->y;

	m_nPropCreateUniqueID = pProp->GetCreateUniqueID();

	CDnNpc::Create(pNpcData);

	EnterWorld();

	return true;
}