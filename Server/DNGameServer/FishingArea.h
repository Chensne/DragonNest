
#pragma once


class CDNFarmGameRoom;
class CDnFarmGameTask;
class CDNUserSession;
class CDNFarmUserSession;
class CFishingArea : public TBoostMemoryPool<CFishingArea>
{
public:
	CFishingArea(CDNFarmGameRoom * pFarmGameRoom, int nIdx, SOBB * pOBB, TFishingPointTableData * pFishingPoint);
	~CFishingArea();

#ifdef PRE_ADD_CASHFISHINGITEM
	int CheckFishingRequirement(CDNFarmUserSession * pSession, TFishingToolInfo &Tool, TFishingMeritInfo &Info);
#else		//#ifdef PRE_ADD_CASHFISHINGITEM
	int CheckFishingRequirement(CDNFarmUserSession * pSession, int nRodInvenIndex, int nBaitInvenIndex, INT64 &nBaitSerial);
#endif		//#ifdef PRE_ADD_CASHFISHINGITEM

	bool CheckInside(EtVector3 &vPoint);
	int CheckFishingAreaRequirement(int nFishingGrade, int nFishingLevel);
	bool GetFishingPattern(int &nPatternID, TFishingTableData &Fishing);

private:
	CDNFarmGameRoom * m_pFarmGameRoom;
	int m_nFishingAreaIdx;
	SOBB m_OBB;

#ifdef PRE_ADD_CASHFISHINGITEM
	TFishingMeritInfo m_FishingMeritInfo;
#endif		//#ifdef PRE_ADD_CASHFISHINGITEM
	TFishingPointTableData m_FishingPoint;
};

