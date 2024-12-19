#include "stdafx.h"
#include "DnServiceSetup.h"

IServiceSetup*	g_pServiceSetup = NULL;

#if ( defined(_KR) || defined(_US) ) && defined(_AUTH)
	#include "DnNexonService.h"
#elif defined(_CH) && defined(_AUTH)
	#include "DnShandaService.h"
#elif defined(_JP) && defined(_AUTH)
	#include "DnNHNService.h"	
#elif defined(_TW) && defined(_AUTH)
	#include "DnTwnService.h"	
#elif defined(_SG) && defined(_AUTH)
	#include "DnSGService.h"
#elif defined(_TH) && defined(_AUTH)
#include "DnThaiService.h"
#elif defined(_ID) && defined(_AUTH)
#include "DnIdnService.h"
#elif defined(_RU) && defined(_AUTH)
#include "DnRusService.h"
#elif defined(_EU) && defined(_AUTH)
#include "DnEUService.h"
#elif defined(_KRAZ) && defined(_AUTH)
#include "DnKRAZService.h"
#endif

IServiceSetup* DnServiceSetup::Create()
{
#if ( defined(_KR) || defined(_US) ) && defined(_AUTH)
	return (new DnNexonService());
#elif defined(_CH) && defined(_AUTH)
	return (new DnShandaService());
#elif defined(_JP) && defined(_AUTH)
	return (new DnNHNService());
#elif defined(_TW) && defined(_AUTH)
	return (new DnTwnService());
#elif defined(_SG) && defined(_AUTH)
	return (new DnSGService());
#elif defined(_TH) && defined(_AUTH)
	return (new DnThaiService());
#elif defined(_ID) && defined(_AUTH)
	return (new DnIdnService());
#elif defined(_RU) && defined(_AUTH)
	return (new DnRusService());
#elif defined(_EU) && defined(_AUTH)
	return (new DnEUService());
#elif defined(_KRAZ) && defined(_AUTH)
	return (new DnKRAZService());
#else
	return (new IServiceSetup());
#endif
	return NULL;
}

//핵쉴드 테스트를 위해 임시생성
//#define  THEMIDA_TEST_HSHIELD

#ifdef THEMIDA_TEST_HSHIELD
#include "HShieldSetup.h"
#include "DnString.h"
#endif 


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif




int IServiceSetup::PreInitialize(void* pCustomData)
{
#ifdef THEMIDA_TEST_HSHIELD
#ifdef _HSHIELD
	int nResult = 0;
	nResult = HShieldSetup::Init(GetEtDevice()->GetHWnd());
	if ( nResult < 0 )	
	{
		HShieldSetup::Stop();
		MessageBoxW(0, STR_HS_INITIALIZED_FAIL , L"DragonNest",0);

		//blondy
		WriteStageLog_( SecurityModule_Failed, _T("Hack Shield not initialized!") );
		//blondy end

		HWND hWnd = GetEtDevice()->GetHWnd();
		SendMessage(hWnd, WM_CLOSE, 0,0);
		return 0;
	}
	nResult = HShieldSetup::Start();
	if ( nResult < 0 )	
	{
		HShieldSetup::Stop();
		MessageBoxW(0, STR_HS_START_FAIL , L"DragonNest",0);
		//blondy
		WriteStageLog_( SecurityModule_Failed, _T("Hack Shield not initialized!") );
		//blondy end
		HWND hWnd = GetEtDevice()->GetHWnd();
		SendMessage(hWnd, WM_CLOSE, 0,0);
		return 0;
	}

#endif //_HSHIELD
#endif 
	return 0;
}