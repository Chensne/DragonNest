#pragma once

#include "Buffer.h"

class CClientSession
{
private:
protected:
	int m_SessionType;

	SOCKET m_Socket;

	CBuffer *m_pRecvBuffer;
	CBuffer *m_pSendBuffer;

	virtual void OnConnect() = 0;
	virtual void OnDisconnect( bool bValidDisconnect );
	virtual void OnRecv() = 0;
	virtual void OnSend() = 0;
	virtual void OnError(int nError) = 0;

public:
	CClientSession(void);
	CClientSession(int nSize);
	virtual ~CClientSession(void);

	virtual void Clear();

	BOOL GetLocalIP(WCHAR* pIP);
	USHORT GetLocalPort(void);

	inline void SetSessionType( int nValue ) { m_SessionType = nValue; }
	inline int GetSessionType() { return m_SessionType; }

	inline SOCKET GetSocket() { return m_Socket; }

	void ClearBuffer();

#if defined( PRE_TEST_PACKETMODULE )
	CBuffer* GetSendBuffer(){ return m_pSendBuffer; }
#endif // #if defined( PRE_TEST_PACKETMODULE )
};
