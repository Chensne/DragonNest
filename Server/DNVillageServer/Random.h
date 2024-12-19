#pragma once

#include "MtRandom.h"
#include "Singleton.h"

class CRandom : public CMtRandom, public CSingleton<CRandom>
{
public:
	CRandom() {}
	virtual ~CRandom() {}
};

extern CRandom g_Random;