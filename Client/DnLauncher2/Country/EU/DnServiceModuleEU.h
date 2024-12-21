#pragma once

#if defined(_EU)

#include "../../DnServiceModule.h"


class CDnServiceModuleEU : public IServiceModule
{
public:
	CDnServiceModuleEU() {}
	virtual ~CDnServiceModuleEU() {}

public:
	virtual BOOL Initialize();
	virtual void Destroy();
};

#endif // _EU