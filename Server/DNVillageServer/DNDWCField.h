
#pragma once
#if defined(PRE_ADD_DWC)
#include "DNPvPField.h"

// DWC ����
class CDNDWCField : public CDNPvPField
{
public:

	CDNDWCField();
	virtual ~CDNDWCField();

	virtual bool bIsPvPVillage(){ return false; }
	virtual bool bIsDWCVillage(){ return true; }

private:

};
#endif