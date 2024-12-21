#pragma once

#if defined _SERVER || defined _CLIENT
#include "DnTableDB.h"
#else
#include "costume_mix_define.h"
#endif
#include "./Boost/tuple/tuple.hpp"

#ifdef _SERVER
#include "DNServerDef.h"
#include "DNUserItem.h"
#elif _CLIENT
#include "DnParts.h"
#include "DnCostumeMixDefine.h"
#endif

#ifdef PRE_ADD_COSRANDMIX

#ifdef _CLIENT
class CDnItem;
class CDnItemSlotButton;
#endif

class CDnCostumeRandomMixDataMgr
{
public:
#if !defined( _FINAL_BUILD ) && !defined (__COVERITY__)
	FRIEND_TEST( CostumeRandomMix_unittest, RANDOMTEST );
	FRIEND_TEST( CostumeRandomMix_unittest, PROBABILITY_TEST );
	FRIEND_TEST( CostumeRandomMix_unittest, PROBABILITY_TEST2 );
	FRIEND_TEST( CostumeRandomMix_unittest, PROBABILITY_TEST3 );
#endif // #if !defined( _FINAL_BUILD )

	typedef std::map<int, std::vector<ITEMCLSID> > COSTUME_RANDOM_MIX_RANKLIST; // key : mixRank
	typedef std::map<std::pair<int, int>, COSTUME_RANDOM_MIX_RANKLIST > COSTUME_RANDOM_MIX_LIST; // key : pair<class, partsId>
#ifdef PRE_ADD_COSRANDMIX_MIXERTYPE
	typedef std::map<boost::tuple<int, int, int>, std::vector<float> > COSRANDMIX_PROBABLE_LIST; // key : tuple<mixerEnablePartsType, rank1, rank2>
	#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	typedef std::map<boost::tuple<int, int, int>, INT64> COSRANDMIX_FEE_LIST; // key : pair<rank1, rank2>
	#endif
#else // PRE_ADD_COSRANDMIX_MIXERTYPE
	typedef std::map<std::pair<int, int>, std::vector<float> > COSRANDMIX_PROBABLE_LIST; // key : pair<rank1, rank2>
	#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	typedef std::map<std::pair<int, int>, INT64> COSRANDMIX_FEE_LIST; // key : pair<rank1, rank2>
	#endif
#endif // PRE_ADD_COSRANDMIX_MIXERTYPE

	struct SRandomMixEnableData 
	{
		int nRank;
		bool bShowWorldMsg;

		SRandomMixEnableData() : nRank(-1), bShowWorldMsg(false) {}
	};

	CDnCostumeRandomMixDataMgr();
	virtual ~CDnCostumeRandomMixDataMgr() {}

	bool IsEnableCosRandomMix() const { return m_bEnableCosRandomMix; }
#if defined (_WORK) && defined (_SERVER)
	bool LoadData(bool bReload = false);
#else		//#if defined (_WORK) && defined (_SERVER)
	bool LoadData();
#endif		//#if defined (_WORK) && defined (_SERVER)

#ifdef _SERVER
	bool IsEnableCostumeRandomMixStuff(const std::vector<TItem>& stuffs, int userClassId) const;
	int DoRandomMix(const INT64* pStuffSerials, int stuffCount, int userClassId, const CDNUserItem& userItem, const INT64& mixerCharacterDBID, eError& lastError);
	bool IsWorldMessagingItem(int nItemId) const;
	#ifdef PRE_ADD_COSRANDMIX_ACCESSORY

	#ifdef PRE_ADD_COSRANDMIX_MIXERTYPE
	const std::vector<float>* GetRandomMixProbableList(int mixType, int stuffRank1, int stuffRank2) const;
	#else // PRE_ADD_COSRANDMIX_MIXERTYPE
	const std::vector<float>* GetRandomMixProbableList(int stuffRank1, int stuffRank2) const;
	#endif // PRE_ADD_COSRANDMIX_MIXERTYPE

	#endif // PRE_ADD_COSRANDMIX_ACCESSORY
#endif

#ifdef _CLIENT

