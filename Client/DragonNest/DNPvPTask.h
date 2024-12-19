#pragma once

#include "Task.h"
//#include "ClientTcpSession.h"
//#include "ClientUdpSession.h"
#include "MessageListener.h"
#include "DNProtocol.h"
#include "DNPacket.h"

class CDNPvPTask : public CTask, public CTaskListener, public CEtUICallback, public CSingleton<CDNPvPTask>
{
public:
	CDNPvPTask();
	~CDNPvPTask();

	bool Initialize();
	void Process(LOCAL_TIME LocalTime, float fDelta);

	void OnConnectTcp() {}
	void OnDisconnectTcp(bool bValidDisconnect) {}

	void OnDispatchMessage(int nMainCmd, int nSubCmd, char *pData, int nSize);

protected:
	void OnRecvPvPMessage(int nSubCmd, char * pData, int nSize);
};