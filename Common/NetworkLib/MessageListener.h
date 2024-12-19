#pragma once

class MessageListener
{
public:
	MessageListener() { }
	virtual ~MessageListener() { }

	virtual void OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize ) {}
};

class CTaskListener : public MessageListener
{
public:
	CTaskListener(bool bIsUdp);
	virtual ~CTaskListener();

	virtual void OnConnectTcp() {}
	virtual void OnDisconnectTcp( bool bValidDisconnect ) {}
	virtual void OnDisconnectUdp( bool bValidDisconnect ) {}

	bool GetType() { return m_bIsUdp; }
	virtual void ConnectResult(bool bRet, eBridgeState eState, char cReqGameIDType, int nGameTaskType) {}

protected:
	bool m_bIsUdp;
};