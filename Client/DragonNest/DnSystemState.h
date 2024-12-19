#pragma once

#include "DnSystemStateDefine.h"

#ifdef PRE_MOD_SYSTEM_STATE

class CDnSystemState
{
public:
	CDnSystemState() : m_bTerminate(false), m_Mode(eSTATEMODE_TERMINATABLE) {}
	virtual ~CDnSystemState() {}

	virtual void Initialize() {}
	virtual void GetStateReadyOption(SStateReadyOption& option) const {}
	virtual void OnStart();
	virtual void OnEnd();
	virtual bool OnTerminate() { return true; }
	virtual void OnTerminateComplete() { m_bTerminate = true; }
	virtual bool IsEmpty() const { return false; }
	virtual bool Process(float fElapsedTime) { return false; }
	bool IsTerminateOK() const { return m_bTerminate; }
	eSystemStateMode IsMode() const { return m_Mode; }

protected:
	bool m_bTerminate;
	eSystemStateMode m_Mode;
};

class CDnNullSystemState : public CDnSystemState
{
public:
	bool OnTerminate() { return true; }
	bool IsEmpty() const { return true; }
};

class CDnSystemState_Exclusive : public CDnSystemState
{
public:
	void Initialize();
};

class CDnCashShopSystemState : public CDnSystemState
{
public:
	void Initialize();
	void GetStateReadyOption(SStateReadyOption& option) const;
	bool OnTerminate();
};

class CDnPartyAcceptRequestSystemState : public CDnSystemState
{
public:
	bool OnTerminate();
};

class CDnPartySystemState : public CDnSystemState
{
public:
	bool OnTerminate();
};

#endif // PRE_MOD_SYSTEM_STATE