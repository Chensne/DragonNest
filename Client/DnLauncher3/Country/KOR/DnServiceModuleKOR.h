#pragma once

#if defined(_KOR)

#include "../../DnServiceModule.h"


class CDnServiceModuleKOR : public IServiceModule
{
public:
	CDnServiceModuleKOR() {}
	virtual ~CDnServiceModuleKOR() {}

public:
	virtual BOOL Initialize();
	virtual void Destroy();
	virtual BOOL OnForceFullVersionPatch();

private:
	BOOL InitWiseLog();
};

#endif // _KOR