#pragma once

#if defined(_TEST)

#include "../../DnServiceModule.h"


class CDnServiceModuleTEST : public IServiceModule
{
public:
	CDnServiceModuleTEST() {}
	virtual ~CDnServiceModuleTEST() {}

public:
	virtual BOOL Initialize();
	virtual void Destroy();
};

#endif // _TEST