#pragma once

#include "DnFarmGameTask.h"
#include "DnLifeGaugeDlg.h"
#include "DnWorldOperationProp.h"


class CDnLifeHarvestIcon;

class CDnLifeSkillPlantTask : public CTask, public CTaskListener, public CEtUICallback, public CSingleton<CDnLifeSkillPlantTask>
{
public:
	CDnLifeSkillPlantTask();
	virtual ~CDnLifeSkillPlantTask();

	enum{
		PLANT_NAMELENMAX = 255,
	};

	enum{
		ePlant_Seed,
		ePlant_Water,
		ePlant_Harvest
	};

	enum{
		GROWING_BOOST_ITEM = 348334081,
	};

	enum{
		GROW_LEVEL1,
		GROW_LEVEL2,
		GROW_LEVEL3
	};

	enum{
		eWater_Enable,
		eWater_Disable_Item,
		eWater_Disable_Enough,
		eWater_Disable_Skill
	};

	enum{
		eHarvest_Enable,
		eHarvest_Disable_Item,
		eHarvest_Disable_Skill
	};

	enum{
		JOINT_FARM_AREA,
		PRIVATE_FARM_AREA,
	};

	struct SFarmArea
	{
		bool m_bPrivate;
		int m_eGrowLevel;
		int m_nSelectAreaIndex;
		int m_iItemID;
		int m_iHarvestItemID;
		int m_nRandSeed;
		int  m_nConsumePerSecond;
		INT64 m_dwTotalTime;
		INT64 m_dwElapsedTime;
		INT64 m_dwTotalWater;
		INT64 m_dwWater;
		INT64 m_dwWaterBottle;
		INT64 m_dwRemainWater;
		float m_fTimeBar;
		float m_fWaterBar;
		SOBB * m_pOBB;
		Farm::AreaState::eState m_eState;
		CDnWorldOperationProp * m_pProp;
		CDnWorldOperationProp * m_pWaterProp;
		CDnLifeGaugeDlg * m_pLifeGaugeDlg;
		WCHAR m_wszCharName[NAMELENMAX];
		WCHAR m_wszPlantName[PLANT_NAMELENMAX];
		char m_szHarvestIconName[255];

		SFarmArea( SOBB * pOBB, Farm::AreaState::eState eState, CDnWorldOperationProp * pProp )
			: m_eGrowLevel( GROW_LEVEL1 )
			, m_bPrivate( false )
			, m_nSelectAreaIndex( -1 )
			, m_iItemID( 0 )
			, m_iHarvestItemID( -1 )
			, m_nRandSeed( 0 ) 
			, m_nConsumePerSecond( 0 )
			, m_dwTotalTime( 0 )
			, m_dwElapsedTime( 0 )
			, m_dwTotalWater( 0 )
			, m_dwWater( 0 )
			, m_dwWaterBottle( 0 )
			, m_dwRemainWater( 0 )
			, m_fTimeBar( 0.f )
			, m_fWaterBar( 0.f )
			, m_pOBB( pOBB )
			, m_eState( eState )
			, m_pProp( pProp )
			, m_pWaterProp( NULL )
			, m_pLifeGaugeDlg( NULL )
		{
			memset( m_wszCharName, NULL, sizeof(m_wszCharName) );
			memset( m_wszPlantName, NULL, sizeof(m_wszPlantName) );
			memset( m_szHarvestIconName, NULL, sizeof(m_szHarvestIconName) );
		}
		~SFarmArea()
		{
			SAFE_DELETE( m_pLifeGaugeDlg );
		}

		void CreateGauge()
		{
			m_pLifeGaugeDlg = new CDnLifeGaugeDlg( UI_TYPE_SELF );
			m_pLifeGaugeDlg->Initialize( true );
			m_pLifeGaugeDlg->SetProp( m_pProp );
		}
		void DeleteGauge()
		{
			SAFE_DELETE( m_pLifeGaugeDlg );
		}

		void SetGrowLevel( int eGrowLevel )
		{
			if( !m_pProp )	return;

			m_eGrowLevel = eGrowLevel;
			if( GROW_LEVEL1 == eGrowLevel )
				m_pProp->SetActionQueue( "Level1" );
			else if( GROW_LEVEL2 == eGrowLevel )
				m_pProp->SetActionQueue( "Level2" );
			else if( GROW_LEVEL3 == eGrowLevel )
				m_pProp->SetActionQueue( "Level3" );
		}
	};

	struct SPlantInfo
	{
		int m_nSelectAreaIndex;
		BYTE m_cInvenIndex;
		BYTE m_cCashItemCount;
		INT64 m_nCashWaterItemSerial;
		int m_nUseWaterItemID;
		INT64 m_nCashBoostItemSerials[Farm::Max::PLANTING_ATTACHITEM];
		BYTE m_pCashBoostItemCount[Farm::Max::PLANTING_ATTACHITEM];
		SFarmArea * m_pFarmArea;

		void Clear()
		{
			m_nSelectAreaIndex = -1;
			m_cInvenIndex = 0;
			m_cCashItemCount = 0;
			m_nCashWaterItemSerial = 0;
			m_nUseWaterItemID = 0;
			m_pFarmArea = NULL;
			memset( m_nCashBoostItemSerials, NULL, sizeof(INT64)*Farm::Max::PLANTING_ATTACHITEM );
			memset( m_pCashBoostItemCount, NULL, sizeof(BYTE)*Farm::Max::PLANTING_ATTACHITEM );
		}
	};

