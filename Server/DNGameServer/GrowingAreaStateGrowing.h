
#pragma once

#include "GrowingAreaState.h"


class CFarmAreaStateAddWater;

class CFarmAreaStateGrowing:public IFarmAreaState,public TBoostMemoryPool<CFarmAreaStateGrowing>
{
public:

	CFarmAreaStateGrowing( CGrowingArea* pArea )
		:IFarmAreaState(pArea)
	{
		m_pAddWaterState = NULL;
	}

	virtual ~CFarmAreaStateGrowing();

	virtual Farm::AreaState::eState GetState(){ return Farm::AreaState::GROWING; }
	virtual void					BeginProcess();
	virtual void					Process( float fDelta );
	virtual void					EndProcess();

	// CheckState
	virtual bool					CanStartAddWater();									// 물 줄 수 있는 상태?
	virtual bool					CanCancelAddWater( CDNUserSession* pSession );		// 물 주는거거 취소할 수 있는 상태?
	virtual bool					CanCompleteAddWater( CDNUserSession* pSession );	// 물 주는거거 완료할 수 있는 상태?
	virtual bool					CanDBUpdate();										// DB Update 해야하는지 검사?
	virtual void					RejectReasonLog(WCHAR* wszCondition, CDNUserSession* pSession = NULL);

	//
	void StartAddWater( CDNUserSession* pSession, const CSFarmAddWater* pPacket );
	void CancelAddWater( CDNUserSession* pSession, const CSFarmAddWater* pPacket );
	void QueryAddWater();
	void CompleteAddWater( int iRet );

private:

	CFarmAreaStateAddWater*			m_pAddWaterState;

};

