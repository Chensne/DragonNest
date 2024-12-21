
#pragma once


class CDNFarm
{
public:
	CDNFarm();
	~CDNFarm();

	//init
	bool GetInitFlag() { return m_bIsInitWorld; }
	void SetInitFlag(bool bInitFlag) { m_bIsInitWorld = bInitFlag; }

#if defined( PRE_ADD_VIP_FARM ) || defined( PRE_ADD_FARM_DOWNSCALE )
	bool AddFarmList( const TFarmItemFromDB* pFarm );
#else
	bool AddFarmList(int nFarmDBID, int nFarmMapID, int nFarmMaxUser, const WCHAR * pFarmName, bool bStart);
#endif // #if defined( PRE_ADD_VIP_FARM ) || defined( PRE_ADD_FARM_DOWNSCALE )
	void GetFarmList(TFarmItemFromDB * pFarm, BYTE &cCount);
	void GetActivateFarmList(TFarmItem * pFarm, BYTE &cCount);
	bool GetFarmMapID(UINT nFarmDBID, int &nFarmMapID);
#if defined( PRE_ADD_FARM_DOWNSCALE )
	bool GetFarmAttr( UINT nFarmDBID, int& iAttr );
#elif defined( PRE_ADD_VIP_FARM )
	bool GetFarmAttr( UINT nFarmDBID, Farm::Attr::eType& Attr );
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )

	void UpdateFarmInfo(TFarmInfo * pInfo, BYTE cCount);

private:
	CSyncLock m_Sync;
	std::vector <TFarmItem*> m_vFarmList;

	bool m_bIsInitWorld;

	bool IsExistAsync(UINT nFarmDBID);
};

extern CDNFarm * g_pFarm;

