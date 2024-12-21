#pragma once
#include "DnSkill.h"
#include "DNPacket.h"

class CDnSkillStoreTabDlg;

class CDnTradeSkill
{
public:
	CDnTradeSkill(void);
	virtual ~CDnTradeSkill(void);

protected:
	struct SSkillStoreInfo
	{
		DnSkillHandle hSkill;
		int nPrice;

		SSkillStoreInfo()
			: nPrice(0)
		{
		}
	};

	typedef std::map<int,SSkillStoreInfo>	SKILL_INVEN_MAP;
	typedef SKILL_INVEN_MAP::iterator		SKILL_INVEN_MAP_ITER;

	SKILL_INVEN_MAP m_mapSkillStore;	// Note : 스킬 상점 인벤토리

	CDnSkillStoreTabDlg *m_pSkillStoreDialog;

protected:
	void DeleteSkill( int nSkillID );

public:
	void SetSkillStoreDialog( CDnSkillStoreTabDlg *pDialog ) { m_pSkillStoreDialog = pDialog; }

	//CDnSkill* GetSkillStoreItem( int nSlotIndex );
	//int GetPriceSkillStoreItem( int nSlotIndex );
	//void DeleteSkillStore();
	
	DnSkillHandle FindSkill( int nSkillTableID );

public:
	//void RequestSkillShopBuy( BYTE cShopIndex );

public:
	void OnRecvOpenSkillShop( SCSkillShopOpen* pPacket );
	//void OnRecvSkillShopList( SCSkillShopList *pPacket );
	//void OnRecvSkillShopBuy( SCSkillShopBuyResult *pPacket );
};
