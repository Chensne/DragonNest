
#pragma once

#define RANDOMMIX_RESULT_SLOT_INDEX (MAX_COSTUME_RANDOMMIX_STUFF + 1)
#define MAX_FEE_CHAR_COUNT 16

struct SRandomProgressData
{
#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	int progressItem; // itemId
#else
	std::vector<int> progressItemList;
	float fShowRatio;
#endif

#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	SRandomProgressData() : progressItem(-1) {}
#else
	SRandomProgressData() : fShowRatio(0.f) {}
#endif
};