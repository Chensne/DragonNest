#pragma once

#if defined(PRE_ADD_TALISMAN_SYSTEM) 
#include "DnTalisman.h"

class MATalismanUser
{
public:
	MATalismanUser();
	virtual ~MATalismanUser();

protected:
	DnTalismanHandle m_hTalisman[TALISMAN_MAX];
	bool m_bSelfDelete[TALISMAN_MAX];

public:
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual bool AttachTalisman( DnTalismanHandle hTalisman, int Index, float fRatio, bool bDelete = false );
	virtual bool DetachTalisman( int Index );
	
	DnTalismanHandle GetTalisman( int Index ) { return m_hTalisman[Index]; }
};

#endif // PRE_ADD_TALISMAN_SYSTEM