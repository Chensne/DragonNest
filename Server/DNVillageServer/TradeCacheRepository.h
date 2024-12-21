#pragma once

#include "TimeSet.h"

struct TMarketPriceCacheData
{
	BYTE cLevel;
	BYTE cOption;

	TMarketPrice ItemPrices[2];
};

namespace TradeSystem
{
	class CMarketPriceCache :public CSingleton<CMarketPriceCache>
	{
		public:
			CMarketPriceCache();
			bool CheckResetCache ();
			bool GetTradePrice (int nItemID, BYTE cLevel, BYTE cOption, TMarketPrice* pPrices);
			void SetTradePrice (int nItemID, BYTE cLevel, BYTE cOption, TMarketPrice* pPrices);
			void ClearCache();

		private:
			CSyncLock m_Sync;

			// Key : <ItemID, VecData>
			typedef std::map<int, std::vector<TMarketPriceCacheData>> MTradePrice;
			typedef MTradePrice::iterator MTradePriceItor;
			MTradePrice m_mTradePrice;

			CTimeSet m_tLastResetTime;
	};

}