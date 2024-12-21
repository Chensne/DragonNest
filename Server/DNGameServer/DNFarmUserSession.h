
#pragma once

#include "DNBreakIntoUserSession.h"


class CDNFishingStatus;
class CFishingArea;

class CDNFarmUserSession:public CDNBreakIntoUserSession,public TBoostMemoryPool<CDNFarmUserSession>
{
public:

	CDNFarmUserSession( UINT uiUID, CDNRUDPGameServer* pServer, CDNGameRoom* pRoom );
	virtual ~CDNFarmUserSession();

	virtual void OnDBRecvFarm( int nSubCmd, char *pData );
	//Update
	virtual void DoUpdate( DWORD dwCurTick );
#if defined(PRE_ADD_MULTILANGUAGE)
	virtual void PreInitializeUser(WCHAR * wszAccountName, UINT nAccountDBID, UINT nSessionID, INT64 biCharacterDBID, int iTeam, int nWorldID, int nVillageID, bool bTutorial, bool bAdult, char cPCBangGrade, char cSelectedLanguage, TMemberVoiceInfo * pInfo = NULL);
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	virtual void PreInitializeUser(WCHAR * wszAccountName, UINT nAccountDBID, UINT nSessionID, INT64 biCharacterDBID, int iTeam, int nWorldID, int nVillageID, bool bTutorial, bool bAdult, char cPCBangGrade, TMemberVoiceInfo * pInfo = NULL);
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

	int		GetFarmActiveFieldCount(){ return  m_iFarmActiveFieldCount; }
	void	SetUpdateFieldListFlag( bool bFlag ){ m_bIsNeedUpdateFieldList = bFlag; }
	INT64	GetLastWareHouseItemUniqueID();
	bool	CheckActiveFieldCount();

	// ³óÀåÃ¢°í
	void							LoadFarmWareHouseList( const TAGetListHarvestDepotItem* pPacket );
	const TItem*					GetFarmWareHouseItem( INT64 biUniqueID );
	void							DeleteFarmWareHouseItem( INT64 biUniqueID );
	void							ClearFarmWareHouseItem(){ m_MapFarmWareHouse.clear(); }

	//Fishing
	bool GetFishingSecondarySkillInfo(SecondarySkill::Grade::eType &nSkillGrade, int &nLevel);
#ifdef PRE_ADD_CASHFISHINGITEM
	int CheckFishingRequirement(int nRequirementType1, int nRequirementType2, TFishingToolInfo &Tool, TFishingMeritInfo &Info);
#else		//#ifdef PRE_ADD_CASHFISHINGITEM
	int CheckFishingRequirement(int nRequirementType1, int nUsingInvenIndex1, int nRequirementType2, int nUsingInvenIndex2, INT64 &nBaitSerial);
#endif		//#ifdef PRE_ADD_CASHFISHINGITEM

	bool SetFishingStatus(CFishingArea * pFishingArea);
	CDNFishingStatus * GetFishingStatus() { return m_pFishingStatus; }
	bool GetIsFishing();	
	bool GetIsFishingReward();	
#ifdef PRE_ADD_CASHFISHINGITEM
	int GetFishingRodItemID();
#endif		//#ifdef PRE_ADD_CASHFISHINGITEM

private:

	int		m_iFarmActiveFieldCount;
	bool	m_bIsNeedUpdateFieldList;
	
	// ³óÀåÃ¢°í
	std::map<INT64, TFarmWareHouseItem>	m_MapFarmWareHouse;

	//Fishing
	CDNFishingStatus * m_pFishingStatus;
};


