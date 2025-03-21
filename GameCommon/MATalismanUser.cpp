#include "stdafx.h"

#if defined(PRE_ADD_TALISMAN_SYSTEM) 
#include "MATalismanUser.h"
MATalismanUser::MATalismanUser()
{
	memset( m_bSelfDelete, 0, sizeof(m_bSelfDelete) );
}

MATalismanUser::~MATalismanUser()
{
	for(DWORD i = 0 ; i < TALISMAN_MAX ; ++i)
	{
		DetachTalisman(i);
	}
}

void MATalismanUser::Process( LOCAL_TIME LocalTime, float fDelta )
{
}

bool MATalismanUser::AttachTalisman( DnTalismanHandle hTalisman, int Index, float fRatio, bool bDelete/* = false*/ )
{
	if(!hTalisman)
		return false;

	if( m_hTalisman[Index] && m_hTalisman[Index] != hTalisman)
	{
		MATalismanUser::DetachTalisman(Index);
	}
	m_hTalisman[Index]	 = hTalisman;
	m_bSelfDelete[Index] = bDelete;

	hTalisman->CalculateRatioValue(fRatio);	//슬롯 가중치 계산
	return true;
}

bool MATalismanUser::DetachTalisman( int Index )
{
	if(!m_hTalisman[Index])
		return false;

	if( m_bSelfDelete[Index] )
	{
		SAFE_RELEASE_SPTR(m_hTalisman[Index]);
		m_bSelfDelete[Index] = false;
	}
	m_hTalisman[Index].Identity();
	return true;
}

#endif // PRE_ADD_TALISMAN_SYSTEM