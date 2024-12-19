#include "StdAfx.h"
#include "DnSystemStateManager.h"

#ifdef PRE_MOD_SYSTEM_STATE

#include "DnMasterTask.h"
#include "DnTradeTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnSystemStateManager::CDnSystemStateManager()
{
	m_Mode = eSTATEMGR_SINGLESTATE;
}

bool CDnSystemStateManager::Initialize(eSystemStateMgrMode mode)
{
	m_Mode = mode;

	return true;
}

void CDnSystemStateManager::Clear()
{
	TerminateStateAll(true);
}

bool CDnSystemStateManager::StartState(eSystemState state)
{
	if (m_ReadySystemState.IsEmpty() == false)
	{
		OutputDebug("[STATE] ALREADY SETSTATE : %d\n", state);
		return false;
	}

	if (IsState(state))
		return false;

	if (m_Mode == eSTATEMGR_EXCLUSIVE)
	{
		if (m_mapSystemState[eSTATESTEP_PROCESS].empty() == false)
			return false;

		if (m_mapSystemState[eSTATESTEP_TERMINATE].empty() == false)
			return false;
	}

	m_ReadySystemState.pStateObj = CreateSystemState(state);
	if (m_ReadySystemState.pStateObj == NULL)
	{
		OutputDebug("[STATE] NO STATE : %d\n", state);
		return false;
	}
	m_ReadySystemState.state = state;

	bool bSetStateImmediate = true;
	if (m_mapSystemState[eSTATESTEP_PROCESS].empty() == false)
	{
		if (IsStateModeTerminatable() == false)
		{
			m_ReadySystemState.Destroy();
			return false;
		}

		SStateReadyOption optionForStart;
		m_ReadySystemState.pStateObj->GetStateReadyOption(optionForStart);
		if (optionForStart.bTerminatePrevStateAll)
		{
			bSetStateImmediate = TerminateStateAll(false);
		}
		else if (optionForStart.vecTerminatePrevState.empty() == false)
		{
			std::vector<eSystemState>::const_iterator iter = optionForStart.vecTerminatePrevState.begin();
			for (; iter != optionForStart.vecTerminatePrevState.end(); ++iter)
			{
				const eSystemState& curState = *iter;
				bool bImm = TerminateOtherState(curState);
				if (bImm == false)
					bSetStateImmediate = false;
			}
		}
	}

	if (bSetStateImmediate)
	{
		if (DoStartState() == false)
		{
			_ASSERT(0);
			m_ReadySystemState.Destroy();
			return false;
		}
	}

	return true;
}

bool CDnSystemStateManager::IsStateModeTerminatable() const
{
	bool bTerminateImmediateAll = true;
	SYSTEMSTATE_MAP::const_iterator iter = m_mapSystemState[eSTATESTEP_PROCESS].begin();
	for (; iter != m_mapSystemState[eSTATESTEP_PROCESS].end();)
	{
		const CDnSystemState* pCurrentState = (*iter).second;
		if (pCurrentState && pCurrentState->IsEmpty() == false)
		{
			if (pCurrentState->IsMode() == eSTATEMODE_EXCLUSIVE)
				return false;
		}
		++iter;
	}

	return true;
}

bool CDnSystemStateManager::TerminateStateAll(bool bImmediate)
{
	bool bTerminateImmediateAll = true;
	SYSTEMSTATE_MAP::iterator iter = m_mapSystemState[eSTATESTEP_PROCESS].begin();
	for (; iter != m_mapSystemState[eSTATESTEP_PROCESS].end();)
	{
		const eSystemState& currentState = (*iter).first;
		CDnSystemState* pCurrentState = (*iter).second;
		if (pCurrentState && pCurrentState->IsEmpty() == false)
		{
			bool bTerminateImmediate = pCurrentState->OnTerminate();
			if (bImmediate == false && bTerminateImmediate == false)
			{
				bTerminateImmediateAll = false;
				m_mapSystemState[eSTATESTEP_TERMINATE].insert(std::make_pair(currentState, pCurrentState));
			}
			else
			{
				SAFE_DELETE(pCurrentState);
			}
			iter = m_mapSystemState[eSTATESTEP_PROCESS].erase(iter);
		}
		else
		{
			++iter;
		}
	}

	if (bImmediate)
	{
		SYSTEMSTATE_MAP::iterator termIter = m_mapSystemState[eSTATESTEP_TERMINATE].begin();
		for (; termIter != m_mapSystemState[eSTATESTEP_TERMINATE].end(); ++termIter)
		{
			CDnSystemState* pCurrentState = (*iter).second;
			if (pCurrentState)
				SAFE_DELETE(pCurrentState);
		}

		m_mapSystemState[eSTATESTEP_TERMINATE].clear();
	}

	return bTerminateImmediateAll;
}

