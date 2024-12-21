#pragma once

#include "DnActorState.h"


class CDnVehicleState : virtual public CDnActorState
{
public:
	CDnVehicleState();
	virtual ~CDnVehicleState();

protected:
	int m_nVehicleClassID;

protected:
	virtual void CalcState( RefreshStateExtentEnum Extent, StateTypeEnum Type );
	virtual void CalcBaseState( StateTypeEnum Type );

public:
	void CalcMoveSpeed( RefreshStateExtentEnum Extent, std::vector<CDnState *> *pVecAbsoluteList, std::vector<CDnState *> *pVecRatioList );
	void SetVehicleClassID(int nID);
};

