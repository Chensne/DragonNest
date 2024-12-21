#pragma once

#if defined(_KRAZ)

#include "../../DnServiceModule.h"


class CDnServiceModuleKRAZ : public IServiceModule
{
public:
	CDnServiceModuleKRAZ() {}
	virtual ~CDnServiceModuleKRAZ() {}

public:
	virtual BOOL Initialize();
	virtual void Destroy();
};

#endif // _KRAZ