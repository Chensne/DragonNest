
#pragma once


class CGrowingArea;

class IFarmAreaState
{
public:

	IFarmAreaState( CGrowingArea* pArea )
		:m_pArea(pArea)
	{
	}
	virtual ~IFarmAreaState(){}

	virtual Farm::AreaState::eState GetState() = 0;
	virtual void					BeginProcess() = 0;
	virtual void					Process( float fDelta ) = 0;
	virtual void					EndProcess() = 0;

	// CheckState
	virtual bool					CanStartPlantSeed(){ return false; }								// 씨앗 심을 수 있는 상태?
	virtual bool					CanCancelPlantSeed(){ return false; }								// 씨앗 심는거 취소할 수 있는 상태?
	virtual bool					CanCompletePlantSeed(){ return false; }								// 씨앗 심는거 완료할 수 있는 상태?
	virtual bool					CanStartHarvest(){ return false; }									// 수확 할 수 있는 상태?
	virtual bool					CanCancelHarvest(){ return false; }									// 수확 취소할 수 있는 상태?
	virtual bool					CanCompleteHarvest(){ return false; }								// 수확 완료할 수 있는 상태?
	virtual bool					CanStartAddWater(){ return false; }									// 물 줄 수 있는 상태?
	virtual bool					CanCancelAddWater( CDNUserSession* pSession ){ return false; }		// 물 주는거거 취소할 수 있는 상태?
	virtual bool					CanCompleteAddWater( CDNUserSession* pSession ){ return false; }	// 물 주는거거 완료할 수 있는 상태?
	virtual bool					CanDBUpdate(){ return false; }										// DB Update 해야하는지 검사?
	virtual void					RejectReasonLog(WCHAR* wszReason, CDNUserSession* pSession=NULL) {}

	void SendBeginAreaState();


protected:

	CGrowingArea* m_pArea;
};

