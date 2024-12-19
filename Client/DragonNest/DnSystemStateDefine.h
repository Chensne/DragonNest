#pragma once

#ifdef PRE_MOD_SYSTEM_STATE

enum eSystemStateMgrMode
{
	eSTATEMGR_EXCLUSIVE,
	eSTATEMGR_SINGLESTATE,
	eSTATEMGR_MULTISTATE,
};

enum eSystemStateMode
{
	eSTATEMODE_EXCLUSIVE,
	eSTATEMODE_TERMINATABLE,
};

enum eSystemState
{
	eSTATE_ALL = 0,
	eSTATE_MASTER = 101,
	eSTATE_DUEL = 201,
	eSTATE_CASHSHOP = 301,
	eSTATE_TRADE_PRIVATE = 401,
	eSTATE_TRADE_MARKET = 402,
	eSTATE_REQ_PARTY = 501,
	eSTATE_PARTY = 502,
	eSTATE_COSTUMEMIX_RANDOM = 601,
};

enum eStateStep
{
	eSTATESTEP_PROCESS,
	eSTATESTEP_TERMINATE,
	eSTATESTEP_MAX
};

struct SStateReadyOption
{
	bool bTerminatePrevStateAll;
	std::vector<eSystemState> vecTerminatePrevState;

	SStateReadyOption() { bTerminatePrevStateAll = false; }
	bool IsEmpty() { return (bTerminatePrevStateAll == false && vecTerminatePrevState.empty()); }
};

#endif // PRE_MOD_INTEG_SYSTEM_STATE