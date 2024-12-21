#include "StdAfx.h"
#include "DnSystemState.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#ifdef PRE_MOD_SYSTEM_STATE

void CDnSystemState::OnStart()
{
	m_bTerminate = false;
}

void CDnSystemState::OnEnd()
{
	m_bTerminate = true;
}

//////////////////////////////////////////////////////////////////////////

void CDnSystemState_Exclusive::Initialize()
{
	m_Mode = eSTATEMODE_EXCLUSIVE;
}

//////////////////////////////////////////////////////////////////////////

#include "DnCashShopTask.h"

void CDnCashShopSystemState::Initialize()
{
	m_Mode = eSTATEMODE_EXCLUSIVE;
}

void CDnCashShopSystemState::GetStateReadyOption(SStateReadyOption& option) const
{
	option.bTerminatePrevStateAll = true;
}

bool CDnCashShopSystemState::OnTerminate()
{
	CDnCashShopTask::GetInstance().RequestCashShopClose();
	return false;
}

//////////////////////////////////////////////////////////////////////////

#include "DnPartyTask.h"

bool CDnPartyAcceptRequestSystemState::OnTerminate()
{
	CDnPartyTask::GetInstance().ClearInviteInfoList(true);
	m_bTerminate = true;

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CDnPartySystemState::OnTerminate()
{
	CDnPartyTask::GetInstance().ReqOutParty();
	m_bTerminate = true;

	return true;
}

#endif // PRE_MOD_SYSTEM_STATE