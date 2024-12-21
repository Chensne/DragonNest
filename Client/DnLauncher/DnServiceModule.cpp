#include "stdafx.h"
#include "DnServiceModule.h"
#if defined(_KOR)
#include "DnServiceModuleKOR.h"
#elif defined(_CHN)
#include "DnServiceModuleCHN.h"
#elif defined(_JPN)
#include "DnServiceModuleJPN.h"
#elif defined(_TWN)
#include "DnServiceModuleTWN.h"
#elif defined(_USA)
#include "DnServiceModuleUSA.h"
#elif defined(_SG)
#include "DnServiceModuleSG.h"
#elif defined(_THAI)
#include "DnServiceModuleTHAI.h"
#elif defined(_EU)
#include "DnServiceModuleEU.h"
#elif defined(_KRAZ)
#include "DnServiceModuleKRAZ.h"
#elif defined(_TEST)
#include "DnServiceModuleTEST.h"
#else
#include "DnServiceModule.h"
#endif


IServiceModule* g_pServiceModule = NULL;


IServiceModule* DnServiceModule::CreateServiceModule()
{
#if defined(_KOR)
	return ( new CDnServiceModuleKOR() );
#elif defined(_CHN)
	return ( new CDnServiceModuleCHN() );
#elif defined(_JPN)
	return ( new CDnServiceModuleJPN() );
#elif defined(_TWN)
	return ( new CDnServiceModuleTWN() );
#elif defined(_USA)
	return ( new CDnServiceModuleUSA() );
#elif defined(_SG)
	return ( new CDnServiceModuleSG() );
#elif defined(_THAI)
	return ( new CDnServiceModuleTHAI() );
#elif defined(_EU)
	return ( new CDnServiceModuleEU() );
#elif defined(_KRAZ)
	return ( new CDnServiceModuleKRAZ() );
#elif defined(_TEST)
	return ( new CDnServiceModuleTEST() );
#else
	return (new IServiceModule());
#endif
	return NULL;
}

