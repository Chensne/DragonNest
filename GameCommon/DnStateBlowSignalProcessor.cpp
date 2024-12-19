#include "StdAfx.h"
#include "DnStateBlowSignalProcessor.h"
#include "DnBlow.h"


CDnStateBlowSignalProcessor::CDnStateBlowSignalProcessor()
{

}

CDnStateBlowSignalProcessor::~CDnStateBlowSignalProcessor()
{

}

void CDnStateBlowSignalProcessor::AddBlow(DnBlowHandle hBlow)
{
	if (hBlow)
		m_AddList.insert(STATE_BLOW_LIST::value_type(hBlow->GetBlowID(), hBlow));
}

void CDnStateBlowSignalProcessor::RemoveBlow(DnBlowHandle hBlow)
{
	if (hBlow)
		m_DeleteList.insert(STATE_BLOW_LIST::value_type(hBlow->GetBlowID(), hBlow));
}

void CDnStateBlowSignalProcessor::OnSignal(LOCAL_TIME localTime, float fDelta)
{
	STATE_BLOW_LIST::iterator iter = m_MainList.begin();
	STATE_BLOW_LIST::iterator endIter = m_MainList.end();
	for (; iter != endIter; ++iter)
	{
		DnBlowHandle hBlow = iter->second;
		if (hBlow && !hBlow->IsEnd())
			hBlow->SignalProcess(localTime, fDelta);
	}
}

void CDnStateBlowSignalProcessor::AddListProcess()
{
	STATE_BLOW_LIST::iterator iter = m_AddList.begin();
	STATE_BLOW_LIST::iterator endIter = m_AddList.end();
	for (; iter != endIter; ++iter)
	{
		DnBlowHandle hBlow = iter->second;
		if( !hBlow )
			continue;		
		m_MainList.insert(STATE_BLOW_LIST::value_type(hBlow->GetBlowID(), hBlow));
	}

	m_AddList.clear();
}

void CDnStateBlowSignalProcessor::RemoveListProcess()
{
	STATE_BLOW_LIST::iterator iter = m_DeleteList.begin();
	STATE_BLOW_LIST::iterator endIter = m_DeleteList.end();
	for (; iter != endIter; ++iter)
	{
		DnBlowHandle hBlow = iter->second;
	
		STATE_BLOW_LIST::iterator findIter = m_MainList.find(iter->first);
		if (findIter != m_MainList.end())
			m_MainList.erase(findIter);
	}

	m_DeleteList.clear();
}

void CDnStateBlowSignalProcessor::Process( LOCAL_TIME localTime, float fDelta)
{
	AddListProcess();
	RemoveListProcess();

	OnSignal(localTime, fDelta);

	//OnSignal에서 또 제거 리스트에 추가되는 녀석들.. 
	RemoveListProcess();
}