bool CDnSystemStateManager::DoStartState()
{
	if (m_ReadySystemState.IsEmpty() || m_ReadySystemState.pStateObj == NULL)
		return false;

	if (m_Mode == eSTATEMGR_SINGLESTATE && m_mapSystemState[eSTATESTEP_PROCESS].size() > 0)
		return false;

	m_ReadySystemState.pStateObj->OnStart();
	m_mapSystemState[eSTATESTEP_PROCESS].insert(std::make_pair(m_ReadySystemState.state, m_ReadySystemState.pStateObj));
	m_ReadySystemState.Clear();

	return true;
}

bool CDnSystemStateManager::TerminateOtherState(eSystemState state)
{
	bool bTerminateImmediate = true;
	SYSTEMSTATE_MAP::iterator iter = m_mapSystemState[eSTATESTEP_PROCESS].find(state);
	if (iter != m_mapSystemState[eSTATESTEP_PROCESS].end())
	{
		const eSystemState& currentState = (*iter).first;
		CDnSystemState* pCurrentState = (*iter).second;
		if (pCurrentState && pCurrentState->IsEmpty() == false)
		{
			bool bTerminateImmediate = pCurrentState->OnTerminate();
			if (bTerminateImmediate == false)
			{
				bTerminateImmediate = false;
				m_mapSystemState[eSTATESTEP_TERMINATE].insert(std::make_pair(currentState, pCurrentState));
			}
			else
			{
				SAFE_DELETE(pCurrentState);
			}

			m_mapSystemState[eSTATESTEP_PROCESS].erase(iter);
		}
	}

	return bTerminateImmediate;
}

bool CDnSystemStateManager::EndState(eSystemState state)
{
	SYSTEMSTATE_MAP::iterator iter = m_mapSystemState[eSTATESTEP_PROCESS].find(state);
	if (iter != m_mapSystemState[eSTATESTEP_PROCESS].end())
	{
		CDnSystemState* pCurrentState = (*iter).second;
		pCurrentState->OnEnd();
		SAFE_DELETE(pCurrentState);
		m_mapSystemState[eSTATESTEP_PROCESS].erase(iter);
	}
	else
	{
		NotifyCompleteTerminateOtherState(state);
	}

	if (m_ReadySystemState.state == state)
	{
		SAFE_DELETE(m_ReadySystemState.pStateObj);
		m_ReadySystemState.Clear();
	}

	return true;
}

bool CDnSystemStateManager::NotifyCompleteTerminateOtherState(eSystemState state)
{
	SYSTEMSTATE_MAP::iterator iter = m_mapSystemState[eSTATESTEP_TERMINATE].find(state);
	if (iter != m_mapSystemState[eSTATESTEP_TERMINATE].end())
	{
		CDnSystemState* pCurrentState = (*iter).second;
		pCurrentState->OnTerminateComplete();
	}

	return true;
}

void CDnSystemStateManager::Process(float fElapsedTime)
{
	SYSTEMSTATE_MAP::iterator iter = m_mapSystemState[eSTATESTEP_TERMINATE].begin();
	for (; iter != m_mapSystemState[eSTATESTEP_TERMINATE].end();)
	{
		CDnSystemState* pCurrentState = (*iter).second;
		if (pCurrentState && pCurrentState->IsTerminateOK())
		{
			SAFE_DELETE(pCurrentState);
			iter = m_mapSystemState[eSTATESTEP_TERMINATE].erase(iter);
		}
		else
		{
			++iter;
		}
	}

	if (m_mapSystemState[eSTATESTEP_TERMINATE].empty() && m_ReadySystemState.IsEmpty() == false)
	{
		if (DoStartState() == false)
		{
			_ASSERT(0);
			return;
		}
	}
}

bool CDnSystemStateManager::IsState(eSystemState state) const
{
	SYSTEMSTATE_MAP::const_iterator iter = m_mapSystemState[eSTATESTEP_PROCESS].find(state);
	return (iter != m_mapSystemState[eSTATESTEP_PROCESS].end());
}

CDnSystemState* CDnSystemStateManager::CreateSystemState(eSystemState state)
{
	CDnSystemState* pState = NULL;
	switch(state)
	{
	case eSTATE_CASHSHOP:
		{
			pState = new CDnCashShopSystemState;
		}
		break;
	case eSTATE_MASTER:
	case eSTATE_DUEL:
	case eSTATE_TRADE_PRIVATE:
	case eSTATE_TRADE_MARKET:
		{
			pState = new CDnSystemState;
		}
		break;
	case eSTATE_PARTY:
		{
			pState = new CDnPartySystemState;
		}
		break;
	case eSTATE_REQ_PARTY:
		{
			pState = new CDnPartyAcceptRequestSystemState;
		}
		break;
	case eSTATE_COSTUMEMIX_RANDOM:
		{
			pState = new CDnSystemState_Exclusive;
		}
		break;
	default:
		return NULL;
	}

	return pState;
}

#endif // PRE_MOD_SYSTEM_STATE