#pragma once

enum emINVEN
{
	PAGE_01 = 0,
	PAGE_02 = 1,
	PAGE_03 = 2,
	PAGE_04 = 3,
	PAGE_05 = 4,
#if defined( PRE_PERIOD_INVENTORY )
	PAGE_PERIOD = 5,
#endif	// #if defined( PRE_PERIOD_INVENTORY )

	ITEM_X = 5,
	ITEM_Y = 6,

	ITEM_MAX = 30,
	PERIOD_ITEM_MAX = 30,
};