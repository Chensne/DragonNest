#pragma once

#ifdef _CH

#include "DnServiceSetup.h"
#include "CryptPacket.h"

class DnShandaService : public IServiceSetup
{
public:
	DnShandaService();
	virtual ~DnShandaService();

	virtual int	PreInitialize(void* pCustomData);
	virtual int	Initialize(void* pCustomData);
	virtual LRESULT MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual int OnEvent(int nType, void* pExtra);
	virtual int Release();

	virtual void OnDispatchMessage(int iMainCmd, int iSubCmd, char * pData, int iLen);
	virtual bool WriteStageLog_( INT32 nStage, LPCTSTR szComment );

private:
	CCryptoKey m_CryptKey;
};

#endif // _CH