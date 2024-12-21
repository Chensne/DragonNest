#pragma once

#if defined(_TWN)

#include "../../DnServiceModule.h"


class CDnServiceModuleTWN : public IServiceModule
{
public:
	CDnServiceModuleTWN() {}
	virtual ~CDnServiceModuleTWN() {}

public:
	virtual BOOL Initialize();
	virtual void Destroy();
};

#endif // _TWN