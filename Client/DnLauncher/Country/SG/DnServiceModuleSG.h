#pragma once

#if defined(_SG)

#include "../../DnServiceModule.h"


class CDnServiceModuleSG : public IServiceModule
{
public:
	CDnServiceModuleSG() {}
	virtual ~CDnServiceModuleSG() {}

public:
	virtual BOOL Initialize();
	virtual void Destroy();
};

#endif // _SG