
#pragma once


class CPlantSeed:public TBoostMemoryPool<CPlantSeed>
{
public:

	CPlantSeed( int iItemID, DWORD dwElapsedTick, INT64 biCharacterDBID, WCHAR* pwszCharName );

	void	Process( float fDelta );

	int				GetSeedItemID() const { return m_iSeedItemID; }
	INT64			GetOwnerCharacterDBID() const { return m_biOwnerCharacterDBID; }
	const WCHAR*	GetOwnerCharacterName() const { return m_wstrOwnerCharacterName.c_str(); }
	void			MakeAttachItemInfo( TFarmAreaInfo& Info );
	DWORD			GetElapsedTimeSec() const { return m_dwElapsedTick/1000; }
	int				GetFirstAttachItemID() const { return m_mAttachItems.empty() ? 0 : (*m_mAttachItems.begin()).first; }
	int				GetConsumeWater(){ return GetElapsedTimeSec()*m_iConsumeWaterPerSec; }
	DWORD			GetRequiredTimeSec(){ return m_dwRequiredTick/1000; }
	const std::vector<int>&	GetResultItems(){ return m_vResultItems; }

	bool	bIsValid();
	bool	bIsComplete();
	bool	bIsEnoughWater();
	bool	bIsFullWater();
	bool	bIsDBCreate() const { return m_bIsDBCreate; }
	bool	bIsForceComplete() const { return m_bForceComplete; }
	
	//
	void	SetDBCreateFlag( bool bFlag ){ m_bIsDBCreate = bFlag; }
	//
	bool	AttachItem( int iItemID );
	bool	ForceAttachItem( int iItemID, int iCount );
	bool	PushResultItem( int iItemID );
	void	ResetResultItem(){ m_vResultItems.clear(); }
	void	CalcAttachItem();
	void	ResetUpdateTick(){ m_dwUpdateTick=0; }
	void	ForceComplete(){ m_dwElapsedTick = m_dwRequiredTick; m_bForceComplete = true; }

	// DBUpdate 해야히는지 검사
	bool	CanDBUpdate();
	bool	CanAttach( const TItem* pItem, bool bFirst );

	// Cheat
	void	SkipSeed( float fMinusSec );
	bool	UpdateOwnerCharacterName (MAChangeCharacterName* pPacket);

private:

	void	_LoadData();

	bool				m_bIsDBCreate;
	int					m_iSeedItemID;
	DWORD				m_dwElapsedTick;
	DWORD				m_dwRequiredTick;
	DWORD				m_dwUpdateTick;
	int					m_iDefaultWater;		// 해당 식물이 기본적으로 가지고 있는 물양
	int					m_iAttachWater;			// 추가된 물양
	int					m_iAttachWaterBottle;	// 추가된 물통
	int					m_iConsumeWaterPerSec;	// 초당 소모되는 물양
	std::map<int,int>	m_mAttachItems;
	std::vector<int>	m_vResultItems;
	bool				m_bForceComplete;

	// OwnerInfo
	INT64			m_biOwnerCharacterDBID;
	std::wstring	m_wstrOwnerCharacterName;
};

