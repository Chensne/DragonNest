#pragma once

#if defined(_CHN)

#include "../../DnServiceModule.h"


class CDnServiceModuleCHN : public IServiceModule
{
public:
	CDnServiceModuleCHN() {}
	virtual ~CDnServiceModuleCHN() {}

public:
	virtual BOOL Initialize();
	virtual void Destroy();
};

#endif // _CHN