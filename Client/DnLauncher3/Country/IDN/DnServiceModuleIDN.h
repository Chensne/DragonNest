#pragma once

#if defined(_IDN)

#include "../../DnServiceModule.h"


class CDnServiceModuleIDN : public IServiceModule
{
public:
	CDnServiceModuleIDN() {}
	virtual ~CDnServiceModuleIDN() {}

public:
	virtual BOOL Initialize();
	virtual void Destroy();
};

#endif // _IDN