#pragma once

#if defined(_JPN)

#include "../../DnServiceModule.h"


class CDnServiceModuleJPN : public IServiceModule
{
public:
	CDnServiceModuleJPN() {}
	virtual ~CDnServiceModuleJPN() {}

public:
	virtual BOOL Initialize();
	virtual void Destroy();
};

#endif // _JPN