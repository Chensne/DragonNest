
#pragma once

class CDNUserSession;
class CFishingArea;

class CDNFishingStatus
{
public :
	CDNFishingStatus(CDNUserSession * pSession, CFishingArea * pFishingArea);
	~CDNFishingStatus();

	void DoUpdate(DWORD nCurTick);

	//Get
	bool IsFishing();
	bool IsFishingReward();
	CFishingArea * GetFishingArea() { return m_pFishingArea; }

	//pattern
	bool SetFishingPattern(CFishingArea * pFishingArea);
#ifdef PRE_ADD_CASHFISHINGITEM
	bool StartFishing(Fishing::Cast::eCast eCastType, TFishingMeritInfo &FishingMerit);
#else		//#ifdef PRE_ADD_CASHFISHINGITEM
	bool StartFishing(Fishing::Cast::eCast eCastType, int nBaitIndex, INT64 biBaitSerial);
#endif		//#ifdef PRE_ADD_CASHFISHINGITEM
	bool StopFishing();
	void FishingControl(Fishing::Control::eControl eControl);
	void CheckAndRewardItem();
#ifdef PRE_ADD_CASHFISHINGITEM
	int GetRodItemID();
#endif		//#ifdef PRE_ADD_CASHFISHINGITEM

protected:
	int m_nFishingPatternID;
	TFishingTableData m_FishingPattern;

private:
	CDNUserSession * m_pSession;
	CFishingArea * m_pFishingArea;
	Fishing::Status::eStatus m_eFishingStatus;
	Fishing::Cast::eCast m_eCastType;
	Fishing::Control::eControl m_eControlType;

	ULONG m_nStartingTime;
	ULONG m_nCheckTick;
	ULONG m_nLastSyncTick;
	int m_nReduceTime;			//when hit target area
	int m_nFishingGauge;
#ifdef PRE_ADD_CASHFISHINGITEM
	TFishingMeritInfo m_FishingToolInfo;
#else		//#ifdef PRE_ADD_CASHFISHINGITEM
	int m_nBaitIndex;
	INT64 m_biBaitSerial;
#endif		//#ifdef PRE_ADD_CASHFISHINGITEM

	bool GetTimeLimit(ULONG &nLimitTime);

	//update & reward
	void UpdateRemainTime(DWORD nCurTick);
	void SendFishingEnd();

	//Send
	void SendFishingResult(int nChatchedItemID, int nRetCode);
};
