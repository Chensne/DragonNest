
#pragma once

#include "DnDropItem.h"

class CEtWorldEventArea;

class CItemRespawnLogic
{
public:

	void	AddItemRespawnArea( CEtWorldEventArea* pArea );
	void	Process( const float fDelta );
	void	Reset();

protected:

	// CItemRespawnLogic 流立 积己 陛瘤
	CItemRespawnLogic( CDNGameRoom* pGameRoom):m_pGameRoom(pGameRoom)
	{
	}

	// ItemRespawn
	typedef struct tagItemRespawn
	{
		float				fOrgRespawnDelta;
		float				fRespawnDelta;
		UINT				uiItemDropTableID;
		SOBB				sOBB;
		DnDropItemHandle	hLastDropItem;
		bool				bIsStartSpawn;
		bool				bIsOrgStartSpawn;
		//
		tagItemRespawn( UINT uiSec, UINT uiTableID, SOBB* _pOBB, bool _bIsStartSpawn ):fOrgRespawnDelta(static_cast<float>(uiSec)),fRespawnDelta(static_cast<float>(uiSec)),uiItemDropTableID(uiTableID),sOBB(*_pOBB)
			,bIsOrgStartSpawn(_bIsStartSpawn),bIsStartSpawn(_bIsStartSpawn),hLastDropItem(CDnDropItem::Identity()){}
		void Reset()
		{
			fRespawnDelta = fOrgRespawnDelta;
		}

		void FinishRound()
		{
			Reset();
			bIsStartSpawn = bIsOrgStartSpawn;
		}
	}SItemRespawn;

	CDNGameRoom*				m_pGameRoom;
	DNVector(SItemRespawn)		m_vItemRespawn;
};

class CPvPItemRespawnLogic:public CItemRespawnLogic,public TBoostMemoryPool<CPvPItemRespawnLogic>
{
public:
	CPvPItemRespawnLogic( CDNGameRoom* pGameRoom ):CItemRespawnLogic(pGameRoom)
	{
	}
	void FinishRound();
};

class CDLItemRespawnLogic:public CItemRespawnLogic
{
public:
	CDLItemRespawnLogic( CDNGameRoom* pGameRoom ):CItemRespawnLogic(pGameRoom)
	{
	}
};
