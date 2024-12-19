#pragma once

#if defined(_RUS)

#include "../../DnServiceModule.h"


class CDnServiceModuleRUS : public IServiceModule
{
public:
	CDnServiceModuleRUS() {}
	virtual ~CDnServiceModuleRUS() {}

public:
	virtual BOOL Initialize();
	virtual void Destroy();
};

#endif // _RUS