#pragma once

#if defined(_SERVER)

#include "Buffer.h"
#include "IocpManager.h"
#include "DNPacket.h"

const unsigned short PACKET_DELAY_TICK = 20;
const unsigned short STORED_RECV_LIMIT = 20;
const unsigned short STORED_RECV_ELAPSED_TICK = 100;
const unsigned short STAGNATED_DISCONNECT_COUNT = 5;
const unsigned short QUERY_DELAY_TICK = 10000;

class CSpinBuffer;
class CConnection
{
private:
	CSpinBuffer* m_pRecvBuffer;
#ifdef _PACKET_COMP
	CSpinBuffer* m_pCompBuffer;
	bool m_bUseComp;
#endif	

protected:
	CBuffer* m_pSendBuffer;
	CSyncLock m_SendSync;			//sendbuffer sync

private:
	int GetSendCount();
	int ViewSendData(char *pData, int nMaxSize);
	void SkipSendData(int nSize);

	//Stagnate DebugReport
	int m_nStagnatedCount;

#if defined( _USE_ACCEPTEX )
	CSyncLock			m_FlushRecvDataLock;
#endif

protected:
	UINT m_nSessionID;	// Connection UniqueID
	CIocpManager* m_pIocpManager;
	ClientSA m_ClientSA;
	HANDLE m_hIocp;

	bool m_bActive;
	bool m_bConnecting;
	bool m_bDelete;
	CSyncLock m_DeleteLock;

	char m_szIP[IPLENMAX];
	WCHAR m_wszIP[IPLENMAX];
	USHORT m_wPort;

	CSocketContext* m_pSocketContext;
	bool m_bServerConnection;
	bool m_bPushSendQueue;

	bool m_bDetachFlag;
	std::wstring m_wstrDelayDetachReson;

public:
	//Debug RecvMsg Cmd
	struct _DEBUG_INFO
	{
		unsigned char _DebugMainCmd;
		unsigned char _DebugSubCmd;
		unsigned long _DebugTick;
		unsigned long _DebugElapsedTick;
	}	m_DebugInfo[32];
	int m_DebugInfoCount;

	void SetPushSendQueue( const bool bFlag ){ m_bPushSendQueue = bFlag; }
	bool bIsPushSendQueue() const { return m_bPushSendQueue; }
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
	void OnDisconnected()
	{
		if( m_pIocpManager )
			m_pIocpManager->OnDisconnected( m_pSocketContext );
	}
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
	CSocketContext* GetSocketContextPtr(){ return m_pSocketContext; }
	CConnection(void);
	virtual ~CConnection(void);

	int Init(int nRecvLen, int nSendLen, int nCompLen = 0);
	void Final();

	//push I/OBuffers
	virtual int AddRecvData( CSocketContext* pSocketContext );
	virtual int AddRecvData( const DNTPacket& Packet );
	virtual int AddRecvData( const DNEncryptPacketSeq& Packet );

	virtual int AddSendData(int iMainCmd, int iSubCmd, char *pData, int iLen, TParamData* pParamData=NULL, BYTE cSeq = 0, bool bForceUncomp = false);

#if (defined(_KR) || defined(_US)) && (defined(_LOGINSERVER) || defined(_MASTERSERVER) || defined(_CASHSERVER))
	virtual int AddSendData(char *pData, int nLen);
#endif

	//Flush Datas
	int FlushSendData();
	bool FlushRecvData(ULONG nCurTick);
#if defined( _DBSERVER ) || defined( _LOGSERVER ) || defined(_CASHSERVER)
	bool FlushDBData( int nThreadID );
#endif	// #if defined( _DBSERVER )
#if defined(_KR) && (defined(_LOGINSERVER) || defined(_MASTERSERVER))
	bool FlushAuthData();
#elif defined(_CASHSERVER) && (defined(_KR) || defined(_US))
	bool FlushBillingData();
#endif
#if defined(_TW)
	bool FlushAuthData_TW();
#endif	// #if defined(_TW)
#if defined(_TH)
	bool FlushAuthData_TH();
	bool FlushOTPData_TH();
#endif	// #if defined(_TW)

#ifdef _PACKET_COMP
	inline bool GetIsUseComp() { return m_bUseComp; }
	void SetPacketComp(bool bComp);
	void FlushCompData();
#endif

	//IOCP Complete Message
#ifdef _USE_SENDCONTEXTPOOL
	bool SendComplete(TIOContext * pContext, int nSize);
#else
	bool SendComplete(int nSize);
#endif

	virtual void DoUpdate(DWORD CurTick) {}

	int FlushRecvMessage(char * pData, int &nElapsedTick, ULONG nCurTick);
	virtual int MessageProcess(int iMainCmd, int iSubCmd, char *pData, int iLen) = 0;

#if defined( _DBSERVER ) || defined( _LOGSERVER ) || defined(_CASHSERVER)
	virtual void DBMessageProcess(char *pData, int nThreadID){}
#endif

	void SetSocketContext(CIocpManager *pIocpManager, CSocketContext *pSocketContext);
	void BufferClear();
	void RecvBufferClear();
	void SendBufferClear();
	void DetachConnection(wchar_t *pwszIdent);
	bool GetDetachFlag() { return m_bDetachFlag; }
	wchar_t * GetDelayDetachMsg() { return const_cast<wchar_t*>(m_wstrDelayDetachReson.c_str()); }
	void DelayDetachConnection(wchar_t *pwszIdent);

	void SetDelete(bool bDelete);
	bool GetDelete();

	inline void SetActive(bool bActive) { m_bActive = bActive; }
	inline bool GetActive() const { return m_bActive; }
	inline void SetIocpHandle(HANDLE hIocp) {m_hIocp = hIocp; }
	inline HANDLE GetIocpHandle() const { return m_hIocp; }
	inline void SetConnecting(bool bConnecting ) { m_bConnecting = bConnecting; }
	inline bool GetConnecting() const { return m_bConnecting;}
	inline void SetSessionID(UINT nUID) { m_nSessionID = nUID; }
	inline UINT GetSessionID() { return m_nSessionID; }

	inline void SetIp(const char *pIp) 
	{
		memcpy(&m_szIP, pIp, sizeof(m_szIP)); 
		MultiByteToWideChar(CP_ACP, 0, pIp, -1, m_wszIP, IPLENMAX);
	}
	inline char* GetIp() { return m_szIP; }
	inline WCHAR* GetwszIp() { return m_wszIP; }

	inline void SetPort(const USHORT nPort) { m_wPort = nPort; }
	inline USHORT GetPort() { return m_wPort; }

	inline void SetServerConnection(bool bServer) { m_bServerConnection = bServer; }
	inline bool GetServerConnection() { return m_bServerConnection; }

	virtual bool bIsUserSession(){ return false; }
	DWORD GetConnectionKey();
};

#endif	// #if defined(_SERVER)