	bool IsEnableCostumeRandomMixStuff(const CDnItem* pStuff) const;
	#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	bool MakePreviewItemList(std::vector<SRandomProgressData>& itemList, const CDnParts::PartsTypeEnum& partsType) const;
	void DoMakePreviewItemList(std::vector<SRandomProgressData>& itemList, const COSTUME_RANDOM_MIX_RANKLIST& srcList) const;
	void OnStartMixDlg(CostumeMix::RandomMix::eOpenType type);
	CostumeMix::RandomMix::eOpenType GetCurrentOpenType() const { return m_OpenType; }
	#else
	bool MakePreviewItemList(std::vector<SRandomProgressData>& itemList, const CDnParts::PartsTypeEnum& partsType, const std::vector<ITEMCLSID>& stuffItemIDs) const;
	#endif

	#ifdef PRE_ADD_COSRANDMIX_MIXERTYPE
	void SetMixerEnablePartsType(int nPartsType);
	int GetMixerEnablePartsType() const { return m_nMixerEnablePartsType; }
	#endif

#endif // _CLIENT

	const COSTUME_RANDOM_MIX_RANKLIST* GetRandomMixRankList(int userClassId, int partsId) const;
#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	INT64 GetRandomMixFee(const std::vector<int>& stuffs) const;
#else
	#ifdef PRE_ADD_COSRANDMIX_MIXERTYPE
	const std::vector<float>* GetRandomMixProbableList(int mixType, int stuffRank1, int stuffRank2) const;
	#else
	const std::vector<float>* GetRandomMixProbableList(int stuffRank1, int stuffRank2) const;
	#endif // PRE_ADD_COSRANDMIX_MIXERTYPE
#endif

#if defined (PRE_ADD_COSRANDMIX_MIXERTYPE) && !defined (_TABLEVERIFIER)
	int GetPartsType(int nItemId) const;
	bool IsBasicCashPartsType(int nPartsType) const;
#endif
	int GetRandomMixStuffItemRank(ITEMCLSID itemId) const;

private:
#ifdef _SERVER
	void GetStuffsRankAndPartsType(std::vector<int>& nStuffsRankList, int& nPartsType, const std::vector<TItem>& stuffItemIDs, const CDNUserItem& userItem, const INT64& mixerCharacterDBID) const;
	int DoRandomMix(const std::vector<TItem>& stuffItemIDs, int userClassId, const CDNUserItem& userItem, const INT64& mixerCharacterDBID, eError& lastError);
#endif
	void MakeRankList(COSTUME_RANDOM_MIX_RANKLIST& rankResultList, std::vector<ITEMCLSID>& itemResultList, const int& rank, const ITEMCLSID& itemId);
#ifdef PRE_FIX_68828
	int GetRandomMixResultItemRank(ITEMCLSID itemId) const;
#else
	int GetRandomMixResultItemRank(ITEMCLSID itemId) const {  return -1; } // temp for unit_test
#endif

	const bool m_bEnableCosRandomMix;
	const int m_nCosMixProbabilityResultCount;
	COSTUME_RANDOM_MIX_LIST m_CosRandomMixResultList;
	std::map<int, SRandomMixEnableData> m_CosRandomMixEnableList; // key : itemId
#ifdef PRE_FIX_68828
	std::map<int, SRandomMixEnableData> m_CosRandomMixResultListForRef; // key : itemId
#endif
	COSRANDMIX_PROBABLE_LIST m_CosRandomMixProbabilityList;
#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	COSRANDMIX_FEE_LIST m_CosRandomMixFeeList;
#endif

#ifdef _CLIENT
	#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	CostumeMix::RandomMix::eOpenType m_OpenType;
	#endif
	#ifdef PRE_ADD_COSRANDMIX_MIXERTYPE
	int m_nMixerEnablePartsType;
	#endif
#endif

#ifdef _SERVER
	CMtRandom m_Random;
#endif
};

#endif // PRE_ADD_COSRANDMIX