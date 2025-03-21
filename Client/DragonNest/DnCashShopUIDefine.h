#pragma once

#include <vector>

#define _MAX_COORDI_SLOT				4
#define _MAX_INVEN_SLOT_PER_LINE		8
#define _MAX_INVEN_LINE					4
#define _MAX_PAGE_COUNT_PER_GROUP		5
#define _MAX_GOODS_SLOT_NUM_PER_PAGE	8

enum eCashShopCatType
{
	eMain,
	eCostume,
	eConsumerGoods,
	eCatSet_MAX,
};

enum eCashShopSubCatType
{
	eMain_Min		= 0,
	eMain_Weapon	= eMain_Min,
	eMain_Max,
	eMain_Count		= eMain_Max - eMain_Min,

	eCostume_Min	= eMain_Max + 1,
	eCostume_Top	= eCostume_Min,
	eCostume_Pants,
	eCostume_Glove,
	eCostume_Shoes,
	eCostume_Max,
	eCostume_Count	= eCostume_Max - eCostume_Min,

	eConsGoods_Min	= eCostume_Max + 1,
	eConsGoods_Top	= eConsGoods_Min,
	eConsGoods_Potion,
	eConsGoods_Max,
	eConsGoods_Count = eConsGoods_Max - eConsGoods_Min
};

//	temp
struct SCashShopData
{
	INT64			sn;
	std::wstring	itemName;
	int				count;
	int				price;
	//	todo 1 : type (costume, general)
	//	todo 2 : ability list & ability selected info (general type)
	//	todo 3 : price list & price selected info (costume type)
	std::vector<const WCHAR*>		abilityList;	// todo 2
	std::vector<const WCHAR*>		priceList;		// todo 3

	SCashShopData()
	{
		//pItem = NULL;
		count = price = 0;
	}
};

typedef std::vector<SCashShopData> CS_DATA_LIST;