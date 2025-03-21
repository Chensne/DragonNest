
#pragma once

#include "GrowingAreaState.h"


class CFarmAreaStateHarvesting:public IFarmAreaState,public TBoostMemoryPool<CFarmAreaStateHarvesting>
{
public:

	CFarmAreaStateHarvesting( CGrowingArea* pArea )
		:IFarmAreaState(pArea)
	{
		m_bQuery					= false;
		m_biHarvestingCharacterDBID = 0;
	}

	virtual Farm::AreaState::eState GetState(){ return Farm::AreaState::HARVESTING; }
	virtual void					BeginProcess();
	virtual void					Process( float fDelta );
	virtual void					EndProcess();

	// CheckState
	virtual bool					CanCompleteHarvest();					// 수확 완료할 수 있는 상태?
	virtual bool					CanCancelHarvest(){ return true; }		// 수확 취소할 수 있는 상태?

	// HarvestInfo
	void	SetHarvestInfo( CDNUserSession* pSession );
	INT64	GetHarvestingCharacterDBID() const { return m_biHarvestingCharacterDBID; }

	void	SetQueryFlag( bool bFlag ){ m_bQuery = bFlag; }
	bool	GetQueryFlag(){ return m_bQuery; }

protected:

	bool			m_bQuery;
	INT64			m_biHarvestingCharacterDBID;
	std::wstring	m_wstrHarvestingCharacterName;
};

