#pragma once

#include "MessageListener.h"

class IServiceSetup : public MessageListener
{
public:
	//공통 디파인
	enum eServiceInfo{
		GameStart = 1,				//게임 메인펑션 스타트 
		Server_Certification = 2,	//서버인증 통과
		InVillage = 3,				//빌리지도착(콜로세움 포함)
		Ingame = 4,					//게임에 도착 (PVP포함)
		Game_Exit = 5,				//정상종료 
		JoinChannel = 6,
		ExitChannel = 7,
		SecurityModule_Failed = 99 , 		//보안모듈 실패
	};

	IServiceSetup() { }
	virtual ~IServiceSetup() { }

	virtual int	PreInitialize(void* pCustomData);
	virtual int	Initialize(void* pCustomData) { return  0; }
	virtual void* GetCustomData() { return NULL; }

	virtual LRESULT MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { return 0; }
	virtual int OnEvent(int nType, void* pExtra) { return 0; }
	virtual void OnDispatchMessage(int iMainCmd, int iSubCmd, char * pData, int iLen) { }
	virtual int Release() { return  0; }

	virtual bool WriteStageLog_( INT32 nStage , LPCTSTR szComment ){return false;};
	virtual bool WriteErrorLog_( INT32 nStage , LPCTSTR szComment ){return false;};
};

// 서비스 생성은 윈도우 생성 후에 할것. 
class DnServiceSetup
{
public:
	// 해제는 알아서. 
	static IServiceSetup* Create();
};

extern IServiceSetup*	g_pServiceSetup;