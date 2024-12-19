#pragma once

#include "DnTableDB.h"
#ifndef _CLIENT
#include "DNServerDef.h"
#endif

class CDnItem;
class CDnParts;
class CDnCostumeMixDataMgr
{
public:
	struct SResultItemInfo
	{
		int showPriority;
		ITEMCLSID itemId;
	};

	template <typename T>
	struct SResultItemCompare : std::binary_function<T, T, bool>
	{
		bool operator() (const T& src, const T& target)
		{
			return src.showPriority > target.showPriority;
		}
	};

	typedef std::set<SResultItemInfo, SResultItemCompare<SResultItemInfo> > MIX_RESULT_LIST;
	typedef std::map<int, MIX_RESULT_LIST> MIXPARTS_RESULT_LIST;
#ifdef _CLIENT
	struct SAddAbility
	{
		int potenOffset;
		int abilityNum;
		float abilityValue;
	};
#endif // _CLIENT

	CDnCostumeMixDataMgr();
	virtual ~CDnCostumeMixDataMgr() {}

#if defined (_WORK) && defined (_SERVER)
	bool LoadData(bool bReload = false);
#else		//#if defined (_WORK) && defined (_SERVER)
	bool LoadData();
#endif		//#if defined (_WORK) && defined (_SERVER)

#ifdef _CLIENT
	int	 GetCurrentClassId() const	{ return m_CurrentClassId; }
	void SetCurrentClassId(int classId);
	const std::vector<SAddAbility>*	GetMixAbilityList(int partsId) const;
#else
	int	 GetMixDropTableID(int classType, int equipType, int weight) const;
	int	 GetPotentialID(int partsId, int rank) const;
#endif
	const MIX_RESULT_LIST* GetMixResultItemList(int classType, int equipType) const;
	bool IsEnableCostumeMix() const { return m_bEnableCostumeMix; }
#ifdef _CLIENT
	bool IsEnableCostumeMixStuff(const CDnParts* pStuff) const;
#endif
	bool IsEnableCostumeDesignMix() const { return m_bEnableCostumeDesignMix; }
	#ifdef _CLIENT
	bool IsEnableCostumeDesignMixStuff(const CDnItem* pStuff) const;
	#else
	bool IsEnableCostumeDesignMixStuff(const TItem* pStuff) const;
	#endif

private:
	MIX_RESULT_LIST&			GetResultItemList(int classType, int equipType);
	bool FillList(MIX_RESULT_LIST* addItemList, std::set<int>& tempGroupList, DNTableFileFormat* pItem, int nDropID, int depthCounter);

	std::map<int, MIXPARTS_RESULT_LIST>			m_CostumeResultList;	// main_key : class, sub_key : parts_id
#ifdef _CLIENT
	std::map<int, std::vector<SAddAbility> >	m_CostumeAddAbility;	// main_key : parts_id
	int											m_CurrentClassId;
#else
	std::multimap<std::pair<int, int>, TCostumeMix>	m_CosMixDropList;	// key : pair<class, equip_type>
	std::map<std::pair<int, int>, int> m_PotentialList;	// key : pair<equip_type, rank>
	std::map<int, int> m_CosDesignMixEnableMap; // key : ItemId
#endif

	bool m_bEnableCostumeMix;
	bool m_bEnableCostumeDesignMix;
};