#pragma once

#include "DnSystemStateDefine.h"
#include "DnSystemState.h"

#ifdef PRE_MOD_SYSTEM_STATE

class CDnSystemStateManager
{
public:
	typedef std::map<eSystemState, CDnSystemState*> SYSTEMSTATE_MAP;
	struct SReadySystemState
	{
		eSystemState state;
		CDnSystemState* pStateObj;

		SReadySystemState() : state(eSTATE_ALL), pStateObj(NULL) {}
		bool IsEmpty() const { return (state == eSTATE_ALL); }
		void Clear() { state = eSTATE_ALL; pStateObj = NULL; }
		void Destroy() { state = eSTATE_ALL; SAFE_DELETE(pStateObj); }
	};

	CDnSystemStateManager();
	virtual ~CDnSystemStateManager() {}

	bool Initialize(eSystemStateMgrMode mode);
	void Clear();

	bool StartState(eSystemState state);
	bool EndState(eSystemState state);

	void Process(float fElapsedTime);

	const SYSTEMSTATE_MAP& GetCurrentProcessSystemState() const { return m_mapSystemState[eSTATESTEP_PROCESS]; }

private:
	bool IsState(eSystemState state) const;
	bool IsStateModeTerminatable() const;

	bool TerminateStateAll(bool bImmediate);
	bool TerminateOtherState(eSystemState state);
	bool NotifyCompleteTerminateOtherState(eSystemState state);

	CDnSystemState* CreateSystemState(eSystemState state);
	bool DoStartState();

protected:
	SYSTEMSTATE_MAP m_mapSystemState[eSTATESTEP_MAX];

	SReadySystemState m_ReadySystemState;
	CDnNullSystemState m_NullState;
	eSystemStateMgrMode m_Mode;
};

#endif // PRE_MOD_SYSTEM_STATE