	struct SPlantTooltip
	{
		wstring m_wszName;
		wstring m_wszState;
		wstring m_wszTime;
		wstring m_wszWater;
		wstring m_wszWaterCount;
		wstring m_wszWaterTime;
		wstring m_wszPlayerName;

		Farm::AreaState::eState m_eState;

		SPlantTooltip()
			: m_eState( Farm::AreaState::NONE )
		{
		
		}
	};

public:
	bool Initialize();
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	void InitializeGrowingArea();
	void InitializeProp();
	void FinalizeStage();

	virtual void OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize );
	
	void OnRecvFarmSystemMessage( int nSubCmd, char *pData, int nSize );
	void OnRecvSeedInfoList(SCAreaInfoList * pPacket);
	void OnRecvPlant( SCFarmPlant * pPacket );
	void OnRecvHarvest( SCFarmHarvest * pPacket );
	void OnRecvAddWater( SCFarmAddWater * pPacket );
	void OnRecvAreaInfo( SCAreaInfo * pPacket );
	void OnRecvAreaState( SCFarmBeginAreaState * pPacket );
	void OnRecvWareHouseList( SCFarmWareHouseList * pPacket );
	void OnRecvTakeWareHouseItem( SCFarmTakeWareHouseItem * pPacket );
	void OnRecvFieldCountInfo( SCFarmFieldCountInfo * pPacket );
	void OnRecvWareHouseItemCount( SCFarmWareHouseItemCount * pPacket );
	void OnRecvAddWaterAnotheruser( SCFarmAddWaterAnotherUser * pPacket );
	void OnRecvChangeOwnerName( SCChangeOwnerName * pData );

	void RequestPlant( const int iAreaIndex, const Farm::ActionType::eType ActionType, const BYTE cSeedInvenIndex, const BYTE cCount, const INT64 * AttachItems, BYTE * pCount );
	void RequestHarvest( const int iAreaIndex, const Farm::ActionType::eType ActionType, const BYTE cNeedItemInvenIndex  );
	void RequestAddWater( const int iAreaIndex, const Farm::ActionType::eType ActionType, const BYTE cInvenType, const BYTE cWaterItemInvenIndex, const INT64 biWaterItemItemSerial );
	void RequestWareHouseList();
	void RequestTakeWareHouseItem( INT64 biUniqueID );

private :

	CDnLifeHarvestIcon * m_pLifeHarvestIcon;

	int		m_eAcceptType;

	UINT m_nWareHouseItemCount;
	UINT m_nTotalAreaCount;
	int m_nPlantAreaCount;
	SPlantInfo m_PlantInfo;
	std::map<int, SFarmArea *>	m_mFarmArea;
	std::vector<SFarmArea *>	m_vGrowingArea;

	std::vector<CDnItem *>		m_vWaterItem;
	std::vector<int>			m_vWaterSlotIndex;
	std::vector<CDnItem *>		m_vWaterBottle;

	bool	m_bLocalBattle;

	SFarmArea * GetFarmArea( int nIndex );
	SFarmArea * GetFarmArea( DnPropHandle hProp );
	void GrowingProcess( float fElapsedTime );
	void SetFarmAreaInfo( SFarmArea * pFarmArea, TFarmAreaInfo * pPacket );
	void InsertHarvest( int nAreaIndex, SFarmArea * pFarmArea );
	void DeleteHarvest( int nAreaIndex );
	void BattleAndVehicleMode( bool bBattle );

	void PrivateFarmErrorMessage();
	int GetExtendPrivateFarmFieldCount();

public :

	void SeedPlant( BYTE cInvenIndex, BYTE cCount, INT64 * nCashSerial, BYTE * pCount );
	Farm::AreaState::eState GetPropState( DnPropHandle hProp );
	void PropOperation( DnPropHandle hProp );
	SPlantTooltip PlantInfo( SFarmArea * pFarmArea );
	SPlantTooltip PlantInfo( int nAreaIndex );
	SPlantTooltip PlantInfo( DnPropHandle hProp );
	int UseWater( DnPropHandle hProp );
	int UseHarvest( DnPropHandle hProp );
	void OpenBaseDialog();
	void WaterSizeChoice();

	EtVector3 GetFarmAreaPos( int nAreaIndex );
	int GetWareHouseItemCount()	{ return m_nWareHouseItemCount; }
	void GetEmptyAreaInfo( std::vector< std::pair<int, EtVector3> > & vecEmptyAreaInfo );

	void Open( int ePlant );

	bool ChangeState( Farm::AreaState::eState eAreaState, int nAreaIndex, SFarmArea * sFarmArea );
	CDnWorldOperationProp * AddProp( const char * szPropName, EtVector3 & vPos, INT64 nSeedValue, bool bInitialize = false );
	CDnWorldOperationProp * DeleteProp( CDnWorldOperationProp * pProp, EtVector3 & vPos );
};

#define GetLifeSkillPlantTask()		CDnLifeSkillPlantTask::GetInstance()

