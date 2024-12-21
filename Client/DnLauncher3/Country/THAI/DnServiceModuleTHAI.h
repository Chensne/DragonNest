#pragma once

#if defined(_THAI)

#include "../../DnServiceModule.h"


class CDnServiceModuleTHAI : public IServiceModule
{
public:
	CDnServiceModuleTHAI() {}
	virtual ~CDnServiceModuleTHAI() {}

public:
	virtual BOOL Initialize();
	virtual void Destroy();
};

#endif // _THAI